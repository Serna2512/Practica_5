#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "Particula.h"

// Proyectil = Particula (posición, velocidad, masa, radio) + gravedad.
// Reutiliza lo hecho en la parte 1. El "id" de la Particula guarda el jugador que disparó.
class Proyectil : public Particula {
public:
    Proyectil(float x, float y, float vx, float vy, float masa, float radio, int duenio);

    // Un paso de tiempo discreto con gravedad (eje y hacia ABAJO, como la pantalla).
    void avanzar(float dt, float g);
    int getDuenio() const;
};

#endif // PROYECTIL_H
