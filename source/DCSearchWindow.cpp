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
#include "DCSearchView.h" 
#include "DCConnection.h"
#include "DCClientConnection.h"
#include "DCSettings.h"
#include "DCSearchWindow.h"


DCSearchWindow::DCSearchWindow(BRect frame) 
: BWindow(frame, "Search", B_TITLED_WINDOW,B_NOT_ZOOMABLE)
{
	BRect b = Bounds();
	BMenu *menu;
	BMenuItem *item;
	
	menubar = (new BMenuBar(b,"menubar"));
	menu = new BMenu("Search");
	menu->AddItem(item = new BMenuItem("Download", new BMessage(DC_GET_FILE)));
	menu->AddItem(item = new BMenuItem("Stop Download", new BMessage(DC_STOPP_DOWNLOAD)));
	menu->AddItem(item = new BMenuItem("Resume Download", NULL));
	menu->AddSeparatorItem();
	menu->AddItem(item = new BMenuItem("Close", new BMessage(DC_CLOSE_WINDOW)));
	menubar->AddItem(menu);
	
	AddChild(menubar);
	theView = new DCSearchView(b);
	AddChild(theView);
	Show();
}

DCSearchWindow::~DCSearchWindow()
{
}

void DCSearchWindow::MessageReceived(BMessage *message)
{
	switch(message->what)
	{
		case DC_SEARCH_REQUEST:
		{
			//search
		}break;
		
		case DC_GET_FILE:
		{
			//download selected file
		}break;
		
		case DC_CLOSE_WINDOW:
		{
		/* close search window */
			Quit();
		}break;
		
		default:
			BWindow::MessageReceived(message);
	}	
}