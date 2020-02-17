//#define AVOID_WALL
#ifdef AVOID_WALL
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

struct WallObstacle {
	glm::vec3 wall_point,
		wall_normal;
	float distance_threshold;
	bool willCollide(const glm::vec3& P, const glm::vec3& v) const {
		float dotV_N = glm::dot(v, wall_normal);
		if (dotV_N == 0)//v || n, no collision
		{
			return false;
		}

		float t = glm::dot(wall_point - P, wall_normal) / dotV_N;
		if (t <0)// can not see
		{
			return false;
		}

		if (t * glm::length(v) < distance_threshold)
		{
			return true;
		}

		return false;
	}
	glm::vec3 avoidanceDirection(const glm::vec3& P, const glm::vec3& v, const glm::vec3& m) const {
		glm::vec3 nXm = glm::normalize(glm::cross(wall_normal, m));
		float a = glm::dot(v, nXm);
		float sign = 1;
		if (a <0)
		{
			sign = -1.0f;
		}

		return sign * nXm;
	}
};

#endif