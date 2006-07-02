/**
 * yateiax.h
 * Yet Another IAX2 Stack
 * This file is part of the YATE Project http://YATE.null.ro
 *
 * Yet Another Telephony Engine - a fully featured software PBX and IVR
 * Copyright (C) 2004-2006 Null Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __YATEIAX_H
#define __YATEIAX_H

#include <yateclass.h>

#ifdef _WINDOWS

#ifdef LIBYIAX_EXPORTS
#define YIAX_API __declspec(dllexport)
#else
#ifndef LIBYIAX_STATIC
#define YIAX_API __declspec(dllimport)
#endif
#endif

#endif /* _WINDOWS */

#ifndef YIAX_API
#define YIAX_API
#endif

/**
 * Holds all Telephony Engine related classes.
 */
namespace TelEngine {

class IAXInfoElement;
class IAXInfoElementString;
class IAXInfoElementNumeric;
class IAXInfoElementBinary;
class IAXFullFrame;
class IAXEvent;
class IAXEngine;

#define IAX_PROTOCOL_VERSION         0x0002           // Protocol version
#define IAX2_MAX_CALLNO              32767            // Max call number value
#define IAX2_MAX_TRANSINFRAMELIST    127              // Max transaction incoming frame list

/**
 * This class holds a single Information Element with no data
 * @short A single IAX2 Information Element
 */
class YIAX_API IAXInfoElement : public RefObject
{
public:
    /**
     * Information Element enumeration types
     */
    enum Type {
	textframe = 0x00,	 // Text	Used internally only to generate an event of type Text
        CALLED_NUMBER = 0x01,    // Text
        CALLING_NUMBER = 0x02,   // Text
        CALLING_ANI = 0x03,      // Text
        CALLING_NAME = 0x04,     // Text
        CALLED_CONTEXT = 0x05,   // Text
        USERNAME = 0x06,         // Text
        PASSWORD = 0x07,         // Text
        CAPABILITY = 0x08,       // DW
        FORMAT = 0x09,           // DW
        LANGUAGE = 0x0a,         // Text
        VERSION = 0x0b,          // W		Value: IAX_PROTOCOL_VERSION
        ADSICPE = 0x0c,          // W
        DNID = 0x0d,             // Text
        AUTHMETHODS = 0x0e,      // W
        CHALLENGE = 0x0f,        // Text
        MD5_RESULT = 0x10,       // Text
        RSA_RESULT = 0x11,       // Text
        APPARENT_ADDR = 0x12,    // BIN
        REFRESH = 0x13,          // W
        DPSTATUS = 0x14,         // W
        CALLNO = 0x15,           // W		Max value: IAX2_MAX_CALLNO
        CAUSE = 0x16,            // Text
        IAX_UNKNOWN = 0x17,      // B
        MSGCOUNT = 0x18,         // W
        AUTOANSWER = 0x19,       // Null
        MUSICONHOLD = 0x1a,      // Text
        TRANSFERID = 0x1b,       // DW
        RDNIS = 0x1c,            // Text
        PROVISIONING = 0x1d,     // BIN
        AESPROVISIONING = 0x1e,  // BIN
        DATETIME = 0x1f,         // DW
        DEVICETYPE = 0x20,       // Text
        SERVICEIDENT = 0x21,     // BIN
        FIRMWAREVER = 0x22,      // W
        FWBLOCKDESC = 0x23,      // DW
        FWBLOCKDATA = 0x24,      // BIN
        PROVVER = 0x25,          // DW
        CALLINGPRES = 0x26,      // B
        CALLINGTON = 0x27,       // B
        CALLINGTNS = 0x28,       // W
        SAMPLINGRATE = 0x29,     // DW
        CAUSECODE = 0x2a,        // B
        ENCRYPTION = 0x2b,       // B
        ENKEY = 0x2c,            // BIN
        CODEC_PREFS = 0x2d,      // Text
        RR_JITTER = 0x2e,        // DW
        RR_LOSS = 0x2f,          // DW
        RR_PKTS = 0x30,          // DW
        RR_DELAY = 0x31,         // W
        RR_DROPPED = 0x32,       // DW
        RR_OOO = 0x33,           // DW
    };

    /**
     * Constructor
     * @param type Type of this IE
     */
    inline IAXInfoElement(Type type) : m_type(type) {}

    /**
     * Destructor
     */
    virtual ~IAXInfoElement() {}

    /**
     * Get the type of this IE
     * @return Type of this IE
     */
    inline Type type() const
        { return m_type; }

    /**
     * Constructs a buffer containing this Information Element
     * @param buf Destination buffer
     */
    virtual void toBuffer(DataBlock& buf);

    /**
     * Get the text associated with an IE type value
     * @param ieCode Numeric code of the IE
     * @return Pointer to the IE text or 0 if it doesn't exist
     */
    static const char* ieText(u_int8_t ieCode);

protected:
    Type m_type;		// Type of this IE
};

/**
 * This class holds a single Information Element with text data
 * @short A single IAX2 text Information Element
 */
class YIAX_API IAXInfoElementString : public IAXInfoElement
{
public:
    /**
     * Constructor
     * @param type Type of this IE
     * @param buf Source buffer to construct this IE
     * @param len Buffer length
     */
    inline IAXInfoElementString(Type type, const char* buf, unsigned len) : IAXInfoElement(type), m_strData(buf,(int)len)
        {}

    /**
     * Destructor
     */
    virtual ~IAXInfoElementString() {}

    /**
     * Get the data length
     * @return The data length
     */
    inline int length() const
        { return m_strData.length(); }

    /**
     * Get the data
     * @return The data
     */
    inline String& data()
        { return m_strData; }

    /**
     * Constructs a buffer containing this Information Element
     * @param buf Destination buffer
     */
    virtual void toBuffer(DataBlock& buf);

protected:
    String m_strData;		// IE text data
};

/**
 * This class holds a single Information Element with 1, 2 or 4 byte(s) length data
 * @short A single IAX2 numeric Information Element
 */
class IAXInfoElementNumeric : public IAXInfoElement
{
public:
    /**
     * Constructor
     * @param type Type of this IE
     * @param val Source value to construct this IE
     * @param len Value length
     */
    IAXInfoElementNumeric(Type type, u_int32_t val, u_int8_t len);

    /**
     * Destructor
     */
    virtual ~IAXInfoElementNumeric() {}

    /**
     * Get the data length
     * @return The data length
     */
    inline int length() const
        { return m_length; }

    /**
     * Get the data
     * @return The data
     */
    inline u_int32_t data() const
	{ return m_numericData; }

    /**
     * Constructs a buffer containing this Information Element
     * @param buf Destination buffer
     */
    virtual void toBuffer(DataBlock& buf);

protected:
    u_int8_t m_length;		// IE data length
    u_int32_t m_numericData;	// IE numeric data
};

/**
 * This class holds a single Information Element with binary data
 * @short A single IAX2 numeric Information Element
 */
class YIAX_API IAXInfoElementBinary : public IAXInfoElement
{
public:
    /**
     * Constructor
     * @param type Type of this IE
     * @param buf Source buffer to construct this IE
     * @param len Buffer length
     */
    IAXInfoElementBinary(Type type, unsigned char* buf, unsigned len) : IAXInfoElement(type), m_data(buf,len)
        {}

    /**
     * Destructor
     */
    virtual ~IAXInfoElementBinary() {}

    /**
     * Get the data length
     * @return The data length
     */
    inline int length() const
        { return m_data.length(); }

    /**
     * Get the data
     * @return The data
     */
    inline DataBlock& data()
        { return m_data; }

    /**
     * Constructs a buffer containing this Information Element
     * @param buf Destination buffer
     */
    virtual void toBuffer(DataBlock& buf);

    /**
     * Constructs an APPARENT_ADDR information element from a SocketAddr object
     * @param addr Source object
     * @return A valid IAXInfoElementBinary pointer
     */
    static IAXInfoElementBinary* packIP(const SocketAddr& addr);

