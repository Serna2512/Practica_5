#ifndef ARCHIVO_H
#define ARCHIVO_H

#include <fstream>
#include <string>
#include <vector>
#include "Particula.h"
using namespace std;

enum TipoColision { PARED, OBSTACULO, PARTICULA };

class Archivo {
private:
    ofstream archivo;
    string nombreArchivo;

public:
    Archivo(string nombre);
    ~Archivo();

    bool abrirArchivo();
    void guardarPosiciones(float t, const vector<Particula>& particulas);
    // idB = 0 si es pared, o el id del obstáculo/partícula
    void guardarColision(float t, TipoColision tipo, int idA, int idB, float x, float y);
    void cerrarArchivo();
};

#endif