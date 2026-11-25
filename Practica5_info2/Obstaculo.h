#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "Entidad.h"

// Rectángulo estático (x, y = esquina superior izquierda).

//si el rectangulo tiene dueño 0 es el jugador 1, si el dueño es 1 el rectangulo pertenece a este
class Obstaculo : public Entidad {
private:
    float ancho;
    float alto;
    float resistencia;      // vida actual
    float resistenciaMax;   // vida inicial (para dibujar el desgaste)
    int duenio;             // a qué jugador pertenece (-1 = ninguno)
    bool destruido;

public:
    Obstaculo(float x, float y, float ancho, float alto, float resistencia = 0.0f, int duenio = -1);

    float getAncho() const;
    float getAlto() const;
    float getResistencia() const;
    float getResistenciaMax() const;
    int getDuenio() const;
    bool estaDestruido() const;

    // Resta daño a la resistencia; si llega a 0 el bloque queda destruido.
    void recibirDanio(float danio);
};

#endif // OBSTACULO_H
