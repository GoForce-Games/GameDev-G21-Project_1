#pragma once
#include "Entity.h"

class Enemy : public Entity
{
public:
    //TODO poner los métodos que hace falta para una entidad (copia de Item por ahora)
    Enemy(EntityType _type = EntityType::ENEMY) : Entity(_type) {}
    ~Enemy() {}

    virtual bool Awake() = 0;

    virtual bool Start() = 0;

    virtual bool Update(float dt) { return true; }

    virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {}

    virtual bool CleanUp() { return Entity::CleanUp(); }

    virtual bool FindPath(iPoint& destination) { return true; }

    virtual bool EnemyBehaviour(float dt) = 0;

public:

    float actionRadius;
    float speed;

    bool pursuit;
    List<iPoint> pathToPlayer;

};

