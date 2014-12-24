#include "Physics.h"

Physics::Physics()
{
	m_at_last_physics = millitime();

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
			return true;
		}
	}

	return false;
}


void Physics::doPhysics()
{
	int i, j;
	Object *del=NULL;
	double timestep = millitime()-m_at_last_physics;
	//printf("timestep: (%l)\n", timestep);
	//std::cout << "timestep: (" << timestep << ")\n";
	for(i = 0; i < m_objects->size(); i++)
	{
		m_objects->at(i)->updatePosition(timestep);
		if (m_objects->at(i)->getPosition() < Vector(-600,-600)
		 || m_objects->at(i)->getPosition() > Vector(600,600)) {
			del=m_objects->at(i);
		}
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		m_objects->at(i)->updateVelocity(m_objects,timestep);
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		for(j = 0; j < m_objects->size(); j++)
		{
			if (m_objects->at(i) != m_objects->at(j))
			{
				if (m_objects->at(i)->collide(m_objects->at(j)))
				{
					std::vector<Object*>::iterator del;
					if (m_objects->at(i)->getName() == "Missile"
					 && m_objects->at(j)->getName() == "Missile") {
						del=m_objects->begin()+i;
						m_objects->erase(del);
						del=m_objects->begin()+j;
						m_objects->erase(del);
					}
					if (m_objects->at(i)->getName() == "Missile") {
						m_objects->at(j)->updateRadius(m_objects->at(j)->getRadius()-1);
						del=m_objects->begin()+i;
						m_objects->erase(del);
					}
					if (m_objects->at(j)->getName() == "Missile") {
						m_objects->at(i)->updateRadius(m_objects->at(i)->getRadius()-1);
						del=m_objects->begin()+j;
						m_objects->erase(del);
					}
					/*std::cout
						<< "BOOM! " << m_objects->at(i)->getName()
						<< " krockar med " << m_objects->at(j)->getName()
						<< ".\n";*/
				}
			}
		}
	}

	m_at_last_physics = millitime();

	if (del != NULL) {
		removeObject(del);
	}
}


std::vector<Object*>* Physics::getObjectsPointer() const
{
	return m_objects;
}

