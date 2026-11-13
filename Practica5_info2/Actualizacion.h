#ifndef ACTUALIZACION_H
#define ACTUALIZACION_H

using namespace std;
#include <vector>
#include "Particula.h"
#include "Obstaculo.h"
#include "Archivo.h"

enum Lado {
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
    NINGUNO
};

class Actualizacion {
private:
    float tiempo;
    float deltaT;
    unsigned int paso;
    float tiempoTotal;
    float coefRestitucion;
    float anchoCaja;
    float altoCaja;

    vector<Particula> particulas;
    vector<Obstaculo> obstaculos;
    Archivo archivo;

    // Métodos privados de lógica
    void actualizarPosiciones();
    bool hayColisionPared(const Particula& par, Lado& lado);
    bool hayColisionObstaculo(const Particula& par, const Obstaculo& obs, Lado& lado);
    bool hayColisionParticulas(const Particula& par1, const Particula& par2);

    void resolverColisionPared(Particula& par, Lado lado);
    void resolverColisionObstaculo(Particula& par, const Obstaculo& obs, Lado lado);
    void resolverColisionParticulas(int i, int j);

public:
    Actualizacion(float ancho, float alto, float dt, float coef);

    void agregarParticula(const Particula& p);
    void agregarObstaculo(const Obstaculo& o);
    void iniciarSimulacion();
    void ejecutarSimulacion();
    void ejecutarPaso();
};

#endif // ACTUALIZACION_H