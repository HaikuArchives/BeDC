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
#include "ColumnListView.h"
#include "ColumnTypes.h"

#include <View.h>
#include <Button.h>
#include <StringView.h>
#include <File.h>

#include <stdio.h>

enum
{
	HUB_CONNECT = 'hbCt',
	HUB_REFRESH = 'hbRf'
};

#define HUB_WIN_ARCHIVE "/boot/home/config/settings/BeDC/Cached.hublist"

DCHubWindow::DCHubWindow(BMessenger target)
	: BWindow(BRect(50, 50, 600, 350), DCStr(STR_HUB_WINDOW_TITLE),
			  B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	SetMessageTarget(target);
	
	InitGUI();
	fList = new DCHTTPConnection(BMessenger(this));
}

DCHubWindow::~DCHubWindow()
{
	// Clean up our http connection
	fList->Disconnect();
	fList->Lock();
	fList->Quit();
}

void
DCHubWindow::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case HUB_REFRESH:
		{
			if (!fList->IsRunning())
				fList->Connect();
			break;
		}
			
		case DC_MSG_HTTP_CONNECTED:
		{
			// Format an HTTP request
			BString http = "GET /";
			http += fList->GetFile();
			http += " HTTP/1.1\r\nUser-Agent: BeDC/alpha\r\nHost: ";
			http += fList->GetServer();
			http += "\r\n\r\n";
			fList->Send(http);
			
			fStatus->SetText(DCStr(STR_STATUS_CONNECTED));
			break;
		}
		
		case DC_MSG_HTTP_DISCONNECTED:
		{
			printf("Got disconnection message\n");
			HandleDisconnect();
			break;
		}
		
		case DC_MSG_HTTP_CONNECT_ERROR:
			fStatus->SetText(DCStr(STR_STATUS_CONNECT_ERROR));
			break;
			
		case DC_MSG_HTTP_SEND_ERROR:
			fStatus->SetText(DCStr(STR_STATUS_SEND_ERROR));
			break;
			
		case DC_MSG_HTTP_RECV_ERROR:
			fStatus->SetText(DCStr(STR_STATUS_RECV_ERROR));
			break;
			
		default:
			BWindow::MessageReceived(msg);
			break;
	}
}

bool
DCHubWindow::QuitRequested()
{
	fTarget.SendMessage(DC_MSG_HUB_CLOSED);
	return true;
}

void
DCHubWindow::InitGUI()
{
	AddChild(
		fView = new BView(BRect(0, 0, Frame().Width(), Frame().Height()), "the_view", 
						  B_FOLLOW_ALL, B_WILL_DRAW)
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
	float left = fView->Frame().Width() - 158;
	height = fView->Frame().Width() - 3;	// var reuse ;)
	
	fView->AddChild(
		fButtonView = new BView(BRect(left, top, height, bottom), "button_view", 
								B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
	);
	fButtonView->SetViewColor(216, 216, 216);
	
	fButtonView->AddChild(/* 0, 0, 80, 28 */
		fConnect = new BButton(BRect(3, 3, 77, 25), "b_connect", DCStr(STR_HUB_CONNECT),
							   new BMessage(HUB_CONNECT), B_FOLLOW_LEFT | B_FOLLOW_BOTTOM)
	);
	fConnect->MakeDefault(true);
	
	fButtonView->AddChild(
		fRefresh = new BButton(BRect(85, 0, 155, 28), "b_refresh",
							   DCStr(STR_HUB_REFRESH), new BMessage(HUB_REFRESH), 
							   B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM)
	);
	
	fView->AddChild(
		fStatus = new BStringView(BRect(5, bottom - 15, left, bottom - 3),
								  "status_bar", DCStr(STR_STATUS_IDLE), 
								  B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM)
	);
	
	// Setup the columns in the list view
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_NAME), 100, 30, 200, 185, B_ALIGN_LEFT), 0
	);
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_ADDR), 100, 30, 200, 185, B_ALIGN_LEFT), 1
	);
	fHubView->AddColumn(
		new BStringColumn(DCStr(STR_SERVER_DESC), 230, 50, 330, 315, B_ALIGN_LEFT), 2
	);
	fHubView->AddColumn(
		new BIntegerColumn(DCStr(STR_SERVER_USERS), 50, 30, 70, B_ALIGN_RIGHT), 3
	);
	
	AddItem("Test Server", "ip.noname.com", "Just a test server... for your viewing pleasure :)", 5);
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
	
	// run through the list of found hubs and add them to the list view
	fHubView->Clear();	// clear old items first
	
	list<DCHTTPConnection::Hub *>::iterator i;
	list<DCHTTPConnection::Hub *> * hubs = fList->GetHubs();
	
	BString strStatus;
	strStatus << DCStr(STR_STATUS_NUM_SERVERS);
	strStatus << (int)hubs->size();
	fStatus->SetText(strStatus.String());
	for (i = hubs->begin(); i != hubs->end(); i++)
	{
		archive->AddData("items", B_ANY_TYPE, (*i), sizeof(DCHTTPConnection::Hub));
		AddItem((*i));
	}
	fList->EmptyHubList();
		
	// Now, archive the list
	BFile file(HUB_WIN_ARCHIVE, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() == B_OK)
		archive->Flatten(&file);
	delete archive;
}

void
DCHubWindow::RestoreList()
{
	BFile file(HUB_WIN_ARCHIVE, B_READ_ONLY);
	if (file.InitCheck() == B_OK)
	{
		BMessage * archive = new BMessage;
		archive->Unflatten(&file);
	}
}
