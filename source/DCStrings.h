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
#ifndef _DC_STRINGS_H_
#define _DC_STRINGS_H_

#include <String.h>

// String constants
enum 
{
	// Hub window
	STR_HUB_WINDOW_TITLE = 0,
	
	// Hub window -> Buttons
	STR_HUB_CONNECT,
	STR_HUB_REFRESH,
	STR_HUB_NEXT50,
	STR_HUB_PREV50,
	
	// Hub window -> List view
	STR_SERVER_NAME,
	STR_SERVER_ADDR,
	STR_SERVER_DESC,
	STR_SERVER_USERS,
	
	// Hub window -> Status
	STR_STATUS_IDLE,
	STR_STATUS_CONNECTING,
	STR_STATUS_CONNECTED,
	STR_STATUS_CONNECT_ERROR,
	STR_STATUS_SEND_ERROR,
	STR_STATUS_RECV_ERROR,
	STR_STATUS_NUM_SERVERS,
	
	// Main window -> Menus
	STR_MENU_FILE,
	STR_MENU_FILE_ABOUT,
	STR_MENU_FILE_CLOSE,
	STR_MENU_EDIT,
	STR_MENU_EDIT_PREFS,
	STR_MENU_WINDOWS,
	STR_MENU_WINDOWS_HUB,
	
	// Prefernces window
	STR_PREFS_TITLE,
	STR_PREFS_GENERAL,
	STR_PREFS_GENERAL_PERSONAL,
	STR_PREFS_GENERAL_CONNECTION_SETTINGS,
	STR_PREFS_GENERAL_NICK,
	STR_PREFS_GENERAL_EMAIL,
	STR_PREFS_GENERAL_DESC,
	STR_PREFS_GENERAL_CONNECTION,
	STR_PREFS_GENERAL_ACTIVE,
	STR_PREFS_GENERAL_PASSIVE,
	STR_PREFS_GENERAL_IP,
	STR_PREFS_GENERAL_PORT,
	
	// For the user list view
	STR_VIEW_NAME = STR_SERVER_NAME,	// Reuse ;)
	STR_VIEW_SPEED = STR_PREFS_GENERAL_PORT + 1,
	STR_VIEW_DESC,
	STR_VIEW_EMAIL,
	STR_VIEW_SHARED,
	STR_VIEW_CHAT,
	
	STR_OK,
	STR_CANCEL,
	
	// Messages
	STR_MSG_SYSTEM,
	STR_MSG_ERROR,
	STR_MSG_CONNECTING_TO,
	STR_MSG_CONNECTED,
	STR_MSG_CONNECT_ERROR,
	STR_MSG_RECONNECTING,
	STR_MSG_DISCONNECTED_FROM_SERVER,
	STR_MSG_INVALID_NICK,
	STR_MSG_USER_LOGGED_IN,
	
	STR_NUM	 // Place holder
};

// Key shortcuts for menu items
enum
{
	KEY_FILE_ABOUT = 0,
	KEY_FILE_CLOSE,
	KEY_EDIT_PREFS,
	KEY_WINDOWS_HUB,
	KEY_NUM
};

enum
{
	DC_LANG_ENGLISH = 0,
	DC_LANG_NUM		// Place holder
};

const char * DCStr(int);
char DCKey(int);
void DCSetLanguage(int);
BString DCUTF8(const char * str);
BString DCMS(const char * str);

#endif	// _DC_STRINGS_H_
