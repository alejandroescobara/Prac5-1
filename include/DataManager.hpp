#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <QVector2D>

#include "Particle.hpp"
#include "Obstacle.hpp"

class DataManager {
private:
  std::ofstream positionsFile;
  std::ofstream collisionsFile;

public:
  explicit DataManager(const std::string& positionsPath = "posiciones.txt",
                       const std::string& collisionsPath = "colisiones.txt");
  ~DataManager();

  void logPositions(float time, const std::vector<Particle>& particles);

  // Particula - Caja
  void logCollision(float time, const Particle& p, const QVector2D& contactPoint);

  // Particula - Obstáculo
  void logCollision(float time, const Particle& p, const Obstacle& obs, const QVector2D& contactPoint);

  // Particula - Particula
  void logCollision(float time, const Particle& p1, const Particle& p2, const QVector2D& contactPoint);

  void flush();
};
