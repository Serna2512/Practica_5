#include "Jugador.h"

Jugador::Jugador(const string& nombre, float canonX, float canonY, int direccion, const Obstaculo& representante)
    : nombre(nombre), canonX(canonX), canonY(canonY), direccion(direccion),
    representante(representante) {}

const string& Jugador::getNombre() const { return nombre; }
float Jugador::getCanonX() const { return canonX; }
float Jugador::getCanonY() const { return canonY; }
int Jugador::getDireccion() const { return direccion; }
const Obstaculo& Jugador::getRepresentante() const { return representante; }
