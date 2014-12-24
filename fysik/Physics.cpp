#include "Physics.h"
#include <algorithm>

Physics::Physics(void (*p_funptr)(Object*, Object*))
{
	m_funptr = p_funptr;

	m_objects = new std::vector<Object*>();
}
Physics::~Physics()
{
	//clean up, free the memory of all objects and then of the vector itself
	int i;
	for(i = 0; i < m_objects->size(); i++)
	{
		delete m_objects->at(i);
	}
	delete m_objects;
}


bool Physics::addObject(Object *p_object)
{
	std::vector<Object*>::iterator itr;
	for(itr = m_objects->begin(); itr != m_objects->end(); itr++)
	{
		if(*itr == p_object)
		{
			return false;
		}
	}

	m_objects->push_back(p_object);
	return true;
}
bool Physics::removeObject(Object *p_object)
{
	std::vector<Object*>::iterator itr;
	for(itr = m_objects->begin(); itr != m_objects->end(); itr++)
	{
		if(*itr == p_object)
		{
			m_objects->erase(itr);
			delete p_object;
			return true;
		}
	}

	return false;
}

void Physics::clear() {
	while (!m_objects->empty())
	{
		removeObject(m_objects->at(0));
	}
}

void Physics::doPhysics(double p_timestep)
{
	int i, j;

	for(i = 0; i < m_objects->size(); i++)
	{
		m_objects->at(i)->updatePosition(p_timestep);
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		m_objects->at(i)->updateForce(m_objects);
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		for(j = i+1; j < m_objects->size(); j++)
		{
			if (m_objects->at(i)->collide(m_objects->at(j)))
			{
				m_funptr(m_objects->at(i), m_objects->at(j));
				/*std::cout
					<< "BOOM! " << m_objects->at(i)->getName()
					<< " krockar med " << m_objects->at(j)->getName()
					<< ".\n";*/
			}
		}
	}
}


std::vector<Object*>* Physics::getObjectsPointer() const
{
	return m_objects;
}

