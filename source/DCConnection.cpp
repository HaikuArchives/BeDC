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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "DCConnection.h"
#include "DCNetSetup.h"
#include "DCStrings.h"

// Internal messages
enum
{
	DCC_CONNECT = 'dCCc',	// Connect
	DCC_SEND = 'dCCs',		// Send some data
};

DCConnection::DCConnection(BMessenger target, const BString & host, int port)
	: BLooper("dcconnection", B_NORMAL_PRIORITY)
{
	fThreadID = -1;
	fConnected = false;
	fSocket = -1;
	fTarget = target;
	fNick = "";
	fType = ACTIVE;
	fSharedSize = 0;
	
	fSpeed = "";
	fEmail = "";
	fNick = "";
	fDesc = "";
	fHost = "";
	fPort = 0;
	
	Run();	// start the looper
	
	if (host != "")
		Connect(host, port);
}

DCConnection::~DCConnection()
{
	Disconnect();
}

void
DCConnection::Disconnect()
{
	if (fThreadID >= 0)
	{
		kill_thread(fThreadID);
		fThreadID = -1;
	}
	if (fSocket >= 0)
	{
#ifdef NETSERVER_BUILD
		closesocket(fSocket);
#else
		close(fSocket);
#endif
		fSocket = -1;
	}
	fConnected = false;
}

void
DCConnection::Connect(const BString & host, int port)
{
	if (!fConnected)
	{
		fHost = host;
		fPort = port;
		BMessage msg(DCC_CONNECT);
		PostMessage(&msg);
	}
}

void
DCConnection::SetNick(const BString & nick)
{
	fNick = nick;
	// TODO, send nick to server
	// Dunno if the server handles a nick change while connected...
	// I think it doesn't so you have to reconnect, but don't quite remember
}

void
DCConnection::SendRawData(const BString & data)
{
	if (fConnected)
	{
		BMessage msg(DCC_SEND);
		msg.AddString("data", data);
		PostMessage(&msg);
	}
}

void
DCConnection::SendData(const BString & data)
{
	if (fConnected)
	{
		BMessage msg(DCC_SEND);
		msg.AddString("data", DCMS(data.String()));
		PostMessage(&msg);
	}
}

void
DCConnection::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DCC_SEND:
		{
			BString str;
			if (msg->FindString("data", &str) == B_OK)
			{
				if (send(fSocket, str.String(), str.Length(), 0) < 0)
					fTarget.SendMessage(DC_MSG_CON_SEND_ERROR);
			}
			break;
		}
		
		case DCC_CONNECT:
		{
			InternalConnect();
			break;
		}
		
		default:
			BLooper::MessageReceived(msg);
			break;
	}
}

void
DCConnection::InternalConnect()
{
	// Notify our target
	fTarget.SendMessage(DC_MSG_CON_CONNECTING);
	
	sockaddr_in sockAddr;
	
	memset(&sockAddr, 0, sizeof(sockAddr));
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(fPort);
	hostent * hostAddr = gethostbyname(fHost.String());
	if (hostAddr)
	{
		sockAddr.sin_addr = *((in_addr *)(hostAddr->h_addr_list[0]));
		fSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (fSocket >= 0)
		{
			if (connect(fSocket, (sockaddr *)&sockAddr, sizeof(sockAddr)) >= 0)
			{
				fThreadID = spawn_thread(DCConnection::ReceiveHandler, "dcc_receiver",
										 B_NORMAL_PRIORITY, this);
				if (fThreadID >= 0)
				{
					resume_thread(fThreadID);
					fTarget.SendMessage(DC_MSG_CON_CONNECTED);
					fConnected = true;
					return;
				}
			}
		}
	}
	
	// Failure
	fTarget.SendMessage(DC_MSG_CON_CONNECT_ERROR);
}

