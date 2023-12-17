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
    const Entity* GetTarget() const;

private:

    float camSpeed;

    iPoint offset{ 0,0 };
    iPoint targetOffset{ 0,0 };
    SDL_Rect rect{ 0,0,0,0 };
    Entity* target = nullptr;

    iPoint mapBounds{ 0,0 };

    friend class Render;

    bool awoken = false;
};

