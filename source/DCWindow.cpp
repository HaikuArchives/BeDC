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

#include <View.h>
#include <Message.h>

DCWindow::DCWindow(BRect pos)
	: BWindow(pos, "BeDC" /* i don't think the name can be localized ;) */,
			  B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS)
{
	InitGUI();
}

DCWindow::~DCWindow()
{
}

void
DCWindow::MessageReceived(BMessage * msg)
{
	switch (msg->what)
	{
		default:
			BWindow::MessageReceived(msg);
			break;
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
	AddChild(
		fParentView = new BView(Bounds(), "parent_view", B_FOLLOW_ALL, B_WILL_DRAW)
	);
	fParentView->SetViewColor(216, 216, 216);
}
