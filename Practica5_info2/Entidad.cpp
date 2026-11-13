#include "Entidad.h"

Entidad::Entidad(float x, float y) : posX(x), posY(y) {}

float Entidad::getPosX() const {
    return posX;
}

float Entidad::getPosY() const {
    return posY;
}

void Entidad::setPosX(float x) {
    posX = x;
}

void Entidad::setPosY(float y) {
    posY = y;
}