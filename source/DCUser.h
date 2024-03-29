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
#ifndef _DC_USER_H_
#define _DC_USER_H_

#include <String.h>

class BRow;
class BColumnListView;

enum
{
	DC_USER_NAME,
	DC_USER_SPEED,
	DC_USER_DESC,
	DC_USER_EMAIL,
	DC_USER_SHARED
};

// The list view to which this user belongs to, the columns should be like this:
//	Name, Speed, Desc, Email, Shared
class DCUser
{
public:
							DCUser(const BString & name = "", const BString & desc = "",
								   const BString & email = "", const BString & speed = "",
								   int64 shared = 0);
							~DCUser();

	// Create a row for use with BCLV
	// Pass NULL to remove from the current list
	BRow *					CreateRow(BColumnListView * owner);
	
	BString					GetName() const { return fName; }
	BString					GetDesc() const { return fDesc; }
	BString					GetEmail() const { return fEmail; }
	BString					GetSpeed() const { return fSpeed; }
	int64					GetShared() const { return fShared; }
	BString					GetSharedString() const;
	
	void					SetName(const BString & s);
	void					SetDesc(const BString & s);
	void					SetEmail(const BString & s);
	void					SetSpeed(const BString & s);
	void					SetShared(int64 size);
	
private:
	BRow * 					fRow;
	BColumnListView * 		fList;
	
	BString					fName;
	BString					fDesc;
	BString					fEmail;
	BString					fSpeed;
	int64					fShared;
};

#endif
