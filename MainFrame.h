#include <afxwin.h>
#include "fysik/Physics.h"
#include "Player.h"
#include "Missile.h"
#include <iostream>

using namespace std;

#define UPDATE_TIMER 1

class CMainFrame: public CFrameWnd {
private:
	Physics engine;
	vector<Object*> *objects;
	Player p1;
	Player p2;
	CPoint MousePos;
	bool start;
	int angle;

public:
	CMainFrame();
	~CMainFrame();

	void Fire(Player &p);

	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRep, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint pt);
	
	DECLARE_MESSAGE_MAP();
};
