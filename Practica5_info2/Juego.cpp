#include "Juego.h"
#include <algorithm>
#include <cmath>

static const float PI = 3.14159265358979f;

Juego::Juego() : estado(EstadoJuego::APUNTANDO), turno(0), ganador(-1), tiempoVuelo(0), impactos(0) {
    reiniciar();
}

void Juego::reiniciar() {
    obstaculos.clear();
    jugadores.clear();
    proyectil.reset();

    // Jugador 0 a la izquierda; la infraestructura del jugador 1 es su ESPEJO horizontal.
    for (int j = 0; j < 2; ++j) {
        // Convierte una x del lado izquierdo a la x equivalente en el lado derecho
        auto X = [j](float x, float w) { return j == 0 ? x : ANCHO - x - w; };

        obstaculos.emplace_back(X(5, 5),  38, 5, 12, 200, j);   // pilar exterior (200)
        obstaculos.emplace_back(X(15, 5), 38, 5, 12, 200, j);   // pilar interior (200)
        obstaculos.emplace_back(X(5, 15), 33, 15, 5, 100, j);   // techo (100)

        // Representante entre los pilares (rectángulo sin resistencia)
        Obstaculo rep(X(10, 5), 40, 5, 10, 0, j);
        jugadores.emplace_back(j == 0 ? "Jugador 1" : "Jugador 2",
                               j == 0 ? 3.0f : ANCHO - 3.0f, 29.0f, j == 0 ? 1 : -1, rep);
    }

    estado = EstadoJuego::APUNTANDO;
    turno = 0;
    ganador = -1;
    tiempoVuelo = 0;
    impactos = 0;
    mensaje = "Turno de Jugador 1";
}

// ---------------------------------------------------------------------------------
// Disparo: convierte (ángulo, rapidez) en componentes de velocidad.
//   vx = dir * v cos(a)        (dir = +1 o -1 según el lado del jugador)
//   vy = -v sin(a)             (negativo porque en pantalla "arriba" es y decreciente)
// El proyectil nace en la boca del cañón: pivote + LARGO_CANON * (dirección del cañón).
// ---------------------------------------------------------------------------------
void Juego::disparar(float anguloGrados, float velocidad) {
    if (estado != EstadoJuego::APUNTANDO) return;
    const Jugador& j = jugadores[turno];
    float a = anguloGrados * PI / 180.0f;

    float vx = j.getDireccion() * velocidad * std::cos(a);
    float vy = -velocidad * std::sin(a);
    float x0 = j.getCanonX() + j.getDireccion() * LARGO_CANON * std::cos(a);
    float y0 = j.getCanonY() - LARGO_CANON * std::sin(a);

    proyectil.reset(new Proyectil(x0, y0, vx, vy, MASA_PROY, RADIO_PROY, turno));
    estado = EstadoJuego::VOLANDO;
    tiempoVuelo = 0;
    impactos = 0;
    mensaje = j.getNombre() + " disparó";
}

void Juego::avanzar(float dt) {
    if (estado != EstadoJuego::VOLANDO || !proyectil) return;

    proyectil->avanzar(dt, GRAVEDAD);
    tiempoVuelo += dt;

    colisionParedes();
    colisionObstaculos();
    colisionRepresentante();

    // Si el proyectil sigue vivo, verificamos si el turno debe acabar
    if (estado == EstadoJuego::VOLANDO && (tiempoVuelo > TIEMPO_MAX_VUELO || impactos >= MAX_IMPACTOS))
        terminarTurno();
}

// ---------------------------------------------------------------------------------
// Paredes de la caja: choque PERFECTAMENTE ELÁSTICO (e = 1).
// Se invierte solo la componente perpendicular a la pared y se corrige la posición
// para que el proyectil no quede "metido" en la pared. La energía cinética se conserva.
// ---------------------------------------------------------------------------------
void Juego::colisionParedes() {
    Proyectil& p = *proyectil;
    float r = p.getRadio();

    if (p.getPosX() - r < 0)     { p.setPosX(r);          p.setVelX(-p.getVelX()); }
    if (p.getPosX() + r > ANCHO) { p.setPosX(ANCHO - r);  p.setVelX(-p.getVelX()); }
    if (p.getPosY() - r < 0)     { p.setPosY(r);          p.setVelY(-p.getVelY()); }
    if (p.getPosY() + r > ALTO)  { p.setPosY(ALTO - r);   p.setVelY(-p.getVelY()); }
}

