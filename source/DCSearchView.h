#ifndef _DC_SEARCH_VIEW_H
#define _DC_SEARCH_VIEW_H_

class BStringColumn;
class BSizeColumn;
class BColumnListView;

class DCSearchView : public BView
{
	public:
		DCSearchView(BRect frame);
		~DCSearchView();
		virtual void Draw(BRect updaterect);
		BTextView *FileView() {return fileCtrl->TextView();};
		void AddItem(char* user, char* fileName, off_t size, char* path, char* slots);
	protected:
		BTextControl *fileCtrl;
		BButton *downloadBtn;
		BButton *stoppBtn;
		BButton *searchBtn;
		BColumnListView *fileList;
		BScrollView *fileScrollView;
		BStringColumn *fileNameColumn, *userColumn, *pathColumn, *slotsColumn;
		BSizeColumn *sizeColumn;
		
};

#endif /* !_DC_SEARCH_VIEW_H_ */		