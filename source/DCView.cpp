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
#include "WrappingTextView.h"

#include <TextView.h>
#include <TextControl.h>
#include <ScrollView.h>

#include <stdlib.h>

enum
{
	// Enter was pressed in the text input
	DCV_SEND_TEXT = 'cvST',
};

DCView::DCView(DCSettings * settings, BMessenger target, BRect pos)
	: BView(pos, "dcview", B_FOLLOW_ALL, 0)
{
	SetViewColor(216, 216, 216);
	fSettings = settings;
	fTarget = target;
	fConn = NULL;
	fClosing = false;
	InitGUI();
}

DCView::~DCView()
{
	fClosing = true;	// definitely ;)
	Disconnect();
	fConn->Lock();
	fConn->Quit();
}

void
DCView::AttachedToWindow()
{
	fConn = new DCConnection(BMessenger(this));
	fInput->SetTarget(this);
	// Initialize our settings for the connection
	SendInfoToServer(true /* update */, false /* but don't send */);
}

void
DCView::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		case DCV_SEND_TEXT:	// Enter was pressed on our BTextControl
		{
			
			ParseSendText();
			break;
		}
		
		case DC_MSG_CON_CONNECTING:
			break;	// Yeah yeah, we know ;)
		
		case DC_MSG_CON_CONNECTED:
		{
			LogSystem(DCStr(STR_MSG_CONNECTED));
			break;
		}
		
		case DC_MSG_CON_CONNECT_ERROR:
		{
			LogError(DCStr(STR_MSG_CONNECT_ERROR));
			LogSystem(DCStr(STR_MSG_RECONNECTING));
			fConn->Connect(fHost, fPort);
			break;
		}
		
		case DC_MSG_CON_DISCONNECTED:
		{
			EmptyUserList();
			LogError(DCStr(STR_MSG_DISCONNECTED_FROM_SERVER));
			if (!fClosing)	// try to reconnect if we are not manually closing this connection
			{
				LogSystem(DCStr(STR_MSG_RECONNECTING));
				fConn->Connect(fHost, fPort);
			}			
			break;
		}
		
		case DC_MSG_CON_VALIDATE_DENIED:
		{
			LogError(DCStr(STR_MSG_INVALID_NICK));
			Disconnect();	// Disconnect from the server.
			break;
		}
		
		case DC_MSG_CON_GET_PASS:	// TODO: Get a password
			break;
		
		case DC_MSG_CON_BAD_PASS:	// TODO: Password was invalid
			break;
		
		case DC_MSG_CON_USER_CONNECTED:
		{
			BString nick;
			if (msg->FindString("nick", &nick) == B_OK)
			{
				LogUserLoggedIn(nick);
				CreateNewUser(nick);
			}
			break;
		}
		
		case DC_MSG_CON_YOU_ARE_OP:	// You're an operator on this hub, TODO: Add menus, etc
			break;
		
		case DC_MSG_CON_GOT_NICK_LIST:
		{
			BString list;
			if (msg->FindString("list", &list) == B_OK)
				ParseNickList(list);
			break;
		}
		
		case DC_MSG_CON_GOT_OP_LIST:	// We got a list of ops.. TODO: Make them bold, etc..
			break;
		
		case DC_MSG_CON_PRIV_MSG:
		{
			BString from, text;
			if (msg->FindString("from", &from) == B_OK &&
				msg->FindString("text", &text) == B_OK)
				LogPrivateChat(from, text, true);
		}
		
		case DC_MSG_CON_CONNECT_TO_ME:	// TODO: p2p transfers
			break;
			
		case DC_MSG_CON_MULTI_CONNECT_TO_ME:	// TODO: p2p transfers
			break;
		
		case DC_MSG_CON_REV_CONNECT_TO_ME:		// TODO: p2p transfers
			break;
		
		case DC_MSG_CON_CHAT_MSG:
		{
			BString nick, text;
			if (msg->FindString("nick", &nick) == B_OK &&
				msg->FindString("text", &text) == B_OK)
				LogChatMessage(nick, text, (nick == fConn->GetNick() ? false : true));
			else if (msg->FindString("chat", &text) == B_OK)
				LogChatMessage("", text, false);
				
			break;
		}
		
		case DC_MSG_CON_FORCE_MOVE:
		{
			BString ip;
			Disconnect();	// disconnect empty the user list
			LogSystem(DCStr(STR_MSG_DISCONNECTED_FROM_SERVER));
			if (msg->FindString("ip", &ip) == B_OK &&
				ip != "")
			{
				int port = 411;	// default DC port
				GetHostAndPort(ip, port);	// parse 'ip' for a server:port address
				BString redir = DCStr(STR_MSG_REDIRECTING);
				redir += ip;
				redir += ":";
				redir << port;
				redir << ".";
				LogSystem(redir);
				fConn->Connect(ip, port);				
			}
			break;
		}
		
		case DC_MSG_CON_QUIT:	// a user logged off
		{
			BString nick;
			if (msg->FindString("nick", &nick) == B_OK)
			{
				LogUserLoggedOut(nick);
				RemoveUser(nick);
			}
			break;
		}
		
		case DC_MSG_CON_USER_INFO:
		{
			BString nick, desc, speed, email;
			int64 size;
			
			if (msg->FindString("nick", &nick) == B_OK &&
				msg->FindString("desc", &desc) == B_OK &&
				msg->FindString("speed", &speed) == B_OK &&
				msg->FindString("email", &email) == B_OK &&
				msg->FindInt64("size", (int64 *)&size) == B_OK)
				UpdateUser(nick, desc, speed, email, size);
			break;
		}
		
		case DC_MSG_HUB_IS_FULL:
		{
			Disconnect();
			LogSystem(DCStr(STR_MSG_HUB_IS_FULL));
			break;
		}
		
		default:
			BView::MessageReceived(msg);
			break;
	}
}

