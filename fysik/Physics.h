#ifndef Physics_h
#define Physics_h

#include <iostream>
#include <vector>

#include "Object.h"

class Physics
{
public:
	Physics(void (*p_funptr)(Object*, Object*));
	~Physics();

	bool addObject(Object *p_object);
	bool removeObject(Object *p_object);
	void clear();
	void doPhysics(double p_timestep);

	std::vector<Object*>* getObjectsPointer() const;	//use this to get the objects for rendering

protected:
	void (*m_funptr)(Object*, Object*);
	std::vector<Object*> *m_objects;
};

#endif //Physics_h

