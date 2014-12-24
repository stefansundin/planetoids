#include <afxwin.h>
#include <afxdlgs.h>
#include <afxcmn.h>
#include "fysik/Physics.h"
#include "Player.h"
#include "resources.h"
#include <iostream>

using namespace std;

#define UPDATE_TIMER 1
#define DRAW_TIMER 2
#define IDC_SLH_ZOOM 300
#define IDC_SLH_SPEED 301

static Physics *engine;
static Player p1;
static Player p2;
static int winner;
static bool start;

class CMainFrame: public CFrameWnd {
private:
	CMenu menu;
	CSliderCtrl zoom, speed;
	vector<Object*> *objects;
	CPoint MousePos;
	string file;
	bool invalidate;
	char keys[255];
	int angle;
	int timestep;
	__int64 scale, stdscale;
	Object *follow;
	Vector adjustview;

public:
	CMainFrame();
	~CMainFrame();

	void OnMenuReset();
	void OnMenuOpen();
	void OnMenuAdd();
	void OnMenuSave();
	void OnMenuBlackhole();
	void OnMenuInvalidate();
	void LoadSystem(string fn);
	void SaveSystem(string fn);

	static void collide(Object* obj1, Object* obj2);

	afx_msg void OnPaint();
	afx_msg void OnKeyDown(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CWnd *pSlider);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CWnd *pSlider);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize();
	
	DECLARE_MESSAGE_MAP();
};
