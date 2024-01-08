#pragma once
#include "Enemy.h"

struct SDL_Texture;

class NotAGoomba : public Enemy
{
public:

	NotAGoomba();

	~NotAGoomba();

	bool Awake() override;

	bool Start() override;

	bool Update(float dt) override;

	bool CleanUp(bool reuse) override;

	bool LoadState(pugi::xml_node& objRootNode) override;

	bool SaveState(pugi::xml_node& objRootNode) override;

	// Contact bodies may not be in the same order as parameters
	void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo);

	iPoint GetOrigin() const override;

	bool FindPath(iPoint& destination) override;

	bool EnemyBehaviour(float dt) override;

private:

	SString texturePath;

	Animation* rightWalk = nullptr;
	Animation* leftWalk = nullptr;


};