int32
DCConnection::ReceiveHandler(void * d)
{
	DCConnection * me = (DCConnection *)d;
	const int32 BUFFER_SIZE = 512;
	char recvBuffer[BUFFER_SIZE + 1];
	BString converted = "";
	BString str1 = "", str2 = "";
	
	while (1)
	{
		int amountRead;
		amountRead = recv(me->fSocket, recvBuffer, BUFFER_SIZE, 0);
		if (amountRead < 0)
		{
#ifdef NETSERVER_BUILD
			if (amountRead != -1)	// on net_server, we only abort if it's not -1
			{
				me->fTarget.SendMessage(DC_MSG_CON_RECV_ERROR);
				return -1;
			}
#else
			me->fTarget.SendMessage(DC_MSG_CON_RECV_ERROR);
			return -1;
#endif
		}
		
		if (amountRead == 0)	// Disconnected
		{
			me->Disconnect();
			me->fTarget.SendMessage(DC_MSG_CON_DISCONNECTED);
			return -2;
		}
		
		// good :)
		recvBuffer[amountRead] = 0;
		str1 = DCUTF8(recvBuffer);
		while (str1[str1.Length() - 1] != '|')	// we haven't gotten the command yet
		{
			amountRead = recv(me->fSocket, recvBuffer, BUFFER_SIZE, 0);
			if (amountRead < 0)
			{
#ifdef NETSERVER_BUILD
				if (amountRead != -1)	// on net_server, we only abort if it's not -1
				{
					me->fTarget.SendMessage(DC_MSG_CON_RECV_ERROR);
					return -1;
				}
#else
				me->fTarget.SendMessage(DC_MSG_CON_RECV_ERROR);
				return -1;
#endif
			}
			
			if (amountRead == 0)	// Disconnected
			{
				me->Disconnect();
				me->fTarget.SendMessage(DC_MSG_CON_DISCONNECTED);
				return -2;
			}
			
			recvBuffer[amountRead] = 0;
			converted = DCUTF8(recvBuffer);
			str1.Append(converted);
		}
		
		// got a command, figure out what it is
		int i = 0;
		while ((i = str1.FindFirst("|")) != B_ERROR)
		{
			str2 = "";
			str1.MoveInto(str2, 0, i);
			str1.Remove(0, 1);;
			
			if (!str2.Compare("$Lock ", 6))
			{
				me->LockReceived(str2);
			}
			else if (!str2.Compare("$HubName ", 9))
			{
				str2.RemoveFirst("$HubName ");
				me->SendMessage(DC_MSG_CON_HUB_NAME, "name", str2);
			}
			else if (!str2.Compare("$ValidateDenide ", 16))
			{
				str2.RemoveFirst("$ValidateDenide ");
				me->SendMessage(DC_MSG_CON_VALIDATE_DENIED, "nick", str2);
			}
			else if (!str2.Compare("$GetPass", 8))	// Password request
			{
				me->SendMessage(DC_MSG_CON_GET_PASS);
			}
			else if (!str2.Compare("$BadPass", 8))
			{
				me->SendMessage(DC_MSG_CON_BAD_PASS);
			}
			else if (!str2.Compare("$Hello ", 7))
			{
				me->HelloReceived(str2);
			}
			else if (!str2.Compare("$LogedIn ", 9))
			{
				str2.RemoveFirst("$LogedIn ");
				me->SendMessage(DC_MSG_CON_YOU_ARE_OP);
			}
			else if (!str2.Compare("$MyINFO ", 8))
			{
				// TODO: Do we want to handle this?
				// Yup, It's how we get another clients info (Speed/Description/Share etc)
				// The Nicklist shows only the nick.
				// Also, if we send out "$GetINFO", this is the response we get
			}
			else if (!str2.Compare("$NickList ", 10))
			{
				str2.RemoveFirst("$NickList ");
				me->SendMessage(DC_MSG_CON_GOT_NICK_LIST, "list", str2);
			}
			else if (!str2.Compare("$OpList ", 8))
			{
				str2.RemoveFirst("$OpList ");
				me->SendMessage(DC_MSG_CON_GOT_OP_LIST, "list", str2);
			}
			else if (!str2.Compare("$To: ", 5))
			{
				me->ToReceived(str2);
			}
			else if (!str2.Compare("$ConnectToMe ", 13))
			{
				me->ConnectToMeReceived(str2);
			}
			else if (!str2.Compare("$MultiConnectToMe ", 19))
			{
				me->MultiConnectToMeReceived(str2);
			}
			else if (!str2.Compare("$RevConnectToMe ", 16))
			{
				str2.RemoveFirst("$RevConnectToMe ");
				str2.Remove(0, str2.FindFirst(" ") + 1);	// remove our name + space
				me->SendMessage(DC_MSG_CON_REV_CONNECT_TO_ME, "nick", str2);
			}
			else if (!str2.Compare("$Search ", 8))	// A query request
			{
				// TODO
			}
			else if (!str2.Compare("$MultiSearch ", 13))	// A query request (Searching on all linked hub)
			{
				// TODO
			}
			else if (!str2.Compare("$SR ", 4))	// Search results (Passive)
			{
				// TODO
			}
			else if (!str2.Compare("$ForceMove ", 11)) // They don't want us here :(
			{
				me->Disconnect();
				str2.RemoveFirst("$ForceMove ");
				me->SendMessage(DC_MSG_CON_DISCONNECTED);
				me->SendMessage(DC_MSG_CON_FORCE_MOVE, "ip", str2);
			}
			else if (!str2.Compare("$Quit ", 6))
			{
				str2.RemoveFirst("$Quit ");
				me->SendMessage(DC_MSG_CON_QUIT, "nick", str2);
			}
			else if(!str2.Compare("<",1)) // Chat message
			{
				// TODO, split Sender and message, and send a DC_MSG_CON_CHAT_MSG message
				// Sender is the text between '<' and '>'
			}
		}
	}
}

