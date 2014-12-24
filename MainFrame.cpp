#include "MainFrame.h"
#include <cmath>
#include <vector>
#include <ctime>
#include <fstream>
#include <sstream>
#include "memdc.h"
#include "fysik/ObjectX.h"

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
//	ON_WM_SIZE()
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
	nuke=0;
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
	zoom.SetRange(1,1520977000);	zoom.SetTicFreq(1);
	zoom.SetPos(0);
	zoom.SetLineSize(1);	zoom.SetPageSize(500000);

	speed.Create(WS_CHILD |WS_VISIBLE |TBS_HORZ |TBS_RIGHT,
		CRect(CPoint(30,0),CSize(window.right-30,30)),
		this, IDC_SLH_SPEED);
	speed.SetRange(1,35000000);	speed.SetTicFreq(500);
	speed.SetPos(timestep);
	speed.SetLineSize(1);	speed.SetPageSize(10000);

	LoadSystem(file);
	objects = engine->getObjectsPointer();

	SetTimer(UPDATE_TIMER, 54, NULL);
	SetTimer(DRAW_TIMER, 100, NULL);
}

CMainFrame::~CMainFrame() {
	KillTimer(UPDATE_TIMER);
	KillTimer(DRAW_TIMER);
	delete engine;
}

void CMainFrame::collide(Object* obj1, Object* obj2) {
	string name1=obj1->getName();
	string name2=obj2->getName();
	if ((name1=="Missile" && name2 != "Missile") || (name2=="Missile" && name1 != "Missile")){
		ObjectX *missile=(ObjectX*)(name1=="Missile"?obj1:obj2);
		ObjectX *planet=(ObjectX*)(missile==obj1?obj2:obj1);
		int damage=10+rand()%6;	
		stringstream ss;
		ss << damage;
		animations.push_back(Animation(missile->getPosition(),"Damage",ss.str()));
		planet->SetHP(planet->GetHP()-damage);
		if (planet->GetHP() <= 0) {
			if (planet->getName() == "Sun") {
				animations.push_back(Animation(missile->getPosition(),"Supernova"));
			}
			else {
				animations.push_back(Animation(missile->getPosition(),"Megasplosion"));
			}
		}
		else {
			animations.push_back(Animation(missile->getPosition(),"Explosion"));
		}
	}
	
	if (name1 == "Missile" && name2 == "Missile") {
		animations.push_back(Animation(obj1->getPosition(),"Explosion"));
		engine->removeObject(obj1);
		engine->removeObject(obj2);
	}
	else if ((name1 == "Missile" && (name2 == "Sun" || name2 == "Blackhole"))
		  || (name2 == "Missile" && (name1 == "Sun" || name1 == "Blackhole"))) {
		ObjectX *missile=(ObjectX*)(name1=="Missile"?obj1:obj2);
		ObjectX *sun=(ObjectX*)(missile==obj1?obj2:obj1);
		if (sun->GetHP() <= 0) {
			engine->removeObject(sun);
		}
		engine->removeObject(missile);
	}
	else if ((name1 == "Missile" && (obj2 == p1.GetPlanet() || obj2 == p2.GetPlanet()))
	      || (name2 == "Missile" && (obj1 == p1.GetPlanet() || obj1 == p2.GetPlanet()))) {
		ObjectX *missile=(ObjectX*)(name1=="Missile"?obj1:obj2);
		ObjectX *planet=(ObjectX*)(missile==obj1?obj2:obj1);
		ObjectX *player=(planet == p1.GetPlanet()?p1.GetPlanet():p2.GetPlanet());
		engine->removeObject(missile);
	}
	else if (name1 == "Missile" || name2 == "Missile") {
		ObjectX *missile=(ObjectX*)(name1=="Missile"?obj1:obj2);
		ObjectX *planet=(ObjectX*)(missile==obj1?obj2:obj1);
		if (planet->GetHP()<=0){
			engine->removeObject(planet);
		}
		engine->removeObject(missile);
		/*planet->updateRadius(planet->getRadius()*0.8);
		if (planet->getRadius()/scale <= 2) {
			engine->removeObject(planet);
		}*/
		//planet->updateMass(planet->getMass()*0.9);
	}
	else if (name1 == "Blackhole" || name2 == "Blackhole") {
		ObjectX *blackhole=(ObjectX*)(name1=="Blackhole"?obj1:obj2);
		ObjectX *planet=(ObjectX*)(blackhole==obj1?obj2:obj1);
		//animations.push_back(Animation(planet->getPosition(),"Explosion"));
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
	p1.Reset();
	p2.Reset();
	start=false;
	winner=0;
	follow=NULL;
	adjustview=Vector(0,0);
	engine->clear();
	animations.clear();
	LoadSystem(file);
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
	CRect window;
	GetClientRect(&window);
	int radius=(window.bottom>window.right?window.right:window.bottom)/2;
	engine->addObject(new ObjectX(Vector(radius*scale*cos(rand()%360*pi/180),radius*scale*sin(rand()%360*pi/180)), Vector(10,10), 33e6, 5e28, "Blackhole", 0,0,0));
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
		float pos,vel,rad,m;
		unsigned char r,g,b;
		char name[100];
		while (fscanf(f,"%f\t%f\t%f\t%f\t%s\t%d,%d,%d",&pos,&vel,&rad,&m,name,&r,&g,&b) != EOF) {
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
			ObjectX *newobject=new ObjectX(Vector(x,y), Vector(velx,vely), rad, m, name, r,g,b);
			engine->addObject(newobject);
			if(name=="Sun"){
				newobject->SetHP(10000);
			}
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
			ObjectX *p=(ObjectX*)objects->at(i);
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
			fprintf(file,"%f\t%f\t%f\t%f\t%s\t%d,%d,%d\n",position,velocity,p->getRadius(),p->getMass(),name,p->getColorR(),p->getColorG(),p->getColorB());
		}
		fclose(file);
	}
}

