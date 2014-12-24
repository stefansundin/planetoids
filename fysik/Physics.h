#ifndef Physics_h
#define Physics_h

#include <iostream>
#include <vector>

#include "Object.h"
#include "millitime.h"

class Physics
{
public:
	Physics();
	~Physics();

	bool addObject(Object *p_object);
	bool removeObject(Object *p_object);
	void clear();
	void doPhysics();

	std::vector<Object*>* getObjectsPointer() const;	//use this to get the objects for rendering

public:
	std::vector<Object*> *m_objects;

	long m_at_last_physics;
};

#endif //Physics_h

