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
#include "DCSettings.h"
#include "DCConnection.h"

#include <View.h>
#include <Message.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <ListView.h>
#include <ScrollView.h>
#include <ListItem.h>
#include <PopUpMenu.h>
#include <Alert.h>
#include <TextView.h>
#include <TextControl.h>

enum
{
	// Menu
	DCW_ABOUT = B_ABOUT_REQUESTED,
	DCW_CLOSE = B_QUIT_REQUESTED,	// heheheh :)
	DCW_PREFS = 'dWpS',
	DCW_HUBS = 'dwHs',
	DCW_HUB_CHANGED = 'dWhC',	// hub list selelction message
	DCW_CLOSE_HUB = 'dWcH'		// close a hub
};

class DCWindowListView : public BListView
{
public:
							DCWindowListView(BRect frame, const char * name, list_view_type type = 
											 B_SINGLE_SELECTION_LIST, uint32 resizeMask = B_FOLLOW_LEFT |
											 B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS |
											 B_NAVIGABLE)
								: BListView(frame, name, type, resizeMask, flags)
							{
							}
	virtual 				~DCWindowListView() {}
	
	virtual void			MouseDown(BPoint where)
	{
		BPoint p = where;
		uint32 buttons;
		GetMouse(&p, &buttons);
		if (buttons & B_SECONDARY_MOUSE_BUTTON)
		{
			if (CurrentSelection() >= 0)	// don't popup if we're not selected
			{
				BPopUpMenu * menu = new BPopUpMenu("popup");
				menu->AddItem(new BMenuItem(DCStr(STR_VIEW_CLOSE), NULL));
				
				ConvertToScreen(&p);
				if (menu->Go(p, false, false, false))	// aha
				{
					Looper()->PostMessage(DCW_CLOSE_HUB);
				}
			}
		}
		
		BListView::MouseDown(where);		
	}
};

