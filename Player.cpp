#include <iostream>
#include "fysik/millitime.h"
#include "Player.h"

const double pi=3.14159265358979323846;

using namespace std;

Player::Player() {
	Planet=NULL;
	HP=100;
	Angle=0;
	Missiles=1000;
	SelectedMissile=0;
	LastFired=0;
	engine=NULL;
}

void Player::SetEngine(Physics *e) {
	engine=e;
}

void Player::SetPlanet(Object *p) {
	Planet=p;
}

void Player::SetHP(int h) {
	HP=h;
}

void Player::SetAngle(int a) {
	if (GetPlanet() != NULL) {
		Angle=a%360;
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

void Player::Fire() {
	if (Planet != NULL && Missiles > 0 && millitime() >= LastFired+1000) {
		engine->addObject(new Object(Planet->getPosition()+Vector(Planet->getRadius()*cos(Angle*pi/180)+3*cos(Angle*pi/180),Planet->getRadius()*sin(Angle*pi/180)+3*sin(Angle*pi/180)), Planet->getVelocity()+Vector(50*cos(Angle*pi/180),50*sin(Angle*pi/180)), 3, 10, "Missile"));
		LastFired=millitime();
		Missiles--;
	}
}

Object *Player::GetPlanet() {
	return Planet;
}

int Player::GetHP() {
	return HP;
}

int Player::GetAngle() {
	return Angle;
}

int Player::GetSelectedMissile() {
	return SelectedMissile;
}

int Player::GetMissiles() {
	return Missiles;
}
