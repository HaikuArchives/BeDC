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

#include <unistd.h>
#include <stdio.h>

const int DC_HTTP_RECV_BUFFER = 512;

enum 
{
	DC_HTTP_MSG_SEND = 'dHmS',
	DC_HTTP_MSG_CONNECT = 'dHmC'
}; 


DCHTTPConnection::DCHTTPConnection(BMessenger target)
	: BLooper("DCHTTPConnection", B_LOW_PRIORITY)
{
	fServer = "";
	fFile = "";
	fThreadID = -1;
	fSocket = -1;
	fTarget = target;
	
	Run();
}

DCHTTPConnection::~DCHTTPConnection()
{
	Disconnect();
}

void
DCHTTPConnection::Disconnect()
{
	if (fThreadID >= 0)
	{
		status_t junk;
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
	fLines.clear();
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
	printf("Connectiong to server %s on port 80\n", fServer.String());
	
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
				fThreadID = spawn_thread(ReceiveHandler, "http_receiver", B_LOW_PRIORITY, this);
				if (fThreadID >= 0)
				{
					resume_thread(fThreadID);
					printf("Connection established\n");
					fTarget.SendMessage(DC_MSG_HTTP_CONNECTED);
					return;
				}
			}
		}
	}
	printf("Connection failed!\n");
	Disconnect();	// failed... cleanup
	fTarget.SendMessage(DC_MSG_HTTP_CONNECT_ERROR);
	return;
}

void
DCHTTPConnection::Send(const BString & text)
{
	printf("DCHTTPConnect::Send: %s\n", text.String());
	BMessage msg(DC_HTTP_MSG_SEND);
	msg.AddString("text", text);
	PostMessage(&msg);
}

int32
DCHTTPConnection::ReceiveHandler(void * data)
{
	DCHTTPConnection * http = (DCHTTPConnection *)data;	// Get our class :)
	char recvBuffer[DC_HTTP_RECV_BUFFER + 1];
	int amountRead = 0;
	
	sleep(1);
	while (1)
	{
		memset(recvBuffer, 0, DC_HTTP_RECV_BUFFER + 1);
#ifdef NETSERVER_BUILD
		if ((amountRead = recv(http->fSocket, recvBuffer, DC_HTTP_RECV_BUFFER, 0)) < 0)
		{
			if (amountRead != -1)
			{
				printf("Disconnecting... failed in ReceiveHandler()\n");
				http->Disconnect();	// clean up.. we're done
				return -1;
			}
		}
#else
		printf("going into recv()\n");
		if ((amountRead = recv(http->fSocket, recvBuffer, DC_HTTP_RECV_BUFFER, 0)) < 0)
		{
			printf("Disconnecting... failed in ReceiveHandler()\n");
			http->Disconnect();
			http->fTarget.SendMessage(DC_MSG_HTTP_RECV_ERROR);
			return -1;
		}
#endif
		
		if (amountRead == 0)	// that's it, our connection has been dropped by the server
		{
			printf("Disconnected from server\n");
			http->Disconnect();
			http->fTarget.SendMessage(DC_MSG_HTTP_DISCONNECTED);
			return 0;	// success
			// TODO: notify our window of the great news :)
		}
		
		recvBuffer[amountRead] = 0;		// NULL-terminate
		printf("Got some data... %s\n", recvBuffer);
		char * iter = strtok(recvBuffer, "\n");
		BString insert;
		if (iter)
		{
			while (iter)
			{
				insert = iter;
				http->fLines.insert(http->fLines.end(), insert);
				iter = strtok(NULL, "\n");
			}
		}
		else
		{
			insert = recvBuffer;
			http->fLines.insert(http->fLines.end(), insert);
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
			printf("DC_HTTP_MSG_SEND\n");
			if (fSocket >= 0)
			{
				BString text;
				if (msg->FindString("text", &text) == B_OK)
				{
					if (send(fSocket, text.String(), text.Length(), 0) <= 0)
						fTarget.SendMessage(DC_MSG_HTTP_SEND_ERROR);
					else
						printf("Sent...\n");
				}
			}
			break;
		}
		
		case DC_HTTP_MSG_CONNECT:
		{
			printf("DC_HTTP_MSG_CONNECT\n");
			InternalConnect();
			break;
		}
		
		default:
			BLooper::MessageReceived(msg);			
	}
}
