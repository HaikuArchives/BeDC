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
#include <Alert.h>


#include "DCDefs.h"
#include "DCApp.h"
#include "DCWindow.h"
#include "DCView.h" 
#include "DCConnection.h"
#include "DCClientConnection.h"
#include "DCSettings.h"
#include "DCSearchWindow.h"

DCWindow::DCWindow(BRect frame)
	: BWindow(frame, "BeDC", B_TITLED_WINDOW, B_NOT_ZOOMABLE)
{
	BRect b = Bounds();
	BMenu *menu, *submenu;
	BMenuItem *item;
	BString tmp;
	
	fMenubar = new BMenuBar(b,"fMenubar");
	menu = new BMenu("File");
	menu->AddItem(item = new BMenuItem("About", new BMessage(DC_FILE_ABOUT)));
	menu->AddSeparatorItem();
	menu->AddItem(item = new BMenuItem("Quit", new BMessage(DC_QUIT),'Q'));
	fMenubar->AddItem(menu);
	
	menu = new BMenu("Edit");
	fMenubar->AddItem(menu);
	
	menu = new BMenu("Settings");
	fConnMenu = submenu = new BMenu("Connection");
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
	fMenubar->AddItem(menu);
	
	menu = new BMenu("Windows");
	item = new BMenuItem("Show hub list", new BMessage(DC_SHOW_HUB_LIST));
	//item->SetEnabled(false);	// 20021205, VV
	menu->AddItem(item);
	item = new BMenuItem("Show search window", new BMessage(DC_SEARCH_WINDOW), 'S');
	//item->SetEnabled(false);
	menu->AddItem(item);
	fMenubar->AddItem(menu);
		
	menu = new BMenu("Help");
	fMenubar->AddItem(menu);
	
	AddChild(fMenubar);
	
	b.top += fMenubar->Bounds().bottom + 1;
	fView = new DCView(b);
	AddChild(fView);
	
	fConnection = new DCConnection;
	fConnection->SetMessageTarget(this);
	
	fHTTP = new DCHTTPConnection;
	
	Show();
	PostMessage(DC_INIT_WINDOW);
}

DCWindow::~DCWindow()
{
	delete fConnection;
	delete fHTTP;
}

void 
DCWindow::Init()
{
	BString tmp;
	if((dc_app->GetSettings()->GetString("nick",&tmp))==B_OK)
	{
		fView->NickView()->SetText(tmp.String());
		fConnection->SetNick(tmp.String());
		fView->NickView()->Invalidate();
	}
	if((dc_app->GetSettings()->GetString("server",&tmp))==B_OK)
	{
		fView->ServerView()->SetText(tmp.String());
		fView->ServerView()->Invalidate();
	}
	
	if((dc_app->GetSettings()->GetString("connection",&tmp))==B_OK)
	{
		fConnection->SetConn(tmp.String());
		for(int i=0;i<fConnMenu->CountItems();i++)
		{
			if(!tmp.Compare(fConnMenu->ItemAt(i)->Label()))
				fConnMenu->ItemAt(i)->SetMarked(true);
		}	
	}
	
	// TEST
	if (fHTTP->Connect())
	{
		BString httpRequest = "GET /";
		httpRequest += fHTTP->GetFile();
		httpRequest += " HTTP/1.1\nUser-Agent: BeDC/0.1-alpha\nHost: ";
		httpRequest += fHTTP->GetServer();
		httpRequest += "\n\n";
		printf("Sending the following request: %s\n", httpRequest.String());
		printf("Sent: %d\n", fHTTP->Send(httpRequest));
	}
}

bool 
DCWindow::QuitRequested()
{
	printf("DCWindow::QuitRequested\n");
	fConnection->Disconnect();
	// TEST
	fHTTP->Disconnect();
	be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}

