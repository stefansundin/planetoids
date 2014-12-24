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
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_COMMAND(IDM_MENU_RESET,OnMenuReset)
	ON_COMMAND(IDM_MENU_OPEN,OnMenuOpen)
	ON_COMMAND(IDM_MENU_ADD,OnMenuAdd)
	ON_COMMAND(IDM_MENU_SAVE,OnMenuSave)
	ON_COMMAND(IDM_MENU_BLACKHOLE,OnMenuBlackhole)
	ON_COMMAND(IDM_MENU_INVALIDATE,OnMenuInvalidate)
END_MESSAGE_MAP()

CMainFrame::CMainFrame() {
	srand(time(NULL));

	memset(keys,0,sizeof(keys));
	start=false;
	winner=0;
	invalidate=true;
	follow=NULL;
	stdscale=scale=1;
	timestep=8000;
	file="system.txt";
	MousePos=CPoint(0,0);

	engine = new Physics(&CMainFrame::collide);
	p1.SetEngine(engine);
	p2.SetEngine(engine);

	Create(NULL,"Planetoids",WS_OVERLAPPEDWINDOW,CRect(20,20,900,700));
	menu.LoadMenu(IDR_MENU);
	SetMenu(&menu);

	HINSTANCE hInst=AfxGetInstanceHandle();
	HICON icon=LoadIcon(hInst,"icon");
	SetIcon(icon,TRUE);

	CRect window;
	GetClientRect(&window);

	zoom.Create(WS_CHILD |WS_VISIBLE |TBS_VERT |TBS_RIGHT,
		CRect(CPoint(0,0),CSize(30,window.bottom)),
		this, IDC_SLH_ZOOM);
	zoom.SetRange(1,152097700);	zoom.SetTicFreq(1);
	zoom.SetPos(0);
	zoom.SetLineSize(1);	zoom.SetPageSize(50);

	speed.Create(WS_CHILD |WS_VISIBLE |TBS_HORZ |TBS_RIGHT,
		CRect(CPoint(30,0),CSize(200,30)),
		this, IDC_SLH_SPEED);
	speed.SetRange(1,1000000);	speed.SetTicFreq(500);
	speed.SetPos(timestep);
	speed.SetLineSize(1);	speed.SetPageSize(50);

	LoadSystem(file);
	objects = engine->getObjectsPointer();

	SetTimer(UPDATE_TIMER, 54, NULL);
	SetTimer(DRAW_TIMER, 100, NULL);
}

CMainFrame::~CMainFrame() {
	KillTimer(UPDATE_TIMER);
}

void CMainFrame::collide(Object* obj1, Object* obj2) {
	string name1=obj1->getName();
	string name2=obj2->getName();
	
	if (name1 == "Missile" && name2 == "Missile") {
		engine->removeObject(obj1);
		engine->removeObject(obj2);
	}
	else if ((name1 == "Missile" && (name2 == "Sun" || name2 == "Blackhole"))
		  || (name2 == "Missile" && (name1 == "Sun" || name1 == "Blackhole"))) {
		Object *missile=(name1=="Missile"?obj1:obj2);
		engine->removeObject(missile);
	}
	else if ((name1 == "Missile" && (obj2 == p1.GetPlanet() || obj2 == p2.GetPlanet()))
	      || (name2 == "Missile" && (obj1 == p1.GetPlanet() || obj1 == p2.GetPlanet()))) {
		Object *missile=(name1=="Missile"?obj1:obj2);
		Object *player=(missile==obj1?obj2:obj1);
		winner=(player == p1.GetPlanet()?2:1);
		start=false;
	}
	else if (name1 == "Missile" || name2 == "Missile") {
		Object *missile=(name1=="Missile"?obj1:obj2);
		Object *planet=(missile==obj1?obj2:obj1);
		engine->removeObject(missile);
		planet->updateRadius(planet->getRadius()*0.9);
		//planet->updateMass(planet->getMass()*0.9);
	}
	else if (name1 == "Blackhole" || name2 == "Blackhole") {
		Object *blackhole=(name1=="Blackhole"?obj1:obj2);
		Object *planet=(blackhole==obj1?obj2:obj1);
		blackhole->updateRadius(blackhole->getRadius()+0.5*planet->getRadius());
		blackhole->updateMass(blackhole->getMass()+0.5*planet->getMass());
		engine->removeObject(planet);
	}
	/*else {
		double newRadius=(obj1->getRadius()+obj2->getRadius())/2;
		obj1->updateRadius(newRadius);
		obj2->updateRadius(newRadius);
	}*/
	/*else {
		Vector vel1=obj1->getVelocity();
		Vector vel2=obj2->getVelocity();
		obj1->setVelocity(vel2);
		obj2->setVelocity(vel1);
	}*/

}

