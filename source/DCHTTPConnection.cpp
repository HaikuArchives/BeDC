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
#include "DCHTTPConnection.h"
#include "DCNetSetup.h"
#include "DCStrings.h"
#include "DCStringTokenizer.h"

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

const int DC_HTTP_RECV_BUFFER = 512;

enum 
{
	DC_HTTP_MSG_SEND = 'dHmS',
	DC_HTTP_MSG_CONNECT = 'dHmC'
}; 


DCHTTPConnection::DCHTTPConnection(BMessenger target)
	: BLooper("DCHTTPConnection", B_DISPLAY_PRIORITY)
{
	fServer = "";
	fFile = "";
	fThreadID = -1;
	fSocket = -1;
	fTarget = target;
}

DCHTTPConnection::~DCHTTPConnection()
{
	Disconnect();
	EmptyHubList(&fHubs);
}

void
DCHTTPConnection::Disconnect()
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
}

void
DCHTTPConnection::Connect(const BString & optServer, const BString & optFile)
{
	fServer = optServer;
	fFile = optFile;
	BMessage msg(DC_HTTP_MSG_CONNECT);
	PostMessage(&msg);
}

void
DCHTTPConnection::InternalConnect()
{
	
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(80);
	
	// Get address
	hostent * hostAddr = gethostbyname(fServer.String());
	if (hostAddr)
	{
		sockAddr.sin_addr = *((in_addr *)(hostAddr->h_addr_list[0]));
		fSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (fSocket >= 0)
		{
			if (connect(fSocket, (sockaddr *)&sockAddr, sizeof(sockAddr)) >= 0)
			{
				fThreadID = spawn_thread(ReceiveHandler, "http_receiver", B_DISPLAY_PRIORITY, this);
				if (fThreadID >= 0)
				{
					resume_thread(fThreadID);
					fTarget.SendMessage(DC_MSG_HTTP_CONNECTED);
					return;
				}
			}
		}
	}
	fTarget.SendMessage(DC_MSG_HTTP_CONNECT_ERROR);
	Disconnect();	// failed... cleanup
	return;
}

void
DCHTTPConnection::Send(const BString & text)
{
	BMessage msg(DC_HTTP_MSG_SEND);
	msg.AddString("text", text);
	PostMessage(&msg);
}

int32
DCHTTPConnection::ReceiveHandler(void * data)
{
	DCHTTPConnection * http = (DCHTTPConnection *)data;	// Get our class :)
	char recvBuffer[DC_HTTP_RECV_BUFFER + 1];
	BString socketBuffer = "";
	int amountRead = 0;
	
	while (1)
	{
		memset(recvBuffer, 0, DC_HTTP_RECV_BUFFER + 1);
#ifdef NETSERVER_BUILD
		if ((amountRead = recv(http->fSocket, recvBuffer, DC_HTTP_RECV_BUFFER, 0)) < 0)
		{
			if (amountRead != -1)
			{
				http->Disconnect();	// clean up.. we're done
				return -1;
			}
		}
#else
		if ((amountRead = recv(http->fSocket, recvBuffer, DC_HTTP_RECV_BUFFER, 0)) < 0)
		{
			http->Disconnect();
			http->fTarget.SendMessage(DC_MSG_HTTP_RECV_ERROR);
			return -1;
		}
#endif
		
		if (amountRead == 0)	// that's it, our connection has been dropped by the server
		{
			// reset thread manually so it doesn't get killed
			// since it will be free auto-magically by the OS
			http->fThreadID = -1;
			// Now cleanup the socket
			http->Disconnect();
			// Parse the lines into a hub list
			http->ParseIntoHubList();
			http->fLines.clear();	// empty the lines list to free memory
			// Notify our target
			http->fTarget.SendMessage(DC_MSG_HTTP_DISCONNECTED);
			return 0;	// success
		}
		
		recvBuffer[amountRead] = 0;		// NULL-terminate
		socketBuffer.Append(DCUTF8(recvBuffer));
		int32 i;
		while ((i = socketBuffer.FindFirst("\r\n")) != B_ERROR)
		{
			BString insert;
			socketBuffer.MoveInto(insert, 0, i);
			socketBuffer.RemoveFirst("\r\n");
			http->fLines.push_back(insert);
		}
	}
}

void
DCHTTPConnection::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DC_HTTP_MSG_SEND:
		{
			if (fSocket >= 0)
			{
				BString text;
				if (msg->FindString("text", &text) == B_OK)
				{
					if (send(fSocket, text.String(), text.Length(), 0) <= 0)
						fTarget.SendMessage(DC_MSG_HTTP_SEND_ERROR);
				}
			}
			break;
		}
		
		case DC_HTTP_MSG_CONNECT:
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
DCHTTPConnection::EmptyHubList(list<Hub *> * hubs)
{
	while (hubs->begin() != hubs->end())
	{
		list<Hub *>::iterator i = hubs->begin();
		delete (*i);	// free the item
		hubs->erase(i);
	}
}

void
DCHTTPConnection::ParseIntoHubList()
{
	list<BString>::iterator i;
	BString name, server, desc;
	uint32 users;
	BString item;
	
	EmptyHubList(&fHubs);
	for (i = fLines.begin(); i != fLines.end(); i++)
	{
		BString tmpUsers;
		item = (*i);

		DCStringTokenizer tok(item, '|');
		if (tok.GetListSize() < 4)
			continue;
		DCTokens::iterator iter = tok.GetTokenList().begin();
		name = *iter++;
		server = *iter++;
		desc = *iter++;
		tmpUsers = *iter++;
		users = atoi(tmpUsers.String());
		
		// Make sure we have a semi-valid server
		if (server == "")
			continue;	// invalid
		if (server.Length() <= 4)	// i'd think it takes at least 5 characters for a domain name..
			continue;
		
		#define DCUTF8(X) X
		// Insert into hub list
		Hub * hub = new Hub(DCUTF8(name.String()), DCUTF8(server.String()), DCUTF8(desc.String()), users);
		if (hub)
			fHubs.push_back(hub);
	}
}

list<DCHTTPConnection::Hub *> *
DCHTTPConnection::GetHubsCopy()
{
	list<DCHTTPConnection::Hub *> * lst = new list<DCHTTPConnection::Hub *>;
	list<DCHTTPConnection::Hub *>::iterator i;
	
	for (i = fHubs.begin(); i != fHubs.end(); i++)
	{
		Hub * newHub = new Hub;
		newHub->fName = (*i)->fName;
		newHub->fServer = (*i)->fServer;
		newHub->fDesc = (*i)->fDesc;
		newHub->fUsers = (*i)->fUsers;
		lst->push_back(newHub);
	}
	return lst;
}

void
DCHTTPConnection::SendListRequest()
{
	// Format an HTTP request
	BString http = "GET /";
	http += GetFile();
	http += " HTTP/1.1\r\nUser-Agent: BeDC/alpha\r\nHost: ";
	http += GetServer();
	http += "\r\n\r\n";
	Send(http);
}
