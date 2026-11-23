#include "Simulator.hpp"


#include <cstdint>
#include <algorithm>
#include <cmath>
#include <cstddef>

#include "DataManager.hpp"

namespace {

  // Partícula - Caja
  bool checkOverlap(const QVector2D& center, float radius, const Box& b, QVector2D& contactPoint) {
    float cx = std::clamp(center.x(), 0.0f, b.width);
    float cy = std::clamp(center.y(), 0.0f, b.height);
    bool hit = false;

    if (center.x() < radius) {
      cx = 0.0f;
      hit = true;
    } else if (center.x() > b.width - radius) {
      cx = b.width;
      hit = true;
    }

    if (center.y() < radius) {
      cy = 0.0f;
      hit = true;
    } else if (center.y() > b.height - radius) {
      cy = b.height;
      hit = true;
    }

    if (hit) {
      contactPoint = QVector2D(cx, cy);
    }
    return hit;
  }

  // Partícula - Obstáculo
  bool checkOverlap(const QVector2D& center, float radius,
                    float minX, float maxX, float minY, float maxY,
                    QVector2D& contactPoint) {
    QVector2D closest(std::clamp(center.x(), minX, maxX),
                      std::clamp(center.y(), minY, maxY));

    float distSq = (center - closest).lengthSquared();
    if (distSq > radius * radius) {
      return false;
    }

    // Si el centro quedó dentro del obstáculo, proyectar a la cara más cercana
    if (distSq == 0.0f) {
      float distLeft   = center.x() - minX;
      float distRight  = maxX - center.x();
      float distBottom = center.y() - minY;
      float distTop    = maxY - center.y();

      float minDist = std::min({distLeft, distRight, distBottom, distTop});
      if (minDist == distLeft)        contactPoint = QVector2D(minX, center.y());
      else if (minDist == distRight)  contactPoint = QVector2D(maxX, center.y());
      else if (minDist == distBottom) contactPoint = QVector2D(center.x(), minY);
      else                            contactPoint = QVector2D(center.x(), maxY);
    } else {
      contactPoint = closest;
    }

    return true;
  }

  // Partícula - Partícula
  bool checkOverlap(const QVector2D& c1, float r1, const QVector2D& c2, float r2, QVector2D& contactPoint) {
    float minDist = r1 + r2;
    if (minDist <= 0.0f) return false;

    QVector2D diff = c2 - c1;
    if (diff.lengthSquared() > minDist * minDist) return false;

    contactPoint = c1 + diff * (r1 / minDist);
    return true;
  }

  // Obstáculo - Caja
  bool checkOverlap(float minX, float maxX, float minY, float maxY, const Box& b, QVector2D& contactPoint) {
    float cx = std::clamp((minX + maxX) * 0.5f, 0.0f, b.width);
    float cy = std::clamp((minY + maxY) * 0.5f, 0.0f, b.height);
    bool hit = false;

    if (minX < 0.0f)          { cx = 0.0f;     hit = true; }
    else if (maxX > b.width)  { cx = b.width;  hit = true; }

    if (minY < 0.0f)          { cy = 0.0f;     hit = true; }
    else if (maxY > b.height) { cy = b.height; hit = true; }

    if (hit) {
      contactPoint = QVector2D(cx, cy);
    }
    return hit;
  }

  // Obstáculo - Obstáculo
  bool checkOverlap(float aMinX, float aMaxX, float aMinY, float aMaxY,
                    float bMinX, float bMaxX, float bMinY, float bMaxY,
                    QVector2D& contactPoint) {
    if (!(aMinX < bMaxX && aMaxX > bMinX && aMinY < bMaxY && aMaxY > bMinY)) {
      return false;
    }

    contactPoint = QVector2D(
      (std::max(aMinX, bMinX) + std::min(aMaxX, bMaxX)) * 0.5f,
      (std::max(aMinY, bMinY) + std::min(aMaxY, bMaxY)) * 0.5f
    );
    return true;
  }

} // namespace

// 1. Partícula - Caja
bool Simulator::isCollision(const Particle& p, QVector2D& contactPoint) const {
  return checkOverlap(p.getPosition(), p.getRadius(), box, contactPoint);
}

