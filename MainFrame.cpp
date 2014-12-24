#include "MainFrame.h"
#include <cmath>
#include <vector>
#include <ctime>
#include <fstream>

const double pi=3.14159265358979323846;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_MENU_OPEN,OnMenuOpen)
	ON_COMMAND(IDM_MENU_ADD,OnMenuAdd)
	ON_COMMAND(IDM_MENU_SAVE,OnMenuSave)
	ON_COMMAND(IDM_MENU_BLACKHOLE,OnMenuBlackhole)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() {
	srand(time(NULL));

	memset(keys,0,sizeof(keys));
	start=false;
	follow=NULL;
	MousePos=CPoint(0,0);

	p1.SetEngine(&engine);
	p2.SetEngine(&engine);

	Create(NULL,"Planetoids",WS_OVERLAPPEDWINDOW,CRect(20,30,900,650));
	menu.LoadMenu(IDR_MENU);
	SetMenu(&menu);

	HINSTANCE hInst=AfxGetInstanceHandle();
	HICON icon=LoadIcon(hInst,"icon");
	SetIcon(icon,FALSE);

	LoadSystem("system.txt");
	objects = engine.getObjectsPointer();

	SetTimer(UPDATE_TIMER, 80, NULL);
}

CMainFrame::~CMainFrame() {
	KillTimer(UPDATE_TIMER);
}

void CMainFrame::OnMenuOpen() {
	CFileDialog FileDlg(TRUE, ".txt", NULL, 0, "Planetoid Systems (*.txt)|*.txt|All Files (*.*)|*.*||");
	if( FileDlg.DoModal() == IDOK )
	{
		char file[255];
		strcpy(file, FileDlg.GetFileName());
		engine.clear();
		LoadSystem(file);
	}
}

void CMainFrame::OnMenuAdd() {
	CFileDialog FileDlg(TRUE, ".txt", NULL, 0, "Planetoid Systems (*.txt)|*.txt|All Files (*.*)|*.*||");
	if( FileDlg.DoModal() == IDOK )
	{
		char file[255];
		strcpy(file, FileDlg.GetFileName());
		LoadSystem(file);
	}
}

void CMainFrame::OnMenuSave() {
	CFileDialog FileDlg(FALSE, ".txt", NULL, 0, "Planetoid Systems (*.txt)|*.txt|All Files (*.*)|*.*||");
	if( FileDlg.DoModal() == IDOK )
	{
		char file[255];
		strcpy(file, FileDlg.GetFileName());
		SaveSystem(file);
	}
}

void CMainFrame::OnMenuBlackhole() {
	engine.addObject(new Object(Vector(200,200), Vector(-3,-3), 13, 1e13, "Blackhole"));
}

void CMainFrame::LoadSystem(string fn) {
	FILE *file;
	if ((file=fopen(fn.c_str(),"rb")) != NULL) {
		float x,y,velx,vely,r,m;
		char name[100];
		while (fscanf(file,"%f\t%f\t%f\t%f\t%f\t%f\t%s",&x,&y,&velx,&vely,&r,&m,name) != EOF) {
			for (char *c=name; *c != '\0'; c++) {
				if (*c == '_') {
					*c=' ';
				}
			}
			engine.addObject(new Object(Vector(x,y), Vector(velx,vely), r, m, name));
		}
		fclose(file);
	}
}

void CMainFrame::SaveSystem(string fn) {
	FILE *file;
	if ((file=fopen(fn.c_str(),"wb")) != NULL) {
		objects = engine.getObjectsPointer();
		for (int i=0; i < objects->size(); i++) {
			Object *p=objects->at(i);
			Vector pos=p->getPosition();
			Vector vel=p->getVelocity();
			char name[100];
			strcpy(name,p->getName().c_str());
			for (char *c=name; *c != '\0'; c++) {
				if (*c == ' ') {
					*c='_';
				}
			}
			fprintf(file,"%f\t%f\t%f\t%f\t%f\t%f\t%s\n",pos.getX(),pos.getY(),vel.getX(),vel.getY(),p->getRadius(),p->getMass(),name);
		}
		fclose(file);
	}
}

