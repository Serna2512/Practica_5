#include "Actualizacion.h"
#include <cmath>
#include <iostream>
using namespace std;

Actualizacion::Actualizacion(float ancho, float alto, float dt, float coef)
    : tiempo(0), deltaT(dt), paso(0), tiempoTotal(0), coefRestitucion(coef),
    anchoCaja(ancho), altoCaja(alto), archivo("simulacion.txt") {}

void Actualizacion::agregarParticula(const Particula& p) {
    particulas.push_back(p);
}

void Actualizacion::agregarObstaculo(const Obstaculo& o) {
    obstaculos.push_back(o);
}

void Actualizacion::iniciarSimulacion() {
    archivo.abrirArchivo();
    tiempo = 0;
    paso = 0;
}

void Actualizacion::ejecutarSimulacion() {
    // Ejemplo: simular 10 segundos
    float duracion = 10.0;
    while (tiempo < duracion) {
        ejecutarPaso();
    }
    archivo.cerrarArchivo();
}

void Actualizacion::ejecutarPaso() {
    // 1. Actualizar posiciones
    actualizarPosiciones();

    // 2. Verificar colisiones con paredes
    for (auto& p : particulas) {
        Lado lado;
        if (hayColisionPared(p, lado)) {
            resolverColisionPared(p, lado);
            archivo.guardarColision(tiempo, PARED, p.getId(), -1, p.getPosX(), p.getPosY());
        }
    }

    // 3. Verificar colisiones con obstáculos
    for (auto& p : particulas) {
        for (const auto& obs : obstaculos) {
            Lado lado;
            if (hayColisionObstaculo(p, obs, lado)) {
                resolverColisionObstaculo(p, obs, lado);
                archivo.guardarColision(tiempo, OBSTACULO, p.getId(), -1, p.getPosX(), p.getPosY());
            }
        }
    }

    // 4. Verificar colisiones entre partículas (requiere iterar con cuidado si se eliminan)

    vector<bool> eliminada(particulas.size(), false);

    for (size_t i = 0; i < particulas.size(); ++i) {
        if (eliminada[i]) continue;
        for (size_t j = i + 1; j < particulas.size(); ++j) {
            if (eliminada[j]) continue;

            if (hayColisionParticulas(particulas[i], particulas[j])) {
                // Fusión completamente inelástica
                particulas[i].fusionar(&particulas[j]);
                eliminada[j] = true; // La partícula j desaparece

                archivo.guardarColision(tiempo, PARTICULA, particulas[i].getId(), particulas[j].getId(), particulas[i].getPosX(), particulas[i].getPosY());
            }
        }
    }

    // Eliminar partículas fusionadas del vector

    vector<Particula> sobrevivientes;
    for (size_t i = 0; i < particulas.size(); ++i) {
        if (!eliminada[i]) sobrevivientes.push_back(particulas[i]);
    }
    particulas = sobrevivientes;

    // 5. Guardar estado
    archivo.guardarPosiciones(tiempo, particulas);

    // 6. Avanzar tiempo
    tiempo += deltaT;
    paso++;
}

void Actualizacion::actualizarPosiciones() {
    for (auto& p : particulas) {
        p.actualizarPosicion(deltaT);
    }
}

bool Actualizacion::hayColisionPared(const Particula& par, Lado& lado) {
    float r = par.getRadio();
    float x = par.getPosX();
    float y = par.getPosY();

    if (x - r < 0) { lado = IZQUIERDA; return true; }
    if (x + r > anchoCaja) { lado = DERECHA; return true; }
    if (y - r < 0) { lado = ARRIBA; return true; } // Asumiendo origen arriba-izquierda
    if (y + r > altoCaja) { lado = ABAJO; return true; }

    return false;
}

void Actualizacion::resolverColisionPared(Particula& par, Lado lado) {
    // Colisión perfectamente elástica: invertir componente perpendicular
    if (lado == IZQUIERDA) {
        par.setPosX(par.getRadio()); // Corregir posición
        par.setVelX(-par.getVelX());
    } else if (lado == DERECHA) {
        par.setPosX(anchoCaja - par.getRadio());
        par.setVelX(-par.getVelX());
    } else if (lado == ARRIBA) {
        par.setPosY(par.getRadio());
        par.setVelY(-par.getVelY());
    } else if (lado == ABAJO) {
        par.setPosY(altoCaja - par.getRadio());
        par.setVelY(-par.getVelY());
    }
}

bool Actualizacion::hayColisionObstaculo(const Particula& par, const Obstaculo& obs, Lado& lado) {

    //datos de la particula
    float pX = par.getPosX();
    float pY = par.getPosY();
    float r = par.getRadio();

    //datos del obstaculo
    float oX = obs.getPosX();
    float oY = obs.getPosY();
    float oW = obs.getAncho();
    float oH = obs.getAlto();

    // Verificar si hay superposición
    if (pX + r > oX && pX - r < oX + oW &&
        pY + r > oY && pY - r < oY + oH) {

        float centroX = oX + oW / 2.0; //aproximacion de la posicion
        float centroY = oY + oH / 2.0;

        float dx = pX - centroX;
        float dy = pY - centroY;

        if (abs(dx) > abs(dy)) {
            lado = (dx > 0) ? DERECHA : IZQUIERDA;
        } else {
            lado = (dy > 0) ? ABAJO : ARRIBA;
        }
        return true;
    }
    return false;
}

void Actualizacion::resolverColisionObstaculo(Particula& par, const Obstaculo& obs, Lado lado) {
    // Colisión inelástica con coeficiente de restitución
    // v_perp = -e * v_perp, cambia la dirección y magnitud de la velocidad de la particula

    if (lado == IZQUIERDA || lado == DERECHA) {
        par.setVelX(-coefRestitucion * par.getVelX());
        // Reposicionar para evitar atasco
        if (lado == IZQUIERDA) par.setPosX(obs.getPosX() - par.getRadio());
        else par.setPosX(obs.getPosX() + obs.getAncho() + par.getRadio());
    } else {
        par.setVelY(-coefRestitucion * par.getVelY());
        if (lado == ARRIBA) par.setPosY(obs.getPosY() - par.getRadio());
        else par.setPosY(obs.getPosY() + obs.getAlto() + par.getRadio());
    }
}

bool Actualizacion::hayColisionParticulas(const Particula& par1, const Particula& par2) {
    float dx = par1.getPosX() - par2.getPosX();
    float dy = par1.getPosY() - par2.getPosY();
    float dist = sqrt(dx*dx + dy*dy);
    return dist < (par1.getRadio() + par2.getRadio());
}

void Actualizacion::resolverColisionParticulas(int i, int j) {
    // La lógica de fusión ya está en Particula::fusionar, llamada desde ejecutarPaso
    // Esta función queda vacía o se usa si se requiere lógica adicional antes de fusionar
}