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
#include "DCPrefs.h"
#include "DCStrings.h"
#include "DCSettings.h"

#include <View.h>
#include <ListView.h>
#include <ScrollView.h>
#include <Button.h>
#include <Box.h>
#include <ListItem.h>
#include <TextControl.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <RadioButton.h>

enum
{
	DCP_OK = 'dPoK',
	DCP_CANCEL = B_QUIT_REQUESTED,
	DCP_ACTIVE = 'dPaC',		// Active radio button
	DCP_PASSIVE = 'dPpC',		// Passive radio button
	DCP_OPTION_CHANGED = 'dPoC'	// Option changed
};

DCPrefs::DCPrefs(BMessenger target, BPoint pos)
	: BWindow(BRect(pos.x, pos.y, pos.x + 450, pos.y + 335), DCStr(STR_PREFS_TITLE), 
			  B_TITLED_WINDOW, B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE)
{
	fTarget = target;
	fSettings = NULL;
	fOKPressed = false;
	InitGUI();
}

DCPrefs::~DCPrefs()
{
	delete fSettings;
}

void
DCPrefs::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DCP_OK:
		{
			fOKPressed = true;
			PostMessage(B_QUIT_REQUESTED);
			break;
		}
			
		case DCP_ACTIVE:
		{
			fActive->SetValue(1);
			fIP->SetEnabled(true);
			fPort->SetEnabled(true);
			break;
		}
			
		case DCP_PASSIVE:
		{
			fPassive->SetValue(1);
			fIP->SetEnabled(false);
			fPort->SetEnabled(false);
			break;
		}
		
		case DCP_OPTION_CHANGED:
		{
			if (fOptions->CurrentSelection() == 0)
			{
				fGeneral->Show();
			}
			else
			{
				fGeneral->Hide();
			}
			break;
		}
		
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

bool
DCPrefs::QuitRequested()
{
	BMessage msg(DC_MSG_PREFS_CLOSED);
	msg.AddPoint("point", BPoint(Frame().left, Frame().top));
	if (fOKPressed)
	{
		fSettings->SetString(DCS_PREFS_NICK, fNick->Text());
		fSettings->SetString(DCS_PREFS_EMAIL, fEmail->Text());
		fSettings->SetString(DCS_PREFS_DESC, fDescription->Text());
		fSettings->SetInt(DCS_PREFS_CONNECTION, fConnMenu->IndexOf(fConnMenu->FindMarked()));
		fSettings->SetBool(DCS_PREFS_ACTIVE, (fActive->Value() == 0) ? false : true);
		fSettings->SetString(DCS_PREFS_PORT, fPort->Text());
		fSettings->SetString(DCS_PREFS_IP, fIP->Text());
		msg.AddMessage("prefs", fSettings);
	}
	fTarget.SendMessage(&msg);
	return true;
}

void
DCPrefs::InitSettings(DCSettings * set)
{
	if (fSettings)
	{
		delete fSettings;
		fSettings = NULL;
	}
	if (set)
	{
		BString str;
		int32 val = 0;
		bool bval = false;
		
		fSettings = new DCSettings(*set);
		if (fSettings->GetString(DCS_PREFS_NICK, &str) == B_OK)
			fNick->SetText(str.String());
		if (fSettings->GetString(DCS_PREFS_EMAIL, &str) == B_OK)
			fEmail->SetText(str.String());
		if (fSettings->GetString(DCS_PREFS_DESC, &str) == B_OK)
			fDescription->SetText(str.String());
		if (fSettings->GetInt(DCS_PREFS_CONNECTION, val) == B_OK)
		{
			switch (val)
			{
				case 0:
					fConnMenu->FindItem("28.8Kbps")->SetMarked(true);
					break;
				
				case 1:
					fConnMenu->FindItem("33.6Kbps")->SetMarked(true);
					break;
					
				case 2:
					fConnMenu->FindItem("56Kbps")->SetMarked(true);
					break;
					
				case 3:
					fConnMenu->FindItem("ISDN")->SetMarked(true);
					break;
					
				case 4:
					fConnMenu->FindItem("DSL")->SetMarked(true);
					break;
					
				case 5:
					fConnMenu->FindItem("Cable")->SetMarked(true);
					break;
					
				case 6:
					fConnMenu->FindItem("LAN(T1)")->SetMarked(true);
					break;
					
				case 7:
					fConnMenu->FindItem("LAN(T3)")->SetMarked(true);
					break;
					
				case 8:
					fConnMenu->FindItem("Satellite")->SetMarked(true);
					break;
			}
		}
		if (fSettings->GetBool(DCS_PREFS_ACTIVE, bval) == B_OK)
		{
			if (bval)
				PostMessage(DCP_ACTIVE);
			else
				PostMessage(DCP_PASSIVE);
		}
		if (fSettings->GetString(DCS_PREFS_IP, &str) == B_OK)
			fIP->SetText(str.String());
		if (fSettings->GetString(DCS_PREFS_PORT, &str) == B_OK)
			fPort->SetText(str.String());
	}
}

