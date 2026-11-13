#ifndef PARTICULA_H
#define PARTICULA_H

#include "Entidad.h"
using namespace std;

class Particula : public Entidad{

private:
    int id;
    float masa;
    float radio;
    float velX;
    float velY;

public:
    Particula(float x, float y, float vx, float vy, float m, float r, int id);

    void actualizarPosicion(float deltaT);
    void fusionar(Particula* otra);

    int getId() const;
    float getMasa() const;
    float getRadio() const;
    float getVelX() const;
    float getVelY() const;

    void setVelX(float vx);
    void setVelY(float vy);
    void setMasa(float m);
    void setRadio(float r);

    ~Particula();


};

#endif // PARTICULA_H
