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

	engine.addObject(new Object(Vector(-50,0), Vector(0,10), 20, 1e13, "Sun"));
	engine.addObject(new Object(Vector(50,0), Vector(0,-10), 20, 1e13, "Sun"));
	engine.addObject(new Object(Vector(250,0), Vector(0,25), 15, 6e5, "Arakkis"));
	engine.addObject(new Object(Vector(-350,0), Vector(0,25), 10, 1e5, "Tatooine"));
	engine.addObject(new Object(Vector(0,-200), Vector(-20,0), 8, 6e5, "Earth"));
	engine.addObject(new Object(Vector(-100,250), Vector(-23,0), 12, 6e5, "Vulcan"));
	engine.addObject(new Object(Vector(100,-150), Vector(24,0), 10, 1e5, "Lanthea"));
//	engine.addObject(new Object(Vector(-75,70), Vector(17,20), 10, 1e5, "Rigel IV"));

	objects = engine.getObjectsPointer();

	Create(NULL,"Planetoids",WS_OVERLAPPEDWINDOW,CRect(20,30,900,650));

	SetTimer(UPDATE_TIMER, 80, NULL);
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
			int middle_missile_x = planet_x+radius*cos(p->GetAngle()*pi/180);
			int middle_missile_y = planet_y+radius*sin(p->GetAngle()*pi/180);
			dc.MoveTo(CPoint(middle_x+middle_missile_x, middle_y+middle_missile_y));
			dc.LineTo(CPoint(middle_x+middle_missile_x+2		*cos((p->GetAngle()+90)*pi/180),			middle_y+middle_missile_y+2*		sin((p->GetAngle()+90)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+6.32455532*cos((p->GetAngle()+18.4349488)*pi/180),	middle_y+middle_missile_y+6.32455532*sin((p->GetAngle()+18.4349488)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+7.21110255*cos((p->GetAngle()+33.6900675)*pi/180),	middle_y+middle_missile_y+7.21110255*sin((p->GetAngle()+33.6900675)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+12		*cos(p->GetAngle()*pi/180),					middle_y+middle_missile_y+12*	sin(p->GetAngle()*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+7.21110255*cos((p->GetAngle()-33.6900675)*pi/180),	middle_y+middle_missile_y+7.21110255*sin((p->GetAngle()-33.6900675)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+6.32455532*cos((p->GetAngle()-18.4349488)*pi/180),	middle_y+middle_missile_y+6.32455532*sin((p->GetAngle()-18.4349488)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x+2		*cos((p->GetAngle()-90)*pi/180),			middle_y+middle_missile_y+2*		sin((p->GetAngle()-90)*pi/180)));
			dc.LineTo(CPoint(middle_x+middle_missile_x, middle_y+middle_missile_y));

//			dc.MoveTo(CPoint(middle_x+planet_x,middle_y+planet_y));
//			dc.LineTo(CPoint(middle_x+planet_x+(radius+7)*cos(p->GetAngle()*pi/180), middle_y+planet_y+(radius+7)*sin(p->GetAngle()*pi/180)));
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

	CString str;
	str.Format("Player 1: %d", p1.GetMissiles());
	dc.TextOut(30,5,str);
	str.Format("Player 2: %d", p2.GetMissiles());
	dc.TextOut(30,20,str);
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


void CMainFrame::Fire(Player &p)  {
	if (p.GetPlanet() != -1 && p.GetMissiles() > 0) {
		objects = engine.getObjectsPointer();
		Object *planet=objects->at(p.GetPlanet());
		engine.addObject(new Object(planet->getPosition(), planet->getVelocity()+Vector(0,20), 3, 10, "Missile"));
		p.DecreaseMissile();
	}
}

void CMainFrame::OnChar(UINT nChar, UINT nRep, UINT nFlags)  {
	switch(nChar) {
	case 'a':
		p1.SetAngle(p1.GetAngle()-3);
		break;
	case 'd':
		p1.SetAngle(p1.GetAngle()+3);
		break;
	case 's':
		Fire(p1);
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
	case 'k':
		Fire(p2);
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