    /**
     * Decode an APPARENT_ADDR information element and copy it to a SocketAddr object
     * @param addr Destination object
     * @param ie Source IE
     * @return False if ie is 0
     */
    static bool unpackIP(SocketAddr& addr, IAXInfoElementBinary* ie);

protected:
    DataBlock m_data;		// IE binary data
};

/**
 * Management class for a list of Information Elements
 * @short Information Element container
 */
class YIAX_API IAXIEList
{
public:
    /**
     * Constructor
     */
    inline IAXIEList() : m_invalidIEList(false)
	{}

    /**
     * Constructor. Construct the list from an IAXFullFrame object
     * @param frame Source object
     */
    inline IAXIEList(const IAXFullFrame* frame) : m_invalidIEList(false)
	{ createFromFrame(frame); }

    /**
     * Destructor
     */
    inline ~IAXIEList()
	{}

    /**
     * Get the invalid IE list flag
     * @return False if the last frame parse was unsuccessful
     */
    inline bool invalidIEList() const
	{ return m_invalidIEList; }

    /**
     * Clear the list
     */
    inline void clear()
	{ m_list.clear(); }

    /**
     * Insert a VERSION Information Element in the list if not already done
     */
    void insertVersion();

    /**
     * Get the validity of the VERSION Information Element of the list if any
     * @return False if version is not IAX_PROTOCOL_VERSION or the list doesn't contain a VERSION Information Element
     */
    inline bool validVersion() {
	    u_int32_t ver = 0xFFFF;
	    getNumeric(IAXInfoElement::VERSION,ver);
	    return ver == IAX_PROTOCOL_VERSION;
	}

    /**
     * Append an Information Element to the list
     * @param ie IAXInfoElement pointer to append
     */
    inline void appendIE(IAXInfoElement* ie)
	{ m_list.append(ie); }

    /**
     * Append an Information Element to the list
     * @param type The type of the IAXInfoElement to append
     */
    inline void appendNull(IAXInfoElement::Type type)
	{ m_list.append(new IAXInfoElement(type)); }

    /**
     * Append a text Information Element to the list from a String
     * @param type The type of the IAXInfoElementString to append
     * @param src The source
     */
    inline void appendString(IAXInfoElement::Type type, const String& src)
	{ m_list.append(new IAXInfoElementString(type,src.c_str(),src.length())); }

    /**
     * Append a text Information Element to the list from a buffer
     * @param type The type of the IAXInfoElementString to append
     * @param src The source
     * @param len Source length
     */
    inline void appendString(IAXInfoElement::Type type, unsigned char* src, unsigned len)
	{ m_list.append(new IAXInfoElementString(type,(char*)src,len)); }

    /**
     * Append a numeric Information Element to the list
     * @param type The type of the IAXInfoElementNumeric to append
     * @param value The source
     * @param len Source length
     */
    inline void appendNumeric(IAXInfoElement::Type type, u_int32_t value, u_int8_t len)
	{ m_list.append(new IAXInfoElementNumeric(type,value,len)); }

    /**
     * Append a binary Information Element to the list
     * @param type The type of the IAXInfoElementBinary to append
     * @param value The source
     * @param len Source length
     */
    inline void appendBinary(IAXInfoElement::Type type, unsigned char* data, unsigned len)
	{ m_list.append(new IAXInfoElementBinary(type,data,len)); }

    /**
     * Construct the list from an IAXFullFrame object.
     *  On exit m_invalidIEList will contain the opposite of the returned value
     * @param frame Source object
     * @return False if the frame contains invalid IEs
     */
    bool createFromFrame(const IAXFullFrame* frame);

    /**
     * Construct a buffer from this list
     * @param buf Destination buffer
     */
    void toBuffer(DataBlock& buf);

    /**
     * Get an IAXInfoElement from the list
     * @param type The desired type
     * @return An IAXInfoElement pointer or 0 if the list doesn't contain an IE of this type
     */
    IAXInfoElement* getIE(IAXInfoElement::Type type);

    /**
     * Get the data of a list item into a String. Before any operation dest is cleared
     * @param type The desired type
     * @param dest The destination String
     * @return False if the list doesn't contain an IE of this type
     */
    bool getString(IAXInfoElement::Type type, String& dest);

    /**
     * Get the data of a list item into a numeric destination
     * @param type The desired type
     * @param dest The destination
     * @return False if the list doesn't contain an IE of this type
     */
    bool getNumeric(IAXInfoElement::Type type, u_int32_t& dest);

    /**
     * Get the data of a list item into a DataBlock. Before any operation dest is cleared
     * @param type The desired type
     * @param dest The destination buffer
     * @return False if the list doesn't contain an IE of this type
     */
    bool getBinary(IAXInfoElement::Type type, DataBlock& dest);

private:
    bool m_invalidIEList;	// Invalid IE flag
    ObjList m_list;		// The IE list
};

/**
 * This class holds the enumeration values for authentication methods
 * @short Wrapper class for authentication methods values
 */
class YIAX_API IAXAuthMethod
{
public:
    /**
     * Authentication method enumeration types
     */
    enum Type {
        Text = 1,
        MD5  = 2,
        RSA  = 4,
    };
};

/**
 * This class holds the enumeration values for audio and video formats
 * @short Wrapper class for audio and video formats
 */
class YIAX_API IAXFormat
{
public:
    /**
     * Audio format enumeration types
     */
    enum Audio {
        G723_1 = (1 <<  0),
        GSM    = (1 <<  1),
        ULAW   = (1 <<  2),
        ALAW   = (1 <<  3),
        MP3    = (1 <<  4),
        ADPCM  = (1 <<  5),
        SLIN   = (1 <<  6),
        LPC10  = (1 <<  7),
        G729A  = (1 <<  8),
        SPEEX  = (1 <<  9),
        ILBC   = (1 << 10),
    };

    /**
     * Video format enumeration types
     */
    enum Video {
        JPEG   = (1 << 16),
        PNG    = (1 << 17),
        H261   = (1 << 18),
        H263   = (1 << 19),
    };

    /**
     * Get the text associated with an audio format
     * @param audio The desired format
     * @return A pointer to the text associated with the format or 0 if the format doesn't exist
    */
    static const char* audioText(u_int32_t audio);

    /**
     * Get the text associated with a video format
     * @param video The desired format
     * @return A pointer to the text associated with the format or 0 if the format doesn't exist
    */
    static const char* videoText(u_int32_t video);

    /**
     * Keep the texts associated with the audio formats
    */
    static TokenDict audioData[];

    /**
     * Keep the texts associated with the video formats
    */
    static TokenDict videoData[];
};

/**
 * This class holds the enumeration values for IAX control (subclass)
 * @short Wrapper class for subclasses of frames of type IAX
 */
class YIAX_API IAXControl
{
public:
    /**
     * IAX control (subclass) enumeration types
     */
    enum Type {
        New       = 0x01,
        Ping      = 0x02,
        Pong      = 0x03,
        Ack       = 0x04,
        Hangup    = 0x05,
        Reject    = 0x06,
        Accept    = 0x07,
        AuthReq   = 0x08,
        AuthRep   = 0x09,
        Inval     = 0x0a,
        LagRq     = 0x0b,
        LagRp     = 0x0c,
        RegReq    = 0x0d,
        RegAuth   = 0x0e,
        RegAck    = 0x0f,
        RegRej    = 0x10,
        RegRel    = 0x11,
        VNAK      = 0x12,
        DpReq     = 0x13,
        DpRep     = 0x14,
        Dial      = 0x15,
        TxReq     = 0x16,
        TxCnt     = 0x17,
        TxAcc     = 0x18,
        TxReady   = 0x19,
        TxRel     = 0x1a,
        TxRej     = 0x1b,
        Quelch    = 0x1c,
        Unquelch  = 0x1d,
        Poke      = 0x1e,
	//Reserved  = 0x1f,
        MWI       = 0x20,
        Unsupport = 0x21,
        Transfer  = 0x22,
        Provision = 0x23,
        FwDownl   = 0x24,
        FwData    = 0x25,
    };
};

/**
 * This class holds all data needded to manage an IAX frame
 * @short This class holds an IAX frame
 */
class YIAX_API IAXFrame : public RefObject
{
public:
    /**
     * IAX frame type enumeration
     */
    enum Type {
        DTMF    = 0x01,
        Voice   = 0x02,
        Video   = 0x03,
        Control = 0x04,
        Null    = 0x05,
        IAX     = 0x06,
        Text    = 0x07,
        Image   = 0x08,
        HTML    = 0x09,
        Noise   = 0x0a,
    };

