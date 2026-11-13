#include "Archivo.h"
#include <iostream>

Archivo::Archivo(std::string nombre) : nombreArchivo(nombre) {}

Archivo::~Archivo() {
    cerrarArchivo();
}

bool Archivo::abrirArchivo() {
    archivo.open(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error al abrir el archivo: " << nombreArchivo << std::endl;
        return false;
    }
    return true;
}

void Archivo::escribirEncabezado() {
    if (archivo.is_open()) {
        archivo << "TIEMPO,POSICIONES_X_Y" << std::endl;
    }
}

void Archivo::guardarPosiciones(float t, const std::vector<Particula>& particulas) {
    if (!archivo.is_open()) return;

    archivo << t;
    for (const auto& p : particulas) {
        archivo << "," << p.getPosX() << "," << p.getPosY();
    }
    archivo << std::endl;
}

void Archivo::guardarColision(float t, TipoColision tipo, int idA, int idB, float x, float y) {
    if (!archivo.is_open()) return;

    std::string tipoStr;
    switch(tipo) {
    case PARED: tipoStr = "PARED"; break;
    case OBSTACULO: tipoStr = "OBSTACULO"; break;
    case PARTICULA: tipoStr = "PARTICULA"; break;
    }

    archivo << "COLISION," << t << "," << tipoStr << "," << idA << "," << idB << "," << x << "," << y << std::endl;
}

void Archivo::cerrarArchivo() {
    if (archivo.is_open()) {
        archivo.close();
    }
}