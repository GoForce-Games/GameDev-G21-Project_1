#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Entity.h"
#include "Point.h"
#include "SDL/include/SDL.h"
#include "Box2D/Box2D/Box2D.h"
#include "Animation.h"

struct SDL_Texture;
class Camera;

class Player : public Entity
{
public:

	Player();
	
	virtual ~Player();

	bool Awake();

	bool Start();

	bool Update(float dt);

	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo);
	
	void OnCoinCollision(PhysBody* thisBody, PhysBody* coinBody);

	void OnPlatformCollision(PhysBody* thisBody, PhysBody* wallBody, b2Contact* contactInfo);

	void OnHurt();

	void OnDeath();

	iPoint GetOrigin() const override;

public:
	const char* texturePath;
	SDL_Texture* texture = NULL;
	int pickCoinFxId;

private:
	float accel;
	b2Vec2 velCap;

	uint jumpsAvailable = 1;
	uint maxJumps;
	float jumpPower;
	bool grounded = false;
	Animation* currentAnimation = nullptr;

	Animation idleAnim;
	Animation forwardAnim;
	Animation backwardAnim;
	Animation forwardjump;
	Animation backwardjump;
	Animation death;
	
	bool alive;

};

#endif // __PLAYER_H__