    /**
     * Constructor. Constructs an incoming frame
     * @param type Frame type
     * @param sCallNo Source call number
     * @param tStamp Frame timestamp
     * @param retrans Retransmission flag
     * @param buf IE buffer
     * @param len IE buffer length
     */
    IAXFrame(Type type, u_int16_t sCallNo, u_int32_t tStamp, bool retrans,
	     const unsigned char* buf, unsigned int len);

    /**
     * Destructor
     */
    virtual ~IAXFrame();

    /**
     * Get the type of this frame as enumeration
     * @return The type of this frame as enumeration
     */
    inline Type type() const
	{ return m_type; }

    /**
     * Get the data buffer of the frame
     * @return The data buffer of the frame
     */
    inline DataBlock& data()
        { return m_data; }

    /**
     * Get the retransmission flag of this frame
     * @return The retransmission flag of this frame
     */
    inline bool retrans() const
	{ return m_retrans; }

    /**
     * Get the source call number of this frame
     * @return The source call number of this frame
     */
    inline u_int16_t sourceCallNo() const
	{ return m_sCallNo; }

    /**
     * Get the timestamp of this frame
     * @return The timestamp of this frame
     */
    inline u_int32_t timeStamp() const
	{ return m_tStamp; }

    /**
     * Get a pointer to this frame if it is a full frame
     * @return A pointer to this frame if it is a full frame or 0
     */
    virtual const IAXFullFrame* fullFrame() const;

    /**
     * Parse a received buffer and returns a IAXFrame pointer if valid
     * @param buf Received buffer
     * @param len Buffer length
     * @param engine The IAXEngine who requested the operation
     * @param addr The source address
     * @return A frame pointer on success or 0
     */
    static IAXFrame* parse(const unsigned char* buf, unsigned int len, IAXEngine* engine = 0, const SocketAddr* addr = 0);

    /**
     * Pack a subclass value according to IAX protocol
     * @param value Value to pack
     * @return The packed subclass value or 0 if invalid (>255 and not a power of 2)
     */
    static u_int8_t packSubclass(u_int32_t value);

    /**
     * Unpack a subclass value according to IAX protocol
     * @param value Value to unpack
     * @return The unpacked subclass value
     */
    static u_int32_t unpackSubclass(u_int8_t value);

protected:
    Type m_type;		// Frame type
    DataBlock m_data;		// Frame IE list if incoming, the whole frame if outgoing
    bool m_retrans;		// Retransmission flag
    u_int16_t m_sCallNo;	// Source call number
    u_int32_t m_tStamp;		// Frame timestamp
};

/**
 * This class holds all data needded to manage an IAX full frame
 * @short This class holds an IAX full frame
 */
class YIAX_API IAXFullFrame : public IAXFrame
{
public:
    /**
     * IAX frame subclass enumeration types for frames of type Control
     */
    enum ControlType {
        Hangup = 0x01,
        //Ring = 0x02,
        Ringing = 0x03,
        Answer = 0x04,
        Busy = 0x05,
        Congestion = 0x08,
        FlashHook = 0x09,
        Option = 0x0b,
        KeyRadio = 0x0c,
        UnkeyRadio = 0x0d,
        Progressing = 0x0e,
        Proceeding = 0x0f,
        Hold = 0x10,
        Unhold = 0x11,
        VidUpdate = 0x12,
    };

    /**
     * Constructor. Constructs an incoming full frame
     * @param type Frame type
     * @param subclass Frame subclass
     * @param sCallNo Source (remote) call number
     * @param dCallNo Destination (local) call number
     * @param oSeqNo Outgoing sequence number
     * @param iSeqNo Incoming (expected) sequence number
     * @param tStamp Frame timestamp
     * @param retrans Retransmission flag
     * @param buf IE buffer
     * @param len IE buffer length
     */
    IAXFullFrame(Type type, u_int32_t subclass, u_int16_t sCallNo, u_int16_t dCallNo,
		 unsigned char oSeqNo, unsigned char iSeqNo,
		 u_int32_t tStamp, bool retrans,
		 const unsigned char* buf, unsigned int len);

    /**
     * Constructor. Constructs an outgoing full frame
     * @param type Frame type
     * @param subclass Frame subclass
     * @param sCallNo Source (remote) call number
     * @param dCallNo Destination (local) call number
     * @param oSeqNo Outgoing sequence number
     * @param iSeqNo Incoming (expected) sequence number
     * @param tStamp Frame timestamp
     * @param buf IE buffer
     * @param len IE buffer length
     */
    IAXFullFrame(Type type, u_int32_t subclass, u_int16_t sCallNo, u_int16_t dCallNo,
		 unsigned char oSeqNo, unsigned char iSeqNo,
		 u_int32_t tStamp,
		 const unsigned char* buf = 0, unsigned int len = 0);

    /**
     * Destructor
     */
    virtual ~IAXFullFrame();

    /**
     * Get the destination call number
     * @return The destination call number
     */
    inline u_int16_t destCallNo() const
        { return m_dCallNo; }

    /**
     * Get the outgoing sequence number
     * @return The outgoing sequence number
     */
    inline unsigned char oSeqNo() const
        { return m_oSeqNo; }

    /**
     * Get the incoming sequence number
     * @return The incoming sequence number
     */
    inline unsigned char iSeqNo() const
        { return m_iSeqNo; }

    /**
     * Get the subclass of this frame
     * @return The subclass of this frame
     */
    inline u_int32_t subclass() const
	{ return m_subclass; }

    /**
     * Get a pointer to this frame if it is a full frame
     * @return A pointer to this frame
     */
    virtual const IAXFullFrame* fullFrame() const;

protected:
    u_int16_t m_dCallNo;	// Destination call number
    unsigned char m_oSeqNo;	// Out sequence number
    unsigned char m_iSeqNo;	// In sequence number
    u_int32_t m_subclass;	// Subclass
};

/**
 * This class holds all data needded to manage an outgoing IAX full frame
 * @short This class holds an outgoing IAX full frame
 */
class YIAX_API IAXFrameOut : public IAXFullFrame
{
public:
    /**
     * Constructor. Constructs an outgoing full frame
     * @param type Frame type
     * @param subclass Frame subclass
     * @param sCallNo Source (remote) call number
     * @param dCallNo Destination (local) call number
     * @param oSeqNo Outgoing sequence number
     * @param iSeqNo Incoming (expected) sequence number
     * @param tStamp Frame timestamp
     * @param buf IE buffer
     * @param len IE buffer length
     * @param retransCount Retransmission counter
     * @param retransInterval Time interval to the next retransmission
     * @param ackOnly Acknoledge only flag. If true, the frame only expects an ACK
     */
    inline IAXFrameOut(Type type, u_int32_t subclass, u_int16_t sCallNo, u_int16_t dCallNo,
                       unsigned char oSeqNo, unsigned char iSeqNo, u_int32_t tStamp, const unsigned char* buf, unsigned int len,
                       u_int16_t retransCount, u_int32_t retransInterval, bool ackOnly)
        : IAXFullFrame(type,subclass,sCallNo,dCallNo,oSeqNo,iSeqNo,tStamp,buf,len),
          m_ack(false), m_ackOnly(ackOnly), m_retransCount(retransCount), m_retransTimeInterval(retransInterval),
	  m_nextTransTime(Time::msecNow() + m_retransTimeInterval)
	{}

    /**
     * Destructor
     */
    virtual ~IAXFrameOut()
	{}

    /**
     * Get the timeout (retransmission counter) of this frame
     * @return True if the retransmission counter is 0
     */
    inline bool timeout() const
        { return m_retransCount == 0; }

    /**
     * Ask the frame if it's time for retransmit
     * @param time Current time
     * @return True if it's time to retransmit
     */
    inline bool timeForRetrans(u_int64_t time) const
        { return time > m_nextTransTime; }

    /**
     * Set the retransmission flag of this frame
     */
    void setRetrans();

