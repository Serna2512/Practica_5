#ifndef ARCHIVO_H
#define ARCHIVO_H

using namespace std;
#include <fstream>
#include <string>
#include <vector>
#include "Particula.h"

// Enum para tipo de colisión según el diagrama
enum TipoColision {
    PARED,
    OBSTACULO,
    PARTICULA
};

// Enum para el lado de impacto
enum Lado {
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
    NINGUNO
};

class Archivo {

private:
    ofstream archivo;
    string nombreArchivo;

public:
    Archivo(string nombre);
    ~Archivo();

    bool abrirArchivo();
    void escribirEncabezado();
    void guardarPosiciones(float t, const vector<Particula>& particulas);
    void guardarColision(float t, TipoColision tipo, int idA, int idB, float x, float y);
    void cerrarArchivo();
};

#endif // ARCHIVO_H