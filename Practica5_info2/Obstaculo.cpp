#include "Obstaculo.h"

Obstaculo::Obstaculo(float x, float y, float ancho, float alto, float resistencia, int duenio)
    : Entidad(x, y), ancho(ancho), alto(alto), resistencia(resistencia),
    resistenciaMax(resistencia), duenio(duenio), destruido(false) {}

float Obstaculo::getAncho() const { return ancho; }
float Obstaculo::getAlto() const { return alto; }
float Obstaculo::getResistencia() const { return resistencia; }
float Obstaculo::getResistenciaMax() const { return resistenciaMax; }
int Obstaculo::getDuenio() const { return duenio; }
bool Obstaculo::estaDestruido() const { return destruido; }

void Obstaculo::recibirDanio(float danio) {
    resistencia -= danio;
    if (resistencia <= 0.0) {
        resistencia = 0.0;
        destruido = true;
    }
}