void
DCView::UpdateUser(const BString & nick, const BString & desc, const BString & speed,
				   const BString & email, int64 size)
{
	DCUser * user = FindUser(nick);
	if (user)
	{
		user->SetDesc(desc);
		user->SetSpeed(speed);
		user->SetEmail(email);
		user->SetShared(size);
		fUsers->Refresh();
	}
}

// Returns NULL if user can't be found
DCUser *
DCView::FindUser(const BString & name, bool caseSen)
{
	DCUser * user;
	for (int32 i = 0; i < fUserList.CountItems(); i++)
	{
		user = fUserList.ItemAt(i);
		if (caseSen)
		{
			if (user->GetName() == name)
				return user;
		}
		else
		{
			if (user->GetName().ICompare(name) == 0)
				return user;
		}
	}
	return NULL;
}

bool
DCView::QuitRequested()
{
	fClosing = true;
	Disconnect();
	return true;	// Just this for now...
}

void
DCView::ParseNickList(BString list)
{
	int32 i;
	while ((i = list.FindFirst("$$")) != B_ERROR)
	{
		BString nick;
		list.MoveInto(nick, 0, i);
		list.RemoveFirst("$$");
		LogUserLoggedIn(nick);
		CreateNewUser(nick);
	}
}

void
DCView::CreateNewUser(const BString & nick)
{
	DCUser * user = new DCUser(nick);
	user->CreateRow(fUsers);
	fUserList.AddItem(user);
	fConn->GetUserInfo(nick);
}

void
DCView::UpdateSettings(DCSettings * set)
{
	fSettings = set;
	SendInfoToServer(true, true);	// update and send
}

void
DCView::Connect(const BString & host, int port)
{
	BString realHost = host;
	GetHostAndPort(realHost, port);
	
	BString msg = DCStr(STR_MSG_CONNECTING_TO);
	msg += realHost;
	msg += ":";
	msg << port;
	msg += ".";
	LogSystem(msg);
	fConn->Connect(realHost, port);
	fHost = realHost;
	fPort = port;
}

// Check to see if 'host' is in the form server:port. If it is,
// replace 'port' with the port from 'host'.
void
DCView::GetHostAndPort(BString & host, int & port)
{
	int32 i;
	BString realHost;
	
	// Parse the host for a server:port type address
	if ((i = host.FindFirst(":")) != B_ERROR)
	{
		host.MoveInto(realHost, 0, i);
		host.RemoveFirst(":");
		if (host.Length() > 0)
			port = atoi(host.String());
		host = realHost;	// reassign host
	}
}