    /**
     * Update the retransmission counter and the time to next retransmission
     */
    void transmitted();

    /**
     * Get the acknoledged flag of this frame
     * @return The acknoledged flag of this frame
     */
    inline bool ack() const
	{ return m_ack; }

    /**
     * Set the acknoledged flag of this frame
     */
    inline void setAck()
	{ m_ack = true; }

    /**
     * Get the acknoledge only flag of this frame
     * @return The acknoledge only flag of this frame
     */
    inline bool ackOnly() const
	{ return m_ackOnly; }

    /**
     * Increase the timeout for acknoledged authentication frames sent and set the counter to 1
     * @param nextTransTime Next transmission time
     */
    void adjustAuthTimeout(u_int64_t nextTransTime);

protected:
    bool m_ack;				// Acknoledge flag
    bool m_ackOnly;			// Frame need only ACK as a response
    u_int16_t m_retransCount;		// Retransmission counter
    u_int32_t m_retransTimeInterval;	// Retransmission interval
    u_int64_t m_nextTransTime;		// Next transmission time
};

/**
 * Handle meta trunk frame with timestamps
 * @short Meta trunk frame
 */
class YIAX_API IAXMetaTrunkFrame : public RefObject, public Mutex
{
public:
    /**
     * Constructor. Constructs an outgoing meta trunk frame
     * @param engine The engine that owns this frame
     * @param addr Remote peer address
     */
    IAXMetaTrunkFrame(IAXEngine* engine, const SocketAddr& addr);

    /**
     * Destructor
     */
    virtual ~IAXMetaTrunkFrame();

    /**
     * Get the remote peer address
     * @return The remote peer address
     */
    inline const SocketAddr& addr() const
	{ return m_addr; }

    /**
     * Get the timestamp of this frame
     * @return The timestamp of this frame
     */
    inline u_int32_t timestamp()
	{ return m_timestamp; }

    /**
     * Set the timestamp of this frame
     * @param tStamp Timestamp value to set
     */
    void setTimestamp(u_int32_t tStamp);

    /**
     * Add a mini frame. If no room, send before adding
     * @param sCallNo Sorce call number
     * @param data Mini frame data
     * @param tStamp Mini frame timestamp
     * @return False if the frame was sent and the write operation failed
     */
    bool add(u_int16_t sCallNo, const DataBlock& data, u_int32_t tStamp);

    /**
     * Send this frame to remote peer
     * @param tStamp Frame timestamp
     * @return The result of the write operation
     */
    bool send(u_int32_t tStamp);

protected:
    u_int8_t* m_data;		// Data buffer
    u_int16_t m_dataAddIdx;	// Current add index
    u_int32_t m_timestamp;	// Frame timestamp
    IAXEngine* m_engine;	// The engine that owns this frame
    SocketAddr m_addr;		// Remote peer address
};

/**
 * This class holds all the data needded for the management of an IAX2 transaction
 *  which might be a call leg, a register/unregister or a poke one
 * @short An IAX2 transaction
 */
class YIAX_API IAXTransaction : public RefObject, public Mutex
{
    friend class IAXEvent;
    friend class IAXEngine;
public:
    /**
     * The transaction type as enumeration
     */
    enum Type {
	Incorrect,			// Unsupported/unknown type
	New,				// Media exchange call
	RegReq,				// Registration
	RegRel,				// Registration release
	Poke,				// Ping
	//FwDownl,
    };

    /**
     * The transaction state as enumeration
     */
    enum State {
        Connected,		     	// Call leg established (Accepted) for transactions of type New
	NewLocalInvite,		     	// New outgoing transaction: Poke/New/RegReq/RegRel
	NewLocalInvite_AuthRecv,     	// Auth request received for an outgoing transaction
	NewLocalInvite_RepSent,	     	// Auth reply sent for an outgoing transaction
	NewRemoteInvite,             	// New incoming transaction: Poke/New/RegReq/RegRel
	NewRemoteInvite_AuthSent,    	// Auth sent for an incoming transaction
	NewRemoteInvite_RepRecv,     	// Auth reply received for an incoming transaction
	Unknown,                     	// Initial state
	Terminated,                  	// Terminated. No more frames accepted
        Terminating,                 	// Terminating. Wait for ACK or timeout to terminate
    };

    /**
     * Constructs an incoming transaction from a received full frame with an IAX
     *  control message that needs a new transaction
     * @param engine The engine that owns this transaction
     * @param frame A valid full frame
     * @param lcallno Local call number
     * @param addr Address from where the frame was received
     * @param data Pointer to arbitrary user data
     */
    static IAXTransaction* factoryIn(IAXEngine* engine, IAXFullFrame* frame, u_int16_t lcallno, const SocketAddr& addr,
		void* data = 0);

    /**
     * Constructs an outgoing transaction with an IAX control message that needs a new transaction
     * @param engine The engine that owns this transaction
     * @param type Transaction type
     * @param lcallno Local call number
     * @param addr Address to use
     * @param ieList Starting IE list
     * @param data Pointer to arbitrary user data
     */
    static IAXTransaction* factoryOut(IAXEngine* engine, Type type, u_int16_t lcallno, const SocketAddr& addr,
		IAXIEList& ieList, void* data = 0);

    /**
     * Destructor
     */
    virtual ~IAXTransaction();

    /**
     * The IAX engine this transaction belongs to
     * @return Pointer to the IAXEngine of this transaction
     */
    inline IAXEngine* getEngine() const
        { return m_engine; }

    /**
     * Get the type of this transaction
     * @return The type of the transaction as enumeration
     */
    inline Type type() const
        { return m_type; }

    /**
     * Get the state of this transaction
     * @return The state of the transaction as enumeration
     */
    inline State state() const
        { return m_state; }

    /**
     * Get the timestamp of this transaction
     * @return The timestamp of this transaction
     */
    inline u_int64_t timeStamp() const
        { return Time::msecNow() - m_timeStamp; }

    /**
     * Get the direction of this transaction
     * @return True if it is an outgoing transaction
     */
    inline bool outgoing() const
        { return m_localInitTrans; }

    /**
     * Store a pointer to arbitrary user data
     * @param data User provided pointer
     */
    inline void setUserData(void* data)
        { m_userdata = data; }

    /**
     * Return the opaque user data stored in the transaction
     * @return Pointer set by user
     */
    inline void* getUserData() const
        { return m_userdata; }

    /**
     * Retrive the local call number
     * @return 15-bit local call number
     */
    inline u_int16_t localCallNo() const
        { return m_lCallNo; }

    /**
     * Retrive the remote call number
     * @return 15-bit remote call number
     */
    inline u_int16_t remoteCallNo() const
        { return m_rCallNo; }

    /**
     * Retrive the remote host+port address
     * @return A reference to the remote address
     */
    inline const SocketAddr& remoteAddr() const
        { return m_addr; }

    /**
     * Retrive the username
     * @return A reference to the username
     */
    inline const String& username()
	{ return m_username; }

    /**
     * Retrive the password
     * @return A reference to the password
     */
    inline const String& password()
	{ return m_password; }

    /**
     * Retrive the calling number
     * @return A reference to the calling number
     */
    inline const String& callingNo()
	{ return m_callingNo; }

    /**
     * Retrive the calling name
     * @return A reference to the calling name
     */
    inline const String& callingName()
	{ return m_callingName; }

    /**
     * Retrive the called number
     * @return A reference to the called number
     */
    inline const String& calledNo()
	{ return m_calledNo; }

    /**
     * Retrive the called context
     * @return A reference to the called context
     */
    inline const String& calledContext()
	{ return m_calledContext; }

    /**
     * Retrive the challenge sent/received during authentication
     * @return A reference to the challenge
     */
    inline const String& challenge()
	{ return m_challenge; }

    /**
     * Retrive the media format used during initialization
     * @return The initial media format
     */
    inline u_int32_t format()
	{ return m_format; }

    /**
     * Retrive the incoming media format
     * @return The incoming media format
     */
    inline u_int32_t formatIn()
	{ return m_formatIn; }

    /**
     * Retrive the outgoing media format
     * @return The outgoing media format
     */
    inline u_int32_t formatOut() const 
	{ return m_formatOut; }

