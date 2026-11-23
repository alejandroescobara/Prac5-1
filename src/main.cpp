#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <QVector2D>

#include "DataManager.hpp"
#include "Obstacle.hpp"
#include "Particle.hpp"
#include "Simulator.hpp"

int main() {

  const std::string ARCHIVO_SALIDA = "simulacion.txt";

  const float ANCHO_CAJA   = 100.0f;
  const float ALTO_CAJA    = 100.0f;
  const float TIEMPO_TOTAL = 3.0f;   // Segundos a simular

  // Obstaculos: Obstacle(QVector2D(x, y), tamano_lado)
  std::vector<Obstacle> obstaculosIniciales = {
    Obstacle(QVector2D(40.0f, 40.0f), 15.0f),
    Obstacle(QVector2D(70.0f, 20.0f), 10.0f)
  };

  // Particulas: Particle(QVector2D(x, y), QVector2D(vx, vy), masa, radio)
  std::vector<Particle> particulasIniciales = {
    Particle(QVector2D(20.0f, 80.0f), QVector2D( 25.0f,  -2.0f), 5.0f, 3.0f),
    Particle(QVector2D(55.0f, 78.0f), QVector2D(-20.0f,   0.0f), 3.0f, 2.5f),
    Particle(QVector2D(15.0f, 47.0f), QVector2D( 35.0f,   3.0f), 4.0f, 2.5f),
    Particle(QVector2D(15.0f, 15.0f), QVector2D(-28.0f, -22.0f), 2.0f, 2.0f),
    Particle(QVector2D(85.0f, 65.0f), QVector2D(-18.0f, -25.0f), 3.5f, 2.2f)
  };

  Box box{ANCHO_CAJA, ALTO_CAJA};
  DataManager logger(ARCHIVO_SALIDA);
  Simulator sim(box, logger);

  std::cout << "=== CARGANDO ESCENARIO ===\n";

  for (std::size_t i = 0; i < obstaculosIniciales.size(); ++i) {
    const std::size_t previos = sim.getObstacles().size();
    sim += obstaculosIniciales[i];

    if (sim.getObstacles().size() == previos) {
      std::cerr << "[AVISO] Obstaculo #" << i << " rechazado por += (fuera de caja o superpuesto).\n";
    }
  }

  for (std::size_t i = 0; i < particulasIniciales.size(); ++i) {
    const std::size_t previas = sim.getParticles().size();
    sim += particulasIniciales[i];

    if (sim.getParticles().size() == previas) {
      std::cerr << "[AVISO] Particula #" << i << " rechazada por += (fuera de caja o superpuesta).\n";
    }
  }

  std::cout << "Obstaculos cargados: " << sim.getObstacles().size() << "/" << obstaculosIniciales.size() << "\n";
  std::cout << "Particulas cargadas: " << sim.getParticles().size() << "/" << particulasIniciales.size() << "\n\n";

  if (sim.getParticles().empty()) {
    std::cerr << "ERROR: No hay particulas validas para simular.\n";
    return 1;
  }

  const int totalPasos = static_cast<int>(std::round(TIEMPO_TOTAL / Simulator::TIME_STEP));
  for (int paso = 0; paso < totalPasos; ++paso) {
    sim.step();
  }
  logger.flush();

  int sobrevivientes = 0;
  for (const auto& p : sim.getParticles()) {
    if (p.getIsActive()) ++sobrevivientes;
  }

  int eventosColision = 0;
  std::ifstream inFile(ARCHIVO_SALIDA);
  std::string linea;
  while (std::getline(inFile, linea)) {
    if (linea.rfind("C ", 0) == 0) {
      ++eventosColision;
    }
  }

  std::cout << "=== SIMULACION FINALIZADA ===\n";
  std::cout << "Tiempo simulado:         " << sim.getCurrentTime() << " s (" << totalPasos << " pasos)\n";
  std::cout << "Particulas al final:     " << sobrevivientes << " activas\n";
  std::cout << "Colisiones registradas:  " << eventosColision << "\n";
  std::cout << "Archivo generado:        " << ARCHIVO_SALIDA << "\n";

  return 0;
}