// 2. Partícula - Obstáculo
bool Simulator::isCollision(const Particle& p, const Obstacle& obs, QVector2D& contactPoint) const {
  return checkOverlap(p.getPosition(), p.getRadius(),
                      obs.getMinX(), obs.getMaxX(),
                      obs.getMinY(), obs.getMaxY(),
                      contactPoint);
}

// 3. Partícula - Partícula
bool Simulator::isCollision(const Particle& p1, const Particle& p2, QVector2D& contactPoint) const {
  return checkOverlap(p1.getPosition(), p1.getRadius(),
                      p2.getPosition(), p2.getRadius(),
                      contactPoint);
}

bool Simulator::isValidParticle(const QVector2D& center, float radius) const {
  if (radius <= 0.0f || 2.0f * radius > box.width || 2.0f * radius > box.height) {
    return false;
  }

  QVector2D pt;

  if (checkOverlap(center, radius, box, pt)) return false;

  for (const auto& obs : obstacles) {
    if (checkOverlap(center, radius, obs.getMinX(), obs.getMaxX(), obs.getMinY(), obs.getMaxY(), pt)) {
      return false;
    }
  }

  for (const auto& p : particles) {
    if (p.getIsActive() && checkOverlap(center, radius, p.getPosition(), p.getRadius(), pt)) {
      return false;
    }
  }

  return true;
}

bool Simulator::isValidObstacle(const QVector2D& pos, float size) const {
  if (size <= 0.0f) return false;

  float minX = pos.x();
  float maxX = minX + size;
  float minY = pos.y();
  float maxY = minY + size;
  QVector2D pt;

  if (checkOverlap(minX, maxX, minY, maxY, box, pt)) return false;

  for (const auto& obs : obstacles) {
    if (checkOverlap(minX, maxX, minY, maxY, obs.getMinX(), obs.getMaxX(), obs.getMinY(), obs.getMaxY(), pt)) {
      return false;
    }
  }

  for (const auto& p : particles) {
    if (p.getIsActive() && checkOverlap(p.getPosition(), p.getRadius(), minX, maxX, minY, maxY, pt)) {
      return false;
    }
  }

  return true;
}

Simulator& Simulator::operator+=(Particle& newParticle) {
  if (!isValidParticle(newParticle.getPosition(), newParticle.getRadius())) {
    return *this;
  }
  newParticle.setId(static_cast<uint32_t>(particles.size()));
  particles.push_back(newParticle);
  return *this;
}

Simulator& Simulator::operator+=(Obstacle& newObstacle) {
  float size = newObstacle.getMaxX() - newObstacle.getMinX();
  QVector2D pos(newObstacle.getMinX(), newObstacle.getMinY());

  if (!isValidObstacle(pos, size)) {
    return *this;
  }
  newObstacle.setId(static_cast<uint32_t>(obstacles.size()));
  obstacles.push_back(newObstacle);
  return *this;
}

void Simulator::resolveWallCollisions() {
  const Wall walls[4] = {
    { QVector2D(0.0f, 0.0f),       QVector2D( 1.0f,  0.0f) }, // Izquierda
    { QVector2D(box.width, 0.0f),  QVector2D(-1.0f,  0.0f) }, // Derecha
    { QVector2D(0.0f, 0.0f),       QVector2D( 0.0f,  1.0f) }, // Inferior
    { QVector2D(0.0f, box.height), QVector2D( 0.0f, -1.0f) }  // Superior
  };

  for (auto& p : particles) {
    if (!p.getIsActive()) continue;

    QVector2D dummyPt;
    if (!isCollision(p, dummyPt)) continue;

    QVector2D pos = p.getPosition();
    QVector2D vel = p.getVelocity();
    float r = p.getRadius();

    for (const auto& wall : walls) {
      float d = QVector2D::dotProduct(pos - wall.point, wall.normal);

      if (d < r) {
        // Punto de impacto exacto sobre el segmento de la pared
        QVector2D contactPoint = pos - d * wall.normal;
        contactPoint.setX(std::clamp(contactPoint.x(), 0.0f, box.width));
        contactPoint.setY(std::clamp(contactPoint.y(), 0.0f, box.height));

        // Desincrustar
        pos += (r - d) * wall.normal;
        p.setPosition(pos);

        // Rebote elástico y registro solo si realmente impacta contra la pared
        float vDotN = QVector2D::dotProduct(vel, wall.normal);
        if (vDotN < 0.0f) {
          vel -= 2.0f * vDotN * wall.normal;
          p.setVelocity(vel);
          logger.logCollision(currentTime, p, contactPoint);
        }
      }
    }
  }
}

