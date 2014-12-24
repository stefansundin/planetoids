#include "fysik/Physics.h"
#include "fysik/Object.h"

#ifndef __player__
#define __player__

class Player {
private:
	Object *Planet;
	Physics *engine;
	int HP;
	int Angle;
	int Missiles;
	int SelectedMissile;
	int LastFired;
	
public:
	Player();
	
	void SetEngine(Physics *e);
	void SetPlanet(Object *p);
	void SetHP(int h);
	void SetAngle(int a);
	void SelectMissile(int s);
	void DecreaseMissile();
	void Fire();

	Object *GetPlanet();
	int GetHP();
	int GetAngle();
	int GetSelectedMissile();
	int GetMissiles();
};

#endif
