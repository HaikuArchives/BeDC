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
#ifndef _DC_HUB_WINDOW_H_
#define _DC_HUB_WINDOW_H_


#include <Window.h>
#include <Messenger.h>

#include "DCHTTPConnection.h"

class BView;
#ifdef NO_CRASH_LIST
class DCHubListView;
class CLVContainerView;
#else
class BColumnListView;
#endif
class BButton;
class BStringView;

enum
{
	// Sent by the hub window when it has been closed
	DC_MSG_HUB_CLOSED = 'dcHC',
	// Sent after a click on Connect is handled
	//	Fields:
	//		'name'		String
	//		'addr'		String
	//		'desc'		String
	DC_MSG_HUB_CONNECT = 'dcHT'
};

class DCHubWindow : public BWindow
{
public:
						// Create a hub window with a target for messages
						DCHubWindow(BMessenger target);
	virtual 			~DCHubWindow();
	
	virtual void		MessageReceived(BMessage * msg);
	virtual bool		QuitRequested();
	
	void				SetMessageTarget(BMessenger target) { fTarget = target; }
	
private:
	BView *				fView;
#ifdef NO_CRASH_LIST
	DCHubListView *	fHubView;
	CLVContainerView *	fHubViewContainer;
#else
	BColumnListView *	fHubView;
#endif
	BView * 			fButtonView;	// for kewl resizing ;)
	BButton *			fConnect;
	BButton *			fRefresh;
	BButton *			fPrev;
	BButton *			fNext;
	BStringView *		fStatus;
	
	BMessenger			fTarget;
	DCHTTPConnection *	fList;
	
	int					fOffset;
	int					fNextOffset;
	list<DCHTTPConnection::Hub *> * fHubs;
	
	void				InitGUI();
	void				AddItem(DCHTTPConnection::Hub * hub)
		{ AddItem(hub->fName, hub->fServer, hub->fDesc, hub->fUsers);}
	void				AddItem(const BString & name, const BString & addr,
								const BString & desc, uint32 users);
	void				HandleDisconnect();
	void				RestoreList();
	void				ListSomeItems();
	void				CleanUpConnection();
};

#endif	// _DC_HUB_WINDOW_H_
