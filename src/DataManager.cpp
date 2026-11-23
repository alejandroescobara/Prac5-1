#include "DataManager.hpp"
#include "Simulator.hpp"

#include <iomanip>
#include <iostream>

DataManager::DataManager(const std::string& outputPath) {
  outFile.open(outputPath);
  if (outFile.is_open()) {
    outFile << std::fixed << std::setprecision(4);
    outFile << "# BOX width height\n";
    outFile << "# OBSTACLE id x y size restitution\n";
    outFile << "# T time id x y vx vy mass radius\n";
    outFile << "# C time type id1 id2 contact_x contact_y\n";
  } else {
    std::cerr << "Error: no se pudo abrir " << outputPath << "\n";
  }
}

DataManager::~DataManager() {
  if (outFile.is_open()) {
    outFile.close();
  }
}

void DataManager::logScenario(const Box& b, const std::vector<Obstacle>& obstacles) {
  if (!outFile.is_open()) return;

  outFile << "BOX " << b.width << " " << b.height << "\n";

  for (const auto& o : obstacles) {
    outFile << "OBSTACLE "
            << o.getId() << " "
            << o.getPosition().x() << " "
            << o.getPosition().y() << " "
            << o.getSize() << " "
            << o.getRestitutionCoeff() << "\n";
  }
}

void DataManager::logPositions(float time, const std::vector<Particle>& particles) {
  if (!outFile.is_open()) return;

  for (const auto& p : particles) {
    if (p.getIsActive()) {
      outFile << "T " << time << " "
              << p.getId() << " "
              << p.getPosition().x() << " " << p.getPosition().y() << " "
              << p.getVelocity().x() << " " << p.getVelocity().y() << " "
              << p.getMass() << " "
              << p.getRadius() << "\n";
    }
  }
}

void DataManager::logCollision(float time, const Particle& p, const QVector2D& contactPoint) {
  if (!outFile.is_open()) return;

  outFile << "C " << time << " PARED "
          << p.getId() << " -1 "
          << contactPoint.x() << " "
          << contactPoint.y() << "\n";
}

void DataManager::logCollision(float time, const Particle& p, const Obstacle& obs, const QVector2D& contactPoint) {
  if (!outFile.is_open()) return;

  outFile << "C " << time << " OBSTACULO "
          << p.getId() << " "
          << obs.getId() << " "
          << contactPoint.x() << " "
          << contactPoint.y() << "\n";
}

void DataManager::logCollision(float time, const Particle& p1, const Particle& p2, const QVector2D& contactPoint) {
  if (!outFile.is_open()) return;

  outFile << "C " << time << " PARTICULA "
          << p1.getId() << " "
          << p2.getId() << " "
          << contactPoint.x() << " "
          << contactPoint.y() << "\n";
}

void DataManager::flush() {
  if (outFile.is_open()) {
    outFile.flush();
  }
}
