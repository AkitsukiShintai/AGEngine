#pragma once
#include <random>
#include <iostream>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

static glm::vec3 Rtu(float theta, const glm::vec3& u, const glm::vec3& v) {
	float cosA = cos(theta);
	return cosA * v + (1 - cosA) * glm::dot(u, v) * u + sin(theta) * glm::cross(u, v);
}

static glm::vec3 HeunMethod(const glm::vec3& x0, const glm::vec3& v0, const glm::vec3& v1, float dt) {
	return x0 + 0.5f * dt * (v0 + v1);
}


static glm::vec3 RK2(const glm::vec3& x0 , const glm::vec3& v0, float dt) {
	return x0;
}

static float FloatRand(float x, float y) {
	assert(x <= y);
	return x + ((float)std::rand() / (float)RAND_MAX) * ( y - x )  ;
}


static int IntRand(int x, int y) {
	assert(x <= y);
	if (x == y)
	{
		return x;
	}
	return x + std::rand() % (y - x);
}

static void Print(const glm::vec2& value, std::string name = "") {
	std::cout << name + ": " << value.x << " " << value.y << std::endl;
}

static void Print(const glm::vec3& value, std::string name = "") {
	std::cout << name + ": " << value.x << " " << value.y << " " << value.z << std::endl;
}