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


#include <stdio.h>
#include <unistd.h>

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
#include <Looper.h>
#include <Message.h>
#include <String.h>
#include <UTF8.h>

#include "DCConnection.h"


DCConnection::DCConnection(const char *host,int port)
{
	connected = false;
	thid = 0;
	connection = 0;
	nick = new BString("Anonymous");
	myconn = new BString("56Kbps");
	msgTarget = NULL;
	cinfo = new conn_info;
	if(host != NULL)
		Connect(host, port);
}

DCConnection::~DCConnection()
{
	Disconnect();
	delete nick;
	delete cinfo;
}


/* TODO: Move this to own thread? (So it doesn't block for redrawing) */
void DCConnection::Connect(const char *host,int port)
{
	printf("DCConnection::Connect()\n");
	sockaddr_in sock_addr;
	memset(&sock_addr,0,sizeof(sock_addr));
	
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port=htons(port);
	/*Gets adress*/
	hostent *host_addr = gethostbyname(host);
	if(host_addr)
	{
		sock_addr.sin_addr = *((in_addr *)(host_addr->h_addr_list[0]));
		connection = socket(AF_INET, SOCK_STREAM, 0);
		if(connect(connection,(sockaddr*)&sock_addr,sizeof(sock_addr)) >=0 )
		{
			memset(cinfo,0,sizeof(cinfo));
			cinfo->target = &msgTarget;
			cinfo->conn = connection;
			cinfo->nick = nick;
			cinfo->conn_obj = this; 
			thid = spawn_thread(receiver,"receiver",B_NORMAL_PRIORITY,cinfo);
			resume_thread(thid);
			connected = true;
		}
		else
			printf("Connection failed!\n");
	}
	else
		printf("Address not found!\n");
	
}

void DCConnection::Disconnect()
{
#ifdef NETSERVER_BUILD	
	closesocket(connection);
#else
	close(connection);
#endif
	kill_thread(thid);
	connected = false;
}

void DCConnection::SetNick(const char *in_nick)
{
	nick->SetTo(in_nick);
	if(connected)
	{
		/* TODO: Check what happens when you change nick while connected */
		//BString tmp("ValidateNick ");
		//tmp.Append(*nick);
		//tmp.Append("|");
		//SendData(tmp.String());
	}
}
int32 DCConnection::SendRawData(const char *command)
{
	BString toSend(command);
	printf("Sending: %s\n",toSend.String());
	return send(connection,toSend.String(),toSend.Length(),0);

}

int32 DCConnection::SendData(const char *command)
{
	BString toSend(command);
	printf("Sending: %s\n",toSend.String());
	int32 length = toSend.Length();
	char convertedbuffer[length+1]; 
	memset(convertedbuffer,0,length+1);
	char *stringData = toSend.LockBuffer(length+1);
	stringData[length] = 0;
	convert_from_utf8(B_MS_WINDOWS_CONVERSION,stringData,&length,convertedbuffer,&length,0);
	toSend.UnlockBuffer(length+1);
	return send(connection,convertedbuffer,length,0);
	
}

void DCConnection::SetMessageTarget(BLooper *looper)
{
	msgTarget = looper;
}