// If update is true, fSettings is read to get the updated nick, etc
void
DCView::SendInfoToServer(bool update, bool send)
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
		fConn->SetSharedSize((int64)65011712 * 1024);	// fake shared size of 62GB
	}
	
	if (send && fConn->IsConnected())
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
	fText = new WrappingTextView(BRect(2, 2, Bounds().Width() - 254 - B_V_SCROLL_BAR_WIDTH,
						  		 Bounds().Height() - 30 - B_H_SCROLL_BAR_HEIGHT), "textview",
						  		 B_FOLLOW_ALL, B_WILL_DRAW);
	AddChild(
		fScrollText = new BScrollView("scroll_view", fText, B_FOLLOW_ALL, B_WILL_DRAW, false, true)
	);
	fScrollText->SetViewColor(216, 216, 216);
	// Adjust the text view a bit
	fText->MakeEditable(false);
	fText->SetStylable(true);
	//fText->MakeResizable(true);
	//fText->SetTextRect(BRect(2, 2, fText->Bounds().Width() - 2, fText->Bounds().Height() - 2));
	
	AddChild(
		fUsers = new BColumnListView(BRect(fScrollText->Frame().right + 4, 0, Bounds().right - 2,
									 fScrollText->Frame().bottom), "users_list", B_FOLLOW_RIGHT | B_FOLLOW_TOP_BOTTOM, 
									 B_WILL_DRAW, B_FANCY_BORDER, true)
	);
	// Add columns to the CLV
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_NAME), 100, 30, 400, 90), 0
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_SPEED), 100, 30, 400, 90), 1
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_DESC), 200, 30, 600, 190), 2
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_EMAIL), 100, 30, 400, 90), 3
	);
	fUsers->AddColumn(
		new BStringColumn(DCStr(STR_VIEW_SHARED), 100, 30, 400, 90), 4
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
	printf("Asking connection to disconnect\n");
	fConn->Disconnect();
	printf("Disconnected\n");
	EmptyUserList();
}

void
DCView::PrintSystem()
{
	text_run_array ta;
	ta.count = 1;
	ta.runs[0].font = *be_bold_font;
	ta.runs[0].offset = 0;
	ta.runs[0].color = dc_app->GetColor(DC_COLOR_SYSTEM);
	
	BString str = DCStr(STR_MSG_SYSTEM);
	
	// Prepend the "System"
	fText->Insert(fText->TextLength(), str.String(), str.Length(), &ta);
}

void
DCView::PrintError()
{
	text_run_array ta;
	ta.count = 1;
	ta.runs[0].font = *be_bold_font;
	ta.runs[0].offset = 0;
	ta.runs[0].color = dc_app->GetColor(DC_COLOR_ERROR);
	
	BString str = DCStr(STR_MSG_ERROR);
	
	// Prepend the "System"
	fText->Insert(fText->TextLength(), str.String(), str.Length(), &ta);
}

void
DCView::PrintText(BString str, bool newLine, int optColor)
{
	text_run_array ta;
	ta.count = 1;
	ta.runs[0].offset = 0;
	ta.runs[0].font = *be_plain_font;
	ta.runs[0].color = dc_app->GetColor((optColor == -1) ? DC_COLOR_TEXT : optColor);
	
	if (newLine)
		str += "\n";	// Add a new line
	fText->Insert(fText->TextLength(), str.String(), str.Length(), &ta);
}

void
DCView::LogSystem(const BString & msg)
{
	int b = GetScrollState();
	PrintSystem();
	PrintText(msg);
	if (b)
		ScrollToBottom();
}

void
DCView::LogError(const BString & msg)
{
	bool b = GetScrollState();
	PrintError();
	PrintText(msg);
	if (b)
		ScrollToBottom();
}

void
DCView::LogUserLoggedIn(const BString & nick)
{
	bool b = GetScrollState();
	
	PrintSystem();
	// Create a string including the nick
	BString msg = DCStr(STR_MSG_USER_LOGGED_IN);
	BString fmt;
	msg.MoveInto(fmt, 0, msg.FindFirst("%s"));
	msg.RemoveFirst("%s");
	fmt += nick;
	fmt += msg;

	PrintText(fmt);
	
	if (b)
		ScrollToBottom();
}

void
DCView::LogUserLoggedOut(const BString & nick)
{
	bool b = GetScrollState();
	
	PrintSystem();
	BString msg = nick;
	msg += DCStr(STR_MSG_USER_LOGGED_OUT);
	PrintText(msg);
	
	if (b)
		ScrollToBottom();
}

void
DCView::EmptyUserList()
{
	DCUser * user = NULL;
	while (fUserList.CountItems() > 0)
	{
		user = fUserList.RemoveItemAt(0);
		delete user;
	}
}

