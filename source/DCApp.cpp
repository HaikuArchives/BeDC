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

#include <Application.h>
#include <Window.h>
#include <Rect.h>
#include <Message.h>
#include <Messenger.h>
#include <File.h>

#include "DCApp.h"
#include "DCWindow.h"
#include "DCSettings.h"
#include "DCDownloadQueue.h"
#include "DCStrings.h"
#include "DCHubWindow.h"
#include "DCPrefs.h"

DCApp::DCApp()
	: BApplication("application/x-vnd.OSS-BeDC")
{
	DCSetLanguage(DC_LANG_ENGLISH);
	
//	fQueue = new DCDownloadQueue;
//	fQueue->Run();

	// Load our settings
	fSettings = new DCSettings;
	fSettings->LoadSettings();
	
	// No hub window yet
	fHubWindow = NULL;
	fWindow = NULL;
	fPrefsWindow = NULL;
}

DCApp::~DCApp()
{
	delete fSettings;
}

bool 
DCApp::QuitRequested()
{
	fSettings->SaveSettings();
	return true;
}

void
DCApp::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DC_MSG_PREFS_CLOSED:
		{
			BPoint p;
			if (msg->FindPoint("point", &p) == B_OK)
				fSettings->SetPoint(DCS_PREFS_POS, p);
			fPrefsWindow = NULL;
			
			break;
		}
		
		case DC_MSG_APP_SHOW_PREFS:
			ShowPrefsWindow();
			break;
			
		case DC_MSG_APP_SHOW_HUB_LIST:
			ShowHubWindow();
			break;
			
		case DC_MSG_HUB_CLOSED:
		{
			BRect rect;
			if (msg->FindRect("rect", &rect) == B_OK)
				fSettings->SetRect(DCS_HUB_RECT, rect);
				
			fHubWindow = NULL;
			if (fWindow == NULL)	// no windows either?
				PostMessage(B_QUIT_REQUESTED);
			break;
		}
		
		case DC_MSG_HUB_CONNECT:
		{
			EnsureWindowAllocated();
			BMessenger(fWindow).SendMessage(msg);	// hehehe :)
			break;
		}
		
		case DC_MSG_WINDOW_CLOSED:
		{
			BRect rect;
			if (msg->FindRect("rect", &rect) == B_OK)
				fSettings->SetRect(DCS_WINDOW_RECT, rect);
			fWindow = NULL;
			if (fHubWindow == NULL)	// no hub window either?
				PostMessage(B_QUIT_REQUESTED);
			break;
		}
		
		default:
			BApplication::MessageReceived(msg);
			break;
	}
}

void
DCApp::ReadyToRun()
{
	EnsureWindowAllocated();	// create the main window
}

void
DCApp::ShowHubWindow()
{
	if (!fHubWindow)
	{
		BRect rect;
		if (fSettings->GetRect(DCS_HUB_RECT, &rect) == B_OK)
			fHubWindow = new DCHubWindow(BMessenger(this), rect);
		else
			fHubWindow = new DCHubWindow(BMessenger(this));
	}
	fHubWindow->Show();
}

void
DCApp::EnsureWindowAllocated()
{
	if (!fWindow)
	{
		BRect winRect;
		if (fSettings->GetRect(DCS_WINDOW_RECT, &winRect) == B_OK)
			fWindow = new DCWindow(winRect);
		else
			fWindow = new DCWindow;	// default rect
		fWindow->Show();
	}
}

void
DCApp::ShowPrefsWindow()
{
	if (!fPrefsWindow)
	{
		BPoint p;
		if (fSettings->GetPoint(DCS_PREFS_POS, &p) == B_OK)
			fPrefsWindow = new DCPrefs(BMessenger(this), p);
		else
			fPrefsWindow = new DCPrefs(BMessenger(this));
		fPrefsWindow->InitSettings(fSettings);
	}
	fPrefsWindow->Show();
}
