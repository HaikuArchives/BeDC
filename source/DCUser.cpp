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
#include "DCUser.h"
#include "ColumnListView.h"
#include "ColumnTypes.h"

#include <stdio.h>

DCUser::DCUser(const BString & name, const BString & desc, const BString & email, const BString & speed,
			   int64 shared)
{
	fName = name; 
	fDesc = desc; 
	fEmail = email; 
	fSpeed = speed;
	fShared = shared;
	
	fRow = NULL;
	fList = NULL;
}

DCUser::~DCUser()
{
	if (fRow)
	{
		fList->RemoveRow(fRow);
		delete fRow;
	}
}

BRow *
DCUser::CreateRow(BColumnListView * list)
{
	if (fRow)
	{
		fList->RemoveRow(fRow);
		delete fRow;
		fRow = NULL;
	}
	if (list)
	{
		fRow = new BRow;
		fRow->SetField(new BStringField(fName.String()), 0);
		fRow->SetField(new BStringField(fSpeed.String()), 1);
		fRow->SetField(new BStringField(fDesc.String()), 2);
		fRow->SetField(new BStringField(fEmail.String()), 3);
		fRow->SetField(new BStringField(GetSharedString().String()), 4);
		list->AddRow(fRow);
		fList = list;
	}
	return fRow;
}

BString
DCUser::GetSharedString() const
{
	int64 size = fShared;
	
	BString str;
	BString append = " bytes";
	
	if (size >= 1024)	// more than a kB shared
	{
		size = size / 1024;
		printf("Size is now: %Ld kb\n", size);
		append = " kB";
		if (size >= 1024)	// more than a MB now shared
		{
			size = size / 1024;
		printf("Size is now: %Ld mb\n", size);
			append = " MB";
			if (size >= 1024)	// more than a GB shared
			{
				size = size / 1024;
		printf("Size is now: %Ld gb\n", size);
				append = " GB";
			}
		}
	}

	str << size;
	str += append;
	return str;
}

void
DCUser::SetName(const BString & s)
{
	fName = s;
	if (fRow)
		((BStringField *)fRow->GetField(DC_USER_NAME))->SetString(fName.String());
}

void
DCUser::SetDesc(const BString & s)
{
	fDesc = s;
	if (fRow)
		((BStringField *)fRow->GetField(DC_USER_DESC))->SetString(fDesc.String());
}

void
DCUser::SetEmail(const BString & s)
{
	fEmail = s;
	if (fRow)
		((BStringField *)fRow->GetField(DC_USER_EMAIL))->SetString(fEmail.String());
}

void
DCUser::SetSpeed(const BString & s)
{
	fSpeed = s;
	if (fRow)
		((BStringField *)fRow->GetField(DC_USER_SPEED))->SetString(fSpeed.String());
}

void
DCUser::SetShared(int64 size)
{
	fShared = size;
	if (fRow)
		((BStringField *)fRow->GetField(DC_USER_SHARED))->SetString(GetSharedString().String());
}
