#include "Camera.h"
#include "MatrixStack.h"
#include <iostream>

using namespace Eigen;

Camera::Camera() :
	aspect(1.0f),
	fovy(45.0f),
	znear(0.1f),
	zfar(1000.0f),
	rotations(0.0, 0.0),
	translations(0.0f, 0.0f, -5.0f),
	rfactor(0.2f),
	tfactor(0.001f),
	sfactor(0.005f)
{
	eye = Vector3f(0.0, 0.0, -25.0);
	up = Vector3f(0, 1, 0);

	yaw = 0;
	pitch = 0;

	f = Vector3f(0.0f, 0.0f, -1.0f);
}

Camera::~Camera()
{
}

void Camera::keyPressed(int key) {
	//foward
	if (key == 'w') {
		eye[0] += sin(yaw);
		eye[2] += cos(yaw);
	} 
	//backward
	if (key == 's') {
		eye[0] -= sin(yaw);
		eye[2] -= cos(yaw);
	} 
	//left
	if (key == 'a') {
		eye[0] -= -cos(yaw);
		eye[2] -= sin(yaw);
	} 
	//right
	if (key == 'd') {
		eye[0] += -cos(yaw);
		eye[2] += sin(yaw);
	} 

}

void Camera::mouseClicked(double x, double y, bool shift, bool ctrl, bool alt)
{
	mousePrev << x, y;
	if(shift) {
		state = Camera::TRANSLATE;
	} else if(ctrl) {
		state = Camera::SCALE;
	} else {
		state = Camera::ROTATE;
	}
}

void Camera::mouseMoved(double x, double y)
{
	Eigen::Vector2f mouseCurr(x, y);
	Eigen::Vector2f dv = mouseCurr - mousePrev;

	if (dv[0] < 0) {
		yaw -= 0.01f;
	}
	else {
		yaw += 0.01f;
	}

	if (dv[1] < 0) {
		if (pitch <= 2) {
			pitch += 0.01f;
		}
	}

	if (dv[1] > 0) {
		if (pitch >= -2) {
			pitch -= 0.01f;
		}
	}
	
	mousePrev = mouseCurr;
}


void Camera::applyProjectionMatrix(std::shared_ptr<MatrixStack> P) const
{
	// Modify provided MatrixStack
	P->perspective(fovy, aspect, znear, zfar);
}

void Camera::applyViewMatrix(std::shared_ptr<MatrixStack> MV)
{
	f = {sin(yaw), sin(pitch), cos(yaw)};
	MV->lookAt(eye, eye + f, up);
}

Vector3f Camera::getEye() {
	return eye;
}