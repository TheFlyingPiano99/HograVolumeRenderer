#include "ObserveObjectControl.h"

#define CROP_SLIDE_SPEED 0.1f

Hogra::ObserveObjectControl::ObserveObjectControl() : rotationSpeed(0.01f), zoomSpeed(0.1f) {
	GlobalVariables::hideCursor = false;
}

void Hogra::ObserveObjectControl::Rotate(const glm::vec2& delta)
{
	if (nullptr == camera) {
		return;
	}
	if (isCropMode) {
		glm::vec3 w_d = (-camera->getRight() * delta.x + camera->getUp() * delta.y) / length(planePosition - camera->GetPosition());
		float d = glm::dot(planeNormal, w_d);
		DragPlane(d * CROP_SLIDE_SPEED);
		return;
	}
	if (!GlobalVariables::hideCursor) {
		return;
	}
	camera->RotateAroundPoint(delta * rotationSpeed);
}

void Hogra::ObserveObjectControl::Zoom(float delta)
{
	if (nullptr == camera) {
		return;
	}

	camera->ApproachCenter(delta * zoomSpeed);
}

void Hogra::ObserveObjectControl::primaryAction()
{

}

void Hogra::ObserveObjectControl::grab()
{
	GlobalVariables::hideCursor = true;
}

void Hogra::ObserveObjectControl::release()
{
	GlobalVariables::hideCursor = false;
}

void Hogra::ObserveObjectControl::grabPlane(float x, float y) {
	if (isCropMode) {
		return;
	}
	glm::vec4 ndc = glm::vec4(x, y, 0, 1);
	glm::vec4 wDir = camera->GetRayDirMatrix() * glm::vec4(x, y, 0.0, 1.0f);
	wDir /= wDir.w;
	glm::vec3 dir = glm::normalize(glm::vec3(wDir));
	Ray ray;
	ray.SetPosition(camera->GetPosition());
	ray.setDirection(dir);

	glm::vec3 w_point(0.0f);
	glm::vec3 w_normal(0.0f);
	auto collidedWith = scene->IntersectRay(ray, w_point, w_normal);
	if (nullptr == collidedWith) {
		return;
	}
	if (collider == collidedWith) {
		isCropMode = true;
		planePosition = w_point;
		planeNormal = w_normal;
		std::cout << "N: " << planeNormal.x << ", " << planeNormal.y << ", " << planeNormal.z << std::endl;
	}
}

void Hogra::ObserveObjectControl::releasePlane(float x, float y) {
	isCropMode = false;
}

void Hogra::ObserveObjectControl::pickVoxel(float x, float y, float distance)
{
	glm::vec4 ndc = glm::vec4(x, y, 0, 1);
	glm::vec4 wDir = camera->GetRayDirMatrix() * ndc;
	wDir /= wDir.w;
	glm::vec3 dir = glm::normalize(glm::vec3(wDir));
	Ray ray;
	ray.SetPosition(camera->GetPosition());
	ray.setDirection(dir);
	auto pos = ray.GetPosition() + ray.getDirection() * distance;
	std::cout << "x: " << pos.x << " y: " << pos.y << " z: " << pos.z << " - click" << std::endl;

	volumeObject->pickVoxel(pos);
}

void Hogra::ObserveObjectControl::DragPlane(float delta) {
	if (nullptr == camera) {
		return;
	}
	glm::vec3 min(0.0f);
	glm::vec3 max(0.0f);
	volumeObject->GetMinAndMax(min, max);
	if (glm::dot(planeNormal, glm::vec3(1,0,0)) > 0.9f) {
		max += glm::vec3(delta, 0, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 1, 0)) > 0.9f) {
		max += glm::vec3(0, delta, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 0, 1)) > 0.9f) {
		max += glm::vec3(0, 0, delta);
	}
	else if (glm::dot(planeNormal, glm::vec3(-1, 0, 0)) > 0.9f) {
		min += glm::vec3(-delta, 0, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, -1, 0)) > 0.9f) {
		min += glm::vec3(0, -delta, 0);
	}
	else if (glm::dot(planeNormal, glm::vec3(0, 0, -1)) > 0.9f) {
		min += glm::vec3(0, 0, -delta);
	}
	volumeObject->ResizeDisplayBoundingBox(min, max);
	volumeObject->GetMinAndMax(min, max);
	collider->SetMin(min);
	collider->SetMax(max);
}