void
DCConnection::MultiConnectToMeReceived(BString str)
{
	BString nick, ip, sip, tmp;
	int32 port, sport;
	int32 i;
	
	str.RemoveFirst("$MultiConnectToMe ");
	
	i = str.FindFirst(" ");
	str.MoveInto(nick, 0, i);
	str.RemoveFirst(" ");
	
	i = str.FindFirst(":");
	str.MoveInto(ip, 0, i);
	str.RemoveFirst(":");
	
	i = str.FindFirst(" ");
	str.MoveInto(tmp, 0, i);
	str.RemoveFirst(" ");
	port = atoi(tmp.String());
	
	i = str.FindFirst(":");
	str.MoveInto(sip, 0, i);
	str.RemoveFirst(":");
	
	sport = atoi(str.String());
	
	
	BMessage msg(DC_MSG_CON_CONNECT_TO_ME);
	msg.AddString("nick", nick);
	msg.AddString("ip", ip);
	msg.AddInt32("port", port);
	msg.AddString("sip", sip);
	msg.AddInt32("sport", sport);
	
	fTarget.SendMessage(&msg);
}

void
DCConnection::ConnectToMeReceived(BString str)
{
	BString nick, ip;
	int32 port;
	int32 i;
	
	str.RemoveFirst("$ConnectToMe ");
	i = str.FindFirst(" ");
	str.MoveInto(nick, 0, i);
	str.RemoveFirst(" ");
	i = str.FindFirst(":");
	str.MoveInto(ip, 0, i);
	str.RemoveFirst(":");
	port = atoi(str.String());
	
	BMessage msg(DC_MSG_CON_CONNECT_TO_ME);
	msg.AddString("nick", nick);
	msg.AddString("ip", ip);
	msg.AddInt32("port", port);
	fTarget.SendMessage(&msg);
}