void CMainFrame::OnMenuReset() {
	engine->clear();
	LoadSystem(file);
	follow=NULL;
	adjustview=Vector(0,0);
}

void CMainFrame::OnMenuOpen() {
	CFileDialog FileDlg(TRUE, ".txt", NULL, 0, "Planetoid Systems (*.txt)|*.txt|All Files (*.*)|*.*||");
	if( FileDlg.DoModal() == IDOK )
	{
		char file[255];
		strcpy(file, FileDlg.GetFileName());
		engine->clear();
		LoadSystem(file);
	}
}

void CMainFrame::OnMenuAdd() {
	CFileDialog FileDlg(TRUE, ".txt", NULL, 0, "Planetoid Systems (*.txt)|*.txt|All Files (*.*)|*.*||");
	if( FileDlg.DoModal() == IDOK )
	{
		char fn[255];
		strcpy(fn, FileDlg.GetFileName());
		LoadSystem(fn);
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
	engine->addObject(new Object(Vector(650e6*cos(rand()%360*pi/180),650e6*sin(rand()%360*pi/180)), Vector(-3,-3), 33e6, 5e28, "Blackhole"));
}

void CMainFrame::OnMenuInvalidate() {
	invalidate=!invalidate;
	Invalidate(TRUE);
}

void CMainFrame::LoadSystem(string fn) {
	FILE *f;
	if ((f=fopen(fn.c_str(),"rb")) != NULL) {
		fscanf(f,"%d\t%d",&scale,&timestep);
		stdscale=scale;
		zoom.SetPos(scale);
		speed.SetPos(timestep);
		float pos,vel,r,m;
		char name[100];
		while (fscanf(f,"%f\t%f\t%f\t%f\t%s",&pos,&vel,&r,&m,name) != EOF) {
			for (char *c=name; *c != '\0'; c++) {
				if (*c == '_') {
					*c=' ';
				}
			}
			int angle=rand()%360;
			float x,y,velx,vely;
			x=pos*cos(angle*pi/180);
			y=pos*sin(angle*pi/180);
			velx=vel*cos((90+angle)*pi/180);
			vely=vel*sin((90+angle)*pi/180);
			engine->addObject(new Object(Vector(x,y), Vector(velx,vely), r, m, name));
		}
		fclose(f);
		file=fn;
	}
}

void CMainFrame::SaveSystem(string fn) {
	FILE *file;
	if ((file=fopen(fn.c_str(),"wb")) != NULL) {
		fprintf(file,"%d\t%d\n",scale,timestep);
		stdscale=scale;
		objects = engine->getObjectsPointer();
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
			float position,velocity;
			position=sqrt(pow(pos.getX(),2)+pow(pos.getY(),2));
			velocity=sqrt(pow(vel.getX(),2)+pow(vel.getY(),2));
			fprintf(file,"%f\t%f\t%f\t%f\t%s\n",position,velocity,p->getRadius(),p->getMass(),name);
		}
		fclose(file);
	}
}

