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

#include "DCStatusBar.h"

DCStatusBar::DCStatusBar(BRect frame, int32 height)
	: BView(BRect(frame.left, frame.bottom - height, frame.right, frame.bottom),
			"statusbar", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM, B_WILL_DRAW)
{
	SetViewColor(216, 216, 216);
}

void 
DCStatusBar::Draw(BRect frame)
{
	// Draw the main border
	SetDrawingMode(B_OP_COPY);
	SetHighColor(131, 131, 131, 255);
	StrokeLine(BPoint(frame.left, Bounds().top), BPoint(frame.right, Bounds().top));
	SetHighColor(255, 255, 255, 255);
	StrokeLine(BPoint(frame.left, Bounds().top + 1), BPoint(frame.right, Bounds().top + 1));
	
	
	float left = 3;
	float width = 0;
	float bottom = Bounds().bottom;	// shorthand
	BString * str = NULL;

	for (int32 i = 0; i < fFieldList.CountItems(); i++)
	{
		str = fFieldList.ItemAt(i);
		width = StringWidth(str->String());
		SetDrawingMode(B_OP_OVER);
		SetHighColor(0, 0, 0, 255);
		DrawString(str->String(), BPoint(left, bottom - 3));
		if (i != fFieldList.CountItems() - 1)	// not the last item?
		{
			left += width;
			left += 3;
			SetDrawingMode(B_OP_COPY);
			SetHighColor(255, 255, 255, 255);
			StrokeLine(BPoint(left + 1, 2), BPoint(left + 1, bottom));
			SetHighColor(131, 131, 131, 255);
			StrokeLine(BPoint(left, 2), BPoint(left, bottom));
			left += 5;
		}
	}
	

	//DrawString("Status Bar (Under construction :)", BPoint(Bounds().left + 2, Bounds().bottom - 2));
}

void
DCStatusBar::AddField(const BString & value)
{
	BString * str = new BString(value);
	fFieldList.AddItem(str, fFieldList.CountItems());
}

void
DCStatusBar::SetField(const BString & value, int32 index)
{
	if ((index >= fFieldList.CountItems()) || (index < 0))	// invalid index
		return;
		
	BString * str = fFieldList.ItemAt(index);
	fFieldList.SwapWithItem(index, new BString(value));
	delete str;
}

void
DCStatusBar::MakeEmpty()
{
	while (fFieldList.CountItems() > 0)
	{
		BString * str = fFieldList.RemoveItemAt(0);
		delete str;
	}
}
