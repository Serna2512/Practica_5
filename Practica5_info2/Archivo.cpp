#include "Archivo.h"
#include <iostream>


Archivo::Archivo(string nombre) : nombreArchivo(nombre) {}

Archivo::~Archivo() {
    cerrarArchivo();
}

bool Archivo::abrirArchivo() {
    archivo.open(nombreArchivo);
    return archivo.is_open();
}

void Archivo::guardarPosiciones(float t, const vector<Particula>& particulas) {
    if (!archivo.is_open()) return;
    for (const auto& p : particulas) {
        // Formato: P;tiempo;id;x;y;radio
        archivo << "P;" << t << ";" << p.getId() << ";"
                << p.getPosX() << ";" << p.getPosY() << ";"
                << p.getRadio() << "\n";
    }
}

void Archivo::guardarColision(float t, TipoColision tipo, int idA, int idB, float x, float y) {
    if (!archivo.is_open()) return;

    string tipoStr;
    switch(tipo) {
    case PARED: tipoStr = "PARED"; break;
    case OBSTACULO: tipoStr = "OBSTACULO"; break;
    case PARTICULA: tipoStr = "PARTICULA"; break;
    }

    // Formato: C;tiempo;tipo;idA;idB;x;y
    archivo << "C;" << t << ";" << tipoStr << ";" << idA << ";"
            << idB << ";" << x << ";" << y << "\n";
}

void Archivo::cerrarArchivo() {
    if (archivo.is_open()) archivo.close();
}