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
#include <errno.h>

// TEST
#ifdef NETSERVER_BUILD
#include <net/socket.h>
#else
#include <sys/select.h>
#endif

#include "DCConnection.h"
#include "DCNetSetup.h"
#include "DCStrings.h"

// Internal messages
enum
{
	DCC_CONNECT = 'dCCc',	// Connect
	DCC_SEND = 'dCCs'		// Send some data
};

DCConnection::DCConnection(BMessenger target, const BString & host, int port)
	: BLooper("dc_connection", B_NORMAL_PRIORITY)
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
	
	fLocker = create_sem(1, "dcc_sem");
	if (fLocker < B_OK)
		debugger("Couldn't create semaphore!");
	if (host != "")
		Connect(host, port);
}

DCConnection::~DCConnection()
{
	Disconnect();
	delete_sem(fLocker);
}

bool
DCConnection::LockList()
{
	return acquire_sem(fLocker) == B_OK ? true : false;
}

void
DCConnection::UnlockList()
{
	release_sem(fLocker);
}

void
DCConnection::Disconnect()
{
	fConnected = false;
	if (fThreadID >= 0)
	{
		kill_thread(fThreadID);
		fThreadID = -1;
	}
	if (fSocket >= 0)
	{
		CLOSE_SOCKET(fSocket);
		fSocket = -1;
	}
	// Clean up our list
	if (LockList())
	{
		EmptyList();
		UnlockList();
	}
}

void
DCConnection::Connect(const BString & host, int port)
{
	if (!fConnected)
	{
		printf("Launching DCC_CONNECT\n");
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
			printf("Got DCC_SEND\n");
			BString str;
			if (msg->FindString("data", &str) == B_OK)
			{
/*				if (send(fSocket, str.String(), str.Length(), 0) < 0)
					fTarget.SendMessage(DC_MSG_CON_SEND_ERROR);
				else
					printf("Sent\n");*/
				if (LockList())
				{
					printf("Acquired sem\n");
					BString * strp = new BString(str);
					fToSend.AddItem(strp, fToSend.CountItems());
					printf("Unlocking\n");
					UnlockList();
				}
			}
			break;
		}
		
		case DCC_CONNECT:
		{
			printf("DCC: Got connect message\n");
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
	printf("Internalconnect()\n");
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
		fSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fSocket >= 0)
		{
			SetNonBlocking(false);
			if (connect(fSocket, (sockaddr *)&sockAddr, sizeof(sockAddr)) >= 0)
			{
				fThreadID = spawn_thread(DCConnection::ReceiveHandler, "dcc_receiver",
										 B_NORMAL_PRIORITY, this);
				if (fThreadID >= 0)
				{
					if (SetNonBlocking(false) == B_ERROR)
						debugger("Could not set non-blocking socket IO\n");
					resume_thread(fThreadID);
					printf("DCC: Connected!\n");
					fTarget.SendMessage(DC_MSG_CON_CONNECTED);
					fConnected = true;
					return;
				}
			}
		}
	}
	
	printf("DCC: Error connecting!\n");
	// Failure
	fTarget.SendMessage(DC_MSG_CON_CONNECT_ERROR);
}

int32
DCConnection::ReceiveHandler(void * d)
{
	DCConnection * me = (DCConnection *)d;
	BString str1 = "", str2 = "";
	
	while (1)
	{
		int ret = me->Sender();
		if (ret == 0)
		{
			me->SendMessage(DC_MSG_CON_DISCONNECTED);
			me->Disconnect();
			return -1;
		}
		else if (ret < 0)	// failed
		{
			me->SendMessage(DC_MSG_CON_SEND_ERROR);
			me->Disconnect();
			return -1;
		}
		
		str2 = "";
		ret = me->Reader(str2);
		if (ret == 0)	// disconenct
		{
			printf("Got disconnect from Reader()\n");
			me->SendMessage(DC_MSG_CON_DISCONNECTED);
			me->Disconnect();
			return -1;
		}
		else if (ret < 0)	// error
		{
			me->SendMessage(DC_MSG_CON_RECV_ERROR);
			me->Disconnect();
			return -1;
		}
		
		if (str2 != "")	// if str2 == "", then recv() was going to block and we couldn't read anything
			str1.Append(str2);
		
		// got a command, figure out what it is
		int i = 0;
		while ((i = str1.FindFirst("|")) != B_ERROR)
		{
			str2 = "";
			str1.MoveInto(str2, 0, i);
			str1.Remove(0, 1);;
			
			printf("DCC: Got command [ %s ]\n", str2.String());
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
				me->MyInfoReceived(str2);
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
				BString name;
				
				// The message is in the form: <username> text
				str2.RemoveFirst("<");
				str2.MoveInto(name, 0, str2.FindFirst(">"));
				str2.RemoveFirst("> ");
				
				BMessage msg(DC_MSG_CON_CHAT_MSG);
				msg.AddString("nick", name);
				printf("Str2 == %s+++\n", str2.String());
				msg.AddString("text", str2);
				me->fTarget.SendMessage(&msg);
			}
			else	// This is just chat text w/out a name
			{
				BMessage msg(DC_MSG_CON_CHAT_MSG);
				msg.AddString("chat", str2);
				me->fTarget.SendMessage(&msg);
			}
		}
	}
}

