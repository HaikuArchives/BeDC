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
#include "DCView.h"
#include "DCStatusBar.h"

#include <View.h>
#include <Message.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ListView.h>
#include <ScrollView.h>
#include <ListItem.h>

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
	// Empty out our view list
	while (fViewList.CountItems() > 0)
	{
		Container * item = fViewList.RemoveItemAt(0);
		delete item;
	}
}

void
DCWindow::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DC_MSG_APP_OPEN_NEW_HUB:
		{
			BString n, a, d;
			if (msg->FindString("name", &n) == B_OK &&
				msg->FindString("addr", &a) == B_OK &&
				msg->FindString("desc", &d) == B_OK)
				OpenNewConnection(n, a, d);
			break;
		}
		
		case DCW_ABOUT:
			break;
			
		case DCW_PREFS:
			dc_app->PostMessage(DC_MSG_APP_SHOW_PREFS);
			break;
		
		case DCW_HUBS:
			dc_app->PostMessage(DC_MSG_APP_SHOW_HUB_LIST);
			break;
		
		case DCW_HUB_CHANGED:
		{
			if (fHubs->CurrentSelection() >= 0)
			{
				HideAll();
				ShowItem(fHubs->ItemAt(fHubs->CurrentSelection()));
			}
			break;
		}
			
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

void
DCWindow::OpenNewConnection(const BString & name, const BString & addr, const BString & desc)
{
	Container * con = NULL;
	
	if ((con = FindItem(name)) != NULL)	// Don't accept duplicate connections
	{
		// Duplicate? show it, and activate the widnow
		if (con->fView->IsHidden())
		{
			HideAll();
			con->fView->Show();
			fHubs->Select(fHubs->IndexOf(con->fListItem));
		}
		Activate(true);
		return;
	}

	con = new Container;
	con->fServerName = name;
	con->fServerAddr = addr;
	con->fServerDesc = desc;
	con->fView = new DCView(dc_app->GetSettings(), BMessenger(this), BRect(167, 2, 
				   			fParentView->Frame().right - 2, fParentView->Bounds().Height() - 20));
	con->fListItem = new BStringItem(name.String());
	// Our view starts out hidden
	con->fView->Hide();	
	fParentView->AddChild(con->fView);
	// Insert into list
	fViewList.AddItem(con);
	// Insert into the list view
	fHubs->AddItem(con->fListItem, fHubs->CountItems());
	// Hide all of our current windows
	HideAll();
	// Show our new connection only
	ShowItem(con->fListItem);
	con->fView->Connect(addr);
	
	Activate(true);
}

// Returns NULL if it can't find the item
DCWindow::Container *
DCWindow::FindItem(BListItem * item)
{
	for (int32 i = 0; i < fViewList.CountItems(); i++)
	{
		Container * it = fViewList.ItemAt(i);
		if (it->fListItem == item)
			return it;
	}
	return NULL;
}

// Returns NULL if item is not found
DCWindow::Container *
DCWindow::FindItem(const BString & name)
{
	for (int32 i = 0; i < fViewList.CountItems(); i++)
	{
		Container * it = fViewList.ItemAt(i);
		if (it->fServerName == name)
			return it;
	}
	return NULL;
}

void
DCWindow::HideAll()
{
	// Go through each item, and if it's not hidden, hide it
	for (int32 i = 0; i < fViewList.CountItems(); i++)
	{
		Container * item = fViewList.ItemAt(i);
		if (!item->fView->IsHidden())
			item->fView->Hide();
	}
}

void
DCWindow::ShowItem(BListItem * item)
{
	// Find our item
	Container * it = FindItem(item);
	if (it && it->fView->IsHidden() == true) // Must be hidden... otherwise we don't need to show it
		it->fView->Show();
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
	
	// TEST
//	fParentView->AddChild(
//		new DCView(dc_app->GetSettings(), BMessenger(this), BRect(167, 2, 
//				   fParentView->Frame().right - 2, fParentView->Bounds().Height() - 20))
//	);
	fParentView->AddChild(fStatusBar = new DCStatusBar(fParentView->Bounds(),16,STATUS_WINDOZE_STYLE));

}