    /**
     * Retrive the media capability of this transaction
     * @return The media capability of this transaction
     */
    inline u_int32_t capability() const
	{ return m_capability; }

    /**
     * Retrive the expiring time for a register/unregister transaction
     * @return The expiring time for a register/unregister transaction
     */
    inline u_int32_t expire() const
	{ return m_expire; }

    /**
     * Retrive the authentication data sent/received during authentication
     * @return A reference to the authentication data
     */
    inline const String& authdata()
	{ return m_authdata; }

    /**
     * Process a frame from remote peer
     * This method is thread safe.
     * @param frame IAX frame belonging to this transaction to process
     * @return 'this' if successful or NULL if the frame is invalid
     */
    IAXTransaction* processFrame(IAXFrame* frame);

    /**
     * Process received mini frame data
     * @param data Received data
     * @param tStamp Mini frame timestamp
     * @param voice True if received mini frame inside a Voice full frame
     * @return 0
     */
    IAXTransaction* processMedia(DataBlock& data, u_int32_t tStamp, bool voice = false);

    /**
     * Send media data to remote peer. Update the outgoing media format if changed
     * @param data Data to send
     * @param format Data format
     * @return 'this' if successful or 0
     */
    IAXTransaction* sendMedia(const DataBlock& data, u_int32_t format);

    /**
     * Get an IAX event from the queue
     * This method is thread safe.
     * @param time The time this method was called
     * @return Pointer to an IAXEvent or 0 if none available
     */
    IAXEvent* getEvent(u_int64_t time);

    /**
     * Get the maximum allowed number of full frames in the incoming frame list
     * @return The maximum allowed number of full frames in the incoming frame list
     */
    static unsigned char getMaxFrameList();

    /**
     * Set the maximum allowed number of full frames in the incoming frame list
     * @param value The new value of m_maxInFrames
     * @return False if value is greater then IAX2_MAX_TRANSINFRAMELIST
     */
    static bool setMaxFrameList(unsigned char value);

    /**
     * Send an ANSWER frame to remote peer
     * This method is thread safe
     * @return False if the current transaction state is not Connected
     */
    inline bool sendAnswer()
	{ return sendConnected(IAXFullFrame::Answer); }

    /**
     * Send a RINGING frame to remote peer
     * This method is thread safe
     * @return False if the current transaction state is not Connected
     */
    inline bool sendRinging()
	{ return sendConnected(IAXFullFrame::Ringing); }

    /**
     * Send an ACCEPT/REGACK frame to remote peer
     * This method is thread safe
     * @return False if the transaction type is not New and state is NewRemoteInvite or NewRemoteInvite_AuthRep or
     *  if the transaction type is not RegReq and state is NewRemoteInvite or
     *  type is not RegReq/RegRel and state is NewRemoteInvite_AuthRep
     */
    bool sendAccept();

    /**
     * Send a HANGUP frame to remote peer
     * This method is thread safe
     * @param cause Optional reason for hangup
     * @param code Optional code of reason
     * @return False if the transaction type is not New or state is Terminated/Terminating
     */
    bool sendHangup(const char* cause = 0, u_int8_t code = 0);

    /**
     * Send a REJECT/REGREJ frame to remote peer
     * This method is thread safe
     * @param cause Optional reason for reject
     * @param code Optional code of reason
     * @return False if the transaction type is not New/RegReq/RegRel or state is Terminated/Terminating
     */
    bool sendReject(const char* cause = 0, u_int8_t code = 0);

    /**
     * Send an AUTHREQ/REGAUTH frame to remote peer
     * This method is thread safe
     * @param pwd Required password
     * @return False if the current transaction state is not NewRemoteInvite
     */
    bool sendAuth(const String& pwd);

    /**
     * Send an AUTHREP/REGREQ/REGREL frame to remote peer as a response to AUTHREQ/REGREQ/REGREL
     * This method is thread safe
     * @return False if the current transaction state is not NewLocalInvite_AuthRecv
     */
    bool sendAuthReply();

    /**
     * Send a DTMF frame to remote peer
     * This method is thread safe
     * @param dtmf DTMF char to send
     * @return False if the current transaction state is not Connected or dtmf is grater then 127
     */
    inline bool sendDtmf(u_int8_t dtmf)
	{ return dtmf <= 127 ? sendConnected((IAXFullFrame::ControlType)dtmf,IAXFrame::DTMF) : false; }

    /**
     * Send a TEXT frame to remote peer
     * This method is thread safe
     * @param text Text to send
     * @return False if the current transaction state is not Connected
     */
    bool sendText(const char* text);

    /**
     * Send a NOISE frame to remote peer
     * This method is thread safe
     * @param noise Noise value to send
     * @return False if the current transaction state is not Connected or noise is grater then 127
     */
    inline bool sendNoise(u_int8_t noise)
	{ return noise <= 127 ? sendConnected((IAXFullFrame::ControlType)noise,IAXFrame::Noise) : false; }

    /**
     * Abort a registration transaction
     * This method is thread safe
     * @return False transaction is not a registration one or is already terminating
     */
    bool abortReg();

    /**
     * Enable trunking for this transaction
     * @param trunkFrame Pointer to IAXMetaTrunkFrame used to send trunked media
     * @return False trunking is already enabled for this transactio or trunkFrame is 0
     */
    bool enableTrunking(IAXMetaTrunkFrame* trunkFrame);

    /**
     * Print transaction data on stdin
     */
    void print();

    /**
     * Standard message sent if unsupported/unknown/none authentication methosd was received
     */
    static String s_iax_modNoAuthMethod;

    /**
     * Standard message sent if unsupported/unknown/none media format was received
     */
    static String s_iax_modNoMediaFormat;

    /**
     * Standard message sent if the received authentication data is incorrect
     */
    static String s_iax_modInvalidAuth;

protected:
    /**
     * Constructor: constructs an incoming transaction from a received full frame with an IAX
     *  control message that needs a new transaction
     * @param engine The engine that owns this transaction
     * @param frame A valid full frame
     * @param lcallno Local call number
     * @param addr Address from where the frame was received
     * @param data Pointer to arbitrary user data
     */
    IAXTransaction(IAXEngine* engine, IAXFullFrame* frame, u_int16_t lcallno, const SocketAddr& addr,
	void* data = 0);

    /**
     * Constructor: constructs an outgoing transaction with an IAX control message that needs a new transaction
     * @param engine The engine that owns this transaction
     * @param type Transaction type: see Type enumeration
     * @param lcallno Local call number
     * @param addr Address to use
     * @param ieList Starting IE list
     * @param data Pointer to arbitrary user data
     */
    IAXTransaction(IAXEngine* engine, Type type, u_int16_t lcallno, const SocketAddr& addr, IAXIEList& ieList,
	void* data = 0);

    /**
     * Init data members from an IE list
     * @param ieList IE list to init from
     */
    void init(IAXIEList& ieList);

    /**
     * Increment sequence numbers (inbound or outbound) for the frames that need it
     * @param frame Received frame if inbound is true, otherwise a transmitted one
     * @param inbound True for inbound frames
     * @return True if incremented.
     */
    bool incrementSeqNo(const IAXFullFrame* frame, bool inbound);

    /**
     * Test if frame is acceptable (not an out of order or a late one)
     * @param frame Frame to test
     * @return True if frame can be added to incoming frame list
     */
    bool isFrameAcceptable(const IAXFullFrame* frame);

    /**
     * Change the transaction state
     * @param newState the new transaction state
     * @return False if trying to change a termination state into a non termination one
     */
    bool changeState(State newState);

    /**
     * Terminate the transaction.
     * @param evType IAXEvent type to generate
     * @param local If true it is a locally generated event
     * @param frame Frame to build event from
     * @param createIEList If true create IE list in the generated event
     * @return Pointer to a valid IAXEvent
     */
    IAXEvent* terminate(u_int8_t evType, bool local, const IAXFullFrame* frame = 0, bool createIEList = true);