void CMainFrame::OnPaint() {
	CPaintDC dc(this);

	dc.SetTextAlign(TA_CENTER);
	dc.SetTextColor(WHITE_BRUSH);

	int i;
	CString str;
	CRect window;
	GetClientRect(&window);
	int middle_x=window.right/2+adjustview.getX()/scale;
	int middle_y=window.bottom/2+adjustview.getY()/scale;
	if (follow != NULL) {
		middle_x-=follow->getPosition().getX()/scale;
		middle_y+=follow->getPosition().getY()/scale;
	}

	CBrush brush;
	brush.CreateSolidBrush(RGB(200,200,200));
	CBrush hoverBrush;
	hoverBrush.CreateSolidBrush(RGB(255,0,0));
	CBrush sunBrush;
	sunBrush.CreateSolidBrush(RGB(255,255,0));

	CFont font;
	font.CreatePointFont(80,"Arial");

	dc.Rectangle(CRect(CPoint(middle_x-600/scale,middle_y-600/scale),CSize(1200/scale,1200/scale)));

	dc.SelectObject(brush);
	dc.SelectObject(font);

	objects = engine->getObjectsPointer();
	if (invalidate) {
		for (i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			string name=planet->getName();
			if (name != "Sun" && name != "Missile" && name != "Blackhole") {
				int planet_x=planet->getPosition().getX()/scale;
				int planet_y=-planet->getPosition().getY()/scale;
				int radius=planet->getRadius()/scale;

				dc.TextOut(middle_x+planet_x,middle_y+planet_y+radius+5,name.c_str());
			}
		}
	}
	for (i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		string name=planet->getName();
		int planet_x=planet->getPosition().getX()/scale;
		int planet_y=-planet->getPosition().getY()/scale;
		int radius=planet->getRadius()/scale;
		radius=(name=="Sun"&&radius<8?8:radius<2?2:radius);

		dc.SelectStockObject(BLACK_PEN);
		dc.SelectObject(brush);

		//Aim
		if (invalidate) {
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
			}
		}

		if (!start && p1.GetPlanet() != planet && p2.GetPlanet() != planet) {
			CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
				middle_x+planet_x+radius,middle_y+planet_y+radius);
			if (planetrect.PtInRect(MousePos)) {
				dc.SelectObject(hoverBrush);
			}
		}

		if (name == "Blackhole") {
			for (int i=0; i < 18; i++) {
				dc.MoveTo(CPoint(middle_x+planet_x+radius*cos((angle+i*20)%360*pi/180),middle_y+planet_y+radius*sin((angle+i*20)%360*pi/180)));
				int randlen=rand()%5;
				dc.LineTo(CPoint(middle_x+planet_x+(radius+7)*cos((angle+i*20+30+randlen)%360*pi/180),middle_y+planet_y+(radius+7)*sin((angle+i*20+30+randlen)%360*pi/180)));
			}
			dc.SelectStockObject(BLACK_BRUSH);
		}
		else if (name == "Sun") {
			if (invalidate) {
				for (int i=0; i < 36; i++) {
					dc.MoveTo(CPoint(middle_x+planet_x+radius*cos((angle+i*10)%360*pi/180),middle_y+planet_y+radius*sin((angle+i*10)%360*pi/180)));
					int randlen=rand()%10;
					dc.LineTo(CPoint(middle_x+planet_x+(radius+4+randlen)*cos((angle+i*10)%360*pi/180),middle_y+planet_y+(radius+4+randlen)*sin((angle+i*10)%360*pi/180)));
				}
			}
			dc.SelectStockObject(NULL_PEN);
			dc.SelectObject(sunBrush);
		}
		
		if (name == "Missile"){
			double angle;

			if(planet->getVelocity().getX()<0){
				angle = 180+(atan((-planet->getVelocity().getY())/(planet->getVelocity().getX()))*180/pi);
			}
			else{
				angle = (atan((-planet->getVelocity().getY())/(planet->getVelocity().getX()))*180/pi);
			}

			dc.MoveTo(middle_x+planet_x,middle_y+planet_y);
			dc.LineTo(middle_x+planet_x+sqrt(25)*cos((angle+90)*pi/180),                        middle_y+planet_y+sqrt(25)*                sin((angle+90)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(3*3+2*2)*cos((angle+33.69)*pi/180),        middle_y+planet_y+sqrt(3*3+2*2)*        sin((angle+33.69)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(9*9+2*2)*cos((angle+12.5288077)*pi/180),middle_y+planet_y+sqrt(9*9+2*2)*        sin((angle+12.5288077)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(144)*cos(angle*pi/180),                                middle_y+planet_y+sqrt(144)*                sin(angle*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(9*9+2*2)*cos((angle-12.5288077)*pi/180),middle_y+planet_y+sqrt(9*9+2*2)*        sin((angle-12.5288077)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(3*3+2*2)*cos((angle-33.69)*pi/180),        middle_y+planet_y+sqrt(3*3+2*2)*        sin((angle-33.69)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(25)*cos((angle-90)*pi/180),                        middle_y+planet_y+sqrt(25)*                sin((angle-90)*pi/180));
			dc.LineTo(middle_x+planet_x,middle_y+planet_y);                        
		}
		else{
			dc.Ellipse(CRect(CPoint(middle_x+planet_x-radius,middle_y+planet_y-radius),CSize(2*radius,2*radius)));
		}
	}

	if (invalidate) {
		if (!start && !winner) {
			if (p1.GetPlanet() == NULL) {
				dc.TextOut(window.right/2,10,"Player 1 - Choose your planet");
			}
			else if (p2.GetPlanet() == NULL) {
				dc.TextOut(window.right/2,10,"Player 2 - Choose your planet");
			}
		}

		dc.SetTextAlign(TA_LEFT);
		str.Format("Player 1: %d HP    %d Missiles", p1.GetHP(), p1.GetMissiles());
		dc.TextOut(40,40,str);
		str.Format("Player 2: %d HP    %d Missiles", p2.GetHP(), p2.GetMissiles());
		dc.TextOut(40,55,str);
		str.Format("%d objects", objects->size());
		dc.TextOut(40,70,str);
		str.Format("Scale: %d", scale);
		dc.TextOut(40,85,str);
		str.Format("Timestep: %d", timestep);
		dc.TextOut(40,100,str);
	}

	if (winner) {
		dc.SetTextAlign(TA_CENTER);
		str.Format("Player %d is the winner", winner);
		dc.TextOut(window.right/2,10,str);
		dc.TextOut(window.right/2,25,"Press enter to start a new round");
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	CClientDC dc(this);

	if (nIDEvent == UPDATE_TIMER && start) {
		for (int a=0;a<25;a++){
			engine->doPhysics(timestep);
		}
	}
	else if (nIDEvent == DRAW_TIMER) {
		angle=(angle-1)%360;
		if (keys['A']) {
			p1.SetAngle(p1.GetAngle()-(keys[16]?1:5));
		}
		if (keys['D']) {
			p1.SetAngle(p1.GetAngle()+(keys[16]?1:5));
		}
		if (keys['J']) {
			p2.SetAngle(p2.GetAngle()-(keys[16]?1:5));
		}
		if (keys['L']) {
			p2.SetAngle(p2.GetAngle()+(keys[16]?1:5));
		}
		if (keys['W'] && start) {
			p1.Fire();
		}
		if (keys['I'] && start) {
			p2.Fire();
		}
		if (keys[37]) { //left
			adjustview+=Vector(10*scale*(keys[16]?10:1),0);
		}
		if (keys[38]) { //up
			adjustview+=Vector(0,10*scale*(keys[16]?10:1));
		}
		if (keys[39]) { //right
			adjustview+=Vector(-10*scale*(keys[16]?10:1),0);
		}
		if (keys[40]) { //down
			adjustview+=Vector(0,-10*scale*(keys[16]?10:1));
		}
		if (keys['0']) {
			follow=NULL;
			adjustview=Vector(0,0);
			scale=stdscale;
			zoom.SetPos(scale);
		}
		if (keys['Z']) {
			scale++;
			zoom.SetPos(scale);
		}
		if (keys['X']) {
			if (scale != 1) {
				scale--;
				zoom.SetPos(scale);
			}
		}
		if (keys['C']) {
			scale+=1000;
			zoom.SetPos(scale);
		}
		if (keys['V']) {
			scale=(scale-1000<1?1:scale-1000);
			zoom.SetPos(scale);
		}
		if (keys['B']) {
			scale+=50000;
			zoom.SetPos(scale);
		}
		if (keys['N']) {
			scale=(scale-50000<1?1:scale-50000);
			zoom.SetPos(scale);
		}

		for (int i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			if (planet->getName() == "Blackhole") {
				planet->updateMass(planet->getMass()*1.01);
			}
		}

		Invalidate(invalidate);
	}
	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRep, UINT nFlags)  {
	keys[nChar]=1;

	if (nChar == 13 && winner) {
		winner=0;
		p1.Reset();
		p2.Reset();
		OnMenuReset();
	}
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
		int middle_x=window.right/2+adjustview.getX()/scale;
		int middle_y=window.bottom/2+adjustview.getY()/scale;
		if (follow != NULL) {
			middle_x-=follow->getPosition().getX()/scale;
			middle_y+=follow->getPosition().getY()/scale;
		}

		for (int i=0; i < objects->size(); i++) {
			Object *planet=objects->at(i);
			string name=planet->getName();
			if (name != "Sun" && name != "Blackhole") {
				int planet_x=planet->getPosition().getX()/scale;
				int planet_y=-planet->getPosition().getY()/scale;
				int radius=planet->getRadius()/scale;
				radius=(name=="Sun"&&radius<8?8:radius<2?2:radius);

				CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
					middle_x+planet_x+radius,middle_y+planet_y+radius);

				if (planetrect.PtInRect(pt)) {
					if (p1.GetPlanet() == NULL && p2.GetPlanet() != planet) {
						p1.SetPlanet(planet);
					}
					else if (p2.GetPlanet() == NULL && p1.GetPlanet() != planet) {
						p2.SetPlanet(planet);
						start=true;
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
	int middle_x=window.right/2+adjustview.getX()/scale;
	int middle_y=window.bottom/2+adjustview.getY()/scale;
	if (follow != NULL) {
		middle_x-=follow->getPosition().getX()/scale;
		middle_y+=follow->getPosition().getY()/scale;
	}
	for (int i=0; i < objects->size(); i++) {
		Object *planet=objects->at(i);
		string name=planet->getName();
		int planet_x=planet->getPosition().getX()/scale;
		int planet_y=-planet->getPosition().getY()/scale;
		int radius=planet->getRadius()/scale;
		radius=(name=="Sun"&&radius<8?8:radius<2?2:radius);

		CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
			middle_x+planet_x+radius,middle_y+planet_y+radius);

		if (planetrect.PtInRect(pt)) {
			follow=planet;
			adjustview=Vector(0,0);
			return;
		}
	}
}

void CMainFrame::OnVScroll(UINT nSBCode, UINT nPos, CWnd *pSlider) {
	CSliderCtrl *pSld;
	pSld=(CSliderCtrl*)pSlider;
	scale=pSld->GetPos();
	//scale=(scale==1?1:scale);
	this->SetFocus();
}

void CMainFrame::OnHScroll(UINT nSBCode, UINT nPos, CWnd *pSlider) {
	CSliderCtrl *pSld;
	pSld=(CSliderCtrl*)pSlider;
	timestep=pSld->GetPos();
	this->SetFocus();
}

BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	if (scale-3000*zDelta >= 1) {
		scale-=3000*zDelta;
	}
	else {
		scale=1;
	}
	zoom.SetPos(scale);
	return 1;
}

void CMainFrame::OnSize() {
/*
	CRect window;
	GetClientRect(&window);
	zoom.GetBuddy()->SetWindowPos(&wndTop,200,200,200,200,SWP_SHOWWINDOW);
*/
}
