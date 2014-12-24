#ifndef Object_h
#define Object_h

#include <vector>
#include <string>

#include "Vector.h"

const double G = 6.67e-11; //6.67*10^(-11)

class Object
{
public:
	Object();
	Object(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name);
	~Object();

	Vector getPosition() const;
	double getRadius() const;
	Vector getResultingForce() const;
	std::string getName() const;

	const Vector getGForce(const Object *p_other);
	bool collide(const Object *p_other);
	void updateForce(std::vector<Object*> *p_others);
	void updatePosition(double p_time_step);
	
	//hax
	Vector getVelocity() const;
	double getMass() const;
	void updateRadius(double p_radius);
	void updateMass(double p_mass);

protected:
	Vector m_position;
	Vector m_velocity;
	Vector m_resulting_force;
	double m_radius;
	double m_mass;
	std::string m_name;
};

#endif //Object_h
