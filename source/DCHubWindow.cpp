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
#include "DCHubWindow.h"
#include "DCStrings.h"
#include <private/interface/ColumnListView.h>
#include <private/interface/ColumnTypes.h>
#include "DCApp.h"

#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <File.h>

#include <stdio.h>

enum
{
	HUB_CONNECT = 'hbCt',
	HUB_REFRESH = 'hbRf',
	HUB_NEXT50 = 'hbN5',
	HUB_PREV50 = 'hbP5'
};

#define HUB_WIN_ARCHIVE "/boot/home/config/settings/BeDC/Cached.hublist"

DCHubWindow::DCHubWindow(BMessenger target, BRect pos)
	: BWindow(pos, DCStr(STR_HUB_WINDOW_TITLE),
			  B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS | B_OUTLINE_RESIZE)
{
	SetMessageTarget(target);

	InitGUI();
	fList = NULL;

	fHubs = new list<DCHTTPConnection::Hub *>;
	RestoreList();
}

DCHubWindow::~DCHubWindow()
{
	// Clean up our http connection
	if (fList)
	{
		fList->Disconnect();
		fList->Lock();
		fList->Quit();
		fList->EmptyHubList(fHubs);
	}
	delete fHubs;
}

void
DCHubWindow::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DC_MSG_APP_UPDATE_LANG:
			UpdateLanguage();
			break;

		case HUB_CONNECT:
		{
			BMessage msg(DC_MSG_HUB_CONNECT);
			BRow * row = fHubView->CurrentSelection();
			if (!row)
				break;	// nothing selected yet

			msg.AddString("name", ((BStringField *)row->GetField(0))->String());
			msg.AddString("addr", ((BStringField *)row->GetField(1))->String());
			msg.AddString("desc", ((BStringField *)row->GetField(2))->String());
			fTarget.SendMessage(&msg);
			break;
		}

		case HUB_REFRESH:
		{
			if (!fList)
				fList = new DCHTTPConnection(BMessenger(this));
			if (!fList->IsRunning())
			{
				fStatus->SetText(DCStr(STR_STATUS_CONNECTING));
				fList->Run();
				fList->Connect();
			}
			break;
		}

		case HUB_NEXT50:
		{
			if (fNextOffset < (int)fHubs->size())
			{
				fOffset = fNextOffset;
				fNextOffset += 50;
				ListSomeItems();
			}
			break;
		}

		case HUB_PREV50:
		{
			if (fOffset > 0)
			{
				fOffset -= 50;
				fNextOffset -= 50;
				ListSomeItems();
			}
			break;
		}

		case DC_MSG_HTTP_CONNECTED:
		{
			fList->SendListRequest();
			fStatus->SetText(DCStr(STR_STATUS_CONNECTED));
			break;
		}

		case DC_MSG_HTTP_DISCONNECTED:
		{
			HandleDisconnect();
			CleanUpConnection();
			break;
		}

		case DC_MSG_HTTP_CONNECT_ERROR:
		{
			fStatus->SetText(DCStr(STR_STATUS_CONNECT_ERROR));
			CleanUpConnection();
			break;
		}

		case DC_MSG_HTTP_SEND_ERROR:
		{
			fStatus->SetText(DCStr(STR_STATUS_SEND_ERROR));
			CleanUpConnection();
			break;
		}

		case DC_MSG_HTTP_RECV_ERROR:
		{
			fStatus->SetText(DCStr(STR_STATUS_RECV_ERROR));
			CleanUpConnection();
			break;
		}

		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

void
DCHubWindow::CleanUpConnection()
{
	if (fList)
	{
		fList->Lock();
		fList->Quit();
		fList = NULL;
	}
}

bool
DCHubWindow::QuitRequested()
{
	BMessage msg(DC_MSG_HUB_CLOSED);
	msg.AddRect("rect", Frame());
	fTarget.SendMessage(&msg);
	return true;
}

void
DCHubWindow::InitGUI()
{
	AddChild(
		fView = new BView(BRect(0, 0, Frame().Width(), Frame().Height()), "the_view",
						  B_FOLLOW_ALL, 0)
	);
	fView->SetViewColor(216, 216, 216);

	float height = fView->Frame().Height() - 35;

	fView->AddChild(
		fHubView = new BColumnListView(BRect(3, 3, fView->Frame().Width() - 6, height),
									   "the_list_view", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
									   B_WILL_DRAW, B_FANCY_BORDER)
	);

	float top = height + 5;
	float bottom = fView->Frame().Height();
	float left = fView->Frame().Width() - 348 /*158*/;
	height = fView->Frame().Width() - 3;	// var reuse ;)

	fView->AddChild(
		fButtonView = new BView(BRect(left, top, height, bottom), "button_view",
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
	);
	fButtonView->SetViewColor(216, 216, 216);

	fButtonView->AddChild(/* 0, 0, 80, 28 */
		fConnect = new BButton(BRect(3, 3, 87, 25), "b_connect", DCStr(STR_HUB_CONNECT),
							   new BMessage(HUB_CONNECT), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
	);
	fConnect->MakeDefault(true);

	fButtonView->AddChild(
		fRefresh = new BButton(BRect(95, 0, 175, 28), "b_refresh",
							   DCStr(STR_HUB_REFRESH), new BMessage(HUB_REFRESH),
							   B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
	);

	fButtonView->AddChild(
		fPrev = new BButton(BRect(180, 0, 260, 28), "b_prev", DCStr(STR_HUB_PREV50),
							new BMessage(HUB_PREV50), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
	);

	fButtonView->AddChild(
		fNext = new BButton(BRect(265, 0, 345, 28), "b_next", DCStr(STR_HUB_NEXT50),
							new BMessage(HUB_NEXT50), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
	);

	fView->AddChild(
		fStatus = new BStringView(BRect(5, bottom - 15, left, bottom - 3),
								  "status_bar", DCStr(STR_STATUS_IDLE),
								  B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM)
	);


	// Setup the columns in the list view
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_NAME), 150, 30, 300, 285, B_ALIGN_LEFT), 0
	);
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_ADDR), 150, 30, 300, 285, B_ALIGN_LEFT), 1
	);
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_DESC), 230, 50, 430, 375, B_ALIGN_LEFT), 2
	);
	fHubView->AddColumn(
		new BIntegerColumn(DCStr(STR_SERVER_USERS), 50, 30, 100, B_ALIGN_RIGHT), 3 // Made the max size bigger to fit the norwegian translation --Vegard
	);

	AddItem("Test Server", "192.168.0.2", "Just a test server... for your viewing pleasure :)", 5);
}