void
DCPrefs::InitGUI()
{
	AddChild(
		fView = new BView(Bounds(), "the_view", B_FOLLOW_NONE, B_WILL_DRAW)
	);
	fView->SetViewColor(216, 216, 216);
	
	fOptions = new BListView(BRect(0, 0, 100, Bounds().Height() - 35),
							 "options_list", B_SINGLE_SELECTION_LIST, B_FOLLOW_NONE);
	fView->AddChild(
		fScrollOptions = new BScrollView("scroll_options_list", fOptions, B_FOLLOW_NONE, 0, false,
										 true, B_FANCY_BORDER)
	);
	
	fView->AddChild(
		fContainer = new BBox(BRect(fOptions->Bounds().Width() + B_V_SCROLL_BAR_WIDTH + 6, 3, 
							  Bounds().Width() - 3, Bounds().Height() - 33))
	);		
	
	fOptions->AddItem(
		new BStringItem(DCStr(STR_PREFS_GENERAL))
	);
	fOptions->SetSelectionMessage(new BMessage(DCP_OPTION_CHANGED));
	
	BRect bounds = fContainer->Bounds();
	// Create general view
	fGeneral = new BView(bounds, "general_view", B_FOLLOW_NONE, B_WILL_DRAW);
	fGeneral->SetViewColor(216, 216, 216);
	
	fGeneral->AddChild(
		fGeneralPersonal = new BBox(BRect(0, 0, bounds.Width() - 1, 200))
	);
	fGeneralPersonal->SetLabel(DCStr(STR_PREFS_GENERAL_PERSONAL));
	fGeneral->AddChild(
		fGeneralConnection = new BBox(BRect(0, 205, bounds.Width() - 1, bounds.Height() - 1))
	);
	fGeneralConnection->SetLabel(DCStr(STR_PREFS_GENERAL_CONNECTION_SETTINGS));
	
	// Insert items into personal
	fGeneralPersonal->AddChild(
		fNick = new BTextControl(BRect(5, 30, fGeneralPersonal->Bounds().Width() - 5, 0), "general_nick",
								 DCStr(STR_PREFS_GENERAL_NICK), "binky", NULL)
	);
	fNick->SetDivider(100);
	fGeneralPersonal->AddChild(
		fEmail = new BTextControl(BRect(5, fNick->Frame().bottom + 20, 
								  fGeneralPersonal->Bounds().Width() - 5, 0), "general_email",
								  DCStr(STR_PREFS_GENERAL_EMAIL), "", NULL)
	);
	fEmail->SetDivider(100);
	fGeneralPersonal->AddChild(
		fDescription = new BTextControl(BRect(5, fEmail->Frame().bottom + 20,
										fGeneralPersonal->Bounds().Width() - 5, 0), "general_desc",
										DCStr(STR_PREFS_GENERAL_DESC), "", NULL)
	);
	fDescription->SetDivider(100);
	// Create menu first
	fConnMenu = new BPopUpMenu("");
	fConnMenu->AddItem(new BMenuItem("28.8Kbps", NULL));
	fConnMenu->AddItem(new BMenuItem("33.6Kbps", NULL));
	fConnMenu->AddItem(new BMenuItem("56Kbps", NULL));
	fConnMenu->AddItem(new BMenuItem("ISDN", NULL));
	fConnMenu->AddItem(new BMenuItem("DSL", NULL));
	fConnMenu->AddItem(new BMenuItem("Cable", NULL));
	fConnMenu->AddItem(new BMenuItem("LAN(T1)", NULL));
	fConnMenu->AddItem(new BMenuItem("LAN(T3)", NULL));
	fConnMenu->AddItem(new BMenuItem("Satellite", NULL));
	fConnMenu->FindItem("28.8Kbps")->SetMarked(true);
	
	fGeneralPersonal->AddChild(
		fConnection = new BMenuField(BRect(5, fDescription->Frame().bottom + 20,
									 fGeneralPersonal->Bounds().Width() - 5, 0), "general_connection",
									 DCStr(STR_PREFS_GENERAL_CONNECTION), fConnMenu, false)
	);
	fConnection->SetDivider(100);
	
	// Insert items into connection
	fGeneralConnection->AddChild(
		fActive = new BRadioButton(BRect(5, 25, 70, 0), "conn_active", DCStr(STR_PREFS_GENERAL_ACTIVE),
								   new BMessage(DCP_ACTIVE))
	);
	fGeneralConnection->AddChild(
		fIP = new BTextControl(BRect(75, 25, 225, fActive->Frame().bottom), "conn_ip", 
							   DCStr(STR_PREFS_GENERAL_IP), "", NULL)
	);
	float top = 25 - (fActive->Frame().Height() / 2 - fIP->Frame().Height() / 2);
	fIP->MoveTo(75, top);
	fIP->SetDivider(25);
	fGeneralConnection->AddChild(
		fPort = new BTextControl(BRect(fIP->Frame().right + 5, fIP->Frame().top, 310, 
								 fIP->Frame().bottom), "conn_port", DCStr(STR_PREFS_GENERAL_PORT),
								 "", NULL)
	);
	fPort->SetDivider(30);
	
	fIP->SetEnabled(false);
	fPort->SetEnabled(false);
	
	fGeneralConnection->AddChild(
		fPassive = new BRadioButton(BRect(5, fActive->Frame().bottom + 7, 200, 0), "conn_passive",
									DCStr(STR_PREFS_GENERAL_PASSIVE), new BMessage(DCP_PASSIVE))
	);
	
	// Add all to container
	fContainer->AddChild(fGeneral);
	fGeneral->Hide();
	
	// Buttons
	int middle = (int)Bounds().Width() >> 1;
	
	fView->AddChild(
		fOK = new BButton(BRect(middle - 75, fContainer->Frame().bottom + 5, middle - 5, 
						  Bounds().Height() - 9), "ok_button", DCStr(STR_OK), new BMessage(DCP_OK))
	);
	fView->AddChild(
		fCancel = new BButton(BRect(middle + 5, fContainer->Frame().bottom + 3, middle + 75,
							  Bounds().bottom - 5), "cancel_buttom", DCStr(STR_CANCEL), 
							  new BMessage(DCP_CANCEL))
	);
	fOK->MakeDefault(true);
}