    /**
     * Wait for ACK to terminate the transaction. No more events will be generated
     * @param evType IAXEvent type to generate
     * @param local If true it is a locally generated event
     * @param frame Frame to build event from
     * @return Pointer to a valid IAXEvent
     */
    IAXEvent* waitForTerminate(u_int8_t evType, bool local, const IAXFullFrame* frame);

    /**
     * Constructs an IAXFrameOut frame, send it to remote peer and put it in the transmission list
     * This method is thread safe
     * @param type Frame type
     * @param subclass Frame subclass
     * @param data Frame IE list
     * @param len Frame IE list length
     * @param tStamp Frame timestamp. If 0 the transaction timestamp will be used
     * @param ackOnly Frame's acknoledge only flag
     */
    void postFrame(IAXFrame::Type type, u_int32_t subclass, void* data = 0, u_int16_t len = 0, u_int32_t tStamp = 0,
		bool ackOnly = false);

    /**
     * Send a full frame to remote peer
     * @param frame Frame to send
     * @param vnak If true the transmission is a response to a VNAK frame
     * @return True on success
     */
    bool sendFrame(IAXFrameOut* frame, bool vnak = false);

    /**
     * Create an event
     * @param evType Event type
     * @param local If true it is a locally generated event.
     * @param frameOut Frame to create from
     * @param newState The transaction new state
     * @return Pointer to an IAXEvent or 0 (invalid IE list)
     */
    IAXEvent* createEvent(u_int8_t evType, bool local, const  IAXFullFrame* frame, State newState);

    /**
     * Create an event from a received frame that is a response to a sent frame and
     *  change the transaction state to newState. Remove the response from incoming list.
     * @param frame Frame to create response for
     * @param findType Frame type to find
     * @param findSubclass Frame subclass to find
     * @param evType Event type to generate
     * @param local Local flag for the generated event.
     * @param newState New transaction state if an event was generated
     * @return Pointer to an IAXEvent or 0 (invalid IE list)
     */
    IAXEvent* createResponse(IAXFrameOut* frame, u_int8_t findType, u_int8_t findSubclass, u_int8_t evType, bool local, State newState);

    /**
     * Find a response for a previously sent frame
     * @param frame Frame to find response for
     * @param delFrame Delete frame flag. If true on exit, a response was found
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* getEventResponse(IAXFrameOut* frame, bool& delFrame);

    /**
     * Find a response for a previously sent frame if the transaction type is New
     * @param frame Frame to find response for
     * @param delFrame Delete frame flag. If true on exit, a response was found
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* IAXTransaction::getEventResponse_New(IAXFrameOut* frame, bool& delFrame);

    /**
     * Process an authentication request. If valid, send an authentication reply
     * @param event Already generated event
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* IAXTransaction::processAuthReq(IAXEvent* event);

    /**
     * Process an authentication reply
     * @param event Already generated event
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* IAXTransaction::processAuthRep(IAXEvent* event);

    /**
     * Find a response for a previously sent frame if the transaction type is RegReq/RegRel
     * @param frame Frame to find response for
     * @param delFrame Delete frame flag. If true on exit, a response was found
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* IAXTransaction::getEventResponse_Reg(IAXFrameOut* frame, bool& delFrame);

    /**
     * Update transaction data from the event
     * @param event Already generated event
     * @return The received event
     */
    IAXEvent* IAXTransaction::processRegAck(IAXEvent* event);

    /**
     * Find out if an incoming frame would start a transaction
     * @param frame Frame to process
     * @param delFrame Delete frame flag. If true on exit, frame is valid
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* getEventStartTrans(IAXFullFrame* frame, bool& delFrame);

    /**
     * Find out if a frame is a remote request
     * @param frame Frame to process
     * @param delFrame Delete rame flag. If true on exit, a request was found
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* getEventRequest(IAXFullFrame* frame, bool& delFrame);

    /**
     * Find out if a frame is a remote request if transaction type is New
     * @param frame Frame to process
     * @param delFrame Delete rame flag. If true on exit, a request was found
     * @return Pointer to an IAXEvent or 0
     */
    IAXEvent* getEventRequest_New(IAXFullFrame* frame, bool& delFrame);

    /**
     * Search for a frame in m_inFrames having the given type and subclass
     * @param type Frame type to find.
     * @param subclass Frame subclass to find.
     * @return Pointer to frame if found or 0.
     */
    IAXFullFrame* findInFrame(IAXFrame::Type type, u_int32_t subclass);

    /**
     * Search in m_inFrames for a frame with the same timestamp as frameOut and deletes it.
     * @param frameOut Frame to find response for
     * @param type Frame type to find
     * @param subclass Frame subclass to find
     * @return True if found.
     */
    bool findInFrameTimestamp(const IAXFullFrame* frameOut, IAXFrame::Type type, u_int32_t subclass);

    /**
     * Search in m_inFrames for an ACK frame which confirm the received frame and deletes it
     * @param frameOut Frame to find response for
     * @return True if found.
     */
    bool findInFrameAck(const IAXFullFrame* frameOut);

    /**
     * Acknoledge the last received full frame
     */
    void ackInFrames();

    /**
     * Send a frame to remote peer in state Connected
     * This method is thread safe
     * @param subclass Frame subclass to send
     * @param subclass Frame type to send
     * @return False if the current transaction state is not Connected
     */
    bool sendConnected(IAXFullFrame::ControlType subclass, IAXFrame::Type frametype = IAXFrame::Control);

    /**
     * Send an ACK frame
     * @param frame Aknoledged frame
     */
    void sendAck(const IAXFullFrame* frame);

    /**
     * Send an INVAL frame
     */
    void sendInval();

    /**
     * Send an VNAK frame
     */
    void sendVNAK();

    /**
     * Internal protocol outgoing frames processing (PING/LAGRQ)
     * @param frame Frame to process
     * @param delFrame Delete frame flag. If true on exit, a response was found
     * @return 0.
     */
    IAXEvent* processInternalOutgoingRequest(IAXFrameOut* frame, bool& delFrame);

    /**
     * Internal protocol incoming frames processing (PING/LAGRQ)
     * @param frame Frame to process
     * @param delFrame Delete frame flag. If true on exit, a request was found
     * @return 0.
     */
    IAXEvent* processInternalIncomingRequest(const IAXFullFrame* frame, bool& delFrame);

    /**
     * Process mid call control frames
     * @param frame Frame to process
     * @param delFrame Delete frame flag. If true on exit, a request was found
     * @return A valid IAXEvent or 0
     */
    IAXEvent* processMidCallControl(const IAXFullFrame* frame, bool& delFrame);

    /**
     * Process mid call IAX control frames
     * @param frame Frame to process
     * @param delFrame Delete frame flag. If true on exit, a request was found
     * @return A valid IAXEvent or 0
     */
    IAXEvent* processMidCallIAXControl(const IAXFullFrame* frame, bool& delFrame);

    /**
     * Test if frame is a Reject/RegRej frame
     * @param frame Frame to process.
     * @param delFrame Delete frame flag. If true on exit, a request was found
     * @return A valid IAXEvent or 0.
     */
    IAXEvent* remoteRejectCall(const IAXFullFrame* frame, bool& delFrame);

    /**
     * Terminate the transaction if state is Terminating on a remote request
     * @param time Current time
     * @return A valid IAXEvent or 0
     */
    IAXEvent* getEventTerminating(u_int64_t time);

    /**
     * Process received Voice frames
     * @param frame Received voice frame
     * @return 0
     */
    IAXTransaction* processVoiceFrame(const IAXFullFrame* frame);

    /**
     * Send all frames from outgoing queue with outbound sequence number starting with seqNo.
     * @param seqNo Requested sequence number
     * @return 0
     */
    IAXTransaction* IAXTransaction::retransmitOnVNAK(u_int16_t seqNo);

    /**
     * Generate an Accept event after internally accepting a transaction
     * @return A valid IAXEvent
     */
    IAXEvent* internalAccept();

    /**
     * Generate a Reject event after internally rejecting a transaction
     * @param reason The reason of rejecting
     * @return A valid IAXEvent
     */
    IAXEvent* internalReject(String& reason);

    /**
     * Event terminated feedback
     * This method is thread safe
     * @param event The event notifying termination
     */
    inline void eventTerminated(IAXEvent* event) {
	Lock lock(this);
	if (event == m_currentEvent)
	    m_currentEvent = 0;
    }

