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
#ifndef _DC_HUB_LIST_VIEW_H_
#define _DC_HUB_LIST_VIEW_H_

#ifdef NO_CRASH_LIST

#include "santa/ColumnListView.h"

class DCHubListView : public ColumnListView
{
public:
						DCHubListView(BRect frame, CLVContainerView ** containerView, const char*  name = NULL, 
									  uint32 resizingMode = B_FOLLOW_ALL, uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE,
									  list_view_type type = B_SINGLE_SELECTION_LIST, bool hierarchical = false,	bool horizontal = true,
									  bool vertical = true, bool scroll_view_corner = true,	border_style border = B_FANCY_BORDER,
									  const BFont * labelFont = be_plain_font)
							: ColumnListView(frame, containerView, name, resizingMode, flags, type, hierarchical, horizontal,
											 vertical, scroll_view_corner, border, labelFont) {}

	virtual void		MouseDown(BPoint point)
	{
		ColumnListView::MouseDown(point);
		MakeFocus(true);
	}
};

#endif

#endif	// _DC_HUB_LIST_VIEW_H_