void
DCConnection::ToReceived(BString str)
{
	// Private message
	str.RemoveFirst("$To: ");
	// This should be sent to us only... but just in case (paranoid)
	if (!str.Compare(fNick, fNick.Length()))
	{
		str.RemoveFirst(fNick);
		str.RemoveFirst(" From: ");
		
		BString from;
		str.MoveInto(from, 0, str.FindFirst(" "));
		str.RemoveFirst(" $");
		str.RemoveFirst(from);
		str.RemoveFirst(" ");
		if (str != "")	// Check to see if we have a message
		{
			BMessage msg(DC_MSG_CON_PRIV_MSG);
			msg.AddString("from", from);
			msg.AddString("text", str);
			fTarget.SendMessage(&msg);
		}
	}
}

void
DCConnection::HelloReceived(BString str)
{
	// Someone just entered the hub
	// If it's us, identify ourselves ;)
	str.RemoveFirst("$Hello ");
	if (str.Compare(fNick) == 0)	// it's us 
	{
		SendVersion();
		SendNickListRequest();
		SendMyInfo();
	}
	else
	{
		SendMessage(DC_MSG_CON_USER_CONNECTED, "nick", str);
	}
}

void
DCConnection::LockReceived(BString str)
{
	str.RemoveFirst("$Lock ");
	int j = str.FindFirst(" ");
	// Remove everything after the first space
	str.Remove(j, str.Length() - j);
	
	BString key = DCConnection::GenerateKey(str);
	// Parse the key, and send it
	str = "$Key ";
	str += key;
	str += "|";
	SendRawData(str);
	
	// Also, send our nick too
	ValidateNick();
}

// Just a helper
void
DCConnection::SendMessage(uint32 cmd, const char * name, const BString & str)
{
	BMessage msg(cmd);
	if (name)
		msg.AddString(name, str);
	fTarget.SendMessage(&msg);
}

void
DCConnection::ValidateNick()
{
	BString str = "$ValidateNick ";
	str += fNick;
	str += "|";
	SendData(str);
}

BString
DCConnection::GenerateKey(BString & lck)
{
	BString key("", 2048);
	char c, v;
	
	for (int i = 0; i < 2048; i++)
	{
		if (i == 0)
			// If it's the first char we gotta XOR with the two last chars and 5
			c = lck[i] ^ lck[lck.Length() - 1] ^ lck[lck.Length() - 2] ^ 5;
		else
			c = lck[i] ^ lck[i - 1]; // XOR with previous char
		
		v = c << 4 | c >> 4; // Shift some bits around :)
		
		if (v == 0 || v == 5 || v == 36 || v == 96 || v == 124 || v == 126)	// escape
		{
			char buf[11];
			memset(buf, 0, sizeof(buf));
			sprintf(buf, "/%%DCN%03d%%/", v);
			key.Append(buf);
		}
		else
		{
			key.Append(v, 1);
		}
	}
	return key;
}

void
DCConnection::SendNickListRequest()
{
	SendRawData("$GetNickList|");
}

void
DCConnection::SendVersion(const BString & version)
{
	BString v("$Version ");
	v.Append((version == "") ? "1,0091" : version);
	v += "|";
	SendRawData(v);	
}

void
DCConnection::SendMyInfo()
{
	BString send = "$MyINFO $ALL ";
	send += fNick;
	send += " ";
	send += fDesc;
	send += "$ $";
	send += fSpeed;
	send.Append((char)1, 1);
	send += "$";
	send += fEmail;
	send += "$";
	send << fSharedSize;
	send += "$|";
	SendData(send);
}

void
DCConnection::SendPassword(const BString & passwd)
{
	BString send = "$MyPass ";
	send += passwd;
	send += "|";
	SendData(send);
}

void
DCConnection::GetUserInfo(const BString & userNick)
{
	BString send = "$GetINFO ";
	send += userNick;
	send += " ";
	send += fNick;
	send += "|";
	SendData(send);
}

void
DCConnection::SendConnectRequest(const BString & userNick)
{
	BString send = "$RevConnectToMe ";
	send += userNick;
	send += " ";
	send += fNick;
	send += "|";
	SendData(send);
}
