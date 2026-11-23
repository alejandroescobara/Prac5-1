#pragma once

#include <QVector2D>
#include <cstdint>

class Particle {
  private:

    uint32_t id;

    QVector2D position; // metros
    QVector2D velocity; // metros por segundo

    float     mass;     // kilogramos
    float     radius;   // metros

    bool      isActive{true};

  public:
    explicit Particle(QVector2D initialPos, QVector2D initialVel, float initialMass, float initialRadius)
                          :position(initialPos), velocity(initialVel), mass(initialMass), radius(initialRadius) {}

    void update(float dt);

    bool operator>=(const Particle& other) const {
      return this->mass >= other.mass;
    }

    void absorb(Particle& other);

    //METODOS TRIVIALES

    uint32_t getId() const { return id; }
    void setId(uint32_t newId) { id = newId; }

    QVector2D getPosition() const { return position; }
    void setPosition(QVector2D newPos) { position = newPos; }


    QVector2D getVelocity() const { return velocity; }
    void setVelocity(QVector2D newVel) { velocity = newVel; }

    float getMass() const { return mass; }
    void setMass(float newMass) { mass = newMass; }

    float getRadius() const { return radius; }
    void setRadius(float newRadius) { radius = newRadius; }

    bool getIsActive() const { return isActive; }
    void setIsActive(bool newIsActive) { isActive = newIsActive; }
};
