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


/* Client/client comm prototype, don't use it yet, it will change */


#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>

#ifdef NETSERVER_BUILD 
#include <netdb.h>
#include <socket.h>
#endif

#ifdef BONE_BUILD
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <OS.h>
#include <File.h>
//#include <Looper.h>
//#include <Message.h>
#include <String.h>

#include "DCClientConnection.h"
#include "DCConnection.h" /* For the keygen */
#include "DCHuffman.h"

int32 clientReceiver(void *data);

DCClientConnection::DCClientConnection()
{
	connected = false;
	thid = -1;
	nick = new BString;
	//direction = DC_UPLOAD_DIR;
	direction = DC_DOWNLOAD_DIR;
	conn_socket = -1;
}

DCClientConnection::~DCClientConnection()
{

}

void DCClientConnection::Connect(const char *host,int port)
{
	printf("DCClientConnection::Connect()\n");
	sockaddr_in sock_addr;
	memset(&sock_addr,0,sizeof(sock_addr));
	
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port=htons(port);
	/*Gets adress*/
	hostent *host_addr = gethostbyname(host);
	if(host_addr)
	{
		sock_addr.sin_addr = *((in_addr *)(host_addr->h_addr_list[0]));
		conn_socket = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(conn_socket,(sockaddr*)&sock_addr,sizeof(sock_addr)) >=0 )
		{
			/*Send our nick and the lock */
			BString nickandlock;
			nickandlock.SetTo("$MyNick ");
			nickandlock.Append(GetNick());
			nickandlock.Append("|$Lock iamjusttesting thisout|");
			send(conn_socket,nickandlock.String(),nickandlock.Length(),0);
			
			thid = spawn_thread(clientReceiver,"client_receiver",B_NORMAL_PRIORITY,this);
			resume_thread(thid);
			connected = true;
		}
		else
			printf("Connection failed!\n");
	}
	else
		printf("Address not found!\n");
	
}

int32 clientReceiver(void *data)
{
	DCClientConnection *theconn;
	theconn = (DCClientConnection*)data;
	char recvBuffer[1024];
	BString bstr1, bstr2;
	while(true)
	{
		bstr2.SetTo("");
		memset(recvBuffer,0,1024);
		status_t what;
		if((what=recv(theconn->GetSocket(),recvBuffer,1023,0)) <= 0)
		{
			//printf("Damn.... %d\n",what);
			exit_thread(-1);
		}
		else
		{
			printf("RCvd: %s\n",recvBuffer);
			bstr1.SetTo(recvBuffer);
			while(bstr1[bstr1.Length()-1] != '|') /*We haven't got the full command, let's get some more */
			{
				memset(recvBuffer,0,1024);
				if(recv(theconn->GetSocket(),recvBuffer,1023,0) <= 0)
					exit_thread(-1);
				else
				{
					printf("RCvd: %s\n",recvBuffer);
					bstr1.Append(recvBuffer); /* Append it to the end of the buffer */
				}
			}
			bstr2.SetTo("");
			int i;
			while((i=bstr1.FindFirst("|")) != B_ERROR)
			{
				bstr2.SetTo("");
				bstr1.MoveInto(bstr2,0,i);
				bstr1.Remove(0,1);
				if(!bstr2.Compare("$Hello ",7))
				{
				
				}
				else if(!bstr2.Compare("$Lock ",6))
				{
					bstr2.RemoveFirst("$Lock "); /* Remove "$Lock "*/
					int j = bstr2.FindFirst(" ");
					bstr2.Remove(j,bstr2.Length()-j); /* Remove everything after the first space */
					
					/* Generate the key */
					BString *bstr3 = generate_key(bstr2);
					
					/* Put together the key string and send it*/
					bstr2.SetTo("$Key ");
					bstr2.Append(*bstr3);
					bstr2.Append("|");
					send(theconn->GetSocket(),bstr2.String(),bstr2.Length(),0);
					delete bstr3;
				}
				else if(!bstr2.Compare("$Key ",5))
				{
					/* Maybe check that the key is valid? */
				}
				else if(!bstr2.Compare("$Direction ",11))
				{
					if(theconn->GetDirection() == DC_UPLOAD_DIR)
						bstr2.SetTo("$Direction Upload 9876|");
					else if(theconn->GetDirection() == DC_DOWNLOAD_DIR)
						bstr2.SetTo("$Direction Download 9876|");
					send(theconn->GetSocket(),bstr2.String(),bstr2.Length(),0);
					if(theconn->GetDirection() == DC_DOWNLOAD_DIR)
					{
						bstr2.SetTo("$Get MyList.DcLst$1|");
						//bstr2.SetTo("$Get Diverse\\mirc591t.exe$1|");
						send(theconn->GetSocket(),bstr2.String(),bstr2.Length(),0);
					}
				}
				else if(!bstr2.Compare("$Get MyList.DcLst$1",19))
				{
					bstr2.SetTo("$FileLength 0|");
					send(theconn->GetSocket(),bstr2.String(),bstr2.Length(),0);
					//bstr2.SetTo("$Direction Upload 1234|");
					//send(theconn,bstr2.String(),bstr2.Length(),0);
				}
				else if(!bstr2.Compare("$Send",5))
				{
					//bstr2.SetTo("Empty\\empty|");
					//send(theconn,bstr2.String(),bstr2.Length(),0);
				}
				else if(!bstr2.Compare("$FileLength ",12))
				{
					bstr2.RemoveFirst("$FileLength ");
					int length = atoi(bstr2.String());
					bstr2.SetTo("$Send|");
					send(theconn->GetSocket(),bstr2.String(),bstr2.Length(),0);
					BFile file;
					file.SetTo("/tmp/DCList",B_READ_WRITE|B_CREATE_FILE);
					//file.SetTo("/boot/home/Desktop/mirc591t.exe",B_READ_WRITE|B_CREATE_FILE);
					char buf[1024];
					memset(buf,0,1024);
					int read;
					while(true)
					{
						read=recv(theconn->GetSocket(),buf,1023,0);
						file.Write(buf,read);
						length -= read;
						if(length <= 0) break;
					}
					printf("Ok...\n");
					file.Seek(0,SEEK_SET);
					DCHuffman huf;
					huf.Decode(&file,NULL);
					//bstr2.SetTo("Empty\\empty|");
					//send(theconn,bstr2.String(),bstr2.Length(),0);
				}
			}
		}
	}
	return(0);
}
