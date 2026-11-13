#ifndef OBSTACULO_H
#define OBSTACULO_H

#include "Entidad.h"

class Obstaculo : public Entidad {
private:
    float ancho;
    float alto;

public:
    Obstaculo(float x, float y, float ancho, float alto);

    float getAncho() const;
    float getAlto() const;
};

#endif // OBSTACULO_H