void CMainFrame::OnPaint() {
	CPaintDC dc(this);

	//dc.SetTextAlign(TA_CENTER);
	dc.SetTextColor(WHITE_BRUSH);

	CString str;
	CRect window;
	GetClientRect(&window);
	int middle_x=window.right/2-adjustview.getX();
	int middle_y=window.bottom/2-adjustview.getY();
	if (follow != NULL) {
		middle_x-=follow->getPosition().getX();
		middle_y-=follow->getPosition().getY();
	}

	CBrush brush;
	brush.CreateSolidBrush(RGB(200,200,200));
	dc.SelectObject(brush);
	CBrush hoverBrush;
	hoverBrush.CreateSolidBrush(RGB(255,0,0));
	CBrush sunBrush;
	sunBrush.CreateSolidBrush(RGB(255,255,0));

	CFont font;
	font.CreatePointFont(80,"Arial");
	dc.SelectObject(font);

	objects = engine.getObjectsPointer();
	for (int i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		string name=planet->getName();
		if (name != "Sun" && name != "Missile" && name != "Blackhole") {
			int planet_x=planet->getPosition().getX();
			int planet_y=planet->getPosition().getY();
			int radius=planet->getRadius();

			//dc.TextOut(middle_x+planet_x-radius-10,middle_y+planet_y+radius+5,name.c_str());
			dc.DrawText(name.c_str(), -1, CRect(middle_x+planet_x-30,middle_y+planet_y+radius+2,middle_x+planet_x+30,middle_y+planet_y+radius+100), DT_SINGLELINE|DT_CENTER);
		}
	}
	for (i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		int planet_x=planet->getPosition().getX();
		int planet_y=planet->getPosition().getY();
		int radius=planet->getRadius();

		dc.SelectStockObject(BLACK_PEN);
		dc.SelectObject(brush);

		Player *p;
		if (p1.GetPlanet() == planet) {
			p=&p1;
		}
		if (p2.GetPlanet() == planet) {
			p=&p2;
		}
		if (p1.GetPlanet() == planet || p2.GetPlanet() == planet) {
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

		if (!start && p1.GetPlanet() != planet && p2.GetPlanet() != planet) {
			CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
				middle_x+planet_x+radius,middle_y+planet_y+radius);
			if (planetrect.PtInRect(MousePos)) {
				dc.SelectObject(hoverBrush);
			}
		}

		if (planet->getName() == "Blackhole") {
			for (int i=0; i < 18; i++) {
				dc.MoveTo(CPoint(middle_x+planet_x+radius*cos((angle+i*20)%360*pi/180),middle_y+planet_y+radius*sin((angle+i*20)%360*pi/180)));
				int randlen=rand()%5;
				dc.LineTo(CPoint(middle_x+planet_x+(radius+7)*cos((angle+i*20+30+randlen)%360*pi/180),middle_y+planet_y+(radius+7)*sin((angle+i*20+30+randlen)%360*pi/180)));
			}
			dc.SelectStockObject(BLACK_BRUSH);
		}
		else if (planet->getName() == "Sun") {
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
		if (p1.GetPlanet() == NULL) {
			dc.DrawText("Player 1 - Choose your planet", -1, CRect(0,10,window.right,100), DT_SINGLELINE|DT_CENTER);
		}
		else if (p2.GetPlanet() == NULL) {
			dc.DrawText("Player 2 - Choose your planet", -1, CRect(0,10,window.right,100), DT_SINGLELINE|DT_CENTER);
		}
	}

	str.Format("Player 1: %d", p1.GetMissiles());
	dc.TextOut(30,5,str);
	str.Format("Player 2: %d", p2.GetMissiles());
	dc.TextOut(30,20,str);
	str.Format("Num objects: %d", objects->size());
	dc.TextOut(30,35,str);
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	CClientDC dc(this);

	if (nIDEvent == UPDATE_TIMER) {
		engine.doPhysics();
		angle=(angle-1)%360;
		if (keys['A']) {
			p1.SetAngle(p1.GetAngle()-5);
		}
		if (keys['D']) {
			p1.SetAngle(p1.GetAngle()+5);
		}
		if (keys['J']) {
			p2.SetAngle(p2.GetAngle()-5);
		}
		if (keys['L']) {
			p2.SetAngle(p2.GetAngle()+5);
		}
		if (keys['W']) {
			p1.Fire();
		}
		if (keys['I']) {
			p2.Fire();
		}
		if (keys[37]) { //left
			adjustview+=Vector(10,0);
		}
		if (keys[38]) { //up
			adjustview+=Vector(0,10);
		}
		if (keys[39]) { //right
			adjustview+=Vector(-10,0);
		}
		if (keys[40]) { //down
			adjustview+=Vector(0,-10);
		}
		if (keys['0']) {
			adjustview=Vector(0,0);
		}
		for (int i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			if (planet->getName() == "Blackhole") {
				planet->updateMass(planet->getMass()*1.01);
			}
		}
		Invalidate();
	}
	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRep, UINT nFlags)  {
	keys[nChar]=1;
/*
	char text[100];
	sprintf(text,"nChar: %d", nChar);
	MessageBox(text,"char");
*/
}

void CMainFrame::OnKeyUp(UINT nChar, UINT nRep, UINT nFlags)  {
	keys[nChar]=0;
}

void CMainFrame::OnMouseMove(UINT nFlags, CPoint pt) {
	MousePos=pt;
}

void CMainFrame::OnLButtonDown(UINT nFlags, CPoint pt) {
	if (!start) {
		CRect window;
		GetClientRect(&window);
		int middle_x=window.right/2-adjustview.getX();
		int middle_y=window.bottom/2-adjustview.getY();
		if (follow != NULL) {
			middle_x-=follow->getPosition().getX();
			middle_y-=follow->getPosition().getY();
		}

		for (int i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			if (planet->getName() != "Sun") {
				int planet_x=planet->getPosition().getX();
				int planet_y=planet->getPosition().getY();
				int radius=planet->getRadius();

				CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
					middle_x+planet_x+radius,middle_y+planet_y+radius);

				if (planetrect.PtInRect(pt)) {
					if (p1.GetPlanet() == NULL && p2.GetPlanet() != planet) {
						p1.SetPlanet(planet);
					}
					else if (p2.GetPlanet() == NULL && p1.GetPlanet() != planet) {
						p2.SetPlanet(planet);
						start=1;
					}
					return;
				}
			}
		}
	}
}

void CMainFrame::OnRButtonDown(UINT nFlags, CPoint pt) {
	CRect window;
	GetClientRect(&window);
	int middle_x=window.right/2-adjustview.getX();
	int middle_y=window.bottom/2-adjustview.getY();
	if (follow != NULL) {
		middle_x-=follow->getPosition().getX();
		middle_y-=follow->getPosition().getY();
	}
	for (int i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		int planet_x=planet->getPosition().getX();
		int planet_y=planet->getPosition().getY();
		int radius=planet->getRadius();

		CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
			middle_x+planet_x+radius,middle_y+planet_y+radius);

		if (planetrect.PtInRect(pt)) {
			follow=planet;
			return;
		}
	}
}
