#ifndef ObjectX_h
#define ObjectX_h

#include "Object.h"
#include "Vector.h"

class ObjectX: public Object
{
protected:
	int time;
	int hp;
	unsigned char m_color_r;
	unsigned char m_color_g;
	unsigned char m_color_b;

public:
	ObjectX(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name);
	ObjectX(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name, unsigned char p_color_r, unsigned char p_color_g, unsigned char p_color_b);

	Vector getVelocity() const;
	double getMass() const;
	void incTime(int t);
	void updateRadius(double p_radius);
	void updateMass(double p_mass);
	void SetHP(int h);
	int GetHP();
	int getTime();
	unsigned char getColorR();
	unsigned char getColorG();
	unsigned char getColorB();
};

#endif //ObjectX_h
