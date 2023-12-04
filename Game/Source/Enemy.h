#pragma once
#include "Entity.h"
class Enemy :
    public Entity
{
public:
    //TODO poner los métodos que hace falta para una entidad (copia de Item por ahora)


public:

    float actionRadius;
    float speed;

    bool pursuit;
    List<iPoint> pathToPlayer;

};

