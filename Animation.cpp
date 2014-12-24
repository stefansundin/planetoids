#include <iostream>
#include <string>
#include "Animation.h"

using namespace std;

Animation::Animation(Vector p, string t, string v) {
	pos=p;
	type=t;
	value=v;
	frame=0;
}

void Animation::step() {
	frame++;
}

Vector *Animation::getPos() {
	return &pos;
}

int Animation::getFrame() {
	return frame;
}

string Animation::getType() {
	return type;
}

string Animation::getValue() {
	return value;
}
