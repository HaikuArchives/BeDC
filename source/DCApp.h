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


#ifndef _DC_APP_H_
#define _DC_APP_H_

#include "ObjectList.h"

class DCWindow;
class DCSettings;
class DCDownloadQueue;
class DCHubWindow;

class BString;

typedef BObjectList<DCWindow *> DCWindowList;
 
enum
{
	// Send this message to the app and it will show
	// the hub window, or bring it to focus if it is
	// already visible
	DC_MSG_APP_SHOW_HUB_LIST = 'dcHL'
};

class DCApp : public BApplication
{
public:
							DCApp();
	virtual					~DCApp();
						
	virtual bool 			QuitRequested();
	virtual void			MessageReceived(BMessage * msg);
	virtual void			ReadyToRun();
	
	DCSettings *			GetSettings() { return fSettings; }
	DCDownloadQueue *		GetQueue() { return fQueue; }
	
protected:
	DCDownloadQueue * 		fQueue	/*theQueue*/;
	DCSettings *			fSettings /*theSettings*/;
	DCWindowList *			fWindowList;
	DCHubWindow *			fHubWindow;
	
	void					ShowHubWindow();
};

extern DCApp * dc_app;


#endif /* !_DC_APP_H_ */
