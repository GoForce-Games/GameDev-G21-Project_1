#pragma once
#include "Entity.h"

struct SDL_Texture;

class NotAGoomba : public Entity
{
public:

	NotAGoomba(EntityType type);

	~NotAGoomba();

	virtual bool Awake();

	virtual bool Start();

	virtual bool Update(float dt);

	virtual bool CleanUp();

	virtual bool LoadState(pugi::xml_node& objRootNode);

	virtual bool SaveState(pugi::xml_node& objRootNode);

	// Contact bodies may not be in the same order as parameters
	virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo);

	virtual iPoint GetOrigin() const override;

private:

	SString texturePath;
	SDL_Texture* texture;

};

