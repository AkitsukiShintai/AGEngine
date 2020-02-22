// Particle.cpp
// -- sample implementation
// cs561 1/20


#include <cmath>
#include <algorithm>
#include <glm/glm.hpp>
#include <string>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include "Particle.h"
#include "HelpFunction.h"
#include "imgui.h"
using namespace std;

 int COUNT = 200;
 float ASPEED = glm::radians(360.0f / 5.0f);

//spiral parameters
 float SPIRAL_SPEED = glm::radians(360.0f / 1.0f);
 glm::vec3 SPIRAL_AROUND(0, 0, 1);
//vortex parameters
 float VORTEX_RATATION_SPEED = glm::radians(360.0f / 3.0f);
 glm::vec3 VORTEX_AROUND(0, 0, 1);
 glm::vec3 V0(0, 0, 0);
 float tightness = 1.0f; //  1<=tightness <=2
 float tightness_2 = tightness * 0.5f;
//damping
 float DAMPING_SLOW_DOWN_FACTOR = 2; // 0 ¡Ü d ¡Ü 10
 float DAMPING_ACCELERATION_FACTOR = 2;
 float DAMPING_THRESHOLD = 4.0f;

const glm::mat4 ID(1);
const glm::vec3 EZ(0, 0, 1);

glm::vec3 startCenter = glm::vec3(0,0,0);
glm::vec3 delta = glm::vec3(1, 1, 1);
float lifeTime = 10.0f;
float deltaLifeTime = 2.0f;

glm::vec3 upVelocity = glm::vec3(0, 0, 2);

bool Vortex = true;
bool Velocity = true;
bool Spiral = true;
bool Damping = true;
bool generateSpeed = true;
bool updateState = true;
void System::DrawImgui()
{
	//ImGui::PushTextColor(1, { 1,0,0,1 });
	ImGui::TextUnformatted("Particle Property");
	//ImGui::PopStyleColor();
	ImGui::SliderInt("Particle number", &COUNT, 1, 1500);
	ImGui::SliderFloat("Particle life time", &lifeTime, 0, 20);
	ImGui::SliderFloat("Particle life time range", &deltaLifeTime, 0, 5);
	ImGui::SliderFloat3("Particle Generate position", &startCenter[0], -10,10);
	ImGui::SliderFloat3("Particle Generate position range", &delta[0], 0, 2);
	ImGui::Spacing();

	ImGui::Checkbox("Vortex Property",&Vortex);
	ImGui::SliderFloat3("Vortex around", &VORTEX_AROUND[0], 0, 1);
	ImGui::SliderFloat3("Vortex around V0", &V0[0], -1, 1);
	ImGui::SliderFloat("Tightness * 0.5",&tightness_2,0.5f,1.0f);
	ImGui::SliderFloat("Vortex speed", &VORTEX_RATATION_SPEED, 0, 6.28f);
	ImGui::Spacing();

	ImGui::Checkbox("Spiral Property",&Spiral);
	ImGui::SliderFloat3("Spiral around", &SPIRAL_AROUND[0], 0, 1);
	ImGui::SliderFloat("Spiral speed", &SPIRAL_SPEED, -6.28f, 62.8f);
	ImGui::Spacing();

	ImGui::Checkbox("Velocity Field Property",&Velocity);
	ImGui::SliderFloat3("Velocity", &upVelocity[0], -10, 10);
	ImGui::Spacing();

	ImGui::Checkbox("Damping Property", &Damping);

	ImGui::SliderFloat("Damping Slow Down Factor", &DAMPING_SLOW_DOWN_FACTOR, 0, 10);
	ImGui::SliderFloat("Damping Acceleration Factor", &DAMPING_ACCELERATION_FACTOR, 0, 10);
	ImGui::SliderFloat("Damping Threshold", &DAMPING_THRESHOLD, 0, 10);
	ImGui::Spacing();

	ImGui::SliderFloat3("First Particle Position", &(*particles.begin()).position[0],-2,2);
	ImGui::SliderFloat3("First Particle Position", &(*particles.begin()).position[0], -2, 2);

	ImGui::Checkbox("Generate Speed", &generateSpeed);
	ImGui::Checkbox("Update", &updateState);

	if (ImGui::Button("Normalize"))
	{
		SPIRAL_AROUND = glm::normalize(SPIRAL_AROUND);
		VORTEX_AROUND = glm::normalize(VORTEX_AROUND);
	}

	if (ImGui::Button("Clear"))
	{
		particles.clear();
		Particle p;
		initParticle(p);
		particles.push_back(p);
	}
}

void System::UseSampleData()
{
	COUNT = 1000;
	upVelocity = glm::vec3(0, 0, 2);
	//VORTEX_AROUND.y = 0.123f;
	lifeTime = 5.0f;
	deltaLifeTime = 0.8f;
}




System::System()
{
	
}

void System::initialize(void) {
	
	float dtheta = glm::radians(360.0f / COUNT);
	int count = IntRand(0.8 * COUNT, COUNT);
	for (int i = 0; i < count; ++i) {
		Particle p;
		p.position.x = 0.5f * cos(i * dtheta);
		p.position.y = 0.5f * sin(i * dtheta);
		p.position.z = 0.0f;
		p.velocity.x = -ASPEED * p.position.y;
		p.velocity.y = ASPEED * p.position.x;
		initParticle(p);
		particles.push_back(p);

	}
}



