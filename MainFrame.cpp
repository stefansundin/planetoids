#include "MainFrame.h"
#include <cmath>

#define IDC_BTN_INC 199
#define IDC_BTN_NUM 200
#define IDC_BTN_DEC 201

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CHAR()
END_MESSAGE_MAP()

CMainFrame::CMainFrame() {
	angle=0;

	engine.addObject(new Object(Vector(0,0), Vector(0,0), 10, 1e12, "stjärna"));
	engine.addObject(new Object(Vector(150,0), Vector(0,2), 4, 6e5, "stor planet"));
	engine.addObject(new Object(Vector(0,-160), Vector(1,0), 1, 1e5, "liten planet"));
	objects = engine.getObjectsPointer();

	Create(NULL,"Planetoids",WS_OVERLAPPEDWINDOW,CRect(20,30,900,510));

	SetTimer(UPDATE_TIMER, 100, NULL);
}

CMainFrame::~CMainFrame() {
	KillTimer(UPDATE_TIMER);
}

void CMainFrame::OnPaint() {
	CPaintDC dc(this);
	
	dc.SelectStockObject(NULL_BRUSH);

	CRect window;
	GetClientRect(&window);

	CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(200,200,200));
	dc.SelectObject(&newBrush);

	const double pi=3.14;
	const int middle_x=window.right/2;
	const int middle_y=window.bottom/2;
	/*dc.MoveTo(CPoint(window.right/2,window.bottom/2));
	dc.LineTo(CPoint(window.right/2+40*cos(angle*pi/180),window.bottom/2+40*sin(angle*pi/180)));
	dc.Ellipse(CRect(CPoint(window.right/2-15,window.bottom/2-15),CSize(30,30)));*/

	objects = engine.getObjectsPointer();
	
	for (int i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		int planet_x=planet->getPosition().getX();
		int planet_y=planet->getPosition().getY();
		dc.MoveTo(CPoint(middle_x+planet_x,middle_y+planet_y));
		dc.LineTo(CPoint(middle_x+planet_x+40*cos(angle*pi/180),middle_y+planet_y+40*sin(angle*pi/180)));
		dc.Ellipse(CRect(CPoint(middle_x-15+planet_x,middle_y-15+planet_y),CSize(30,30)));
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	CClientDC dc(this);

	if (nIDEvent == UPDATE_TIMER) {
		engine.doPhysics();
		Invalidate();
	}
	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnChar(UINT nChar, UINT nRep, UINT nFlags)  {
	switch(nChar) {
	case 'a':
		angle=(angle-3)%360;
		break;
	case 'd':
		angle=(angle+3)%360;
		break;
	case 'x':
		angle=0;
		break;
	default:
		break;
	}
	Invalidate();
}