    /**
     * Set the current event
     * @param event The event notifying termination
     * @return event
     */
    inline IAXEvent* keepEvent(IAXEvent* event) {
	m_currentEvent = event;
	return event;
    }

private:
    // Params
    bool m_localInitTrans;			// True: local initiated transaction
    bool m_localReqEnd;				// Local client requested terminate
    Type m_type;				// Transaction type
    State m_state;				// Transaction state
    u_int64_t m_timeStamp;			// Transaction creation timestamp
    u_int32_t m_timeout;			// Transaction timeout (in seconds) on remote termination request
    SocketAddr m_addr;				// Socket
    u_int16_t m_lCallNo;			// Local peer call id
    u_int16_t m_rCallNo;			// Remote peer call id
    unsigned char m_oSeqNo;			// Outgoing frame sequence number
    unsigned char m_iSeqNo;			// Incoming frame sequence number
    IAXEngine* m_engine;			// Engine that owns this transaction
    void* m_userdata;				// Arbitrary user data
    u_int32_t m_lastFullFrameOut;		// Last transmitted full frame timestamp
    u_int16_t m_lastMiniFrameOut;		// Last transmitted mini frame timestamp
    u_int32_t m_lastMiniFrameIn;		// Last received mini frame timestamp
    u_int16_t m_lastAck;			// Last ack'd received frame's oseqno
    Mutex m_mutexInMedia;			// Keep received media thread safe
    IAXEvent* m_pendingEvent;			// Pointer to a pending event or 0
    IAXEvent* m_currentEvent;			// Pointer to last generated event or 0
    // Outgoing frames management
    ObjList m_outFrames;			// Transaction & protocol control outgoing frames
    u_int16_t m_retransCount;			// Retransmission counter. 0 --> Timeout
    u_int32_t m_retransInterval;		// Frame retransmission interval
    // Incoming frames management
    ObjList m_inFrames;				// Transaction & protocol control incoming frames
    static unsigned char m_maxInFrames;		// Max frames number allowed in m_inFrames
    // Call leg management
    u_int32_t m_pingInterval;			// Ping remote peer interval
    u_int64_t m_timeToNextPing;			// Time of the next Ping
    // Statistics
    u_int32_t m_inTotalFramesCount;		// Total received frames
    u_int32_t m_inOutOfOrderFrames;		// Total out of order frames
    u_int32_t m_inDroppedFrames;		// Total dropped frames
    // Data
    IAXAuthMethod::Type m_authmethod;		// Authentication method to use
    String m_username;				// Username
    String m_password;				//  Password
    String m_callingNo;				// Calling number
    String m_callingName;			// Calling name
    String m_calledNo;				// Called number
    String m_calledContext;			// Called context
    String m_challenge;				// Challenge
    String m_authdata;				// Auth data received with auth reply
    u_int32_t m_expire;				// Registration expiring time
    u_int32_t m_format;				// Media format used for initial negotiation
    u_int32_t m_formatIn;			// Incoming media format
    u_int32_t m_formatOut;			// Outgoing media format
    u_int32_t m_capability;			// Media capability of this transaction
    // Meta trunking
    IAXMetaTrunkFrame* m_trunkFrame;		// Reference to a trunk frame if trunking is enabled for this transaction
};

/**
 * This class holds an event generated by a transaction
 * @short Event class
 */
class YIAX_API IAXEvent
{
    friend class IAXTransaction;
    friend class IAXConnectionlessTransaction;
public:
    /**
     * Event type as enumeration
     */
    enum Type {
        Invalid = 0,		// Invalid frame received
	Terminated,		// Transaction terminated
        Timeout,		// Transaction timeout
	NotImplemented,		// Feature not implemented
	New,			// New remote transaction
	AuthReq,		// Auth request. Internally processed
	AuthRep,		// Auth reply
	Accept,			// Request accepted
	Hangup,			// Remote hangup
	Reject,			// Remote reject
	Busy,			// Call busy
	Text,			// Text frame received
	Dtmf,			// DTMF frame received
	Noise,			// Noise frame received
	Answer,			// Call answered
	Quelch,			// Quelch the call
	Unquelch,		// Unquelch the call
	Progressing,		// Call progressing
	Ringing,		// Ringing
    };

    /**
     * Destructor
     * Dereferences the transaction possibly causing its destruction
     */
    ~IAXEvent();

    /**
     * Get the type of this event
     * @return The type of the event as enumeratio
     */
    inline Type type() const
	{ return m_type; }

    /**
     * Check if this is a locally generated event
     * @return True if it is a locally generated event
     */
    inline bool local() const
        { return m_local; }

    /**
     * Check if this is a transaction finalization event
     * @return True if the transaction has finalized and will be destroyed
     */
    inline bool final() const
        { return m_final; }

    /**
     * Set the final flag.
     */
    inline void setFinal()
        { m_final = true; }

    /**
     * Get the type of the frame that generated the event
     * If 0 (internal event), the event consumer must delete the event
     * @return Frame type
     */
    inline u_int8_t frameType()
	{ return m_frameType; }

    /**
     * Get the subclass of the frame that generated the event
     * @return Frame subclass
     */
    inline u_int8_t subclass()
	{ return m_subClass; }

    /**
     * Get the IAX engine this event belongs to, if any
     * @return The IAX engine this event belongs to, if any
     */
    inline IAXEngine* getEngine() const
	{ return m_transaction ? m_transaction->getEngine() : 0; }

    /**
     * Get the IAX transaction that generated the event, if any
     * @return The IAX transaction that generated the event, if any
     */
    inline IAXTransaction* getTransaction() const
	{ return m_transaction; }

    /**
     * Get the opaque user data stored in the transaction
     * @return The opaque user data stored in the transaction
     */
    inline void* getUserData() const
	{ return m_transaction ? m_transaction->getUserData() : 0; }

    /**
     * Get the IE list
     * @return IE list reference
     */
    inline IAXIEList& getList()
	{ return m_ieList; }

protected:
    /**
     * Constructor
     * @param type Event type
     * @param local Local flag
     * @param final Final flag
     * @param transaction IAX transaction that generated the event
     * @param frameType The type of the frame that generated the event
     * @param subclass The subclass of the frame that generated the event
     */
    IAXEvent(Type type, bool local, bool final, IAXTransaction* transaction, u_int8_t frameType = 0, u_int8_t subclass = 0);

    /**
     * Constructor
     * @param type Event type
     * @param local Local flag
     * @param final Final flag
     * @param transaction IAX transaction that generated the event
     * @param frame The frame that generated the event
     */
    IAXEvent(Type type, bool local, bool final, IAXTransaction* transaction, const IAXFullFrame* frame = 0);

private:
    inline IAXEvent() {}		// Default constructor

    Type m_type;			// Event type
    u_int8_t m_frameType;		// Frame type
    u_int8_t m_subClass;		// Frame subclass
    bool m_local;			// If true the event is generated locally, the receiver MUST not respond
    bool m_final;			// Final event flag
    IAXTransaction* m_transaction;	// Transaction that generated this event
    IAXIEList m_ieList;			// IAXInfoElement list
};

/**
 * This class holds all information needded to manipulate all IAX transactions and events
 * @short IAX engine class
 */
class YIAX_API IAXEngine : public DebugEnabler, public Mutex
{
public:
    /**
     * Constructor
     * @param port UDP port to run the protocol on
     * @param transListCount Number of entries in the transaction hash table
     * @param retransCount Retransmission counter for each transaction belonging to this engine
     * @param retransInterval Retransmission interval default value in miliseconds
     * @param authTimeout Timeout (in seconds) of acknoledged auth frames sent
     * @param transTimeout Timeout (in seconds) on remote request of transactions belonging to this engine
     * @param maxFullFrameDataLen Max full frame IE list (buffer) length
     * @param format Default media format
     * @param capab Media capabilities of this engine
     * @param trunkSendInterval Send trunk meta frame interval
     */
    IAXEngine(int port, u_int16_t transListCount, u_int16_t retransCount, u_int16_t retransInterval,
	u_int16_t authTimeout, u_int16_t transTimeout, u_int16_t maxFullFrameDataLen,
	u_int32_t format, u_int32_t capab, u_int32_t trunkSendInterval);