int32 receiver(void *data)
{
	conn_info *cinfo;
	cinfo = (conn_info*)data;
	int32 R_BUF_SIZE = /*4095*/512;
	int32 R_CONV_SIZE = R_BUF_SIZE*2; /* UTF-8 does two bytes for extended chars, doesn't it? */
	char recvBuffer[R_BUF_SIZE+1];
	char convertedbuffer[R_CONV_SIZE+1];
	BString bstr1, bstr2;
	BMessage *msg = NULL;
	printf("We're in our thread\n");
	while(true)
	{
		memset(recvBuffer,0,R_BUF_SIZE+1);
		memset(convertedbuffer,0,R_CONV_SIZE+1);
		int bufread;
#ifdef  NETSERVER_BUILD
		if((bufread = recv(cinfo->conn,recvBuffer,R_BUF_SIZE,0)) < 0)
		{
			if(bufread != -1)     /* On net_server recv returns -1 when we send chat text, dunno why... */
				exit_thread(-1);  /* But the socket is still good to go, so we just ignore it and go on */
		}
#else
		if((bufread = recv(cinfo->conn,recvBuffer,R_BUF_SIZE,0)) < 0)
				exit_thread(-1);  /* BONE only gives us errors when something is wrong */
#endif				
		else
		{
			convert_to_utf8(B_MS_WINDOWS_CONVERSION,recvBuffer,&R_BUF_SIZE,convertedbuffer,&R_CONV_SIZE,0);
			printf("RCvd: %s\n",convertedbuffer);
			bstr1.SetTo(convertedbuffer);
			while(bstr1[bstr1.Length()-1] != '|') /*We haven't got the full command, let's get some more */
			{
				memset(recvBuffer,0,R_BUF_SIZE+1);
				memset(convertedbuffer,0,R_CONV_SIZE+1);
#ifdef  NETSERVER_BUILD
				if((bufread = recv(cinfo->conn,recvBuffer,R_BUF_SIZE,0)) < 0)
				{
					if(bufread != -1)     
						exit_thread(-1);
				}  
#else
				if((bufread = recv(cinfo->conn,recvBuffer,R_BUF_SIZE,0)) < 0)
						exit_thread(-1);  
#endif
				else
				{
					convert_to_utf8(B_MS_WINDOWS_CONVERSION,recvBuffer,&R_BUF_SIZE,convertedbuffer,&R_CONV_SIZE,0);
					printf("RCvd: %s\n",convertedbuffer);
					bstr1.Append(convertedbuffer); /* Append it to the end of the buffer */
				}
			}
			bstr2.SetTo("");
			int i;
			while((i=bstr1.FindFirst("|")) != B_ERROR)
			{
				bstr2.SetTo("");
				bstr1.MoveInto(bstr2,0,i);
				bstr1.Remove(0,1);
				
				if(!bstr2.Compare("$Lock ",6))
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
					(cinfo->conn_obj)->SendRawData(bstr2.String()); /* No charset conversion on the key */
					delete bstr3;
					
					/* Send our nick */
					bstr2.SetTo("$ValidateNick ");
					bstr2.Append((cinfo->conn_obj)->GetNick());
					bstr2.Append("|");
					(cinfo->conn_obj)->SendData(bstr2.String());
				}
				else if(!bstr2.Compare("$HubName ",9))
				{
					// We got the hubname
				}
				else if(!bstr2.Compare("$Hello ",7))
				{
					/* Someone entered the hub */
					/* Upload our info and get the nicklist if it's us that's connecting. */
					/* If it's someone else we add them to the list                       */
					bstr2.RemoveFirst("$Hello ");
					if(!bstr2.Compare(*cinfo->nick))
					{
						(cinfo->conn_obj)->SendData("$Version 1,0091|");
						(cinfo->conn_obj)->SendData("$GetNickList|");
						bstr2.SetTo("$MyINFO $ALL ");
						bstr2.Append(*cinfo->nick);
						bstr2.Append(" Just testing$ $");
						bstr2.Append((cinfo->conn_obj)->GetConn());
						bstr2.Append((char)1,1);
						//bstr2.Append("$$0$|");
						bstr2.Append("$$1048576$|");
						(cinfo->conn_obj)->SendData(bstr2.String());
					}
					else
					{
						msg = new BMessage(DC_USER_CONNECTED);
						msg->AddString("nick",bstr2.String());
						(*cinfo->target)->PostMessage(msg);
					}
				}
				else if(!bstr2.Compare("$Quit ",6))
				{
					/* A user disconnects */
					bstr2.RemoveFirst("$Quit ");
					msg = new BMessage(DC_USER_DISCONNECTED);
					msg->AddString("nick",bstr2.String());
					(*cinfo->target)->PostMessage(msg);
					
				}
				else if(!bstr2.Compare("$NickList ",10))
				{
					/* The list of users */
					BString bstr3;
					int j;
					bstr2.RemoveFirst("$NickList ");
					while((j=bstr2.FindFirst("$$")) != B_ERROR)
					{
						bstr3.SetTo("");
						bstr2.MoveInto(bstr3,0,j);
						bstr2.Remove(0,2);
						//printf("Nick: %s\n",bstr3.String());
						msg = new BMessage(DC_USER_CONNECTED);
						msg->AddString("nick",bstr3.String());
						(*cinfo->target)->PostMessage(msg);
					}
					
				}
				else if(!bstr2.Compare("$OpList ",8))
				{
					/* The operators that's online                                  */
					/* Are sendt when you get the nicklist, and when an op connects */
				}
				else if(!bstr2.Compare("$ValidateDenide ", 16))
				{
					/* Oops, couldn't use that nick */
					bstr2.RemoveFirst("$ValidateDenide ");
					bstr2.Prepend("<Client> The nick \"");
					bstr2.Append("\" is unavailable. Change nick and reconnect");
					msg = new BMessage(DC_TEXT);
					msg->AddString("thetext",bstr2.String());
					(*cinfo->target)->PostMessage(msg);
					(cinfo->conn_obj)->Disconnect();
				}
				else if(!bstr2.Compare("$To: ",5))
				{
					/* Private message */
					bstr2.RemoveFirst("$To: ");
					/* should only be addressed to us, but we check the nick anyway */
					if(!bstr2.Compare(*cinfo->nick,cinfo->nick->Length())) 
					{
						bstr2.RemoveFirst(*cinfo->nick);
						bstr2.RemoveFirst(" From: ");
						BString bstr3;
						int count=bstr2.FindFirst(" ");
						bstr2.MoveInto(bstr3,0,count);
						bstr2.RemoveFirst(" $");
						if(bstr2!="") /* We don't want no empty strings */
						{
							msg = new BMessage(DC_PRIV_MSG);
							msg->AddString("nick",bstr3.String());
							msg->AddString("thetext",bstr2.String());
							(*cinfo->target)->PostMessage(msg);
						}
					}
				}
				else if(!bstr2.Compare("$MyINFO ",8))
				{
					/* We got user info, do something with it */
				}
				else if(!bstr2.Compare("$Search ",8))
				{
					/* Someone searches for something */
				}
				else if(!bstr2.Compare("$ConnectToMe ",13))
				{
#ifdef CLIENT_TO_CLIENT_COMMUNICATION
					/* Someone wants us to connect to them */
					bstr2.RemoveFirst("$ConnectToMe ");
					bstr2.RemoveFirst(*cinfo->nick);
					bstr2.RemoveFirst(" ");
					msg = new BMessage(DC_USER_CONNECT);
					msg->AddString("address",bstr2.String());
					(*cinfo->target)->PostMessage(msg);
#endif
				}
				else if(!bstr2.Compare("$RevConnectToMe ",16))
				{
					/* Someone in passive mode wants us to give them our */
					/* host and port, so they can connect to us          */
				}
				else if(!bstr2.Compare("$ForceMove ",11))
				{
					/* We're being redirected */
					bstr2.SetTo("<Client> Received redirect request, disconnecting");
					(cinfo->conn_obj)->Disconnect();
					msg = new BMessage(DC_TEXT);
					msg->AddString("thetext",bstr2.String());
					(*cinfo->target)->PostMessage(msg);
				}
				else if(!bstr2.Compare("$GetPass",8))
				{
					msg = new BMessage(DC_NEED_PASS);
					(*cinfo->target)->PostMessage(msg);
				}
				else if(!bstr2.Compare("$BadPass",8))
				{
					//msg = new BMessage(DC_NEED_PASS);
					//(*cinfo->target)->PostMessage(msg);
				}
				else if(!bstr2.Compare("$LogedIn ",9))
				{
					/* Wooo, we're logged in as an operator */
					bstr2.SetTo("<Client> Logged in as operator");
					msg = new BMessage(DC_TEXT);
					msg->AddString("thetext",bstr2.String());
					(*cinfo->target)->PostMessage(msg);
				}
				else
				{
					/* Probably chat text, send it to the BLooper */
					if(bstr2!="") /* We don't want no empty strings */
					{
						msg = new BMessage(DC_TEXT);
						msg->AddString("thetext",bstr2.String());
						(*cinfo->target)->PostMessage(msg);
					}
				}
				//delete msg;
			}
		}
	}
	return 0;
}

BString *generate_key(BString &lck)
{
	BString *key;
	key= new BString("",2048);
	char c, v;
	for(int i=0;i<lck.Length();i++)
	{
		if(i==0)
		{
			c = lck[i]^lck[lck.Length()-1]^lck[lck.Length()-2]^5;
		}
		else
		{
			c = lck[i]^lck[i-1];
		}
		v = (c<<4|c>>4);
		if ((v==0)||(v==5)||(v==36)||(v==96)||(v==124)||(v==126)) /* chars we must escape */
		{
				char sbuf[11];
				memset(sbuf,0,sizeof(sbuf));
				sprintf(sbuf,"/%%DCN%03d%%/",v);
				key->Append(sbuf);
		}
		else
			key->Append(v,1);
	}
	return key;
}
