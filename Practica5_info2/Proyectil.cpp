#include "Proyectil.h"

Proyectil::Proyectil(float x, float y, float vx, float vy, float masa, float radio, int duenio)
    : Particula(x, y, vx, vy, masa, radio, duenio) {}

void Proyectil::avanzar(float dt, float g) {
    // Ecuaciones (1) y (2) de la guía, pero con y positivo hacia abajo (por eso suma g):
    //   x  = x0 + vx*dt
    //   y  = y0 + vy*dt + 1/2 g dt^2
    //   vy = vy + g*dt        (vx no cambia: MRU en x, caída libre en y)
    setPosX(getPosX() + getVelX() * dt);
    setPosY(getPosY() + getVelY() * dt + 0.5 * g * dt * dt);
    setVelY(getVelY() + g * dt);
}

int Proyectil::getDuenio() const { return getId(); }