void
DCView::LogChatMessage(const BString & nick, const BString & text, bool remote)
{
	bool b = GetScrollState();
	if (nick != "")
	{
		text_run_array ta;
		ta.count = 1;
		ta.runs[0].offset = 0;
		ta.runs[0].font = *be_bold_font;
		ta.runs[0].color = dc_app->GetColor(remote ? DC_COLOR_REMOTE_NICK : DC_COLOR_LOCAL_NICK);
		
		BString n = nick;
		n += ": ";
		fText->Insert(fText->TextLength(), n.String(), n.Length(), &ta);
	}
	
	BString str(text);
	str.ReplaceAll("\r\n", "\n");
	PrintText(str);
	if (b)
		ScrollToBottom();
}

// "from" is the user who send the text if remote is true. Otherwise, it is the person
// to whom the text is being sent to.
void
DCView::LogPrivateChat(const BString & from, const BString & text, bool remote)
{
	bool b = GetScrollState();
	
	text_run_array ta;
	ta.count = 1;
	ta.runs[0].offset = 0;
	ta.runs[0].font = *be_bold_font;
	ta.runs[0].color = dc_app->GetColor(remote ? DC_COLOR_REMOTE_NICK : DC_COLOR_LOCAL_NICK);
	
	BString n;
	if (remote)
	{
		n = from;
		n += ": ";
	}
	else
	{
		n = fSettings->GetString(DCS_PREFS_NICK);
		n += " -> ";
		n += from;
		n += ": ";
	}
	fText->Insert(fText->TextLength(), n.String(), n.Length(), &ta);
	
	n = text;
	n.ReplaceAll("\r\n", "\n");
	PrintText(n, true, remote ? DC_COLOR_PRIVATE_TEXT : -1);
	
	if (b)
		ScrollToBottom();
}

bool
DCView::GetScrollState()
{
	bool scroll = false;
	
	BScrollBar * bar = fScrollText->ScrollBar(B_VERTICAL);
	if (bar)
	{
		float min, max;
		bar->GetRange(&min, &max);
		max -= 5;
		if (bar->Value() >= max)
			scroll = true;
	}
	
	return scroll;
}

void
DCView::ScrollToBottom()
{
	fText->ScrollToOffset(fText->TextLength());
}

void
DCView::RemoveUser(const BString & nick)
{
	DCUser * u = NULL;
	for (int32 i = 0; i < fUserList.CountItems(); i++)
	{
		u = fUserList.ItemAt(i);
		if (u->GetName() == nick)
		{
			fUserList.RemoveItemAt(i);
			delete u;
			break;
		}
	}
}

void
DCView::ParseSendText()
{
	BString text = fInput->Text();
	fInput->SetText("");
	
	if (!text.ICompare("/msg ", 5))
	{
		text.Remove(0, 5);
		
		DCUser * user = NULL;
		
		for (int32 j = 0; j < fUserList.CountItems(); j++)
		{
			user = fUserList.ItemAt(j);
			if (text.IFindFirst(user->GetName()) == 0)
			{
				text.Remove(0, user->GetName().Length() + 1);
				break;
			}
			user = NULL;	// for below failure
		}
		
		if (!user)
		{
			LogError(DCStr(STR_MSG_USER_NOT_FOUND));
			return;
		}
		
		// You CAN send private messages to yourself, but i'm not
		// going to let you :P
		if (user->GetName() == fConn->GetNick())
			return;
		// Otherwise, send a private message
		BString priv = "$To: ";
		priv += user->GetName();
		priv += " From: ";
		priv += fConn->GetNick();
		priv += " $<";
		priv += fConn->GetNick();
		priv += "> ";
		priv += text;
		priv += "|";
		fConn->SendData(priv);	
		LogPrivateChat(user->GetName(), text, false);	
	}
	else if(!text.ICompare("/help", 5))
	{
		LogSystem("Available commands:");
		LogSystem("  /help - Show this help text");
		LogSystem("  /msg <name> <text> - Send a private message");
	}
	else if(!text.ICompare("/", 1))
	{
		LogError("Unknown command");
	}
	else
	{
		BString chat = "<";
		chat += fConn->GetNick();
		chat += "> ";
		chat += text;
		chat += "|";
		fConn->SendData(chat);
	}
}
