#ifndef OBJECT_H
#define OBJECT_H
#include <Eigen/Dense>
#include <memory>

class Shape;
class Texture;

class Object {
public:
	void step(float h);


	Eigen::Vector3f position;
	Eigen::Vector3f velocity;
	float xRot;
	Eigen::Vector3f scale;
	Eigen::Vector3f ka;
	Eigen::Vector3f kd;
	Eigen::Vector3f ks;
	std::shared_ptr<Shape> shape;
	float mass;
	float c;

	std::shared_ptr<Texture> texture;
};

#endif // !OBJECT_H