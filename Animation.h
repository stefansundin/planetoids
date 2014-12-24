#include "fysik/Vector.h"

#ifndef __animation__
#define __animation__

using namespace std;

class Animation {
private:
	Vector pos;
	string type;
	string value;
	int frame;
	
public:
	Animation(Vector p, string t, string v);
	Animation(Vector p, string t);
	
	void step();
	Vector *getPos();
	int getFrame();
	string getType();
	string getValue();
};

#endif