void
DCHubWindow::AddItem(const BString & name, const BString & addr, const BString & desc, uint32 users)
{
	BRow * row = new BRow;
	row->SetField(new BStringField(name.String()), 0);
	row->SetField(new BStringField(addr.String()), 1);
	row->SetField(new BStringField(desc.String()), 2);
	row->SetField(new BIntegerField((int32)users), 3);

	fHubView->AddRow(row);
}

void
DCHubWindow::HandleDisconnect()
{
	BMessage * archive = new BMessage;
	list<DCHTTPConnection::Hub *>::iterator i;
	list<DCHTTPConnection::Hub *> * hubs = fList->GetHubsCopy();
	fList->EmptyHubList();

	BString strStatus;
	strStatus << DCStr(STR_STATUS_NUM_SERVERS);
	strStatus << (int)hubs->size();
	fStatus->SetText(strStatus.String());

	// Archive the string
	for (i = hubs->begin(); i != hubs->end(); i++)
	{
		//archive->AddData("items", B_ANY_TYPE, (*i), sizeof(DCHTTPConnection::Hub));
		archive->AddString("name", (*i)->fName);
		archive->AddString("addr", (*i)->fServer);
		archive->AddString("desc", (*i)->fDesc);
		archive->AddInt32("users", (*i)->fUsers);
	}

	// Now save the archive to file
	BFile file(HUB_WIN_ARCHIVE, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() == B_OK)
		archive->Flatten(&file);
	delete archive;

	fHubs = hubs;	// keep a pointer to it.. it's OK.. it's a copy

	fOffset = 0;
	fNextOffset = 50;
	ListSomeItems();
}

void
DCHubWindow::RestoreList()
{
	BFile file(HUB_WIN_ARCHIVE, B_READ_ONLY);

	fList->EmptyHubList(fHubs);

	if (file.InitCheck() == B_OK)
	{
		BMessage * archive = new BMessage;
		if (archive->Unflatten(&file) == B_OK)
		{
			BString name, addr, desc;
			int32 users, i = 0;
			for (; (archive->FindString("name", i, &name) == B_OK) &&
				   (archive->FindString("addr", i, &addr) == B_OK) &&
				   (archive->FindString("desc", i, &desc) == B_OK) &&
				   (archive->FindInt32("users", i, &users) == B_OK); i++)
			{
				DCHTTPConnection::Hub * hub = new DCHTTPConnection::Hub;
				hub->fName = name;
				hub->fServer = addr;
				hub->fDesc = desc;
				hub->fUsers = users;
				fHubs->push_back(hub);
			}
			BString status(DCStr(STR_STATUS_NUM_SERVERS));
			status << (int)fHubs->size();
			fStatus->SetText(status.String());

			fOffset = 0;
			fNextOffset = 50;
			ListSomeItems();
		}
		delete archive;
	}
}

void
DCHubWindow::ListSomeItems()
{
	list<DCHTTPConnection::Hub *>::iterator i;
	int f = 0;
	if (fOffset >= (int)fHubs->size())	// offset great than the list size? ;)
		return;
	// find the first item in our list
	i = fHubs->begin();
	while (f != fOffset)
	{
		i++;
		f++;
	}
	fHubView->Clear();

	for (int j = fOffset; j < fNextOffset; j++)
	{
		if (i != fHubs->end())
			AddItem((*i));
		else
			break;
		i++;
	}
}

void
DCHubWindow::UpdateLanguage()
{
	fConnect->SetLabel(DCStr(STR_HUB_CONNECT));
	fRefresh->SetLabel(DCStr(STR_HUB_REFRESH));
	fPrev->SetLabel(DCStr(STR_HUB_PREV50));
	fNext->SetLabel(DCStr(STR_HUB_NEXT50));
	((BTitledColumn *)fHubView->ColumnAt(0))->SetTitle(DCStr(STR_SERVER_NAME));
	((BTitledColumn *)fHubView->ColumnAt(1))->SetTitle(DCStr(STR_SERVER_ADDR));
	((BTitledColumn *)fHubView->ColumnAt(2))->SetTitle(DCStr(STR_SERVER_DESC));
	((BTitledColumn *)fHubView->ColumnAt(3))->SetTitle(DCStr(STR_SERVER_USERS));
	fHubView->Hide();
	fHubView->Show();

	SetTitle(DCStr(STR_HUB_WINDOW_TITLE));
}
