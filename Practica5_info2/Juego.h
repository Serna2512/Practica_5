#ifndef JUEGO_H
#define JUEGO_H

#include <memory>
#include <string>
#include <vector>
#include "Jugador.h"
#include "Obstaculo.h"
#include "Proyectil.h"

enum class EstadoJuego { APUNTANDO, VOLANDO, TERMINADO };

// Lógica del juego + física. NO usa Qt: la ventana solo dibuja lo que esta clase calcula.

class Juego {
public:
    //
    static constexpr float ANCHO = 100.0;   //que es static constexpr?
    static constexpr float ALTO = 50.0;
    static constexpr float GRAVEDAD = 9.8;
    static constexpr float COEF_REST = 0.5;
    static constexpr float FACTOR_DANIO = 1.0;
    static constexpr float MASA_PROY = 2.0;
    static constexpr float RADIO_PROY = 0.8;
    static constexpr float LARGO_CANON = 3.5;
    static constexpr float UMBRAL_IMPACTO = 1.0;
    static constexpr int MAX_IMPACTOS = 3;
    static constexpr float TIEMPO_MAX_VUELO = 12.0;

    Juego();
    void reiniciar();

    // Inicia el turno actual: angulo en grados (0 = horizontal, 90 = vertical), velocidad en m/s.
    void disparar(float anguloGrados, float velocidad);
    // Avanza UN paso de tiempo de la simulación (dt pequeño, p. ej. 1/240 s).
    void avanzar(float dt);

    EstadoJuego getEstado() const;
    int getTurno() const;
    int getGanador() const;
    const string& getMensaje() const;
    const vector<Obstaculo>& getObstaculos() const;
    const vector<Jugador>& getJugadores() const;
    const Proyectil* getProyectil() const;   // nullptr si no hay proyectil en vuelo

private:
    vector<Obstaculo> obstaculos;
    vector<Jugador> jugadores;
    unique_ptr<Proyectil> proyectil;
    EstadoJuego estado;
    int turno;
    int ganador;
    float tiempoVuelo;
    int impactos;
    string mensaje;

    void colisionParedes();
    void colisionObstaculos();
    void colisionRepresentante();
    void terminarTurno();

    // Prueba círculo (proyectil) contra rectángulo. Si chocan devuelve true y entrega:
    //  (nx, ny) = normal unitaria que sale del rectángulo hacia el círculo,
    //  (cx, cy) = punto del borde del rectángulo donde ocurre el contacto.
    static bool contactoCirculoRect(const Proyectil& p, const Obstaculo& o, float& nx, float& ny, float& cx, float& cy);
};

#endif // JUEGO_H
