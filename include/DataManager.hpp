#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <QVector2D>

#include "Particle.hpp"
#include "Obstacle.hpp"

struct Box;

class DataManager {
private:
  std::ofstream outFile;

public:
  explicit DataManager(const std::string& outputPath = "simulacion.txt");
  ~DataManager();

  void logScenario(const Box& b, const std::vector<Obstacle>& obstacles);

  void logPositions(float time, const std::vector<Particle>& particles);

  // Particula - Caja
  void logCollision(float time, const Particle& p, const QVector2D& contactPoint);

  // Particula - Obstaculo
  void logCollision(float time, const Particle& p, const Obstacle& obs, const QVector2D& contactPoint);

  // Particula - Particula
  void logCollision(float time, const Particle& p1, const Particle& p2, const QVector2D& contactPoint);

  void flush();
};
