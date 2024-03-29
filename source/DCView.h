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


#ifndef _DC_VIEW_H
#define _DC_VIEW_H_

#include "ObjectList.h"

#include <View.h>
#include <Messenger.h>
#include <String.h>

class DCSettings;
class DCConnection;
class DCUser;
class WrappingTextView;

class BTextView;
class BTextControl;
class BColumnListView;
class BScrollView;
class BMessageRunner;

enum
{
	// A user has logged off or no... so this message is
	// sent to the target to update it's counters, whatever.
	// in DCWindow's case, it updates it's status bar
	//	'view'		pointer		--> The sender
	DC_MSG_VIEW_UPDATE_USERS = 'mVuU'
};

class DCView : public BView
{
	typedef BObjectList<DCUser> UserList;
	
public:
							DCView(DCSettings * settings, BMessenger target, BRect pos);
	virtual					~DCView();
	
	virtual void			AttachedToWindow();
	virtual void			MessageReceived(BMessage * msg);
	virtual bool			QuitRequested();
	
	// Get a pointer to the updated settings
	void					UpdateSettings(DCSettings * set);
	void					Connect(const BString & host, int port = 411);
	void					Disconnect();
	
	int32					GetNumUsers() const { return fUserList.CountItems(); }
	BString					AutoCompleteNick(const BString & txt);
	BTextControl *			GetInputControl() const { return fInput; }
	DCConnection *			GetConnection() const { return fConn; }
	
	void					UpdateLanguage();
	
	static BString			GetConnectionText(int val);
	
private:
	BMessenger				fTarget;
	DCSettings *			fSettings;
	DCConnection *			fConn;
	
	WrappingTextView *		fText;
	BScrollView *			fScrollText;
	BColumnListView *		fUsers;
	BTextControl *			fInput;
	BMessageRunner *		fRunner;	// for user list updating
	
	UserList				fUserList;
	BString					fHost;	// For reconnect
	int						fPort;
	bool					fClosing;	// Are we manually closing the connection?
	
	void					InitGUI();
	void					LogSystem(const BString & msg);
	void					LogError(const BString & msg);
	void					LogUserLoggedIn(const BString & nick);
	void					LogUserLoggedOut(const BString & nick);
	void					LogChatMessage(const BString & nick, const BString & text, bool remote);
	void					LogPrivateChat(const BString & from, const BString & text, bool remote);
	
	void					PrintSystem();	// Print the "Error: " part
	void					PrintError();	// Print the "System: " part
	void					PrintText(BString str, bool newLine = true, int optColor = -1);	// We need the copy of the string
	
	void					ParseNickList(BString list);	// Yes, we need a copy of the list
	void					CreateNewUser(const BString & nick);
	void					RemoveUser(const BString & nick);
	void					EmptyUserList();
	void					UpdateUser(const BString & nick, const BString & desc, const BString & speed,
									   const BString & email, int64 size);
	DCUser *				FindUser(const BString & nick, bool caseSen = true);	// Find user by name
	
	void					SendInfoToServer(bool update = false, bool send = true);
	void					GetHostAndPort(BString & host, int & port);
	
	bool					GetScrollState();
	void					ScrollToBottom();
	
	void					ParseSendText();
	void					SendChat(const BString & text);
	
	void					SendUpdateNotification();
};


#endif /* !_DC_VIEW_H_ */
