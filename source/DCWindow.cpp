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
#include <stdlib.h>

#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Rect.h> 
#include <TextControl.h>
#include <TextView.h>
#include <String.h>
#include <ScrollView.h>
#include <ListView.h>
#include <ListItem.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>


#include "DCDefs.h"
#include "DCApp.h"
#include "DCWindow.h"
#include "DCView.h" 
#include "DCConnection.h"
#include "DCClientConnection.h"
#include "DCSettings.h"

DCWindow::DCWindow(BRect frame)
: BWindow(frame,"BeDC",B_TITLED_WINDOW,B_NOT_ZOOMABLE)
{
	BRect b = Bounds();
	BMenu *menu, *submenu;
	BMenuItem *item;
	BString tmp;
	
	menubar= (new BMenuBar(b,"menubar"));
	menu = new BMenu("File");
	menubar->AddItem(menu);
	
	menu = new BMenu("Edit");
	menubar->AddItem(menu);
	
	menu = new BMenu("Settings");
	connmenu = submenu = new BMenu("Connection");
	submenu->SetRadioMode(true);
	submenu->AddItem(item = new BMenuItem("28.8Kbps",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("33.6Kbps",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("56Kbps",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("ISDN",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("DSL",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("Cable",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("LAN(T1)",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("LAN(T3)",new BMessage(DC_CONNECTION_CHANGED)));
	submenu->AddItem(item = new BMenuItem("Satelite",new BMessage(DC_CONNECTION_CHANGED)));
	menu->AddItem(submenu);
	submenu = new BMenu("Upload slots");
	submenu->AddItem(item = new BMenuItem("1",NULL));
	submenu->AddItem(item = new BMenuItem("2",NULL));
	submenu->AddItem(item = new BMenuItem("3",NULL));
	submenu->AddItem(item = new BMenuItem("4",NULL));
	submenu->AddItem(item = new BMenuItem("5",NULL));
	submenu->AddItem(item = new BMenuItem("10",NULL));
	submenu->AddItem(item = new BMenuItem("15",NULL));
	submenu->AddItem(item = new BMenuItem("20",NULL));
	submenu->AddItem(item = new BMenuItem("Other...",NULL));
	item->SetEnabled(false);
	submenu->SetRadioMode(true);
	menu->AddItem(submenu);
	submenu->SetEnabled(false);
	menu->AddSeparatorItem();
	item = new BMenuItem("Use passive mode (firewalled)",NULL);
	item->SetMarked(true);
	item->SetEnabled(false);
	menu->AddItem(item);
	menubar->AddItem(menu);
	
	menu = new BMenu("Windows");
	item = new BMenuItem("Show hub list",NULL);
	item->SetEnabled(false);
	menu->AddItem(item);
	item = new BMenuItem("Show search window",NULL);
	item->SetEnabled(false);
	menu->AddItem(item);
	menubar->AddItem(menu);
		
	menu = new BMenu("Help");
	menubar->AddItem(menu);
	
	AddChild(menubar);
	
	b.top += menubar->Bounds().bottom + 1;
	theView = new DCView(b);
	AddChild(theView);
	
	theConnection = new DCConnection;
	theConnection->SetMessageTarget(this);
	
	Show();
	PostMessage(DC_INIT_WINDOW);
}
 
void DCWindow::Init()
{
	BString tmp;
	if((dc_app->GetSettings()->GetString("nick",&tmp))==B_OK)
	{
		theView->NickView()->SetText(tmp.String());
		theConnection->SetNick(tmp.String());
		theView->NickView()->Invalidate();
	}
	if((dc_app->GetSettings()->GetString("server",&tmp))==B_OK)
	{
		theView->ServerView()->SetText(tmp.String());
		theView->ServerView()->Invalidate();
	}
	
	if((dc_app->GetSettings()->GetString("connection",&tmp))==B_OK)
	{
		theConnection->SetConn(tmp.String());
		for(int i=0;i<connmenu->CountItems();i++)
		{
			if(!tmp.Compare(connmenu->ItemAt(i)->Label()))
				connmenu->ItemAt(i)->SetMarked(true);
		}	
	}
}

DCWindow::~DCWindow()
{
	delete theConnection;
}

bool DCWindow::QuitRequested()
{
	printf("DCWindow::QuitRequested\n");
	theConnection->Disconnect();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void DCWindow::MessageReceived(BMessage *message)
{
	BTextControl *ptr;
	switch(message->what)
	{
		case DC_INIT_WINDOW :
		{
			Init();
			theView->InputView()->MakeFocus();
			break;
		}
		
		/* The following messages are sendt by the view, */
		/* the user has done something                   */
		
		case DC_CONNECTION_CHANGED : /* User selected their connection in the menu */
		{
			BMenuItem *menuitm;
			message->FindPointer("source",(void**)&menuitm);
			if((menuitm))
			{
				theConnection->SetConn(menuitm->Label());
				dc_app->GetSettings()->SetString("connection",menuitm->Label());	
			}
			break;
		}
		
		case DC_NICK_CHANGE: /* Nick Changed*/
		{
			message->FindPointer("source",(void**)&ptr);
			theConnection->SetNick(ptr->TextView()->Text());
			if(theConnection->IsConnected())
			{
				theConnection->Disconnect();
				theView->NickList()->RemoveItems(0,theView->NickList()->CountItems());
				theConnection->Connect(theView->GetServer(),411);
			}
			dc_app->GetSettings()->SetString("nick",ptr->TextView()->Text());
			break;
		}
		case DC_SERVER_CHANGE: /* Server Changed */
		{
			message->FindPointer("source",(void**)&ptr);
			if(strcmp(theView->GetNick(),"") != 0)
				theConnection->SetNick(theView->GetNick());
			if(theConnection->IsConnected())
			{
				theConnection->Disconnect();
				theView->NickList()->RemoveItems(0,theView->NickList()->CountItems());
			}
			dc_app->GetSettings()->SetString("server",ptr->TextView()->Text());
			theConnection->Connect(ptr->TextView()->Text(),411);
			break;
		}
		case DC_CONNECT_BUTTON : /* User has clicked connect button */
		{
			if(strcmp(theView->GetNick(),"") != 0)
				theConnection->SetNick(theView->GetNick());
			if(strcmp(theView->GetServer(),"") != 0)
			{
				if(theConnection->IsConnected())
				{
					theConnection->Disconnect();
					theView->NickList()->RemoveItems(0,theView->NickList()->CountItems());
				}
				theConnection->Connect(theView->GetServer(),411);
			}
			break;	
		}
		case DC_SEND_BUTTON : /* User har clicked Send button */
		{
			/* Todo: move parsing of commands to seperate method */
			BString tmp = theView->InputView()->Text();
			if (tmp!= "")
			{
				if(tmp[0] != '/')
				{
					if (theConnection->IsConnected())
					{
						tmp.Prepend("> ");
						tmp.Prepend(theConnection->GetNick());
						tmp.Prepend("<");
						tmp.Append("|");
						theConnection->SendData(tmp.String());
					}
					else
						theView->Output()->Insert("Error; Not connected to hub\n");
				}
				else
				{
					int tmpint;
					if((tmpint = tmp.FindFirst(" ")) < 0) /* no spaces */
						tmpint = tmp.Length();
					BString tmpb;
					tmp.CopyInto(tmpb,0,tmpint);
					tmpb.ToUpper();
					printf("->%s<-\n",tmpb.String());
					if(!tmpb.Compare("/HELP")) /* Show the available commands */
					{
						theView->Output()->Insert("Available commands:\n");
						theView->Output()->Insert("/help                 - shows this text\n");
						theView->Output()->Insert("/clear                - clear this view\n");
						theView->Output()->Insert("/msg <nick> <message> - sends <message> to <nick> (Case sensitive)\n");
					}
					else if(!tmpb.Compare("/MSG")) /* Sends a private message */
					{
						if(theConnection->IsConnected())
						{
							if(tmp.Length() <= 5)
								theView->Output()->Insert("Error; Use /msg <nick> <message>\n");
							else
							{
								tmp.Remove(0,5);
								if((tmpint = tmp.FindFirst(" ")) < 0) /* no spaces */
									theView->Output()->Insert("Error; Use /msg <nick> <message>\n");
								else
								{
									/* Check if the nick exists, and send the message if it does    */
									/* Dunno if the NM Client does this or just send it to the hub  */
									bool foundit = false;
									BString tmpc;
									tmp.MoveInto(tmpc,0,tmpint);
									for(int i=0;i<theView->NickList()->CountItems();i++)
									{
										if(!tmpc.Compare(((BStringItem*)theView->NickList()->ItemAt(i))->Text()))
											foundit = true;
									}
									if(!foundit)
										theView->Output()->Insert("Error; No such user\n");
									else
									{
										tmpc.Prepend("$To: ");
										tmpc.Append(" From: ");
										tmpc.Append(theConnection->GetNick());
										tmpc.Append(" $<");
										tmpc.Append(theConnection->GetNick());
										tmpc.Append(">");
										tmpc.Append(tmp);
										tmpc.Append("|");
										theConnection->SendData(tmpc.String());
									}
								}
							}
						}
						else
							theView->Output()->Insert("Error; Not connected to hub\n");	
					}
					else if(!tmpb.Compare("/CLEAR")) /* Clears the output view */
					{
						theView->Output()->SetText("");
					}
					else if(!tmpb.Compare("/PASS"))
					{
						if(tmp.Length() <= 6)
								theView->Output()->Insert("Error; Use /pass <password>\n");
						else
						{
							tmp.Remove(0,6);
							tmp.Prepend("$MyPass ");
							tmp.Append("|");
							theConnection->SendData(tmp.String());
						}

					}
				}
			}
			theView->InputView()->SetText("");
			break;
		}
		
		case DC_GET_FILE_LIST :
		{
#ifdef CLIENT_TO_CLIENT_COMMUNICATION
			/* User har doubleclicked on an item in the list view */
			printf("Invoked\n");
			int32 index;
			BString nick;
			if((index=theView->NickList()->CurrentSelection(0)) >= 0)
			{
				nick = ((BStringItem*)theView->NickList()->ItemAt(index))->Text();
				printf("Got nick: %s\n",nick.String());
			}
#endif
			break;
		}
		
		/* Messages sendt by the Connection object, something to do */
		/* with the server                                          */
		
		case DC_PRIV_MSG: /* Private message */
		{
			BString rvcdstring;
			message->FindString("thetext",&rvcdstring);
			rvcdstring.Prepend("(Priv. msg): ");
			rvcdstring.Append("\n");
			theView->Output()->Insert(rvcdstring.String());
			/* Scroll down when we get new text */
			float scMin,scMax;
			theView->OutScroll()->ScrollBar(B_VERTICAL)->GetRange(&scMin,&scMax);
			theView->OutScroll()->ScrollBar(B_VERTICAL)->SetValue(scMax);
			//printf("Rcvd: %s\n",rvcdstring.String());
			break;
		}
		case DC_TEXT : /* Text, probably chat text */
		{	
			BString rvcdstring;
			message->FindString("thetext",&rvcdstring);
			rvcdstring.RemoveAll("\r"); /* windows lines are terminated by \r\n, strip out the \r's*/
			rvcdstring.Append("\n");
			theView->Output()->Insert(rvcdstring.String());
			/* Scroll down when we get new text */
			float scMin,scMax;
			theView->OutScroll()->ScrollBar(B_VERTICAL)->GetRange(&scMin,&scMax);
			theView->OutScroll()->ScrollBar(B_VERTICAL)->SetValue(scMax);
			//printf("Rcvd: %s\n",rvcdstring.String());
			break;
		}
		case DC_USER_CONNECTED: /* New user */
		{
			BString rvcdstring;
			bool foundit = false;
			message->FindString("nick",&rvcdstring);
			//printf("Got nick: %s\n",rvcdstring.String());
			for(int i=0;i<theView->NickList()->CountItems();i++)
			{
				if(!rvcdstring.Compare(((BStringItem*)theView->NickList()->ItemAt(i))->Text()))
					foundit = true;
			}
			if(!foundit)
				theView->NickList()->AddItem(new BStringItem(rvcdstring.String()));
			break;
		}
		case DC_USER_DISCONNECTED : /* User left hub */
		{
			BString rvcdstring;
			bool foundit = false;
			int index = 0;
			message->FindString("nick",&rvcdstring);
			//printf("Got nick: %s\n",rvcdstring.String());
			for(int i=0;i<theView->NickList()->CountItems();i++)
			{
				if(!rvcdstring.Compare(((BStringItem*)theView->NickList()->ItemAt(i))->Text()))
				{
					foundit = true;
					index = i;
				}
			}
			if(foundit)
				theView->NickList()->RemoveItem(index);
			break;
		}
		case DC_NEED_PASS:
		{
			theView->Output()->Insert("Use /pass <password> to supply the password, or choose another nick\n");
			break;
		}
		case DC_USER_CONNECT : /* Other user wants us to connect to him */
		{
#ifdef CLIENT_TO_CLIENT_COMMUNICATION
			BString rvcdstring, tmpstr;
			message->FindString("address",&rvcdstring);
			int colonindex = rvcdstring.FindFirst(":");
			rvcdstring.MoveInto(tmpstr,0,colonindex);
			rvcdstring.RemoveFirst(":");
			DCClientConnection dccc;
			dccc.Connect(tmpstr.String(),atoi(rvcdstring.String()));
#endif
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}

void DCWindow::FrameResized(float width, float height)
{
	BRect fr;
	fr = theView->Output()->Bounds();
	fr.left += 3;
	fr.right -= 3;
	theView->Output()->SetTextRect(fr);
}

void DCWindow::DispatchMessage(BMessage * msg, BHandler * target)
{
	if(msg->what==B_KEY_DOWN)
	{
		int8 ch;
		int32 modifier; /* We don't use this for anything yet */
		if((msg->FindInt8("byte",&ch)==B_NO_ERROR)&&(msg->FindInt32("modifiers",&modifier)==B_NO_ERROR)&&(ch==B_ENTER))
		{
			BTextView * targetView = dynamic_cast<BTextView *>(target);
			if((target)&&(targetView==theView->InputView()))
			{
				PostMessage(new BMessage(DC_SEND_BUTTON)); /* Hmm, should rename this... */
				msg = NULL;
			}
		}
	}
	if(msg)
		BWindow::DispatchMessage(msg,target);
}
