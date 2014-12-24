#include <afxwin.h>
#include "fysik/Physics.h"
#include <iostream>

using namespace std;

#define UPDATE_TIMER 1

class CMainFrame: public CFrameWnd {
private:
	int angle;
	Physics engine;
	vector<Object*> *objects;

public:
	CMainFrame();
	~CMainFrame();
	
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	
	DECLARE_MESSAGE_MAP();
};