void Simulator::resolveObstacleCollisions() {
  for (auto& p : particles) {
    if (!p.getIsActive()) continue;

    for (const auto& obs : obstacles) {
      QVector2D contactPoint;
      if (!isCollision(p, obs, contactPoint)) continue;

      QVector2D pos = p.getPosition();
      QVector2D vel = p.getVelocity();
      float r = p.getRadius();

      QVector2D diff = pos - contactPoint;
      float dist = diff.length();

      bool centerInside = (pos.x() >= obs.getMinX() && pos.x() <= obs.getMaxX() &&
                           pos.y() >= obs.getMinY() && pos.y() <= obs.getMaxY());

      QVector2D normal;
      float penetration = 0.0f;

      if (centerInside || dist <= 1e-6f) {
        // Fallback para tunelado profundo: empujar hacia la cara más cercana
        float distLeft   = std::abs(pos.x() - obs.getMinX());
        float distRight  = std::abs(obs.getMaxX() - pos.x());
        float distBottom = std::abs(pos.y() - obs.getMinY());
        float distTop    = std::abs(obs.getMaxY() - pos.y());

        float minDist = std::min({distLeft, distRight, distBottom, distTop});
        if (minDist == distLeft)        normal = QVector2D(-1.0f,  0.0f);
        else if (minDist == distRight)  normal = QVector2D( 1.0f,  0.0f);
        else if (minDist == distBottom) normal = QVector2D( 0.0f, -1.0f);
        else                            normal = QVector2D( 0.0f,  1.0f);

        penetration = r + minDist;
      } else {
        // Caso general (caras y esquinas): normal real desde el punto de contacto
        normal = diff / dist;
        penetration = r - dist;
      }

      float vDotN = QVector2D::dotProduct(vel, normal);

      // Ignorar contacto tangente si ya se está alejando
      if (vDotN >= 0.0f && penetration <= 0.0f) continue;

      if (penetration > 0.0f) {
        pos += normal * penetration;
        p.setPosition(pos);
      }

      // Impulso con coeficiente de restitución e y registro solo si se aproxima
      if (vDotN < 0.0f) {
        float e = obs.getRestitutionCoeff();
        vel -= (1.0f + e) * vDotN * normal;
        p.setVelocity(vel);
        logger.logCollision(currentTime, p, obs, contactPoint);
      }
    }
  }
}

void Simulator::resolveParticleCollisions() {
  const std::size_t n = particles.size();

  for (std::size_t i = 0; i < n; ++i) {
    if (!particles[i].getIsActive()) continue;

    for (std::size_t j = i + 1; j < n; ++j) {
      if (!particles[j].getIsActive()) continue;

      QVector2D contactPoint;
      if (isCollision(particles[i], particles[j], contactPoint)) {
        logger.logCollision(currentTime, particles[i], particles[j], contactPoint);
        particles[i].absorb(particles[j]);

        if (!particles[i].getIsActive()) break;
      }
    }
  }
}

void Simulator::step() {
  if (!scenarioLogged) {
    logger.logScenario(box, obstacles);
    logger.logPositions(currentTime, particles);
    scenarioLogged = true;
  }

  for (auto& p : particles) {
    if (p.getIsActive()) {
      p.update(TIME_STEP);
    }
  }

  currentTime += TIME_STEP;

  resolveParticleCollisions();
  resolveObstacleCollisions();
  resolveWallCollisions();

  logger.logPositions(currentTime, particles);
}
