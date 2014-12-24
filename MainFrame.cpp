#include "MainFrame.h"
#include <cmath>
#include <vector>
#include <ctime>

#define IDC_BTN_INC 199
#define IDC_BTN_NUM 200
#define IDC_BTN_DEC 201

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CMainFrame::CMainFrame() {
	srand(time(NULL));

	start=false;
	MousePos=CPoint(0,0);

	engine.addObject(new Object(Vector(0,0), Vector(0,0), 30, 1e12, "Sun"));
	engine.addObject(new Object(Vector(150,0), Vector(0,2), 15, 6e5, "stor planet"));
	engine.addObject(new Object(Vector(0,-160), Vector(1,0), 10, 1e5, "liten planet"));
	objects = engine.getObjectsPointer();

	Create(NULL,"Planetoids",WS_OVERLAPPEDWINDOW,CRect(20,30,900,510));

	SetTimer(UPDATE_TIMER, 100, NULL);
}

CMainFrame::~CMainFrame() {
	KillTimer(UPDATE_TIMER);
}

void CMainFrame::OnPaint() {
	CPaintDC dc(this);

	dc.SetTextColor(WHITE_BRUSH);


	const double pi=3.14159265358979323846;
	CRect window;
	GetClientRect(&window);
	const int middle_x=window.right/2;
	const int middle_y=window.bottom/2;

	CBrush brush;
	brush.CreateSolidBrush(RGB(200,200,200));
	dc.SelectObject(brush);
	CBrush hoverBrush;
	hoverBrush.CreateSolidBrush(RGB(255,0,0));
	CBrush sunBrush;
	sunBrush.CreateSolidBrush(RGB(255,255,0));

	objects = engine.getObjectsPointer();
	
	for (int i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		int planet_x=planet->getPosition().getX();
		int planet_y=planet->getPosition().getY();
		int radius=planet->getRadius();

		dc.SelectStockObject(BLACK_PEN);
		dc.SelectObject(brush);

		Player *p;
		if (p1.GetPlanet() == i) {
			p=&p1;
		}
		if (p2.GetPlanet() == i) {
			p=&p2;
		}
		if (p1.GetPlanet() == i || p2.GetPlanet() == i) {
			dc.MoveTo(CPoint(middle_x+planet_x,middle_y+planet_y));
			dc.LineTo(CPoint(middle_x+planet_x+(radius+7)*cos(p->GetAngle()*pi/180),middle_y+planet_y+(radius+7)*sin(p->GetAngle()*pi/180)));
		}

		if (!start && p1.GetPlanet() != i && p2.GetPlanet() != i) {
			CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
				middle_x+planet_x+radius,middle_y+planet_y+radius);
			if (planetrect.PtInRect(MousePos)) {
				dc.SelectObject(hoverBrush);
			}
		}

		if (planet->getName() == "Sun") {
			for (int i=0; i < 36; i++) {
				dc.MoveTo(CPoint(middle_x+planet_x,middle_y+planet_y));
				int randlen=rand()%10;
				dc.LineTo(CPoint(middle_x+planet_x+(radius+4+randlen)*cos((angle+i*10)%360*pi/180),middle_y+planet_y+(radius+4+randlen)*sin((angle+i*10)%360*pi/180)));
			}
			dc.SelectStockObject(NULL_PEN);
			dc.SelectObject(sunBrush);
		}

		dc.Ellipse(CRect(CPoint(middle_x+planet_x-radius,middle_y+planet_y-radius),CSize(2*radius,2*radius)));
	}

	if (!start) {
		if (p1.GetPlanet() == -1) {
			dc.TextOut(middle_x-95,window.bottom-30,"Player 1 - Choose your planet");
		}
		else if (p2.GetPlanet() == -1) {
			dc.TextOut(middle_x-95,window.bottom-30,"Player 2 - Choose your planet");
		}
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	CClientDC dc(this);

	if (nIDEvent == UPDATE_TIMER) {
		engine.doPhysics();
		angle=(angle+1)%360;
		Invalidate();
	}
	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnChar(UINT nChar, UINT nRep, UINT nFlags)  {
	switch(nChar) {
	case 'a':
		p1.SetAngle(p1.GetAngle()-3);
		break;
	case 'd':
		p1.SetAngle(p1.GetAngle()+3);
		break;
	case 'x':
		p1.SetAngle(0);
		break;
	case 'j':
		p2.SetAngle(p2.GetAngle()-3);
		break;
	case 'l':
		p2.SetAngle(p2.GetAngle()+3);
		break;
	case ',':
		p2.SetAngle(0);
		break;
	default:
		break;
	}
	Invalidate();
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint pt) {
	MousePos=pt;
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint pt) {
	if (!start) {
		CRect window;
		GetClientRect(&window);
		const int middle_x=window.right/2;
		const int middle_y=window.bottom/2;

		for (int i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			if (planet->getName() != "Sun") {
				int planet_x=planet->getPosition().getX();
				int planet_y=planet->getPosition().getY();
				int radius=planet->getRadius();

				CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
					middle_x+planet_x+radius,middle_y+planet_y+radius);

				if (planetrect.PtInRect(pt)) {
					if (p1.GetPlanet() == -1 && p2.GetPlanet() != i) {
						p1.SetPlanet(i);
					}
					else if (p2.GetPlanet() == -1 && p1.GetPlanet() != i) {
						p2.SetPlanet(i);
						start=1;
					}
					return;
				}
			}
		}
	}
}