///////////////////////////////////////////////////////////////////////////////////
// DCWindow
///////////////////////////////////////////////////////////////////////////////////
DCWindow::DCWindow(BRect pos)
	: BWindow(pos, DC_WINDOW_TITLE /* i don't think the name can be localized ;) */,
			  B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_OUTLINE_RESIZE)
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
		case DCW_CLOSE_HUB:
		{
			if (fHubs->CurrentSelection() >= 0)
			{
				Container * c = FindItem(fHubs->ItemAt(fHubs->CurrentSelection()));
				fViewList.RemoveItem(c);
				c->fView->Disconnect();
				fHubs->RemoveItem(c->fListItem);
				fParentView->RemoveChild(c->fView);
				delete c->fView;
				delete c->fListItem;
				delete c;
				
				SetTitle(DC_WINDOW_TITLE);
				
				fStatusBar->MakeEmpty();
				fStatusBar->Invalidate();
				
				ShowFirstHub();
			}
			break;
		}
		
		case DC_MSG_CLOSE_VIEW:
		{
			DCView * v = NULL;
			if (msg->FindPointer("view", (void **)&v) == B_OK)
			{
				Container * c = FindItem(v);
				if (c)
				{
					fViewList.RemoveItem(c);
					c->fView->Disconnect();
					fHubs->RemoveItem(c->fListItem);
					fParentView->RemoveChild(c->fView);
					delete c->fView;
					delete c->fListItem;
					delete c;
					
					SetTitle(DC_WINDOW_TITLE);
					
					fStatusBar->MakeEmpty();
					fStatusBar->Invalidate();

					ShowFirstHub();
				}
			}
			break;
		}
		
		case DC_MSG_APP_OPEN_NEW_HUB:
		{
			BString n, a, d;
			if (msg->FindString("name", &n) == B_OK &&
				msg->FindString("addr", &a) == B_OK &&
				msg->FindString("desc", &d) == B_OK)
				OpenNewConnection(n, a, d);
			break;
		}
		
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
		
		// Update our settings
		case DC_MSG_APP_NEW_SETTINGS:
		{
			Container * c = NULL;
			for (int32 i = 0; i < fViewList.CountItems(); i++)
			{
				c = fViewList.ItemAt(i);
				c->fView->UpdateSettings(dc_app->GetSettings());
			}
			break;
		}
		
		case DC_MSG_VIEW_UPDATE_USERS:
		{
			DCView * v = NULL;
			if (msg->FindPointer("view", (void **)&v) == B_OK)
			{
				if (!v->IsHidden())
				{
					BString users = DCStr(STR_USERS);
					users += ": ";
					users << v->GetNumUsers();
					fStatusBar->SetField(users, 4);
					fStatusBar->Invalidate();
				}
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

	// Check for a valid nick name
	if (dc_app->GetSettings()->GetString(DCS_PREFS_NICK) == "")
	{
		(new BAlert(DCStr(STR_ERROR), DCStr(STR_ALERT_BAD_NICK), DCStr(STR_OK), NULL, NULL, 
					B_WIDTH_AS_USUAL, B_IDEA_ALERT))->Go();
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
	
	// Connect
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

// Returns NULL if the item is not found
DCWindow::Container *
DCWindow::FindItem(DCView * item)
{
	for (int32 i = 0; i < fViewList.CountItems(); i++)
	{
		Container * it = fViewList.ItemAt(i);
		if (it->fView == item)
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
	SetTitle(DC_WINDOW_TITLE);
}

void
DCWindow::ShowItem(BListItem * item)
{
	// Find our item
	Container * it = FindItem(item);
	if (it && it->fView->IsHidden() == true) // Must be hidden... otherwise we don't need to show it
	{
		it->fView->Show();
		fHubs->Select(fHubs->IndexOf(it->fListItem));
		
		BString title = DC_WINDOW_TITLE;
		title += " - ";
		title += it->fServerName;
		SetTitle(title.String());
		
		// Update the status are
		fStatusBar->MakeEmpty();

		fStatusBar->AddField(it->fView->GetConnection()->GetNick());
		fStatusBar->AddField(it->fServerName);
		fStatusBar->AddField(it->fServerAddr);
		fStatusBar->AddField(it->fServerDesc);
		
		BString users = DCStr(STR_USERS);
		users += ": ";
		users << it->fView->GetNumUsers();
		fStatusBar->AddField(users);
		
		fStatusBar->Invalidate();
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
	BMenuItem * tmp;
	
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
		tmp = new BMenuItem(DCStr(STR_MENU_FILE_ABOUT), new BMessage(DCW_ABOUT), DCKey(KEY_FILE_ABOUT))
	);
	tmp->SetTarget(be_app_messenger);
	
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
	fHubs = new DCWindowListView(BRect(0, 0, 150, fParentView->Bounds().Height() - 20),
						  		 "hub_list", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);
	fHubs->SetSelectionMessage(new BMessage(DCW_HUB_CHANGED));
	
	fParentView->AddChild(
		fScrollHubs = new BScrollView("scoll_hub_list", fHubs, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, 0,
									  false, true, B_FANCY_BORDER)
	);
	fScrollHubs->SetViewColor(216, 216, 216);
	
	fParentView->AddChild(
		fStatusBar = new DCStatusBar(fParentView->Bounds(), 18)
	);
}

void
DCWindow::DispatchMessage(BMessage * msg, BHandler * target)
{
	switch (msg->what)
	{
		case B_KEY_DOWN:
		{
			int8 c;
			int32 mod;
			
			if (msg->FindInt8("byte", &c) == B_OK &&
				msg->FindInt32("modifiers", &mod) == B_OK)
			{
				BTextView * tv = dynamic_cast<BTextView *>(target);
				if (tv && tv->IsEditable())	// aha our target is a text view....
				{
					if (c == B_TAB)	// Eat the tab
					{
						DCView * v = NULL;
						
						if ((mod == 0) && (tv->TextLength() > 0) && ((v = FindOwner(tv)) != NULL))
						{
							BString text = tv->Text();
							int32 i = text.FindLast(" ");	// Look for a space..
							if (i == B_ERROR)	// no space? use the whole string
							{
								text = v->AutoCompleteNick(text);
							}
							else
							{
								BString cut;
								text.MoveInto(cut, i + 1, text.Length() - (i + 1));
								if (cut != "")
								{
									cut = v->AutoCompleteNick(cut);
									text += cut;
								}
							}
							tv->SetText(text.String());
							tv->Select(text.Length(), text.Length());
							return;
						}
						else if (tv->TextLength() != 0) // but only we have text in the view, and modifiers aren't down
						{
							if ((mod & (B_CONTROL_KEY)) == 0)	
								return;
						}
					}
				}
			}
			break;
		}
	}
	
	// Let the window handle it
	BWindow::DispatchMessage(msg, target);
}


DCView *
DCWindow::FindOwner(BTextView * tv)
{
	DCView * ret = NULL;
	
	for (int32 i = 0; i < fViewList.CountItems(); i++)
	{
		Container * c = fViewList.ItemAt(i);
		if (c->fView->GetInputControl()->TextView() == tv)
		{
			ret = c->fView;
			break;
		}
	}
	return ret;
}

// Show the first hub in the list.... IF we have any open
void
DCWindow::ShowFirstHub()
{
	if (fViewList.CountItems() > 0)
	{
		HideAll();
		Container * c = fViewList.ItemAt(0);
		ShowItem(c->fListItem);
	}
}
