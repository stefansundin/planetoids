#include "fysik/Physics.h"
#include "fysik/Object.h"
#include "fysik/ObjectX.h"

#ifndef __player__
#define __player__

class Player {
private:
	ObjectX *planet;
	Physics *engine;
	int angle;
	int cooldown;
	
public:
	Player();
	
	void SetEngine(Physics *e);
	void SetPlanet(ObjectX *p);
	void SetAngle(int a);
	void Fire();
	void Reset();

	ObjectX *GetPlanet();
	int GetAngle();
};

#endif
