#ifndef ENTIDAD_H
#define ENTIDAD_H
using namespace std;

class Entidad{

protected:
    float posX;
    float posY;

public:
    Entidad(float x = 0.0f, float y = 0.0f);
    virtual ~Entidad() = default;

    float getPosX() const;
    float getPosY() const;
    void setPosX(float x);
    void setPosY(float y);

};


#endif // ENTIDAD_H
