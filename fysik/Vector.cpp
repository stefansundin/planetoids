#include "Vector.h"

Vector::Vector()
{
	m_x = 0;
	m_y = 0;
}
Vector::Vector(double p_x, double p_y)
{
	m_x = p_x;
	m_y = p_y;
}
Vector::~Vector()
{
}

const Vector& Vector::makeUnit()
{
	double length = getLength();
	m_x /= length;
	m_y /= length;
	return *this;
}
const Vector& Vector::invert()
{
	m_x = -m_x;
	m_y = -m_y;
	return *this;
}

double Vector::getLength() const
{
	return sqrt(m_x*m_x + m_y*m_y);
}
double Vector::getX() const
{
	return m_x;
}
double Vector::getY() const
{
	return m_y;
}

const Vector& Vector::operator=(const Vector &p_rhs)
{
	m_x = p_rhs.m_x;
	m_y = p_rhs.m_y;
	return *this;
}

const Vector Vector::operator+(const Vector &p_rhs) const
{
	Vector temp = *this;
	temp += p_rhs;
	return temp;
}
const Vector Vector::operator-(const Vector &p_rhs) const
{
	Vector temp = *this;
	temp -= p_rhs;
	return temp;
}
const Vector Vector::operator*(double p_scalar) const
{
	Vector temp = Vector(m_x*p_scalar, m_y*p_scalar);
	return temp;
}
const Vector Vector::operator/(double p_scalar) const
{
	Vector temp = Vector(m_x/p_scalar, m_y/p_scalar);
	return temp;
}

Vector& Vector::operator+=(const Vector &p_rhs)
{
	m_x += p_rhs.m_x;
	m_y += p_rhs.m_y;
	return *this;
}
Vector& Vector::operator-=(const Vector &p_rhs)
{
	m_x -= p_rhs.m_x;
	m_y -= p_rhs.m_y;
	return *this;
}
bool Vector::operator==(const Vector &p_rhs) const
{
	return m_x == p_rhs.m_x &&
	m_y == p_rhs.m_y;
}

