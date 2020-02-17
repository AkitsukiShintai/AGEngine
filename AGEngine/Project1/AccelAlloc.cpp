//#define ACCEL_ALLOC
#ifdef ACCEL_ALLOC
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

glm::vec3 prioritizedAcceleration(const std::vector<glm::vec3>& accels, float max_accel) {
	glm::vec3 a(0.0f, 0.0f,0.0f);
	int i = 0;
	while (i< accels.size() && glm::length(a) < max_accel)
	{	
		a += accels[i];	
		i++;	
	}
	if (glm::length(a) > max_accel)
	{
		float a_2 = glm::length2(a);
		float ai_2 = glm::length2(accels[i - 1]);
		float dotA_Ai = glm::dot(a, accels[i - 1]);
		float x = dotA_Ai - sqrtf(ai_2 * max_accel * max_accel - a_2 * ai_2 + dotA_Ai * dotA_Ai) / ai_2;
		a = a - x * accels[i - 1];
	}
	
	return a;
}


#endif