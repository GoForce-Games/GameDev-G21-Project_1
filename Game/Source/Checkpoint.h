#pragma once
#include "Entity.h"
class Checkpoint :
    public Entity
{
public:

    Checkpoint();
    ~Checkpoint();

	bool Awake();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo);

private:

	int id;
};

