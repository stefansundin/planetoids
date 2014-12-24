#include <iostream>
#include "Player.h"

using namespace std;

Player::Player() {
	Planet=-1;
	HP=100;
	Angle=0;
	bAngle=0;
	Missiles=1000;
	SelectedMissile=0;
}


void Player::SetPlanet(int p) {
	Planet=p;
}

void Player::SetHP(int h) {
	HP=h;
}

void Player::SetAngle(int a) {
	if (GetPlanet() != -1) {
		Angle=a%360;
		bAngle=a*-1%360;
	}
}

void Player::SelectMissile(int s) {
	SelectedMissile=s;
}

void Player::DecreaseMissile() {
	if (Missiles > 0) {
		Missiles--;
	}
}


int Player::GetPlanet() {
	return Planet;
}

int Player::GetHP() {
	return HP;
}

int Player::GetAngle() {
	return Angle;
}

int Player::GetbAngle(){
	return bAngle;
}

int Player::GetSelectedMissile() {
	return SelectedMissile;
}

int Player::GetMissiles() {
	return Missiles;
}
/*
void Player::WriteMissile(int angle, int middle_x, int middle_y, int planet_x, int planet_y, int radius){
	
	dc.MoveTo(CPoint(middle_x+100, middle_y+102));
	dc.LineTo(CPoint(middle_x+100+2*cos((angle+90)*pi/180), middle_y+102+2*-sin((angle+90)*pi/180)));
	dc.LineTo(CPoint(middle_x+100+6.32455532*cos((angle+18.4349488)*pi/180), middle_y+102+6.32455532*-sin((angle+18.4349488)*pi/180)));
	dc.LineTo(CPoint(middle_x+100+7.21110255*cos((angle+33.6900675)*pi/180), middle_y+102+7.21110255*-sin((angle+33.6900675)*pi/180)));
	dc.LineTo(CPoint(middle_x+100+12*cos(angle*pi/180), middle_y+102+12*-sin(angle*pi/180)));
	dc.LineTo(CPoint(middle_x+100+7.21110255*cos((angle-33.6900675)*pi/180), middle_y+102+7.21110255*-sin((angle-33.6900675)*pi/180)));
	dc.LineTo(CPoint(middle_x+100+6.32455532*cos((angle-18.4349488)*pi/180), middle_y+102+6.32455532*-sin((angle-18.4349488)*pi/180)));
	dc.LineTo(CPoint(middle_x+100+2*cos((angle-90)*pi/180), middle_y+102+2*-sin((angle-90)*pi/180)));
	dc.LineTo(CPoint(middle_x+100, middle_y+102));
}*/