void CMainFrame::OnPaint() {
	//Flicker free DC
/*	CPaintDC loldc(this);
	CRect rect;
	GetClientRect(rect);
	CMemDC dc(&loldc,&rect);
*/
	CPaintDC dc(this);

	dc.SetTextAlign(TA_CENTER);
	dc.SetTextColor(WHITE_BRUSH);
	dc.SetBkMode(TRANSPARENT);

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

	CBrush boomBrush1;
	boomBrush1.CreateSolidBrush(RGB(240,180,90));
	CBrush boomBrush2;
	boomBrush2.CreateSolidBrush(RGB(255,128,128));
	CBrush boomBrush3;
	boomBrush3.CreateSolidBrush(RGB(255,0,0));

	CFont font;
	font.CreatePointFont(80,"Arial");
	dc.SelectObject(font);

	//Draw names
	objects = engine->getObjectsPointer();
	if (invalidate) {
		for (i=0; i < objects->size(); i++) {
			ObjectX *planet=(ObjectX*)objects->at(i);
			string name=planet->getName();
			if (name != "Sun" && name != "Missile" && name != "Blackhole" && name != "Meteor") {
				int planet_x=planet->getPosition().getX()/scale;
				int planet_y=-planet->getPosition().getY()/scale;
				int radius=planet->getRadius()/scale;

				dc.TextOut(middle_x+planet_x,middle_y+planet_y+radius+5,name.c_str());
			}
		}
	}

	//Draw animations
	dc.SelectStockObject(NULL_PEN);
	for (i=0; i < animations.size(); i++) {
		Animation *ani=&animations.at(i);
		string type=ani->getType();
		Vector *pos=ani->getPos();
		double x=pos->getX()/scale;
		double y=-pos->getY()/scale;
		int frame=ani->getFrame();
		string value=ani->getValue();
		if (type == "Explosion" && frame <= 9) {
			int radius[]={4,12,16,23,16,11,6,3,1,1,1};
			int r=radius[frame];
			dc.SelectObject(boomBrush1);
			dc.Ellipse(CRect(CPoint(middle_x+x-r,middle_y+y-r),CSize(2*r,2*r)));
			dc.SelectObject(boomBrush2);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.6*r,middle_y+y-0.6*r),CSize(2*0.6*r,2*0.6*r)));
			dc.SelectObject(boomBrush3);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.3*r,middle_y+y-0.3*r),CSize(2*0.3*r,2*0.3*r)));
		}
		if (type == "Damage" && frame <= 10) {
			dc.TextOut(middle_x+x,middle_y+y-30-frame,value.c_str());
		}
	}

	//Draw objects
	int savedc = dc.SaveDC();
	for (i=0; i < objects->size(); i++) {
		ObjectX *planet=(ObjectX*)objects->at(i);
		string name=planet->getName();
		int planet_x=planet->getPosition().getX()/scale;
		int planet_y=-planet->getPosition().getY()/scale;
		int radius=planet->getRadius()/scale;
		radius=(name=="Sun"&&radius<4?4:radius<2?2:radius);

		dc.SelectStockObject(BLACK_PEN);

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

		if (name == "Blackhole") {
			if (invalidate) {
				for (int i=0; i < 18; i++) {
					dc.MoveTo(CPoint(middle_x+planet_x+radius*cos((angle+i*20)%360*pi/180),middle_y+planet_y+radius*sin((angle+i*20)%360*pi/180)));
					int randlen=rand()%5;
					dc.LineTo(CPoint(middle_x+planet_x+(radius+7)*cos((angle+i*20+30+randlen)%360*pi/180),middle_y+planet_y+(radius+7)*sin((angle+i*20+30+randlen)%360*pi/180)));
				}
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
		}
		
		if (name == "Missile"){
			double angle = atan(-planet->getVelocity().getY()/planet->getVelocity().getX())*180/pi;
			if (planet->getVelocity().getX()<0){
				angle+=180;
			}
			if (angle == 0) {
				angle=0.00001;
			}

			dc.MoveTo(middle_x+planet_x,middle_y+planet_y);
			dc.LineTo(middle_x+planet_x+sqrt(25)*cos((angle+90)*pi/180),             middle_y+planet_y+sqrt(25)*      sin((angle+90)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(3*3+2*2)*cos((angle+33.69)*pi/180),     middle_y+planet_y+sqrt(3*3+2*2)* sin((angle+33.69)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(9*9+2*2)*cos((angle+12.5288077)*pi/180),middle_y+planet_y+sqrt(9*9+2*2)* sin((angle+12.5288077)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(144)*cos(angle*pi/180),                 middle_y+planet_y+sqrt(144)*     sin(angle*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(9*9+2*2)*cos((angle-12.5288077)*pi/180),middle_y+planet_y+sqrt(9*9+2*2)* sin((angle-12.5288077)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(3*3+2*2)*cos((angle-33.69)*pi/180),     middle_y+planet_y+sqrt(3*3+2*2)* sin((angle-33.69)*pi/180));
			dc.LineTo(middle_x+planet_x+sqrt(25)*cos((angle-90)*pi/180),             middle_y+planet_y+sqrt(25)*      sin((angle-90)*pi/180));
			dc.LineTo(middle_x+planet_x,middle_y+planet_y);                        
		}
		else{
			CBrush brush;
			CRect planetrect(middle_x+planet_x-radius,middle_y+planet_y-radius,
				middle_x+planet_x+radius,middle_y+planet_y+radius);
			if (!start && name != "Blackhole" && name != "Sun" && p1.GetPlanet() != planet && p2.GetPlanet() != planet && planetrect.PtInRect(MousePos)) {
				brush.CreateSolidBrush(RGB(255,0,0));
			}
			else {
				brush.CreateSolidBrush(RGB(planet->getColorR(),planet->getColorG(),planet->getColorB()));
			}
			dc.SelectObject(brush);
			dc.Ellipse(CRect(CPoint(middle_x+planet_x-radius,middle_y+planet_y-radius),CSize(2*radius,2*radius)));
		}
	}
	dc.RestoreDC(savedc);

	if (invalidate) {
		dc.SetTextAlign(TA_LEFT);
		str.Format("Player 1: %d HP", (p1.GetPlanet()!=NULL?p1.GetPlanet()->GetHP():100));
		dc.TextOut(40,40,str);
		str.Format("Player 2: %d HP", (p2.GetPlanet()!=NULL?p2.GetPlanet()->GetHP():100));
		dc.TextOut(40,55,str);
		str.Format("%d objects", objects->size());
		dc.TextOut(40,70,str);
		str.Format("Scale: %d", scale);
		dc.TextOut(40,85,str);
		str.Format("Timestep: %d", timestep);
		dc.TextOut(40,100,str);
	}

	dc.SetTextAlign(TA_CENTER);
	if (!start || winner) {
		CFont font;
		font.CreatePointFont(120,"Arial");
		dc.SelectObject(font);
	}

	//Check if someone has won
	if (p2.GetPlanet() != NULL && (p1.GetPlanet()->GetHP() <= 0 || p2.GetPlanet()->GetHP() <= 0)) {
		winner=(p1.GetPlanet()->GetHP() <= 0?2:1);
		start=false;
	}

	if (invalidate && !start && !winner) {
		if (p1.GetPlanet() == NULL) {
			dc.TextOut(window.right/2,40,"Player 1 - Choose your planet");
		}
		else if (p2.GetPlanet() == NULL) {
			dc.TextOut(window.right/2,40,"Player 2 - Choose your planet");
		}
	}

	if (winner) {
		str.Format("Player %d is the winner", winner);
		dc.TextOut(window.right/2,40,str);
		dc.TextOut(window.right/2,55,"Press enter to start a new round");
	}

	dc.SelectStockObject(NULL_PEN);
	for (i=0; i < animations.size(); i++) {
		Animation *ani=&animations.at(i);
		string type=ani->getType();
		Vector *pos=ani->getPos();
		double x=pos->getX()/scale;
		double y=-pos->getY()/scale;
		int frame=ani->getFrame();
		if (type == "Megasplosion" && frame <= 10) {
			int radius[]={7,20,30,40,50,60,65,65,70,50,30,10};
			int r=radius[frame];
			dc.SelectObject(boomBrush1);
			dc.Ellipse(CRect(CPoint(middle_x+x-r,middle_y+y-r),CSize(2*r,2*r)));
			dc.SelectObject(boomBrush2);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.6*r,middle_y+y-0.6*r),CSize(2*0.6*r,2*0.6*r)));
			dc.SelectObject(boomBrush3);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.3*r,middle_y+y-0.3*r),CSize(2*0.3*r,2*0.3*r)));
		}
		if (type == "Supernova" && frame <= 11) {
			int radius[]={7,20,30,40,50,60,100,160,200,250,300,350};
			int r=radius[frame];
			dc.SelectObject(boomBrush1);
			dc.Ellipse(CRect(CPoint(middle_x+x-r,middle_y+y-r),CSize(2*r,2*r)));
			dc.SelectObject(boomBrush2);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.6*r,middle_y+y-0.6*r),CSize(2*0.6*r,2*0.6*r)));
			dc.SelectObject(boomBrush3);
			dc.Ellipse(CRect(CPoint(middle_x+x-0.3*r,middle_y+y-0.3*r),CSize(2*0.3*r,2*0.3*r)));
		}
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) {
	CClientDC dc(this);

	if (nIDEvent == UPDATE_TIMER && start) {
		for (int a=0;a<5;a++){
			engine->doPhysics(timestep);
		}
	}
	else if (nIDEvent == DRAW_TIMER) {
		angle=(angle-1)%360;
		for(int o=0; o < objects->size(); o++){
			ObjectX *missile=(ObjectX*)objects->at(o);
			if(missile->getName() == "Missile"){
				if(missile->getTime() > 10000002000){
					animations.push_back(Animation(missile->getPosition(),"Explosion"));
					engine->removeObject(missile);
				}
				missile->incTime(timestep);
			}
		}
		if (keys['A']) {
			p1.SetAngle(p1.GetAngle()-(keys[16]?1:8));
		}
		if (keys['D']) {
			p1.SetAngle(p1.GetAngle()+(keys[16]?1:8));
		}
		if (keys['J']) {
			p2.SetAngle(p2.GetAngle()-(keys[16]?1:8));
		}
		if (keys['L']) {
			p2.SetAngle(p2.GetAngle()+(keys[16]?1:8));
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
		/*if (keys['Z']) {
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
		}*/
		/*if (keys['X']) {
			animations.push_back(Animation(Vector(0,0),"Explosion"));
		}*/
		if (keys[' ']) {
			nuke++;
			if (nuke == 3) {
				CRect window;
				GetClientRect(&window);
				int radius=(window.bottom>window.right?window.right:window.bottom)/2;
				for (int angle=0; angle < 360; angle+=16) {
					engine->addObject(new ObjectX(Vector(radius*scale*cos(angle*pi/180),radius*scale*sin(angle*pi/180)), Vector(40000*cos((angle+165)*pi/180),40000*sin((angle+165)*pi/180)), 5*scale, 10000, "Missile"));
				}
				nuke=0;
			}
		}
		
		//Add mass to black holes
		for (int i=0; i < objects->size(); i++) {
			ObjectX *planet=(ObjectX*)objects->at(i);
			if (planet->getName() == "Blackhole") {
				planet->updateMass(planet->getMass()*1.01);
			}
		}


		for (i=0; i < animations.size(); i++) {
			Animation *ani=&animations.at(i);
			string type=ani->getType();
			Vector *pos=ani->getPos();
			int frame=ani->getFrame();
			if (type == "Megasplosion" && frame == 8) {
				int r=70;
				for(int q=0;q<objects->size();q++){
					ObjectX *planet = (ObjectX*)objects->at(q);
					Vector *poss = &planet->getPosition();
					if((*pos-*poss).getLength()<r*scale+planet->getRadius()){
						planet->SetHP(planet->GetHP()-50);
						animations.push_back(Animation(planet->getPosition(),"Damage","50"));
					}
					if(planet->GetHP()<=0){
						string name = planet->getName();
						if(name == "Sun"){
							animations.push_back(Animation(planet->getPosition(),"Supernova"));							
						}
						else if(name == "Missile"){
							animations.push_back(Animation(planet->getPosition(),"Explosion"));							
						}
						else{
							animations.push_back(Animation(planet->getPosition(),"Megasplosion"));
						}
						engine->removeObject(planet);
					}
				}
			}
			else if (type == "Supernova" && frame == 11) {
				int r=350;
				for(int q=0;q<objects->size();q++){
					ObjectX *planet = (ObjectX*)objects->at(q);
					Vector *poss = &planet->getPosition();
					if((*pos-*poss).getLength()<r*scale+planet->getRadius()){
						planet->SetHP(planet->GetHP()-1000);
						animations.push_back(Animation(planet->getPosition(),"Damage","1000"));
					}
					if(planet->GetHP()<=0){
						string name = planet->getName();
						if(name == "Sun"){
							animations.push_back(Animation(planet->getPosition(),"Supernova"));							
						}
						else if(name == "Missile"){
							animations.push_back(Animation(planet->getPosition(),"Explosion"));							
						}
						else{
							animations.push_back(Animation(planet->getPosition(),"Megasplosion"));
						}
						engine->removeObject(planet);		
					}
				}
			}
			else if (type == "Explosion" && frame == 5) {
				int r=23;
				for(int q=0;q<objects->size();q++){
					ObjectX *planet = (ObjectX*)objects->at(q);
					Vector *poss = &planet->getPosition();
					if((*pos-*poss).getLength()<r*scale+planet->getRadius()){
						planet->SetHP(planet->GetHP()-5);
						animations.push_back(Animation(planet->getPosition(),"Damage","5"));
					}
					if(planet->GetHP()<=0){
						string name = planet->getName();
						if(name == "Sun"){
							animations.push_back(Animation(planet->getPosition(),"Supernova"));							
						}
						else if(name == "Missile"){
							animations.push_back(Animation(planet->getPosition(),"Explosion"));							
						}
						else{
							animations.push_back(Animation(planet->getPosition(),"Megasplosion"));
						}
						engine->removeObject(planet);
					}
				}
			}
		}





		//Step animations
		for (i=0; i < animations.size(); i++) {
			animations.at(i).step();
		}

		//Remove animations that are done
		for (i=0; i < animations.size(); i++) {
			string type=animations.at(i).getType();
			int frame=animations.at(i).getFrame();
			if ((type == "Explosion" && frame > 9)
			 || (type == "Damage" && frame > 10)
			 || (type == "Megasplosion" && frame > 10)
			 || (type == "Supernova" && frame > 10)) {
				vector<Animation>::iterator it=animations.begin()+i;
				animations.erase(it);
				break;
			}
		}

		//Invalidate(invalidate);
		CRect window;
		GetClientRect(&window);
		InvalidateRect(CRect(30,30,window.right, window.bottom),invalidate);
	}

	CWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnKeyDown(UINT nChar, UINT nRep, UINT nFlags)  {
	keys[nChar]=1;

	if (winner && nChar == 13) {
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
			ObjectX *planet=(ObjectX*)objects->at(i);
			string name=planet->getName();
			if (name != "Sun" && name != "Blackhole") {
				int planet_x=planet->getPosition().getX()/scale;
				int planet_y=-planet->getPosition().getY()/scale;
				int radius=planet->getRadius()/scale;
				radius=(name=="Sun"&&radius<4?4:radius<2?2:radius);

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
		ObjectX *planet=(ObjectX*)objects->at(i);
		string name=planet->getName();
		int planet_x=planet->getPosition().getX()/scale;
		int planet_y=-planet->getPosition().getY()/scale;
		int radius=planet->getRadius()/scale;
		radius=(name=="Sun"&&radius<4?4:radius<2?2:radius);

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
	scale-=300000*zDelta;
	if (keys[16]) {
		scale-=3000000*zDelta;
	}
	if (scale < 1) {
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
