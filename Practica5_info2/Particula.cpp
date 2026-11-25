using namespace std;
#include "Particula.h"
#include <cmath>


Particula::Particula(float x, float y, float vx, float vy, float m, float r, int id)
    : Entidad(x,y), id(id), masa(m), radio(r), velX(vx), velY(vy) {}

void Particula::actualizarPosicion(float deltaT) {
    // Ecuaciones de movimiento rectilíneo (la gravedad se manejaría en la simulación si aplica)
    posX += velX * deltaT;
    posY += velY * deltaT;
}

void Particula::fusionar(Particula* otra) {
    // Conservación del momento lineal: m1*v1 + m2*v2 = (m1+m2)*v_final
    float nuevaMasa = this->masa + otra->masa;

    // La partícula fusionada nace en el CENTRO DE MASA de las dos que chocaron:
    // x_cm = (m1*x1 + m2*x2) / (m1 + m2)   (igual para y)
    float nuevaX = (this->masa * this->posX + otra->masa * otra->posX) / nuevaMasa;
    float nuevaY = (this->masa * this->posY + otra->masa * otra->posY) / nuevaMasa;
    this->posX = nuevaX;
    this->posY = nuevaY;

    // Calcular nuevas velocidades
    float nuevaVelX = (this->masa * this->velX + otra->masa * otra->velX) / nuevaMasa;
    float nuevaVelY = (this->masa * this->velY + otra->masa * otra->velY) / nuevaMasa;

    // Actualizar propiedades de esta partícula (la que sobrevive)
    this->masa = nuevaMasa;
    this->velX = nuevaVelX;
    this->velY = nuevaVelY;

    // El radio podría aumentar según el área: A_total = A1 + A2 -> pi*r^2 = pi*r1^2 + pi*r2^2
    this->radio = sqrt(this->radio * this->radio + otra->radio * otra->radio);
}

int Particula::getId() const {
    return id;
}
float Particula::getMasa() const {
    return masa;
}
float Particula::getRadio() const {
    return radio;
}
float Particula::getVelX() const {
    return velX;
}
float Particula::getVelY() const {
    return velY;
}

void Particula::setVelX(float vx) {
    velX = vx;
}
void Particula::setVelY(float vy) {
    velY = vy;
}
void Particula::setMasa(float m) {
    masa = m;
}
void Particula::setRadio(float r) {
    radio = r;
}
