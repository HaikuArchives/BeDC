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


#include "DCView.h"
#include "DCSettings.h"
#include "DCConnection.h"
#include "ColumnListView.h"
#include "ColumnTypes.h"
#include "DCUser.h"
#include "DCStrings.h"
#include "DCApp.h"

#include <TextView.h>
#include <TextControl.h>
#include <ScrollView.h>

#include <stdlib.h>

enum
{
	// Enter was pressed in the text input
	DCV_SEND_TEXT = 'cvST'
};

DCView::DCView(DCSettings * settings, BMessenger target, BRect pos)
	: BView(pos, "dcview", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(216, 216, 216);
	fSettings = settings;
	fTarget = target;
	
	InitGUI();
	fConn = new DCConnection(BMessenger(this));
}

DCView::~DCView()
{
	Disconnect();
	fConn->Lock();
	fConn->Quit();
}

void
DCView::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		default:
			BView::MessageReceived(msg);
			break;
	}
}

bool
DCView::QuitRequested()
{
	return true;	// Just this for now...
}

void
DCView::UpdateSettings(DCSettings * set)
{
	fSettings = set;
	SendInfoToServer(true);
}

void
DCView::Connect(const BString & host, int port)
{
	BString h = host;
	BString realHost;
	int32 i;
	
	// Parse the host for a server:port type address
	if ((i = h.FindFirst(":")) != B_ERROR)
	{
		h.MoveInto(realHost, 0, i);
		h.RemoveFirst(":");
		if (h.Length() > 0)
			port = atoi(h.String());
	}
	else
		realHost = h;
	BString msg = DCStr(STR_MSG_CONNECTING_TO);
	msg += realHost;
	msg += ":";
	msg << port;
	msg += ".";
	LogSystem(msg);
	// TODO, actually connect ;^)
}

// If update is true, fSettings is read to get the updated nick, etc
void
DCView::SendInfoToServer(bool update)
{
	if (update)
	{
		BString str;
		int32 i;
		bool b;
		
		if (fSettings->FindString(DCS_PREFS_NICK, &str) == B_OK)
			fConn->SetNick(str);
		else
			fConn->SetNick("binky");
		
		if (fSettings->GetString(DCS_PREFS_EMAIL, &str) == B_OK)
			fConn->SetEmail(str);
		if (fSettings->GetString(DCS_PREFS_DESC, &str) == B_OK)
			fConn->SetDescription(str);
		if (fSettings->GetInt(DCS_PREFS_CONNECTION, i) == B_OK)
			fConn->SetSpeed(GetConnectionText(i));
		if (fSettings->GetBool(DCS_PREFS_ACTIVE, b) == B_OK)
			fConn->SetType(b ? DCConnection::ACTIVE : DCConnection::PASSIVE);
		fConn->SetSharedSize(65011712 * 1024);	// fake shared size of 62GB
	}
	
	if (fConn->IsConnected())
		fConn->SendMyInfo();
}

BString
DCView::GetConnectionText(int val)
{
	BString ret;
	switch (val)
	{
		case 0:
			ret = "28.8Kbps";
			break;
		case 1:
			ret = "33.6Kbps";
			break;
		case 2:
			ret = "56Kbps";
			break;
		case 3:
			ret = "ISDN";
			break;
		case 4:
			ret = "DSL";
			break;
		case 5:
			ret = "Cable";
			break;
		case 6:
			ret = "LAN(T1)";
			break;
		case 7:
			ret = "LAN(T3)";
			break;
		case 8:
			ret = "Satellite";
			break;
		default:
			ret = "Unknown";
			break;
	}
	return ret;
}

void
DCView::InitGUI()
{
	fText = new BTextView(BRect(2, 2, Bounds().Width() - 150 - B_V_SCROLL_BAR_WIDTH,
						  Bounds().Height() - 30 - B_H_SCROLL_BAR_HEIGHT), "textview",
						  BRect(2, 2, Bounds().Width() - 154 - B_V_SCROLL_BAR_WIDTH,
						  Bounds().Height() - 34 - B_H_SCROLL_BAR_HEIGHT), B_FOLLOW_ALL, 
						  B_WILL_DRAW);
	AddChild(
		fScrollText = new BScrollView("scroll_view", fText, B_FOLLOW_ALL, B_WILL_DRAW, true, true)
	);
	fScrollText->SetViewColor(216, 216, 216);
	// Adjust the text view a bit
	fText->MakeEditable(false);
	fText->SetStylable(true);
	fText->MakeResizable(true);
	
	AddChild(
		fUsers = new BColumnListView(BRect(fScrollText->Frame().right + 2, 0, Bounds().right - 2,
									 fScrollText->Frame().bottom), "users_list", B_FOLLOW_RIGHT | B_FOLLOW_TOP_BOTTOM, 
									 B_WILL_DRAW, B_FANCY_BORDER, true)
	);
	// Add columns to the CLV
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_NAME), 50, 30, 100, 90), 0
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_SPEED), 50, 30, 100, 90), 1
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_DESC), 50, 30, 200, 190), 2
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_EMAIL), 50, 30, 100, 90), 3
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_SHARED), 50, 30, 100, 90), 4
	);
	
	// Insert our input text control
	AddChild(
		fInput = new BTextControl(BRect(0, fScrollText->Frame().bottom + 5, Bounds().Width() - 2, Bounds().Height() - 2),
								  "chat_input", DCStr(STR_VIEW_CHAT), "", new BMessage(DCV_SEND_TEXT),
								  B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM)
	);
	fInput->SetDivider(40);
}

void
DCView::Disconnect()
{
	fConn->Disconnect();
}

void
DCView::LogSystem(const BString & msg)
{
	// TextLength
	text_run_array ta;
	ta.count = 1;
	ta.runs[0].font = *be_bold_font;
	ta.runs[0].offset = 0;
	ta.runs[0].color = dc_app->GetColor(DC_COLOR_SYSTEM);
	
	BString str = DCStr(STR_MSG_SYSTEM);
	
	// Prepend the "System"
	fText->Insert(fText->TextLength(), str.String(), str.Length(), &ta);
	// Now the message
	ta.runs[0].font = *be_plain_font;
	ta.runs[0].color = dc_app->GetColor(DC_COLOR_TEXT);
	fText->Insert(fText->TextLength(), msg.String(), msg.Length(), &ta);
}
