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


#ifndef _DC_CLIENT_CONNECTION_H_
#define _DC_CLIENT_CONNECTION_H_

enum 
{
	DC_UPLOAD_DIR = 0,
	DC_DOWNLOAD_DIR = 1
};


class DCClientConnection
{
	public:
		DCClientConnection();
		~DCClientConnection();
		void Connect(const char *host,int port = 412);
		void SetNick(const char *in_nick) {nick->SetTo(in_nick);};
		void SetDirection(int in_direction = DC_UPLOAD_DIR) {direction = in_direction;};
		void GetFile(const char *remote_path, const char *local_path,int resume_count = 1);
		int GetSocket(){return conn_socket;};
		int GetDirection(){return direction;};
		const char *GetNick(){return nick->String();};
	private:
		bool connected;
		BString *nick;
		int conn_socket;
		int direction;
		thread_id thid;
		
};


#endif /* _DC_CLIENT_CONNECTION_H_ */