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

#ifndef _DC_HTTP_CONNECTION_H_
#define _DC_HTTP_CONNECTION_H_


#include <String.h>
#include <OS.h>

#include <list>
using std::list;

class DCHTTPConnection
{
public:
						DCHTTPConnection();		// Constructor... does nothing...
						~DCHTTPConnection();	// Destructor... closes the connection if still running
	
	bool				Connect(const BString & optServer = "www.neo-modus.com", 
								const BString & optFile = "PublicHubList.config");
	void				Disconnect();
	
	bool				IsRunning() const { return (fThreadID != -1 && fSocket != -1) ? true : false; }
	// You are giving a COPY of the list
	list<BString> 		GetLines() const { return fLines; }
	BString				GetServer() const { return fServer; }
	BString				GetFile() const { return fFile; }
	
	int32				Send(const BString & text);
	
private:
	list<BString>		fLines;	// The config file is a set of lies
	BString				fServer;
	BString				fFile;
	thread_id			fThreadID;
	int					fSocket;
	
	static int32		ReceiveHandler(void *);
};


#endif	// _DC_HTTP_CONNECTION_H_
