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
#ifndef _DC_PREFS_H_
#define _DC_PREFS_H_

#include <Window.h>
#include <Messenger.h>
#include <Point.h>

class BView;
class BListView;
class BScrollView;
class BButton;
class BBox;
class BTextControl;
class BMenuField;
class BPopUpMenu;
class BRadioButton;

class DCSettings;

enum
{
	// Prefs closed
	//	'point'		BPoint	--> Position
	DC_MSG_PREFS_CLOSED = 'dMpC',
	// Prefs OK'd
	//	'prefs'		BMessage --> New prefs
	DC_MSG_PREFS_UPDATE = 'dMpU'
};

class DCPrefs : public BWindow
{
public:
						DCPrefs(BMessenger target, BPoint pos = BPoint(60, 30));
	virtual				~DCPrefs();
	
	virtual void		MessageReceived(BMessage * msg);
	virtual bool		QuitRequested();

	void				InitSettings(DCSettings * set);
	
private:
	BMessenger 			fTarget;
	bool				fOKPressed;
	
	BView *				fView;		// main view
	BListView *			fOptions;
	BScrollView *		fScrollOptions;
	BBox *				fContainer;
	BButton *			fOK;
	BButton *			fCancel;
	
	// General
	BView *				fGeneral;
		BBox *			fGeneralPersonal;
		BTextControl *	fNick;
		BTextControl *	fEmail;
		BTextControl *	fDescription;
		BMenuField *	fConnection;
		BPopUpMenu *	fConnMenu;
		BBox *			fGeneralConnection;
		BRadioButton *	fActive;
		BRadioButton *	fPassive;
		BTextControl *	fIP;
		BTextControl *	fPort;
		
	DCSettings *		fSettings;	// a COPY of the original settings
	
	void				InitGUI();
};


#endif	// _DC_PREFS_H_
