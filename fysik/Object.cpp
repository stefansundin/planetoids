#include "Object.h"

Object::Object()
{
	m_position = Vector(0,0);
	m_velocity = Vector(0,0);
	m_resulting_force = Vector(0,0);
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
double Object::getRadius() const
{
	return m_radius;
}
Vector Object::getResultingForce() const
{
	return m_resulting_force;
}
std::string Object::getName() const
{
	return m_name;
}


const Vector Object::getGForce(const Object *p_other)
{
	//F = G * m1*m2
	//        -----
	//         r*r

	Vector distance = p_other->m_position - m_position;
	double force = (G * (m_mass * p_other->m_mass)) / (distance.getLength() * distance.getLength());
	distance.makeUnit();

	return distance * force;
}

bool Object::collide(const Object *p_other)
{
	double distance = (m_position - p_other->m_position).getLength();
	double cookie = m_radius + p_other->m_radius;
//	printf("distance between (%s) and (%s): (%f)(%f)\n", m_name.c_str(), p_other->getName().c_str(), distance, cookie);
	return distance <= m_radius + p_other->m_radius;
}

void Object::updateForce(std::vector<Object*> *p_objects)
{
	m_resulting_force = Vector(0,0);
	for(int i = 0; i < p_objects->size(); i++)
	{
		if(this != p_objects->at(i))
		{
			m_resulting_force += getGForce(p_objects->at(i));
		}
	}
}

void Object::updatePosition(double p_time_step)
{
	Vector old_velocity = m_velocity;

	Vector acceleration = m_resulting_force/m_mass;
	Vector diff = acceleration*(p_time_step/1000);
	m_velocity += diff;

	m_position += old_velocity*(p_time_step/1000) + acceleration*(p_time_step/1000)*(p_time_step/1000)*0.5;
}
