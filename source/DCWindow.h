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
#ifndef _DC_WINDOW_H_
#define _DC_WINDOW_H_

#include "ObjectList.h"

#include <Window.h>
#include <String.h>
#include <ListView.h>

class BView;
class BMenuBar;
class BMenu;
class BListView;
class BTextView;
class BScrollView;
class BStringItem;
class DCStatusBar;
class DCView;
class DCWindowListView;	// List view that handles right clicks

#define DC_WINDOW_TITLE DC_APP_NAME

enum
{
	// The window was closed
	//	'rect'	BRect	--> The position of the window
	DC_MSG_WINDOW_CLOSED = 'msWC',
	// DCView will send this to the window on a /close
	// command
	//	'view'	pointer	--> The view to close
	DC_MSG_CLOSE_VIEW = 'msCV'
};

class DCWindow : public BWindow
{
public:
						DCWindow(BRect pos = BRect(30, 70, 730, 530));
	virtual				~DCWindow();

	virtual void		MessageReceived(BMessage * msg);
	virtual void		DispatchMessage(BMessage * msg, BHandler * target);
	virtual bool		QuitRequested();

private:
	BView *				fParentView;	// The main view everything gets slapped into
	BMenuBar *			fMenuBar;
	BMenu *				fFileMenu;
	BMenu *				fEditMenu;
	BMenu *				fEditLangMenu;
	BMenu *				fWindowsMenu;
	BScrollView *		fScrollHubs;
	DCWindowListView *	fHubs;
	DCStatusBar *		fStatusBar;		// The status bar (Doh..)

	struct Container
	{
		BString 		fServerName;
		BString 		fServerAddr;
		BString 		fServerDesc;
		DCView *		fView;
		BStringItem *	fListItem;
	};
	typedef BObjectList<Container> ViewList;

	ViewList			fViewList;

	void				InitGUI();
	void				OpenNewConnection(const BString & name, const BString & addr,
										  const BString & desc);
	void				HideAll();
	void				ShowItem(BListItem * item);
	void				ShowFirstHub();

	Container *			FindItem(BListItem * item);
	Container *			FindItem(const BString & name);	// find item by server name
	Container *			FindItem(DCView * item);
	// Find which view owns the following text view
	// This is a helper for DispatchMessage()
	// for tab-completion
	DCView *			FindOwner(BTextView * tv);

	void				InitializeLanguage(int lang);
};

#endif	// _DC_WINDOW_H_
