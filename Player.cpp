#include <iostream>
#include <windows.h>
#include "Player.h"

const double pi=3.14159265358979323846;

using namespace std;

Player::Player() {
	planet=NULL;
	angle=0;
	cooldown=0;
	engine=NULL;
}

void Player::SetEngine(Physics *e) {
	engine=e;
}

void Player::SetPlanet(ObjectX *p) {
	planet=p;
	angle=rand()%360;
}

void Player::SetAngle(int a) {
	if (GetPlanet() != NULL) {
		angle=a%360;
	}
}

void Player::Fire() {
	if (planet != NULL && GetTickCount() >= cooldown+2000) {
		Vector pos(planet->getPosition()+Vector(
			planet->getRadius()*cos(angle*pi/180*-1)+(planet->getRadius()+2000000)*cos(angle*pi/180*-1),
			planet->getRadius()*sin(angle*pi/180*-1)+(planet->getRadius()+2000000)*sin(angle*pi/180*-1)));
		Vector vel(planet->getVelocity()+Vector(
			50000*cos(angle*pi/180*-1),
			50000*sin(angle*pi/180*-1)));
		ObjectX *missile = new ObjectX(pos,vel, 1900000, 10000, "Missile");
		engine->addObject(missile);
		missile->SetHP(1);
		cooldown=GetTickCount();
	}
}

void Player::Reset() {
	planet=NULL;
	angle=0;
	cooldown=0;
}

ObjectX *Player::GetPlanet() {
	return planet;
}

int Player::GetAngle() {
	return angle;
}