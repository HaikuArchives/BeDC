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

#include <Message.h>
#include <File.h>
#include <Path.h>
#include <Directory.h>
#include <String.h>

#include "DCSettings.h"


void 
DCSettings::LoadSettings()
{
	BFile settingsfile;
	if(settingsfile.SetTo("/boot/home/config/settings/BeDC/BeDC-Settings",B_READ_ONLY)==B_OK)
	{
		Unflatten(&settingsfile);
	}
}

void 
DCSettings::SaveSettings()
{
	BPath path("/boot/home/config/settings/BeDC/BeDC-Settings");
	BFile settingsfile;
	if(settingsfile.InitCheck() != B_OK) /* Didn't work, try to create parent dir */
	{
		BPath parent;
		path.GetParent(&parent);
		create_directory(parent.Path(),0777);
	}
	if(settingsfile.SetTo(path.Path(),B_WRITE_ONLY|B_CREATE_FILE)==B_OK)
		Flatten(&settingsfile);
}

status_t 
DCSettings::GetString(const char *name, BString *string)
{
	return FindString(name,string);
}

void 
DCSettings::SetString(const char *name, const char *string)
{
	BString tmp;
	if((FindString(name,&tmp))==B_OK)
		ReplaceString(name,string);
	else
		AddString(name,string);
}

status_t 
DCSettings::GetRect(const char *name, BRect *rect)
{
	return FindRect(name,rect);
}

void 
DCSettings::SetRect(const char *name, const BRect & rect)
{
	BRect tmp;
	if((FindRect(name,&tmp))==B_OK)
		ReplaceRect(name,rect);
	else
		AddRect(name,rect);
}

status_t
DCSettings::GetPoint(const char * name, BPoint * point)
{
	return FindPoint(name, point);
}

void
DCSettings::SetPoint(const char * name, const BPoint & point)
{
	BPoint tmp;
	if (FindPoint(name, &tmp) == B_OK)
		ReplacePoint(name, point);
	else
		AddPoint(name, point);
}

status_t
DCSettings::GetBool(const char * name, bool & val)
{
	return FindBool(name, &val);
}

void
DCSettings::SetBool(const char * name, bool val)
{
	bool temp;
	if (FindBool(name, &temp) == B_OK)
		ReplaceBool(name, val);
	else
		AddBool(name, val);
}

status_t
DCSettings::GetInt(const char * name, int32 & val)
{
	return FindInt32(name, &val);
}

void
DCSettings::SetInt(const char * name, int32 val)
{
	int32 temp;
	if (FindInt32(name, &temp) == B_OK)
		ReplaceInt32(name, val);
	else
		AddInt32(name, val);
}

void
DCSettings::SetColor(const char * name, const rgb_color & col)
{
	const void * temp = NULL;
	ssize_t size = 0;
	if (FindData(name, B_ANY_TYPE, &temp, &size) == B_OK)
		RemoveData(name);
	AddData(name, B_ANY_TYPE, (const void *)&col, sizeof(col));
}

status_t
DCSettings::GetColor(const char * name, rgb_color & col)
{
	rgb_color * ret = NULL;
	ssize_t numBytes = 0;
	status_t status = FindData(name, B_ANY_TYPE, (const void **)&ret, &numBytes);
	if (status != B_OK)
		return B_ERROR;
	col = *ret;
	return B_OK;
}
