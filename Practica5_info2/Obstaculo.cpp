#include "Obstaculo.h"

Obstaculo::Obstaculo(float x, float y, float ancho, float alto)
    : Entidad(x, y), ancho(ancho), alto(alto) {}

float Obstaculo::getAncho() const {
    return ancho;
}
float Obstaculo::getAlto() const {
    return alto;
}