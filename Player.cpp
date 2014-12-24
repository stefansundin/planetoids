#include <iostream>
#include <windows.h>
#include "Player.h"

const double pi=3.14159265358979323846;

using namespace std;

Player::Player() {
	planet=NULL;
	hp=100;
	angle=0;
	missiles=1000;
	cooldown=0;
	engine=NULL;
}

void Player::SetEngine(Physics *e) {
	engine=e;
}

void Player::SetPlanet(Object *p) {
	planet=p;
}

void Player::SetHP(int h) {
	hp=h;
}

void Player::SetAngle(int a) {
	if (GetPlanet() != NULL) {
		angle=a%360;
	}
}

void Player::DecreaseMissile() {
	if (missiles > 0) {
		missiles--;
	}
}

void Player::Fire() {
	if (planet != NULL && missiles > 0 && GetTickCount() >= cooldown+2000) {
		Vector pos(planet->getPosition()+Vector(
			planet->getRadius()*cos(angle*pi/180*-1)+(planet->getRadius()+5000000)*cos(angle*pi/180*-1),
			planet->getRadius()*sin(angle*pi/180*-1)+(planet->getRadius()+5000000)*sin(angle*pi/180*-1)));
		Vector vel(planet->getVelocity()+Vector(
			30000*cos(angle*pi/180*-1),
			30000*sin(angle*pi/180*-1)));
		engine->addObject(new Object(pos,vel, 500000, 10000, "Missile"));
		cooldown=GetTickCount();
		missiles--;
	}
}

void Player::Reset() {
	planet=NULL;
	hp=100;
	angle=0;
	missiles=1000;
	cooldown=0;
}

Object *Player::GetPlanet() {
	return planet;
}

int Player::GetHP() {
	return hp;
}

int Player::GetAngle() {
	return angle;
}

int Player::GetMissiles() {
	return missiles;
}
