#include "Object.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

using namespace std;

void Object::step(float h) {
	//velocity += h / mass * ((-c * mass) / pow(position.norm() + 0.1f * 0.1f, 3/2))) * position;
	velocity += h / mass * ((-(c * mass) / pow(position.norm() + 0.1f * 0.1f, 3 / 2))) * position;
	position += h * velocity;
}