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
#include "DCWindow.h"
#include "DCStrings.h"
#include "DCApp.h"

#include <View.h>
#include <Message.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ListView.h>
#include <ScrollView.h>

enum
{
	// Menu
	DCW_ABOUT = 'dWaB',
	DCW_CLOSE = B_QUIT_REQUESTED,	// heheheh :)
	DCW_PREFS = 'dWpS',
	DCW_HUBS = 'dwHs',
	DCW_HUB_CHANGED = 'dWhC',	// hub list selelction message
};

DCWindow::DCWindow(BRect pos)
	: BWindow(pos, "BeDC" /* i don't think the name can be localized ;) */,
			  B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	InitGUI();
}

DCWindow::~DCWindow()
{
}

void
DCWindow::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DCW_ABOUT:
			break;
			
		case DCW_PREFS:
			dc_app->PostMessage(DC_MSG_APP_SHOW_PREFS);
			break;
		
		case DCW_HUBS:
			dc_app->PostMessage(DC_MSG_APP_SHOW_HUB_LIST);
			break;
		
		case DCW_HUB_CHANGED:
			break;
			
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

bool
DCWindow::QuitRequested()
{
	BMessage msg(DC_MSG_WINDOW_CLOSED);
	msg.AddRect("rect", Frame());
	dc_app->PostMessage(&msg);
	return true;
}

void
DCWindow::InitGUI()
{
	// Create all the menus
	AddChild(
		fMenuBar = new BMenuBar(BRect(0, 0, Bounds().Width(), 1) /* automagically resized by menubar */,
								"menubar_title")
	);
	// File menu
	fMenuBar->AddItem(
		fFileMenu = new BMenu(DCStr(STR_MENU_FILE))
	);
	fFileMenu->AddItem(
		new BMenuItem(DCStr(STR_MENU_FILE_ABOUT), new BMessage(DCW_ABOUT), DCKey(KEY_FILE_ABOUT))
	);
	fFileMenu->AddItem(
		new BMenuItem(DCStr(STR_MENU_FILE_CLOSE), new BMessage(DCW_CLOSE), DCKey(KEY_FILE_CLOSE))
	);
	
	// Edit menu
	fMenuBar->AddItem(
		fEditMenu = new BMenu(DCStr(STR_MENU_EDIT))
	);
	fEditMenu->AddItem(
		new BMenuItem(DCStr(STR_MENU_EDIT_PREFS), new BMessage(DCW_PREFS), DCKey(KEY_EDIT_PREFS))
	);
	
	// Windows menu
	fMenuBar->AddItem(
		fWindowsMenu = new BMenu(DCStr(STR_MENU_WINDOWS))
	);
	fWindowsMenu->AddItem(
		new BMenuItem(DCStr(STR_MENU_WINDOWS_HUB), new BMessage(DCW_HUBS), DCKey(KEY_WINDOWS_HUB))
	);
	
	// Main ugly, grey view ;)
	AddChild(
		fParentView = new BView(BRect(0, fMenuBar->Bounds().Height(), Bounds().Width(),
								Bounds().Height()), "parent_view", B_FOLLOW_ALL, B_WILL_DRAW)
	);
	fParentView->SetViewColor(216, 216, 216);
	
	// Hub list
	fHubs = new BListView(BRect(0, 0, 150, fParentView->Bounds().Height() - 20),
						  "hub_list", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
	fHubs->SetSelectionMessage(new BMessage(DCW_HUB_CHANGED));
	
	fParentView->AddChild(
		fScrollHubs = new BScrollView("scoll_hub_list", fHubs, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, 0,
									  false, true, B_FANCY_BORDER)
	);
	fScrollHubs->SetViewColor(216, 216, 216);
}
