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


#ifndef _DC_SETTINGS_H_
#define _DC_SETTINGS_H_

#include <Message.h>
#include <Rect.h>
#include <Point.h>
#include <String.h>

#define DCS_WINDOW_RECT 		"window_rect"
#define DCS_HUB_RECT			"hub_rect"
#define DCS_PREFS_POS			"prefs_pos"
#define DCS_PREFS_NICK			"prefs_nick"
#define DCS_PREFS_EMAIL			"prefs_email"
#define DCS_PREFS_DESC			"prefs_desc"
#define DCS_PREFS_CONNECTION	"prefs_connection"
#define DCS_PREFS_ACTIVE		"prefs_active"
#define DCS_PREFS_PORT			"prefs_port"
#define DCS_PREFS_IP			"prefs_ip"

class DCSettings : public BMessage
{
public:
						DCSettings() {}
						DCSettings(const DCSettings & copy) : BMessage(copy) {}
						
	virtual				~DCSettings() {}
	
	void 				LoadSettings();
	void 				SaveSettings();
	
	void 				SetString(const char *name, const char *string);
	void				SetString(const char * name, const BString & str) { SetString(name, str.String()); }
	void 				SetRect(const char *name, const BRect & rect);
	void				SetPoint(const char * name, const BPoint & point);
	void				SetBool(const char * name, bool val);
	void				SetInt(const char * name, int32 val);
	
	status_t 			GetRect(const char *name, BRect *rect);
	status_t 			GetString(const char *name, BString *string);
	status_t			GetPoint(const char * name, BPoint * point);
	status_t			GetBool(const char * name, bool & val);
	status_t			GetInt(const char * name, int32 & val);
	
	// Quick helpers
	BString 			GetString(const char * name)
		{ BString ret; GetString(name, &ret); return ret; }
};

#endif /* !_DC_SETTINGS_H_ */
