//#define OP_SPLITE
#ifdef OP_SPLITE
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <glm/glm.hpp>

//parameter of F(X)
const float k = 0.5f;
const float t = 1.0f;
const float t_2 = t / 2;
const float dt = 0.2f; //time step
const glm::vec2 GA = glm::vec2(0, 1) * -0.75f; // gravitational acceleration

glm::vec2 GetVelocityInVortexField(const glm::vec2& position) {
	float theta = k / powf(position.x * position.x + position.y * position.y, t_2);
	float x = -position.y * theta;
	float y = position.x * theta;
	return glm::vec2(x,y);
}

template<class T>
static T HeunMethod(const T& x0, const T& v0, const T& v1, float dt) {
	return x0 + 0.5f * dt * (v0 + v1);
}

int main() {
	unsigned int n = 0;
	std::cout << "Input how many times you want to integrate" << std::endl;
	std::cin >> n;

	glm::vec2 position(1,0);
	glm::vec2 velocity(0, 0);
	glm::vec2 previousSpeed;
	float totalTime = 0.0f;
	std::cout << std::fixed << std::setprecision(4);
	std::cout << "The output is:" << std::endl;

	std::cout << totalTime << " | " << position.x << " | " << position.y << std::endl;
	while (n != 0)
	{
		n--;
		previousSpeed = velocity;
		//get velocity at current position
		velocity = GetVelocityInVortexField(position);
		//gravitational velocity
		glm::vec2 gravityUpdateSpeed = GA * dt;
		//total velocity
		velocity += gravityUpdateSpeed;
		position = HeunMethod(position, previousSpeed, velocity, dt);
		totalTime += dt;
		std::cout << totalTime << " | " << position.x << " | " << position.y << std::endl;

	}



	return 0;
}



#endif