int
DCConnection::Sender()
{
	// Lock our list
	if (!LockList())
		return -2;	// Bah! couldn't get the lock
	if (fToSend.CountItems() == 0)
	{
		// Nothing to send
		UnlockList();
		return 1;
	}
	BString * str = fToSend.ItemAt(0);	// first Item
	int i = 0;
	int totalSent = 0;
	while (true)
	{
		i = send(fSocket, str->String(), str->Length(), 0);
		printf("Sent: %d\n", i);
		if (i <= 0)
		{
			UnlockList();
			if (errno == EWOULDBLOCK)
			{
				return totalSent == 0 ? 1 : totalSent;	// don't send a disconnect if we weren't able to send anything
			}
			return i == 0 ? 0 : -1;	// return -1 on error, 0 on disconnet
		}
		totalSent += i;
		if (i < str->Length())
		{
			str->Remove(0, i);
		}
		else
		{
			fToSend.RemoveItemAt(0);
			delete str;
			if (fToSend.CountItems() == 0)
			{
				UnlockList();
				SetNonBlocking(true);
				return totalSent;
			}
			str = fToSend.ItemAt(0);
		}
	}
}

int
DCConnection::Reader(BString & ret)
{
	const int32 BUFFER_SIZE = 512;
	char recvBuffer[BUFFER_SIZE + 1];
	BString converted = "";
	int r = 0;
	int totalRead = 0;
	
	SetNonBlocking(true);
	while (true)
	{
		r = recv(fSocket, recvBuffer, BUFFER_SIZE, 0);
		if (r <= 0)
		{
			SetNonBlocking(false);
			if (errno == EWOULDBLOCK)
			{
				errno = 0;
				ret = converted;
				return totalRead == 0 ? 1 : totalRead;
			}
			else if (errno == ENOTCONN)
			{
				printf("\t\tReader: Not connected anymore\n");
			}
			return r == 0 ? 0 : -1;	// return -1 for error, and 0 for disconnect
		}
		
		totalRead += r;
		recvBuffer[r] = 0;	// null terminate
		converted.Append(DCUTF8(recvBuffer));
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
	printf("Got hello\n");
	str.RemoveFirst("$Hello ");
	if (str.Compare(fNick) == 0)	// it's us 
	{
		printf("It's just me :)\n");
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
	printf("\t\tGOT $Lock\n");
	str.RemoveFirst("$Lock ");
	int32 i = str.FindFirst(" ");
	str.Remove(i, str.Length() - i);
	printf("Str is now: %s [%d]\n", str.String(), (int)str.Length());
	string key = DCConnection::GenerateKey(str);

	printf("Length: %d [%s ]\n", (int)key.size(), key.c_str());
	
	// Dump the key for debugging
	FILE * fp = fopen("key.txt", "w");
	if (fp)
	{
		fprintf(fp, "Lock size: %d\n", (int)str.Length());
		for (int j = 0; j < str.Length(); j++)
		{
			fprintf(fp, "%d, ", (unsigned char)str[j]);
			if (j % 10 == 0 && j != 0)
				fprintf(fp, "\n");
		}
		
		fprintf(fp, "\n\nKey Size: %d\n", (int)key.size());
		for (int j = 0; j < key.size(); j++)
		{
			fprintf(fp, "%d, ", (unsigned char)key.c_str()[j]);
			if (j % 10 == 0 && j != 0)
				fprintf(fp, "\n");
		}
		fclose(fp);
	}
	SetNonBlocking(false);
	send(fSocket, "$Key ", 5, 0);
	printf("KeySend %ld\n", send(fSocket, key.c_str(), key.size(), 0));
	send(fSocket, "|", 1, 0);
	SetNonBlocking(true);
	
	
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

#define EXTRA(X) (X == 0) || (X == 5) || (X == 124) || (X == 126) || (X == 36) || (X == 96)

string
DCConnection::GenerateKey(BString & lck)
{
	printf("Using lock: [ %s ]\n", lck.String());
	uint8 * tmp = new uint8[lck.Length() + 1];
	uint8 vl;
	int extra = 0;
	
	vl = (uint8)(lck[0] ^ 5);
	vl = (uint8)(((vl >> 4) | (vl << 4)) && 0xFF);
	tmp[0] = vl;
	 
	int32 i;
	for (i = 1; i < lck.Length(); i++)
	{
		vl = (uint8)(lck[i] ^ lck[i - 1]);
		vl = (uint8)(((vl >> 4) | (vl << 4)) & 0xFF);
		tmp[i] = vl;
		if (EXTRA(tmp[i]))
			extra++;
	}
	
	tmp[0] = (uint8)(tmp[0] ^ tmp[lck.Length() - 1]);
	if (EXTRA(tmp[0]))
		extra++;
	
	string key = SubKey(tmp, lck.Length(), extra);
	delete [] tmp;

//	key.Prepend("$Key ");
//	key.Append("|");
	return key;	
}

string
DCConnection::SubKey(const uint8 * key, int32 length, int extra)
{
	uint8 * tmp = new uint8[length + extra * 10];
	int32 j = 0;
	int32 i = 0;
	
	for (; i < length; i++)
	{
		if (EXTRA(key[i]))
		{
			tmp[j++] = '/';
			tmp[j++] = '%';
			tmp[j++] = 'D';
			tmp[j++] = 'C';
			tmp[j++] = 'N';
			switch (key[i])
			{
				case 0:
				{
					tmp[j++] = '0'; tmp[j++] = '0'; tmp[j++] = '0'; 
					break;
				}
				
				case 5: 
				{
					tmp[j++] = '0'; tmp[j++] = '0'; tmp[j++] = '5'; 
					break;
				}
				
				case 36: 
				{
					tmp[j++] = '0'; tmp[j++] = '3'; tmp[j++] = '6'; 
					break;
				}
				
				case 96: 
				{
					tmp[j++] = '0'; tmp[j++] = '9'; tmp[j++] = '6'; 
					break;
				}
				
				case 124: 
				{
					tmp[j++] = '1'; tmp[j++] = '2'; tmp[j++] = '4'; 
					break;
				}
				
				case 126: 
				{
					tmp[j++] = '1'; tmp[j++] = '2'; tmp[j++] = '6'; 
					break;
				}
			}
			tmp[j++] = '%'; 
			tmp[j++] = '/';
		}
		else
		{
			tmp[j++] = key[i];
		}
	}
	
	string ret((const char *)tmp, j);
	delete [] tmp;
	return ret;
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
	v.Append((version == "") ? "2,1191" : version);
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

void
DCConnection::MyInfoReceived(BString str)
{
	BString nick, desc, speed, email, tmp;
	uint64 size;
	str.RemoveFirst("$MyINFO $ALL ");
	str.MoveInto(nick, 0, str.FindFirst(" "));
	str.RemoveFirst(" ");
	str.MoveInto(desc, 0, str.FindFirst("$"));
	str.RemoveFirst("$ $");
	str.MoveInto(speed, 0, str.FindFirst("$") - 1);	// -1 to remove the special char
	str.Remove(0, 2);
	str.MoveInto(email, 0, str.FindFirst("$"));
	str.RemoveFirst("$");
	str.MoveInto(tmp, 0, str.FindFirst("$"));
	size = (uint64)atoll(tmp.String());
	printf("Read size: %s\t%Ld\n", tmp.String(), size);
	
	BMessage msg(DC_MSG_CON_USER_INFO);
	msg.AddString("nick", nick);
	msg.AddString("desc", desc);
	msg.AddString("speed", speed);
	msg.AddString("email", email);
	msg.AddInt64("size", (int32)size);
	fTarget.SendMessage(&msg);
}

void
DCConnection::EmptyList()
{
	while (fToSend.CountItems() > 0)
	{
		BString * item = fToSend.RemoveItemAt(0);
		delete item;
	}
}
