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
#include "DCStrings.h"

#include <UTF8.h>

#include <string.h>
#include <stdio.h>

// This is the implementation of internationalization... 
// If you want your own language to be implemented, translate the
// english strings to your language, and send it my way :)
const char * DC_STR_ENGLISH[STR_NUM] =
{
	"Hubs",
	"Connect",
	"Refresh",
	"Next 50",
	"Previous 50",
	"Name",
	"Address",
	"Description",
	"Users",
	"Idle",
	"Connected, retreiving server list",
	"Error connecting",
	"Error sending request",
	"Error receiving data",
	"Number of servers: "
};

const char ** DC_STR_USE = 0;	// Set to the current language in use

const char * 
DCStr(int str)
{
	return DC_STR_USE[str];	// invalid index will cause a CRASH
}

void
DCSetLanguage(int lang)
{
	switch (lang)
	{
		case DC_LANG_ENGLISH:
			DC_STR_USE = DC_STR_ENGLISH;
			break;
			
		default:
			break;
	};
}

BString
DCUTF8(const char * str)
{
	BString ret(str);
	int32 srcSize = strlen(str) + 1;
	int32 convSize = srcSize * 2 + 4 /* 4 for padding... just in case */;
	char * convStr = new char[convSize + 1];
	
	if (convert_to_utf8(B_MS_WINDOWS_CONVERSION, str, &srcSize, convStr, &convSize, NULL) == B_OK)
	{
		ret.SetTo(convStr);
	}
	delete [] convStr;
	return ret;
}