#pragma once
#include "Entity.h"

#include "SDL/include/SDL_rect.h"
#include "Box2D/Box2D/Box2D.h"


class Camera :
    public Entity
{
public:

    Camera(Entity* e);

    virtual ~Camera();

    bool Awake() override;

    bool Start() override;

    bool Update(float dt) override;

    bool CleanUp() override;

    bool DebugDraw();

    void SetTarget(Entity* e);

private:
    
    float camSpeed;

    b2Vec2 offset;
    SDL_Rect rect{ 0,0,0,0 };
    Entity* target;
};

