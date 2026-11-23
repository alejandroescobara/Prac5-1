#include "DataManager.hpp"

#include <iomanip>
#include <iostream>

DataManager::DataManager(const std::string& positionsPath,
                         const std::string& collisionsPath) {
  positionsFile.open(positionsPath);
  if (positionsFile.is_open()) {
    positionsFile << std::fixed << std::setprecision(4);
    positionsFile << "tiempo,id_particula,x,y\n";
  } else {
    std::cerr << "Error: no se pudo abrir " << positionsPath << "\n";
  }

  collisionsFile.open(collisionsPath);
  if (collisionsFile.is_open()) {
    collisionsFile << std::fixed << std::setprecision(4);
    collisionsFile << "tiempo,tipo,id_1,id_2,contacto_x,contacto_y\n";
  } else {
    std::cerr << "Error: no se pudo abrir " << collisionsPath << "\n";
  }
}

DataManager::~DataManager() {
  if (positionsFile.is_open()) {
    positionsFile.close();
  }
  if (collisionsFile.is_open()) {
    collisionsFile.close();
  }
}

void DataManager::logPositions(float time, const std::vector<Particle>& particles) {
  if (!positionsFile.is_open()) return;

  for (const auto& p : particles) {
    if (!p.getIsActive()) continue;

    positionsFile << time << ","
                  << p.getId() << ","
                  << p.getPosition().x() << ","
                  << p.getPosition().y() << "\n";
  }
}

void DataManager::logCollision(float time, const Particle& p, const QVector2D& contactPoint) {
  if (!collisionsFile.is_open()) return;

  collisionsFile << time << ",PARED,"
                 << p.getId() << ",-1,"
                 << contactPoint.x() << ","
                 << contactPoint.y() << "\n";
}

void DataManager::logCollision(float time, const Particle& p, const Obstacle& obs, const QVector2D& contactPoint) {
  if (!collisionsFile.is_open()) return;

  collisionsFile << time << ",OBSTACULO,"
                 << p.getId() << ","
                 << obs.getId() << ","
                 << contactPoint.x() << ","
                 << contactPoint.y() << "\n";
}

void DataManager::logCollision(float time, const Particle& p1, const Particle& p2, const QVector2D& contactPoint) {
  if (!collisionsFile.is_open()) return;

  collisionsFile << time << ",PARTICULA,"
                 << p1.getId() << ","
                 << p2.getId() << ","
                 << contactPoint.x() << ","
                 << contactPoint.y() << "\n";
}

void DataManager::flush() {
  if (positionsFile.is_open()) positionsFile.flush();
  if (collisionsFile.is_open()) collisionsFile.flush();
}
