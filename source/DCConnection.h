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

class BLooper;
struct conn_info;

int32 receiver(void *data);
BString *generate_key(BString &lck);


enum
{
	DC_TEXT = 'dctx',
	DC_PRIV_MSG = 'dcpr',
	DC_USER_CONNECTED ='dcuc',
	DC_USER_DISCONNECTED ='dcud',
	DC_DISCONNECTED_FROM_SERVER = 'dcfs',
	DC_USER_CONNECT = 'dcum',
	DC_NEED_PASS = 'dcnp'
};

class DCConnection 
{
	public:
		DCConnection(const char *host=NULL,int port = 411);
		~DCConnection();
		void Connect(const char *host,int port = 411);
		void Disconnect();
		bool IsConnected()  {return connected;};
		void SetNick(const char *in_nick);
		void SetConn(const char *in_conn) {myconn->SetTo(in_conn);};
		const char *GetNick() { return nick->String();};
		const char *GetConn() { return myconn->String();};
		void SetMessageTarget(BLooper *looper);
		int32 SendData(const char *command);
	private:
		bool connected;
		BString *nick;
		BString *myconn;
		int connection;
		BLooper *msgTarget;
		thread_id thid;
		conn_info *cinfo;
};

struct conn_info
{
		BLooper			**target;
		int				conn;
		BString			*nick;
		DCConnection 	*conn_obj;
};

#endif /* !_DC_CONNECTION_H_ */