    /**
     * Destructor
     * Closes all transactions belonging to this engine and flush all queues
     */
    virtual ~IAXEngine();

    /**
     * Add a parsed frame to the transaction list
     * @param addr Address from which the frame was received
     * @param frame A parsed IAX frame
     * @return Pointer to the transaction or 0 to deref the frame
     */
    IAXTransaction* addFrame(const SocketAddr& addr, IAXFrame* frame);

    /**
     * Add a raw frame to the transaction list
     * @param addr Address from which the message was received
     * @param buf Pointer to the start of the buffer holding the IAX frame
     * @param len Length of the message buffer
     * @return Pointer to the transaction or 0
     */
    IAXTransaction* addFrame(const SocketAddr& addr, const unsigned char* buf, unsigned int len);

    /**
     * Process media from remote peer. Descendents must override this method
     * @param transaction IAXTransaction that owns the call leg
     * @param data Media data
     * @param tStamp Media timestamp
     */
    virtual void processMedia(IAXTransaction* transaction, DataBlock& data, u_int32_t tStamp)
	{}

    /**
     * Event processor method. Keeps calling getEvent() and passing
     *  any events to processEvent() until there are no more events
     * @return True if at least one event was processed
     */
    bool process();

    /**
     * Get default frame retransmission counter
     * @return Frame retransmission counter
     */
    inline u_int16_t retransCount() const
        { return m_retransCount; }

    /**
     * Get default frame retransmission starting interval
     * @return Frame retransmission starting interval
     */
    inline u_int16_t retransInterval() const
        { return m_retransInterval; }

    /**
     * Get the timeout (in seconds) of acknoledged auth frames sent
     * @return Auth timeout in seconds
     */
    inline u_int16_t authTimeout() const
        { return m_authTimeout; }

    /**
     * Get the timeout (in seconds) of transactions belonging to this engine
     * @return Timeout (in seconds) of transactions belonging to this engine
     */
    inline u_int32_t transactionTimeout() const
        { return m_transTimeout; }

    /**
     * Get the maximum allowed frame length
     * @return The maximum allowed frame length
     */
    inline u_int16_t maxFullFrameDataLen() const
        { return m_maxFullFrameDataLen; }

    /**
     * Get the default media format
     * @return The default media format
     */
    inline u_int32_t format() const
        { return m_format; }

    /**
     * Get the media capability of this engine
     * @return The media capability of this engine
     */
    inline u_int32_t capability() const
        { return m_capability; }

    /**
     * Read data from socket
     * @param addr Socket to read from
     */
    void readSocket(SocketAddr& addr);

    /**
     * Write data to socket.
     * @param buf Data to write
     * @param len Data length
     * @param addr Socket to write to
     * @return True on success
     */
    bool writeSocket(const void* buf, int len, const SocketAddr& addr);

    /**
     * Read events
     */
    void runGetEvents();

    /**
     * Removes a transaction from queue. Free the allocated local call number
     *  Does not delete it
     * @param transaction Transaction to remove
     */
    void removeTransaction(IAXTransaction* transaction);

    /**
     * Return the transactions count
     * This method is thread safe
     * @return Transactions count
     */
    u_int32_t transactionCount();

    /**
     * Send an INVAL with call numbers set to 0 to a remote peer to keep it alive
     * @param addr Address to send to
     */
    void keepAlive(SocketAddr& addr);

    /**
     * Process a new format received with a Voice frame
     * @param trans Transaction that received the new format
     * @param format The received format
     * @return True if accepted
     */
    virtual bool voiceFormatChanged(IAXTransaction* trans, u_int32_t format)
	{ return false; }

    /**
     * Process the initial received format and capability. If accepted on exit will set the transaction format and capability
     * @param trans Transaction that received the new format
     * @return True if accepted
     */
    bool acceptFormatAndCapability(IAXTransaction* trans);

    /**
     * Default event handler. event MUST NOT be deleted
     * @param event The event to handle
     */
    virtual void defaultEventHandler(IAXEvent* event);

    /**
     * Enable trunking for the given transaction. Allocate a trunk meta frame if needded
     * @param trans Transaction to enable trunking for
     */
    void enableTrunking(IAXTransaction* trans);

    /**
     * Remove a trunk meta frame from the queue and deref it
     * @param trunkFrame The trunk meta frame to remove
     */
    void removeTrunkFrame(IAXMetaTrunkFrame* metaFrame);

    /**
     * Keep calling processTrunkFrames to send trunked media data
     */
    void runProcessTrunkFrames();

    /**
     * Get the socket used for engine operation
     * @return Reference to the UDP socket
     */
    inline Socket& socket()
	{ return m_socket; }

    /**
     * Print engine data on stdin
     */
    void print();

    /**
     * Get the MD5 data from a challenge and a password
     * @param md5data Destination String
     * @param challenge Challenge source
     * @param password Password source
     */
    static void getMD5FromChallenge(String& md5data, const String& challenge, const String& password);

    /**
     * Test if a received response to an authentication request is correct
     * @param md5data Data to compare with
     * @param challenge Received challenge
     * @param password Password source
     */
    static bool isMD5ChallengeCorrect(const String& md5data, const String& challenge, const String& password);

protected:
    /**
     * Process all trunk meta frames in the queue
     * @param time Time of the call
     * @return True if at least one frame was sent
     */
    bool processTrunkFrames(u_int32_t time = Time::msecNow());

    /**
     * Default event for connection transactions handler. This method may be overriden to perform custom
     *  processing
     * This method is thread safe
     * @param event Event to process
     */
    virtual void processEvent(IAXEvent* event);

    /**
     * Get an IAX event from the queue.
     * This method is thread safe.
     * @param time Time of the call
     * @return Pointer to an IAXEvent or 0 if none is available
     */
    IAXEvent* getEvent(u_int64_t time);

    /**
     * Generate call number. Update used call numbers list
     * @return Call number or 0 if none available
     */
    u_int16_t generateCallNo();

    /**
     * Release a call number
     * @param lcallno Call number to release
     */
    void releaseCallNo(u_int16_t lcallno);

    /**
     * Start a transaction based on a local request
     * @param type Transaction type
     * @param addr Remote address to send the request
     * @param ieList First frame IE list
     * @param trunking Enable/disable trunking for this transaction
     * @return IAXTransaction pointer on success.
     */
    IAXTransaction* startLocalTransaction(IAXTransaction::Type type, const SocketAddr& addr, IAXIEList& ieList, bool trunking = false);

private:
    Socket m_socket;				// Socket
    ObjList** m_transList;			// Full transactions
    ObjList m_incompleteTransList;		// Incomplete transactions (no remote call number)
    bool m_lUsedCallNo[IAX2_MAX_CALLNO + 1];	// Used local call numnmbers flags
    int m_lastGetEvIndex;			// getEvent: keep last array entry
    // Parameters
    int m_maxFullFrameDataLen;			// Max full frame data (IE list) length
    u_int16_t m_startLocalCallNo;		// Start index of local call number allocation
    u_int16_t m_transListCount;			// m_transList count
    u_int16_t m_retransCount;			// Retransmission counter for each transaction belonging to this engine
    u_int16_t m_retransInterval;		// Retransmission interval default value in miliseconds
    u_int16_t m_authTimeout;			// Timeout (in seconds) of acknoledged auth frames sent
    u_int32_t m_transTimeout;			// Timeout (in seconds) on remote request of transactions belonging to this engine
    // Media
    u_int32_t m_format;				// The default media format
    u_int32_t m_capability;			// The media capability
    // Trunking
    Mutex m_mutexTrunk;				// Mutex for trunk operations
    ObjList m_trunkList;			// Trunk frames list
    u_int32_t m_trunkSendInterval;		// Trunk frame send interval
    // Statistics
    u_int64_t m_writeCommands;
    u_int64_t m_writeCommandsFail;
};

}

#endif /* __YATEIAX_H */

/* vi: set ts=8 sw=4 sts=4 noet: */