glm::vec3 SpiralSpeedUpdate(const glm::vec3& previousSpeed, float dt) {
	float deltaAngle = SPIRAL_SPEED * dt;

	//glm::vec4 vt = glm::rotate(ID, SPIRAL_SPEED * dt, SPIRAL_AROUND) * glm::vec4(p.velocity,0.0f);
	//p.velocity = glm::vec3(vt.x,vt.y,vt.z);

	//float cosA = cos(deltaAngle);
	return Rtu(deltaAngle, SPIRAL_AROUND, previousSpeed);
}


glm::vec3 GetSpeedInVortexField(const glm::vec3& position) {
	glm::vec3 x = position;
	if (x == V0 )
	{
		return glm::vec3(0, 0, 0);
	}
	return VORTEX_RATATION_SPEED * glm::cross(VORTEX_AROUND, x - V0) / powf(glm::length2(x - V0) - glm::dot(VORTEX_AROUND, x - V0) * glm::dot(VORTEX_AROUND, x - V0), tightness_2);
}


void VortexPositionAndSpeedUpdate(Particle& p, float dt) {
	//float dAngle = VORTEX_RATATION_SPEED * dt;
	//glm::vec3 x0_v0 = p.prevPosition - V0;

	//float theta = dAngle / powf((glm::length2(x0_v0) - glm::dot( VORTEX_AROUND , x0_v0)* glm::dot(VORTEX_AROUND, x0_v0)), tightness_2);
	//p.position = V0 + Rtu(theta, VORTEX_AROUND, x0_v0);

	glm::vec3 x = p.position;

	p.velocity = VORTEX_RATATION_SPEED * glm::cross(VORTEX_AROUND, x - V0) / powf(glm::length2(x - V0) - glm::dot(VORTEX_AROUND, x - V0) * glm::dot(VORTEX_AROUND, x - V0), tightness_2);
	p.position = HeunMethod(p.prevPosition, p.prevVelocity, p.velocity, dt);
}


void DampingVelocitySpeedDownUpdate(Particle& p, float dt) {
	float fac = std::max(1.0f - dt * DAMPING_SLOW_DOWN_FACTOR, std::min(1.0f, DAMPING_THRESHOLD / glm::length(p.velocity)));
	p.velocity = p.velocity * fac;
}

void DampingVelocitySpeedUpUpdate(Particle& p, float dt) {
	float fac = std::min(1.0f + dt * DAMPING_ACCELERATION_FACTOR, std::max(1.0f, DAMPING_THRESHOLD / glm::length(p.velocity)));
	p.velocity = p.velocity * fac;
}

void DampingPositionUpdate(Particle& p, float dt) {
	p.position = HeunMethod(p.position, p.prevVelocity, p.velocity, dt);
}

void System::update(float dt, std::string& code) {

	//glm::mat3 R = glm::mat3(glm::rotate(ID, ASPEED * dt, EZ));
	/*for (Particle& p : particles) {
		p.position = R * p.position;
		p.velocity = R * p.velocity;
	}*/
	if (!updateState)
	{
		return;
	}

	auto start = particles.begin();
	auto end = particles.end();
	int deleteNumber = 0;
	std::list<std::list<Particle>::iterator> deleteList;
	for (; start != end; start ++) {
		Particle& p = *start;
		p.time_remaining -= dt;
		if (p.time_remaining < 0)
		{
			deleteNumber++;
			//start = particles.erase(start);
			deleteList.push_back(start);
			continue;
		}
		p.prevPosition = p.position;
		p.prevVelocity = p.velocity;
		glm::vec3 previousVelocity = p.prevVelocity;
		if (Vortex && Velocity )
		{
			previousVelocity = GetSpeedInVortexField(p.prevPosition) + upVelocity;
		}else if (Vortex)
		{
			previousVelocity = GetSpeedInVortexField(p.prevPosition);
		}else if (Velocity)
		{
 			previousVelocity = upVelocity;
		}
		p.prevVelocity = previousVelocity;

		if (Spiral)
		{
			previousVelocity = SpiralSpeedUpdate(previousVelocity, dt);
		}

		p.velocity = previousVelocity;
		if (Damping)
		{
			DampingVelocitySpeedUpUpdate(p, dt);
			DampingVelocitySpeedDownUpdate(p, dt);
		}

		p.position = HeunMethod(p.prevPosition, p.prevVelocity, p.velocity, dt);
		
	}
	code.append("Delete " + std::to_string(deleteNumber) + " particles \n");
	for (auto t : deleteList)
	{
		particles.erase(t);
	}
	int generateSize = IntRand((COUNT - particles.size() ) * 0.7f, COUNT - particles.size());
	code.append("Generate " + std::to_string(generateSize) + " particles \n");
	for (int i = 0; i <= generateSize && particles.size() < COUNT; i++)
	{
		Particle p;
		initParticle(p);
		particles.push_back(p);
	}
	code.append("Current has " + std::to_string(particles.size()) + " particles\n");

}

void System::initParticle(Particle& p)
{
	
	float x = FloatRand(-delta.x , delta.x);
	float y = FloatRand(-delta.y, delta.y);
	float z = FloatRand(-delta.z, delta.z);


	p.position = startCenter + glm::vec3(x,y,z);
	if (generateSpeed)
	{
		p.velocity.x = -ASPEED * p.position.y;
		p.velocity.y = ASPEED * p.position.x;
		p.velocity.z = 0.0f;
	}
	else
	{
		p.velocity = glm::vec3(0, 0, 0);
	}
	
	//
	p.motion_normal = glm::vec3(0, 0, 1);
	p.size = 0.1f;
	float t = FloatRand(-deltaLifeTime, deltaLifeTime);
	p.time_remaining = lifeTime + t;
	p.rotation = glm::quat(1, 0, 0, 0);
	p.prevPosition = p.position;
	p.prevVelocity = p.velocity;
	p.prevRotation = p.rotation;
}