void 
DCWindow::MessageReceived(BMessage *message)
{
	BTextControl *ptr;
	switch(message->what)
	{
		case DC_INIT_WINDOW :
		{
			Init();
			//fView->InputView()->MakeFocus();
			break;
		}
		
		/* The following messages are sendt by the view, */
		/* the user has done something                   */
		
		case DC_FILE_ABOUT:
		{
			BAlert* aboutBox = new BAlert("About", "BeDC \n Direct Connect Client for BeOS\n 
										   http://www.sourceforge.net/projects/bedc", "OK");
			aboutBox->Go();
		}break;
		
		case DC_QUIT:
		{
			QuitRequested();
		}break;
		
		case DC_SEARCH_WINDOW:
		{
			BRect r;
			r = Frame();
			r.OffsetBy(20,20);
			new DCSearchWindow(r);
			/* Creates a new search window(S.W.) if no S.W. exists */
			/* if a searsh window exists, make it focus */
					
		}break;
		case DC_CONNECTION_CHANGED : /* User selected their connection in the menu */
		{
			BMenuItem *menuitm;
			message->FindPointer("source",(void**)&menuitm);
			if((menuitm))
			{
				fConnection->SetConn(menuitm->Label());
				dc_app->GetSettings()->SetString("connection",menuitm->Label());	
			}
			break;
		}
		
		case DC_NICK_CHANGE: /* Nick Changed*/
		{
			message->FindPointer("source",(void**)&ptr);
			fConnection->SetNick(ptr->TextView()->Text());
			if(fConnection->IsConnected())
			{
				fConnection->Disconnect();
				fView->NickList()->RemoveItems(0,fView->NickList()->CountItems());
				fConnection->Connect(fView->GetServer(),411);
			}
			dc_app->GetSettings()->SetString("nick",ptr->TextView()->Text());
			break;
		}
		case DC_SERVER_CHANGE: /* Server Changed */
		{
			message->FindPointer("source",(void**)&ptr);
			if(strcmp(fView->GetNick(),"") != 0)
				fConnection->SetNick(fView->GetNick());
			if(fConnection->IsConnected())
			{
				fConnection->Disconnect();
				fView->NickList()->RemoveItems(0,fView->NickList()->CountItems());
			}
			dc_app->GetSettings()->SetString("server",ptr->TextView()->Text());
			fConnection->Connect(ptr->TextView()->Text(),411);
			break;
		}
		case DC_CONNECT_BUTTON : /* User has clicked connect button */
		{
			if(strcmp(fView->GetNick(),"") != 0)
				fConnection->SetNick(fView->GetNick());
			if(strcmp(fView->GetServer(),"") != 0)
			{
				if(fConnection->IsConnected())
				{
					fConnection->Disconnect();
					fView->NickList()->RemoveItems(0,fView->NickList()->CountItems());
				}
				fConnection->Connect(fView->GetServer(),411);
			}
			break;	
		}
		case DC_SEND_BUTTON : /* User har clicked Send button */
		{
			/* Todo: move parsing of commands to seperate method */
			BString tmp = fView->InputView()->Text();
			if (tmp!= "")
			{
				if(tmp[0] != '/')
				{
					if (fConnection->IsConnected())
					{
						tmp.Prepend("> ");
						tmp.Prepend(fConnection->GetNick());
						tmp.Prepend("<");
						tmp.Append("|");
						fConnection->SendData(tmp.String());
					}
					else
						fView->Output()->Insert("Error; Not connected to hub\n");
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
						fView->Output()->Insert("Available commands:\n");
						fView->Output()->Insert("/help                 - shows this text\n");
						fView->Output()->Insert("/clear                - clear this view\n");
						fView->Output()->Insert("/msg <nick> <message> - sends <message> to <nick> (Case sensitive)\n");
					}
					else if(!tmpb.Compare("/MSG")) /* Sends a private message */
					{
						if(fConnection->IsConnected())
						{
							if(tmp.Length() <= 5)
								fView->Output()->Insert("Error; Use /msg <nick> <message>\n");
							else
							{
								tmp.Remove(0,5);
								if((tmpint = tmp.FindFirst(" ")) < 0) /* no spaces */
									fView->Output()->Insert("Error; Use /msg <nick> <message>\n");
								else
								{
									/* Check if the nick exists, and send the message if it does    */
									/* Dunno if the NM Client does this or just send it to the hub  */
									bool foundit = false;
									BString tmpc;
									tmp.MoveInto(tmpc,0,tmpint);
									for(int i=0;i<fView->NickList()->CountItems();i++)
									{
										if(!tmpc.Compare(((BStringItem*)fView->NickList()->ItemAt(i))->Text()))
											foundit = true;
									}
									if(!foundit)
										fView->Output()->Insert("Error; No such user\n");
									else
									{
										tmpc.Prepend("$To: ");
										tmpc.Append(" From: ");
										tmpc.Append(fConnection->GetNick());
										tmpc.Append(" $<");
										tmpc.Append(fConnection->GetNick());
										tmpc.Append(">");
										tmpc.Append(tmp);
										tmpc.Append("|");
										fConnection->SendData(tmpc.String());
									}
								}
							}
						}
						else
							fView->Output()->Insert("Error; Not connected to hub\n");	
					}
					else if(!tmpb.Compare("/CLEAR")) /* Clears the output view */
					{
						fView->Output()->SetText("");
					}
#ifdef CLIENT_TO_CLIENT_COMMUNICATION
					else if(!tmpb.Compare("/GET")) /* Download a file */
					{
						if(tmp.Length() <= 5)
								fView->Output()->Insert("Error; Use /get <nick> <remotepath>\n");
						tmp.Remove(0,5);
						printf("%s\n",tmp.String());					
					}
#endif
					else if(!tmpb.Compare("/PASS"))
					{
						if(tmp.Length() <= 6)
								fView->Output()->Insert("Error; Use /pass <password>\n");
						else
						{
							tmp.Remove(0,6);
							tmp.Prepend("$MyPass ");
							tmp.Append("|");
							fConnection->SendData(tmp.String());
						}

					}
				}
			}
			fView->InputView()->SetText("");
			break;
		}
		
		case DC_GET_FILE_LIST :
		{
#ifdef CLIENT_TO_CLIENT_COMMUNICATION
			/* User har doubleclicked on an item in the list view */
			printf("Invoked\n");
			int32 index;
			BString nick;
			if((index=fView->NickList()->CurrentSelection(0)) >= 0)
			{
				nick = ((BStringItem*)fView->NickList()->ItemAt(index))->Text();
				printf("Got nick: %s\n",nick.String());
				nick.Prepend(" ");
				nick.Prepend(fConnection->GetNick());
				nick.Prepend("$RevConnectToMe ");
				nick.Append("|");
				fConnection->SendData(nick.String());
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
			fView->Output()->Insert(rvcdstring.String());
			/* Scroll down when we get new text */
			float scMin,scMax;
			fView->OutScroll()->ScrollBar(B_VERTICAL)->GetRange(&scMin,&scMax);
			fView->OutScroll()->ScrollBar(B_VERTICAL)->SetValue(scMax);
			//printf("Rcvd: %s\n",rvcdstring.String());
			break;
		}
		case DC_TEXT : /* Text, probably chat text */
		{	
			BString rvcdstring;
			message->FindString("thetext",&rvcdstring);
			rvcdstring.RemoveAll("\r"); /* windows lines are terminated by \r\n, strip out the \r's*/
			rvcdstring.Append("\n");
			fView->Output()->Insert(rvcdstring.String());
			/* Scroll down when we get new text */
			float scMin,scMax;
			fView->OutScroll()->ScrollBar(B_VERTICAL)->GetRange(&scMin,&scMax);
			fView->OutScroll()->ScrollBar(B_VERTICAL)->SetValue(scMax);
			//printf("Rcvd: %s\n",rvcdstring.String());
			break;
		}
		case DC_USER_CONNECTED: /* New user */
		{
			BString rvcdstring;
			bool foundit = false;
			message->FindString("nick",&rvcdstring);
			//printf("Got nick: %s\n",rvcdstring.String());
			for(int i=0;i<fView->NickList()->CountItems();i++)
			{
				if(!rvcdstring.Compare(((BStringItem*)fView->NickList()->ItemAt(i))->Text()))
					foundit = true;
			}
			if(!foundit)
				fView->NickList()->AddItem(new BStringItem(rvcdstring.String()));
			break;
		}
		case DC_USER_DISCONNECTED : /* User left hub */
		{
			BString rvcdstring;
			bool foundit = false;
			int index = 0;
			message->FindString("nick",&rvcdstring);
			//printf("Got nick: %s\n",rvcdstring.String());
			for(int i=0;i<fView->NickList()->CountItems();i++)
			{
				if(!rvcdstring.Compare(((BStringItem*)fView->NickList()->ItemAt(i))->Text()))
				{
					foundit = true;
					index = i;
				}
			}
			if(foundit)
				fView->NickList()->RemoveItem(index);
			break;
		}
		case DC_NEED_PASS:
		{
			fView->Output()->Insert("Use /pass <password> to supply the password, or choose another nick\n");
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
			DCClientConnection *dccc = new DCClientConnection();
			dccc->SetNick(fConnection->GetNick());
			dccc->DownloadNickList();
			dccc->Connect(tmpstr.String(),atoi(rvcdstring.String()));
			
#endif
			break;
		}
		default:
			BWindow::MessageReceived(message);
	}
}

void 
DCWindow::FrameResized(float width, float height)
{
	/* Change text rect in output view */
	BRect fr;
	fr = fView->Output()->Bounds();
	fr.left += 3;
	fr.right -= 3;
	fView->Output()->SetTextRect(fr);
	
	dc_app->GetSettings()->SetRect("windowrect",Frame());
}

void 
DCWindow::FrameMoved(BPoint origin)
{
	dc_app->GetSettings()->SetRect("windowrect",Frame());
}

void 
DCWindow::DispatchMessage(BMessage * msg, BHandler * target)
{
	if (msg->what == B_KEY_DOWN)
	{
		int8 ch;
		int32 modifier; /* We don't use this for anything yet */
		if ((msg->FindInt8("byte",&ch) == B_NO_ERROR) &&
			(msg->FindInt32("modifiers",&modifier) == B_NO_ERROR) &&
			(ch == B_ENTER))
		{
			BTextView * targetView = dynamic_cast<BTextView *>(target);
			if(target && (targetView==fView->InputView()))
			{
				PostMessage(new BMessage(DC_SEND_BUTTON)); /* Hmm, should rename this... */
				msg = NULL;
			}
		}
	}
	if (msg)
		BWindow::DispatchMessage(msg, target);
}