// ---------------------------------------------------------------------------------
// Círculo vs rectángulo.
// Idea: el punto del rectángulo más cercano al centro del círculo es el centro "recortado"
// (clamp) a los límites del rectángulo. Si la distancia a ese punto es menor que el radio
// hay choque, y el vector (centro - punto) es la NORMAL de la superficie golpeada.
// La normal responde "¿con cuál lado/esquina chocó?": (±1,0) lado izq/der, (0,±1) arriba/abajo,
// y una diagonal si golpea una esquina. Así el rebote depende del ángulo de impacto.
// ---------------------------------------------------------------------------------
bool Juego::contactoCirculoRect(const Proyectil& p, const Obstaculo& o,
                                float& nx, float& ny, float& cx, float& cy) {
    float px = p.getPosX(), py = p.getPosY(), r = p.getRadio();
    float x0 = o.getPosX(), y0 = o.getPosY();
    float x1 = x0 + o.getAncho(), y1 = y0 + o.getAlto();

    cx = std::max(x0, std::min(px, x1));
    cy = std::max(y0, std::min(py, y1));
    float dx = px - cx, dy = py - cy;
    float d2 = dx * dx + dy * dy;
    if (d2 > r * r) return false;                      // no se tocan

    if (d2 > 1e-9f) {                                  // caso normal: el centro está fuera del rectángulo
        float d = std::sqrt(d2);
        nx = dx / d;
        ny = dy / d;
    } else {
        // Caso borde: el centro quedó DENTRO del rectángulo (por velocidad alta).
        // Se expulsa por la cara más cercana.
        float izq = px - x0, der = x1 - px, arr = py - y0, aba = y1 - py;
        float m = std::min(std::min(izq, der), std::min(arr, aba));
        nx = ny = 0;
        if (m == izq)      { nx = -1; cx = x0; cy = py; }
        else if (m == der) { nx = 1;  cx = x1; cy = py; }
        else if (m == arr) { ny = -1; cx = px; cy = y0; }
        else               { ny = 1;  cx = px; cy = y1; }
    }
    return true;
}

// ---------------------------------------------------------------------------------
// Infraestructura: choque INELÁSTICO con coeficiente de restitución e.
//   v_perp' = -e * v_perp      v_par' = v_par   (la componente paralela no cambia)
// Con la normal n:  v_perp = (v·n) n   ->   v' = v - (1 + e)(v·n) n
// Daño = FACTOR_DANIO * masa * |v_impacto|   (proporcional al momento lineal), calculado
// con la velocidad ANTES del rebote. Solo se daña la infraestructura del RIVAL.
// ---------------------------------------------------------------------------------
void Juego::colisionObstaculos() {
    Proyectil& p = *proyectil;

    for (Obstaculo& o : obstaculos) {
        if (o.estaDestruido()) continue;

        float nx, ny, cx, cy;
        if (!contactoCirculoRect(p, o, nx, ny, cx, cy)) continue;

        float vx = p.getVelX(), vy = p.getVelY();
        float vn = vx * nx + vy * ny;       // componente de v sobre la normal
        if (vn >= 0) continue;              // ya se está alejando: no es un choque nuevo

        float vImpacto = std::sqrt(vx * vx + vy * vy);

        // 1) Sacar el proyectil de dentro del bloque (si no, quedaría "pegado")
        p.setPosX(cx + nx * p.getRadio());
        p.setPosY(cy + ny * p.getRadio());

        // 2) Rebote inelástico
        p.setVelX(vx - (1 + COEF_REST) * vn * nx);
        p.setVelY(vy - (1 + COEF_REST) * vn * ny);

        // 3) Daño (se ignoran roces: sin esto, un proyectil apoyado sobre un techo
        //    "golpearía" el bloque en cada paso de la simulación por culpa de la gravedad)
        if (-vn > UMBRAL_IMPACTO) {
            impactos++;
            if (o.getDuenio() != p.getDuenio()) {
                float danio = FACTOR_DANIO * p.getMasa() * vImpacto;
                o.recibirDanio(danio);
                mensaje = jugadores[p.getDuenio()].getNombre() + " golpeó un bloque: -"
                          + std::to_string(static_cast<int>(danio)) + " de daño"
                          + (o.estaDestruido() ? " (¡destruido!)" : "");
            } else {
                mensaje = "El proyectil rebotó en tu propia infraestructura (sin daño)";
            }
        }
    }
}

// Condición de victoria: el proyectil toca al representante del rival.
void Juego::colisionRepresentante() {
    const Jugador& rival = jugadores[1 - turno];
    float nx, ny, cx, cy;
    if (contactoCirculoRect(*proyectil, rival.getRepresentante(), nx, ny, cx, cy)) {
        ganador = turno;
        estado = EstadoJuego::TERMINADO;
        mensaje = "¡" + jugadores[turno].getNombre() + " gana! Alcanzó al representante rival";
        proyectil.reset();
    }
}

void Juego::terminarTurno() {
    proyectil.reset();
    turno = 1 - turno;
    estado = EstadoJuego::APUNTANDO;
    mensaje = "Turno de " + jugadores[turno].getNombre();
}

EstadoJuego Juego::getEstado() const { return estado; }
int Juego::getTurno() const { return turno; }
int Juego::getGanador() const { return ganador; }
const std::string& Juego::getMensaje() const { return mensaje; }
const std::vector<Obstaculo>& Juego::getObstaculos() const { return obstaculos; }
const std::vector<Jugador>& Juego::getJugadores() const { return jugadores; }
const Proyectil* Juego::getProyectil() const { return proyectil.get(); }
