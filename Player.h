
#ifndef __player__
#define __player__
//angle, hp, planetn, missilesleft, missileselected
class Player {
private:
	int Planet;
	int HP;
	int Angle;
	int Missiles;
	int SelectedMissile;
	
public:
	Player();
	
	void SetPlanet(int p);
	void SetHP(int h);
	void SetAngle(int a);
	void SelectMissile(int s);
	void DecreaseMissile();
	
	int GetPlanet();
	int GetHP();
	int GetAngle();
	int GetSelectedMissile();
	int GetMissiles();
};

#endif
