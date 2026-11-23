#include "Particle.hpp"
#include <cmath>

void Particle::update(float dt) {
  if (!isActive) return;
  position += velocity*dt;
}

void Particle::absorb(Particle& other) {
  if (!isActive || !other.isActive) return;

  float totalMass = mass + other.mass;

  velocity = (mass * velocity + other.mass * other.velocity) / totalMass;

  position = (mass * position + other.mass * other.position) / totalMass;

  mass = totalMass;
  radius = std::hypot(radius, other.radius);

  other.isActive = false;
}
