#include <afxwin.h>
#include <afxdlgs.h>
#include "fysik/Physics.h"
#include "Player.h"
#include "resources.h"
#include <iostream>

using namespace std;

#define UPDATE_TIMER 1

class CMainFrame: public CFrameWnd {
private:
	CMenu menu;
	Physics engine;
	vector<Object*> *objects;
	Player p1;
	Player p2;
	CPoint MousePos;
	bool start;
	char keys[255];
	int angle;
	Object *follow;
	Vector adjustview;

public:
	CMainFrame();
	~CMainFrame();

	void OnMenuOpen();
	void OnMenuAdd();
	void OnMenuSave();
	void OnMenuBlackhole();
	void LoadSystem(string fn);
	void SaveSystem(string fn);

	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint pt);
	
	DECLARE_MESSAGE_MAP();
};
