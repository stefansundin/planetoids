#include "Physics.h"
#include <algorithm>

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

void Physics::clear() {
	std::vector<Object*>::iterator itr;
	while (!m_objects->empty())
	{
		itr=m_objects->begin();
		m_objects->erase(itr);
	}
}

void Physics::doPhysics()
{
	int i, j;
	double timestep = millitime()-m_at_last_physics;
	timestep+=100000;
	std::vector<Object*> destroy;
	//printf("timestep: (%l)\n", timestep);
	//std::cout << "timestep: (" << timestep << ")\n";
	for(i = 0; i < m_objects->size(); i++)
	{
		m_objects->at(i)->updatePosition(timestep);
		/*if (m_objects->at(i)->getName() == "Missile"
		 && (m_objects->at(i)->getPosition() < Vector(-600,-600)
		 || m_objects->at(i)->getPosition() > Vector(600,600))) {
			destroy.push_back(m_objects->at(i));
		}*/
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		//m_objects->at(i)->updateVelocity(m_objects,timestep);
		m_objects->at(i)->updateForce(m_objects);
	}

	for(i = 0; i < m_objects->size(); i++)
	{
		for(j = i+1; j < m_objects->size(); j++)
		{
			//if (m_objects->at(i) != m_objects->at(j))			{
				if (m_objects->at(i)->collide(m_objects->at(j)))
				{
					std::string name1=m_objects->at(i)->getName();
					std::string name2=m_objects->at(j)->getName();
					if (name1 == "Missile" && name2 == "Missile") {
						destroy.push_back(m_objects->at(i));
						destroy.push_back(m_objects->at(j));
					}
					else if ((name1 == "Missile" && (name2 == "Sun" || name2 == "Blackhole"))
						  || (name2 == "Missile" && (name1 == "Sun" || name1 == "Blackhole"))) {
						int missile=(name1=="Missile"?i:j);
						destroy.push_back(m_objects->at(missile));
					}
					else if (name1 == "Missile" || name2 == "Missile") {
						int missile=(name1=="Missile"?i:j);
						int planet=(missile==i?j:i);
						//Decrease HP here
						m_objects->at(planet)->updateRadius(m_objects->at(planet)->getRadius()*0.9);
						//m_objects->at(planet)->updateMass(m_objects->at(planet)->getMass()*0.9);
						destroy.push_back(m_objects->at(missile));
					}
					/*else if (name1 == "Blackhole" || name2 == "Blackhole") {
						int blackhole=(name1=="Blackhole"?i:j);
						int planet=(blackhole==i?j:i);
						m_objects->at(blackhole)->updateRadius(m_objects->at(blackhole)->getRadius()+0.5*m_objects->at(planet)->getRadius());
						m_objects->at(blackhole)->updateMass(m_objects->at(blackhole)->getMass()+0.5*m_objects->at(planet)->getMass());
						destroy.push_back(m_objects->at(planet));
					}*/
					/*else {
						double newRadius=(m_objects->at(i)->getRadius()+m_objects->at(j)->getRadius())/2;
						m_objects->at(i)->updateRadius(newRadius);
						m_objects->at(j)->updateRadius(newRadius);
					}*/
					/*else {
						Vector vel1=m_objects->at(i)->getVelocity();
						Vector vel2=m_objects->at(j)->getVelocity();
						m_objects->at(i)->setVelocity(vel2);
						m_objects->at(j)->setVelocity(vel1);
					}*/
					/*std::cout
						<< "BOOM! " << m_objects->at(i)->getName()
						<< " krockar med " << m_objects->at(j)->getName()
						<< ".\n";*/
				}
			//}
		}
	}

	for (i=0; i < destroy.size(); i++) {
		removeObject(destroy.at(i));
	}

	m_at_last_physics = millitime();
}


std::vector<Object*>* Physics::getObjectsPointer() const
{
	return m_objects;
}

