/*
----------------------
BeDC License
----------------------

Copyright 2002, The BeDC team.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions, and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions, and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the BeDC team nor the names of its 
   contributors may be used to endorse or promote products derived from 
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
*/


#ifndef _DC_CONNECTION_H_
#define _DC_CONNECTION_H_

#include <Looper.h>
#include <String.h>
#include <Messenger.h>
#include <OS.h>

#include <string>
using std::string;

#include "ObjectList.h"

// Messages launched by DCConnection
enum
{
	DC_MSG_CON_CONNECTING = 'DMcc',		// Connect in progress
	DC_MSG_CON_CONNECTED = 'DMcC',		// Connected!
	DC_MSG_CON_CONNECT_ERROR = 'DMCe',	// Connection error
	DC_MSG_CON_DISCONNECTED = 'DMDc',	// Disconnected
	DC_MSG_CON_SEND_ERROR = 'DMsE',		// Error sending!??!
	DC_MSG_CON_RECV_ERROR = 'DMrE',		// Receive error?!?
	
	// Hub messages
	// -------------------------------------------------------
	// Got the hub name
	//	'name'		BString 	--> the name
	DC_MSG_CON_HUB_NAME = 'cHNM',
	// The nick sent to the server was invalid
	//	'nick'		BString		--> bad nick
	DC_MSG_CON_VALIDATE_DENIED = 'cHVD',
	// The server requested a password
	DC_MSG_CON_GET_PASS = 'chGP',
	// The specified password was bad, server closes the connection
	DC_MSG_CON_BAD_PASS = 'chBP',
	// A new user has connected
	//	'nick'		BString		--> user nick
	DC_MSG_CON_USER_CONNECTED = 'chUC',
	// We got a $MyINFO response
	//	'nick'		BString		--> User name
	//	'desc'		BString		--> Description
	//	'speed'		BString		--> Speed
	//	'email'		BString		--> email
	//	'size'		uint32		--> Shared size
	DC_MSG_CON_USER_INFO = 'chUI',
	// You are an op
	DC_MSG_CON_YOU_ARE_OP = 'chAO',
	// Got the list of users
	//	'list'		BString		--> Unparsed list of users
	DC_MSG_CON_GOT_NICK_LIST = 'chNL',
	// Got the list of ops
	//	'list'		BString		--> Unparsed list of ops
	DC_MSG_CON_GOT_OP_LIST = 'chOL',
	// Private message!
	//	'from'		BString		--> From user
	//	'text'		BString		--> body of message
	DC_MSG_CON_PRIV_MSG = 'chPM',
	// User asked to be connected to
	//	'nick'		BString		--> User nick
	//	'ip'		BString		--> User ip
	//	'port'		int32		--> User port
	DC_MSG_CON_CONNECT_TO_ME = 'chCM',
	// User from another hub asked to be connected to
	//	'nick'		BString		--> User nick
	//	'ip'		BString		--> User ip
	//	'port'		int32		--> User port
	//	'sip'		BString		--> User's server ip
	//	'sport'		int32		--> Users's server port
	DC_MSG_CON_MULTI_CONNECT_TO_ME = 'chMM',
	// User asked to have a $ConnectToMe command sent to them
	// because they are firewalled.
	//	'nick'		BString		--> User to connect to
	DC_MSG_CON_REV_CONNECT_TO_ME = 'chRC',
	// The server is redirecting us
	//	'ip'		BString		--> Address of new server
	DC_MSG_CON_FORCE_MOVE = 'chFM',
	// User has disconnected
	//	'nick'		BString		--> User's nick
	DC_MSG_CON_QUIT = 'chCQ',
	// Chat Message
	//	Can be:
	//	'nick'		BString		--> Sender's nick
	//	'text'		BString		--> The message
	//	Or:
	//	'chat'		BString		--> Just chat text... no nick
	DC_MSG_CON_CHAT_MSG = 'chCH'
};
	
class DCConnection : public BLooper
{
public:
	enum
	{
		ACTIVE = 0,
		PASSIVE
	};
						// If a host is specified, it will automatically begin connecting
						DCConnection(BMessenger target, const BString & host = "", int port = 411);
	virtual				~DCConnection();
	
	void				Connect(const BString & host, int port = 411);
	void				Disconnect();
	bool				IsConnected() { return fConnected; }
	
	void				SetNick(const BString & nick);
	void				SetDescription(const BString & desc) { fDesc = desc; }
	void				SetEmail(const BString & email) { fEmail = email; }
	void				SetSpeed(const BString & speed) { fSpeed = speed; }
	void				SetSharedSize(uint32 size) { fSharedSize = size; }
	void				SetType(int type) { fType = type; }
	
	BString				GetNick() const { return fNick; }
	BString				GetDescription() const { return fDesc; }
	BString				GetEmail() const { return fEmail; }
	BString				GetSpeed() const { return fSpeed; }
	uint32				GetSharedSize() const { return fSharedSize; }
	int					GetType() const { return fType; }
	
	void				SendRawData(const BString & data);
	void				SendData(const BString & data);	// converts to Windows encoding
	
	virtual void		MessageReceived(BMessage * msg);
	
	static string		GenerateKey(BString & lock);

	// Protocol implementation
	void				SendNickListRequest();
	// If 'version' is "", then the default is used.
	void				SendVersion(const BString & version = "");
	// Send our info the the server
	void				SendMyInfo();
	// Send the server a password upon request
	void				SendPassword(const BString & passwd);
	// Ask the server for info about a user
	void				GetUserInfo(const BString & userNick);
	// Ask the remote user to send me a $ConnectToMe message because
	// I'm firewalled
	void				SendConnectRequest(const BString & userNick);
	// TODO: Launch search ($Search/$MultiSearch)
	
private:
	bool				fConnected;
	// My info
	BString				fNick;
	BString				fDesc;
	BString				fEmail;
	BString				fSpeed;
	uint32				fSharedSize;
	int					fType;
	
	typedef BObjectList<BString> StrList;
	
	StrList				fToSend;
	sem_id				fLocker;
	
	int					fSocket;
	BMessenger			fTarget;
	thread_id			fThreadID;
	// For connecting
	BString				fHost;
	int					fPort;
	
	void				InternalConnect();
	
	void				LockReceived(BString str);
	void				HelloReceived(BString str);
	void				ToReceived(BString str);
	void				ConnectToMeReceived(BString str);
	void				MultiConnectToMeReceived(BString str);
	void				MyInfoReceived(BString str);
	
	void				ValidateNick();
	// Name CAN be NULL
	void				SendMessage(uint32 cmd, const char * name = NULL, const BString & str = "");
	
	bool				LockList();
	void				UnlockList();
	void				EmptyList();
	
	status_t			SetNonBlocking(bool val)
	{
		int flags = fcntl(fSocket, F_GETFL);
		if (val)
			flags |= O_NONBLOCK;
		else
			flags &= ~O_NONBLOCK;
		if (fcntl(fSocket, F_SETFL, flags) < 0)
			return B_ERROR;
		
		return B_OK;
	}
	
	static int32		ReceiveHandler(void * data);
	// Helper for GenerateKey()
	static string		SubKey(const uint8 * key, int32 length, int extra);
	
	// If these return 0, the it disconnected
	// If it returns < 0, than an error occured
	// Otherwise, it returns the amount read/written
	int					Sender();
	int					Reader(BString & ret);
};


#endif /* !_DC_CONNECTION_H_ */
