#pragma once
#include "Entity.h"
class Warp : public Entity
{
public:

	Warp();
	~Warp();

	bool Awake();
	bool Update(float dt);
	bool CleanUp();
	void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo);

private:

	int id;
	Warp* targetWarp = nullptr;

	//Direction the player has to try to move in in order to use this warp
	b2Vec2 entryDir;
};

