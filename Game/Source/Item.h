#ifndef __ITEM_H__
#define __ITEM_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"

struct SDL_Texture;

class Item : public Entity
{
public:

	Item();
	virtual ~Item();

	bool Awake();

	bool Start();

	bool Update(float dt);

	void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) override;

	iPoint GetOrigin() const override;

	bool CleanUp();

	bool LoadState(pugi::xml_node& node);

	bool SaveState(pugi::xml_node& node);

public:

	bool isPicked = false;

private:

	SDL_Texture* texture;
	const char* texturePath;
};

#endif // __ITEM_H__