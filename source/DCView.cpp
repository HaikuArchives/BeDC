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


//#include <stdio.h>

#include <Application.h>
#include <View.h>
#include <Rect.h>
#include <TextControl.h>
#include <TextView.h>
#include <Button.h>
#include <ListView.h>
#include <ScrollView.h>

#include "DCDefs.h"
#include "DCView.h"

DCView::DCView(BRect frame)
: BView(frame,"dcView",B_FOLLOW_ALL_SIDES,B_WILL_DRAW)
{
	SetViewColor(216,216,216);
	BRect fr;
	
	/* Nick Box */
	fr = BRect(10,10,150,20);
	nickCtrl = new BTextControl(fr,"nick","Nickname:",NULL, new BMessage(DC_NICK_CHANGE));
	nickCtrl->TextView()->DisallowChar('$'); /* Nicknames can't contain $,| or spaces */
	nickCtrl->TextView()->DisallowChar('|');
	nickCtrl->TextView()->DisallowChar(' ');
	AddChild(nickCtrl);

	/* Server Box */
	fr = BRect(170,10,320,20);
	serverCtrl = new BTextControl(fr,"server","Server:",NULL,new BMessage(DC_SERVER_CHANGE));
	AddChild(serverCtrl);
	
	/* Connect button */
	fr = BRect(350,10,440,20);
	connBtn = new BButton(fr,"connect","Connect",new BMessage(DC_CONNECT_BUTTON));
	AddChild(connBtn);

	/* Output textview */
	fr = Bounds();
	fr.top += 40;
	fr.left += 10;
	fr.right -= (140+B_V_SCROLL_BAR_WIDTH);
	fr.bottom -= 30;
	outView = new BTextView(fr,"output",fr,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	AddChild(outScrollView = new BScrollView("outscrollview", outView,
				B_FOLLOW_ALL_SIDES, 0, false, true));
	fr = outView->Bounds();
	fr.left += 3;
	fr.right -= 3;
	outView->SetTextRect(fr);
	outView->MakeEditable(false);

	/* User ListView */
	fr = Bounds();
	fr.top +=40;
	fr.left = fr.right-125;
	fr.right -= (10+B_V_SCROLL_BAR_WIDTH);
	fr.bottom -= 30;
	nickList = new BListView(fr,"nicks",B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES);
	nickList->SetInvocationMessage(new BMessage(DC_GET_FILE_LIST));
	AddChild(nickScrollView = new BScrollView("nickscrollview", nickList,
				B_FOLLOW_RIGHT|B_FOLLOW_TOP_BOTTOM, 0, false, true));
	

	/* Input box */
	fr = Bounds();
	fr.top = fr.bottom - 25;
	fr.left += 5;
	fr.right -= 10;
	fr.bottom -= 5;
	inCtrl = new BTextControl(fr,"input",NULL,NULL,NULL,B_FOLLOW_BOTTOM|B_FOLLOW_LEFT_RIGHT);
	inCtrl->TextView()->DisallowChar('|');
	inCtrl->TextView()->DisallowChar('$'); /* $ works in public chat, but in private messages it fscks up... */
	AddChild(inCtrl);                      /* Only the stuff after the last $ show up in the other end       */
}

DCView::~DCView()
{
}

void DCView::Draw(BRect updaterect)
{

}
