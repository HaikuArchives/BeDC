#include <Application.h>
#include <View.h>
#include <Rect.h>
#include <TextControl.h>
#include <TextView.h>
#include <Button.h>
#include <ListView.h>
#include <ScrollView.h>
#include <List.h>
#include <iostream.h>

#if 0
#include "ColumnListView.h"
#include "ColumnTypes.h"

#include "DCDefs.h"
#include "DCSearchView.h"

DCSearchView::DCSearchView(BRect frame)
: BView(frame, "dcSearchView", B_FOLLOW_ALL_SIDES, B_WILL_DRAW)
{
	SetViewColor(216,216,216);
	BRect fr;
	
	/* search box */
	fr = BRect(10,30,550,50);
	fileCtrl = new BTextControl(fr,"search", "Search:",NULL, new BMessage(DC_SEARCH_REQUEST));
	AddChild(fileCtrl);
	
	/*search button */
	fr = BRect(570,30,620,50);
	searchBtn = new BButton(fr, "search", "Search", new BMessage(DC_SEARCH_REQUEST));
	AddChild(searchBtn);
	
	/* download button */
	//fr = BRect(40,360,130,380);
	fr = Bounds();
	downloadBtn = new BButton(BRect(40,fr.bottom-35,130,fr.bottom-55), "download", "Download", new BMessage(DC_GET_FILE));
	AddChild(downloadBtn);
	
	/* stopp download button */
	//fr = BRect(150,360,260,380);
	stoppBtn = new BButton(BRect(150,fr.bottom-35,260,fr.bottom-55), "stoppdownload" , "Stopp Download" , new BMessage(DC_STOPP_DOWNLOAD));
	AddChild(stoppBtn);
	
	/* Search file ListView */
	fr = Bounds();
	fr.top +=80;
	fr.left = 20;
	fr.right -= (10+B_V_SCROLL_BAR_WIDTH);
	fr.bottom -=60;
	fileList = new BColumnListView(fr,"files", B_SINGLE_SELECTION_LIST,B_FOLLOW_ALL_SIDES);
	fileList->SetInvocationMessage(new BMessage(DC_GET_FILE));
	AddChild(fileList); 
	/* Add Columns */
	userColumn = new BStringColumn("User",100,50,200,B_ALIGN_LEFT);
	fileNameColumn = new BStringColumn("File Name",150,50,200,B_ALIGN_LEFT);
	sizeColumn = new BSizeColumn("Size",100,50,200,B_ALIGN_LEFT);
	pathColumn = new BStringColumn("Path",150,50,200,B_ALIGN_LEFT);
	slotsColumn = new BStringColumn("Slots",100,50,200,B_ALIGN_LEFT);
	
	fileList->AddColumn(userColumn,0);
	fileList->AddColumn(fileNameColumn,1);
	fileList->AddColumn(sizeColumn,2);
	fileList->AddColumn(pathColumn,3); 
	fileList->AddColumn(slotsColumn,4);
	
	/* Test Fields */
	AddItem("nils", "cool-movie.avi", 60452323,"boot/home","3/4");
	AddItem("nils", "cool-music.mp3", 6452323,"boot/home","3/4");
	AddItem("unkown", "software.zip", 6452323,"prog/misc","1/4");
  AddItem("doctor", "dokument.doc", 6452323,"data/docs","1/5");

}

void DCSearchView::AddItem(char* user, char* fileName, off_t size, char* path, char* slots)
{
	BRow *row(new BRow());
	BStringField *userField (new BStringField (user));
	BStringField *fileNameField (new BStringField (fileName));
	BSizeField *sizeField(new BSizeField (size));
	BStringField *pathField(new BStringField(path));
	BStringField *slotsField(new BStringField(slots));
	
	row->SetField (userField, 0);
	row->SetField (fileNameField,1);
	row->SetField (sizeField,2);
	row->SetField (pathField,3);
	row->SetField (slotsField,4);
	
	fileList->AddRow(row);	
}

DCSearchView::~DCSearchView()
{
}

void DCSearchView::Draw(BRect updaterect)
{
}
#endif
