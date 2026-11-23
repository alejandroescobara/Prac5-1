#pragma once

#include <QVector2D>
#include <vector>

#include "Particle.hpp"
#include "Obstacle.hpp"
#include "DataManager.hpp"

struct Box {
  float width{100.0f};
  float height{100.0f};

  bool contains(const QVector2D& center, float radius) const {
    return (center.x() - radius >= 0.0f) &&
           (center.x() + radius <= width) &&
           (center.y() - radius >= 0.0f) &&
           (center.y() + radius <= height);
  }

  bool contains(const Particle& p) const {
    return contains(p.getPosition(), p.getRadius());
  }
};

class Simulator {
  private:

    Box box;
    std::vector<Particle> particles;
    std::vector<Obstacle> obstacles;
    DataManager& logger;
    
    struct Wall {
      QVector2D point;   // Punto que pertenece a la pared
      QVector2D normal;  // Normal unitaria hacia adentro
    };

    float currentTime{0.0f};

    bool isCollision(const Particle& p, QVector2D& contactPoint) const;

    bool isCollision(const Particle& p, const Obstacle& obs, QVector2D& contactPoint) const;

    bool isCollision(const Particle& p1, const Particle& p2, QVector2D& contactPoint) const;

    void resolveWallCollisions();
    void resolveObstacleCollisions();
    void resolveParticleCollisions();

  public:  
    static constexpr float TIME_STEP = 0.01f;

    explicit Simulator(const Box& b, DataManager& log)
                      :box(b), logger(log) {}

    bool isValidParticle(const QVector2D& pos, float radius) const;

    bool isValidObstacle(const QVector2D& pos, float size) const;

    Simulator& operator +=(Particle& newParticle);

    Simulator& operator +=(Obstacle& newObstacle);

    void step();

    float getCurrentTime() const { return currentTime; } 
    const Box& getBox() const { return box; }
    const std::vector<Particle>& getParticles() const { return particles; }
    const std::vector<Obstacle>& getObstacles() const { return obstacles; }
};
