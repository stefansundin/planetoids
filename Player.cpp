#include <iostream>
#include "Player.h"

using namespace std;

Player::Player() {
	Planet=-1;
	HP=100;
	Angle=0;
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

int Player::GetSelectedMissile() {
	return SelectedMissile;
}

int Player::GetMissiles() {
	return Missiles;
}

