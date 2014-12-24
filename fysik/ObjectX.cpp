#include "ObjectX.h"

ObjectX::ObjectX(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name)
: Object(p_position, p_velocity, p_radius, p_mass, p_name)
{
	m_color_r=0;
	m_color_g=0;
	m_color_b=0;
	time=0;
	hp=100;
}

ObjectX::ObjectX(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name, unsigned char p_color_r, unsigned char p_color_g, unsigned char p_color_b)
: Object(p_position, p_velocity, p_radius, p_mass, p_name)
{
	m_color_r=p_color_r;
	m_color_g=p_color_g;
	m_color_b=p_color_b;
	time=0;
	hp=100;
}

ObjectX::ObjectX(Vector p_position, Vector p_velocity, double p_radius, double p_mass, std::string p_name, unsigned char p_color_r, unsigned char p_color_g, unsigned char p_color_b, int p_hp)
: Object(p_position, p_velocity, p_radius, p_mass, p_name)
{
	m_color_r=p_color_r;
	m_color_g=p_color_g;
	m_color_b=p_color_b;
	time=0;
	hp=p_hp;

}

Vector ObjectX::getVelocity() const
{
	return m_velocity;
}

double ObjectX::getMass() const
{
	return m_mass;
}

void ObjectX::updateRadius(double p_radius)
{
	m_radius=(p_radius<3?3:p_radius);
}

void ObjectX::updateMass(double p_mass)
{
	m_mass = p_mass;
}

unsigned char ObjectX::getColorR() {
	return m_color_r;
}

unsigned char ObjectX::getColorG() {
	return m_color_g;
}

unsigned char ObjectX::getColorB() {
	return m_color_b;
}

void ObjectX::incTime(int timestep){
	time+=timestep;
}

int ObjectX::getTime(){
	return time;
}
void ObjectX::SetHP(int h) {
	hp=h;
}

int ObjectX::GetHP() {
	return hp;
}


