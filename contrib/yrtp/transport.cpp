/**
 * transport.cpp
 * Yet Another RTP Stack
 * This file is part of the YATE Project http://YATE.null.ro
 *
 * Yet Another Telephony Engine - a fully featured software PBX and IVR
 * Copyright (C) 2004, 2005 Null Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <yatertp.h>

#define BUF_SIZE 1500

using namespace TelEngine;

RTPGroup::RTPGroup(Priority prio)
    : Mutex(true), Thread("RTP Group",prio)
{
    XDebug(DebugInfo,"RTPGroup::RTPGroup() [%p]",this);
}

RTPGroup::~RTPGroup()
{
    XDebug(DebugInfo,"RTPGroup::~RTPGroup() [%p]",this);
}

void RTPGroup::cleanup()
{
    XDebug(DebugInfo,"RTPGroup::cleanup() [%p]",this);
    lock();
    ObjList* l = &m_processors;
    for (;l;l = l->next()) {
	RTPProcessor* p = static_cast<RTPProcessor*>(l->get());
	if (p)
	    p->group(0);
    }
    m_processors.clear();
    unlock();
}

void RTPGroup::run()
{
    bool ok = true;
    while (ok) {
	lock();
	Time t;
	ObjList* l = &m_processors;
	for (ok = false;l;l = l->next()) {
	    RTPProcessor* p = static_cast<RTPProcessor*>(l->get());
	    if (p) {
		ok = true;
		p->timerTick(t);
	    }
	}
	unlock();
	Thread::msleep(1,true);
    }
    XDebug(DebugInfo,"RTPGroup::run() ran out of processors [%p]",this);
}

void RTPGroup::join(RTPProcessor* proc)
{
    XDebug(DebugAll,"RTPGroup::join(%p) [%p]",proc,this);
    lock();
    m_processors.append(proc)->setDelete(false);
    startup();
    unlock();
}

void RTPGroup::part(RTPProcessor* proc)
{
    XDebug(DebugAll,"RTPGroup::part(%p) [%p]",proc,this);
    lock();
    m_processors.remove(proc,false);
    unlock();
}

RTPProcessor::RTPProcessor(RTPGroup* grp)
    : m_group(0)
{
    XDebug(DebugAll,"RTPProcessor::RTPProcessor(%p) [%p]",grp,this);
    group(grp);
}

RTPProcessor::~RTPProcessor()
{
    XDebug(DebugAll,"RTPProcessor::~RTPProcessor() [%p]",this);
    group(0);
}

void RTPProcessor::group(RTPGroup* newgrp)
{
    XDebug(DebugAll,"RTPProcessor::group(%p) old=%p [%p]",newgrp,m_group,this);
    if (newgrp == m_group)
	return;
    if (m_group)
	m_group->part(this);
    m_group = newgrp;
    if (m_group)
	m_group->join(this);
}

RTPTransport::RTPTransport(RTPGroup* grp)
    : RTPProcessor(grp),
      m_processor(0), m_monitor(0)
{
}

RTPTransport::~RTPTransport()
{
    setProcessor();
    group(0);
}

void RTPTransport::timerTick(const Time& when)
{
    if (m_rtpSock.valid()) {
	bool ok = false;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (m_rtpSock.select(&ok,0,0,&tv) && ok) {
	    char buf[BUF_SIZE];
	    SocketAddr addr;
	    int len = m_rtpSock.recvFrom(buf,sizeof(buf),addr);
	    if ((len >= 12) && (addr == m_remoteAddr)) {
		if (m_processor)
		    m_processor->rtpData(buf,len);
		if (m_monitor)
		    m_monitor->rtpData(buf,len);
	    }
	}
    }
    if (m_rtcpSock.valid()) {
	bool ok = false;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (m_rtcpSock.select(&ok,0,0,&tv) && ok) {
	    char buf[BUF_SIZE];
	    SocketAddr addr;
	    int len = m_rtcpSock.recvFrom(buf,sizeof(buf),addr);
	    if ((len >= 8) && (addr == m_remoteRTCP)) {
		if (m_processor)
		    m_processor->rtcpData(buf,len);
		if (m_monitor)
		    m_monitor->rtcpData(buf,len);
	    }
	}
    }
}

void RTPTransport::rtpData(const void* data, int len)
{
    if ((len < 12) || !data)
	return;
    if (m_rtpSock.valid() && m_remoteAddr.valid())
	m_rtpSock.sendTo(data,len,m_remoteAddr);
}

void RTPTransport::rtcpData(const void* data, int len)
{
    if ((len < 8) || !data)
	return;
    if (m_rtcpSock.valid() && m_remoteRTCP.valid())
	m_rtcpSock.sendTo(data,len,m_remoteRTCP);
}

void RTPTransport::setProcessor(RTPProcessor* processor)
{
    if (processor) {
	// both should run in the same RTP group
	if (group())
	    processor->group(group());
	else
	    group(processor->group());
    }
    m_processor = processor;
}

void RTPTransport::setMonitor(RTPProcessor* monitor)
{
    m_monitor = monitor;
}

bool RTPTransport::localAddr(SocketAddr& addr)
{
    // check if sockets are already created and bound
    if (m_rtpSock.valid())
	return false;
    int p = addr.port();
    // make sure we don't have a port or it's an even one
    if ((p & 1))
	return false;
    if (m_rtpSock.create(addr.family(),SOCK_DGRAM) && m_rtpSock.bind(addr)) {
	if (!p) {
	    m_rtpSock.getSockName(addr);
	    p = addr.port();
	    if (p & 1) {
		// allocated odd port - have to swap sockets
		m_rtcpSock.attach(m_rtpSock.detach());
		addr.port(p-1);
		if (m_rtpSock.create(addr.family(),SOCK_DGRAM) && m_rtpSock.bind(addr)) {
		    m_localAddr = addr;
		    return true;
		}
		m_rtpSock.terminate();
		m_rtcpSock.terminate();
		return false;
	    }
	}
	addr.port(p+1);
	if (m_rtcpSock.create(addr.family(),SOCK_DGRAM) && m_rtcpSock.bind(addr)) {
	    addr.port(p);
	    m_localAddr = addr;
	    return true;
	}
    }
    m_rtpSock.terminate();
    m_rtcpSock.terminate();
    return false;
}

bool RTPTransport::remoteAddr(SocketAddr& addr)
{
    int p = addr.port();
    // make sure we have a port and it's an even one
    if (p && ((p & 1) == 0)) {
	m_remoteAddr = addr;
	m_remoteRTCP = addr;
	m_remoteRTCP.port(addr.port()+1);
	return true;
    }
    return false;
}

/* vi: set ts=8 sw=4 sts=4 noet: */
