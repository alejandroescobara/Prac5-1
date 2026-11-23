#pragma once

#include <QVector2D>
#include <cstdint>

class Obstacle {
  private:
    uint32_t id;
    QVector2D position;
    float size;
  
  public:
    static constexpr float RESTITUTION_COEFF = 0.8f;

    explicit Obstacle(const QVector2D& pos, float size) 
                      :position(pos), size(size) {}

    uint32_t getId() const { return id; }
    void setId(uint32_t newId) { id = newId; }

    QVector2D getPosition() const { return position; }

    float getSize() const { return size; }

    static constexpr float getRestitutionCoeff() { return RESTITUTION_COEFF; }

    float getMinX() const { return position.x(); }
    float getMaxX() const { return position.x() + size; }
    float getMinY() const { return position.y(); }
    float getMaxY() const { return position.y() + size; }
};
