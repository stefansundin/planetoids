#ifndef Vector_h
#define Vector_h

#include <math.h>

class Vector
{
public:
	Vector();
	Vector(double p_x, double p_y);
	virtual ~Vector();

	const Vector& makeUnit();
	const Vector& invert();

	double getLength() const;
	double getX() const;
	double getY() const;

	const Vector& operator=(const Vector &p_rhs);
	const Vector operator+(const Vector &p_rhs) const;
	const Vector operator-(const Vector &p_rhs) const;
	const Vector operator*(double p_scalar) const;
	const Vector operator/(double p_scalar) const;
	Vector& operator+=(const Vector &p_rhs);
	Vector& operator-=(const Vector &p_rhs);
	bool operator==(const Vector &p_rhs) const;

protected:
	double m_x;
	double m_y;
};

#endif //Vector_h

