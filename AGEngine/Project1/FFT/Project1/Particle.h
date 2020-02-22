// Particle.h
// -- particle system bare-bones framework.
// cs561 1/20

#ifndef CS561_PARTICLE_H
#define CS561_PARTICLE_H


#include <list>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

struct Particle {

	glm::vec3 position, prevPosition,
            velocity,prevVelocity,
			acceleration,
            motion_normal  = glm::vec3(0,0,1); // normal to plane of motion
  glm::quat rotation, prevRotation ;
  float size = 0.1f,
        time_remaining = 0.0f;
};


struct System {
  System();
  std::list<Particle> particles;
  void initialize(void);
  void update(float dt,std::string& code);
  void initParticle(Particle& particle);
  void DrawImgui();
  void UseSampleData();
};


#endif

