#include "Object.h"

Object::Object()
{
	m_position = Vector(0,0);
	m_velocity = Vector(0,0);
	m_resultant_force = Vector(0,0);
	m_radius = 0.0;
	m_mass = 0.0;
	m_name = "Noname";
}
Object::Object(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name)
{
	m_position = p_position;
	m_velocity = p_velocity;
	m_radius = p_radius;
	m_mass = p_mass;
	m_name = p_name;
}
Object::~Object()
{
}

Vector Object::getPosition() const
{
	return m_position;
}
Vector Object::getVelocity() const
{
	return m_velocity;
}
double Object::getRadius() const
{
	return m_radius;
}
Vector Object::getResultantForce() const
{
	return m_resultant_force;
}
std::string Object::getName() const
{
	return m_name;
}

double Object::getMass() const
{
	return m_mass;
}


const Vector Object::getGForce(const Object *p_other)
{
	//F = G * m1*m2
	//        -----
	//         r*r

	Vector distance = p_other->m_position - m_position;
	double force;
	if(distance.getLength() == 0){
		force = 1;
	}
	else{
		force = ( G * (m_mass * p_other->m_mass) ) / ( distance.getLength() * distance.getLength() );
	}

	return distance * force;
}

bool Object::collide(const Object *p_other)
{
	double distance = (m_position - p_other->m_position).getLength();
	double cookie = m_radius + p_other->m_radius;
//	printf("distance between (%s) and (%s): (%f)(%f)\n", m_name.c_str(), p_other->getName().c_str(), distance, cookie);
	return distance <= m_radius + p_other->m_radius;
}

void Object::updateVelocity(std::vector<Object*> *p_objects, double p_time_step)
{
	Vector force = Vector(0,0);
	for(int i = 0; i < p_objects->size(); i++)
	{
		if(this != p_objects->at(i))
		{
			force += getGForce(p_objects->at(i));
		}
	}

	m_resultant_force = force;				//save the resultant force for easy rendering of it
	Vector acceleration = force/m_mass;
	Vector position = acceleration*0.5*(p_time_step/1000);
	m_velocity += position;
}

void Object::updatePosition(double p_time_step)
{
	m_position += m_velocity*(p_time_step/1000);
}

void Object::updateRadius(double p_radius)
{
	m_radius=(p_radius<3?3:p_radius);
}

void Object::updateMass(double p_mass)
{
	m_mass = p_mass;
}
