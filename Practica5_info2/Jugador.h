#ifndef JUGADOR_H
#define JUGADOR_H

#include <string>
#include "Obstaculo.h"

// Un jugador: su cañón (pivote), hacia dónde dispara y su representante (el "Rival" del enunciado)

class Jugador {
private:
    string nombre;
    float canonX, canonY;   // pivote del cañón (m)
    int direccion;          // +1 dispara hacia la derecha, -1 hacia la izquierda
    Obstaculo representante;

public:
    Jugador(const string& nombre, float canonX, float canonY, int direccion, const Obstaculo& representante);

    const string& getNombre() const;
    float getCanonX() const;
    float getCanonY() const;
    int getDireccion() const;
    const Obstaculo& getRepresentante() const;
};

#endif // JUGADOR_H
