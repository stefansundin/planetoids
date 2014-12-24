#include "fysik/Physics.h"
#include "fysik/Object.h"
#include "fysik/ObjectX.h"

#ifndef __player__
#define __player__

class Player {
private:
	ObjectX *planet;
	Physics *engine;
	int hp;
	int angle;
	int missiles;
	int cooldown;
	
public:
	Player();
	
	void SetEngine(Physics *e);
	void SetPlanet(ObjectX *p);
	void SetHP(int h);
	void SetAngle(int a);
	void DecreaseMissile();
	void Fire();
	void Reset();

	ObjectX *GetPlanet();
	int GetHP();
	int GetAngle();
	int GetMissiles();
};

#endif
