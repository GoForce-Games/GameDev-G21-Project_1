#pragma once
#include "Entity.h"

#include "App.h"
#include "Textures.h"

#include "Animation.h"

enum class EnemyState
{
	IDLE,
	PURSUIT,
	ROAMING,
	FLEEING,
	DEAD,
	UNKNOWN
};

class Enemy : public Entity
{
public:
	Enemy(EntityType _type = EntityType::ENEMY) : Entity(_type) {}
	~Enemy() {}

	virtual bool Awake() = 0;
	virtual bool Start() = 0;
	virtual bool Update(float dt);
	virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {}
	virtual bool CleanUp(bool reuse = false);

	virtual bool LoadState(pugi::xml_node& objNode) override;
	virtual bool SaveState(pugi::xml_node& objNode) override;

	void SetPosition(const iPoint& newPos, bool newHome) {
		Entity::SetPosition(newPos);
		if (newHome)
			home = newPos;
	}

	// General path finding for enemies
	virtual bool FindPath(iPoint& destination);

	// Specific enemy behaviour. Must be defined
	virtual bool EnemyBehaviour(float dt) = 0;

	void LoadAllAnimations();

	Animation* GetAnimation(SString name);

public:

	float actionRadius;
	float speed;
	b2Vec2 velCap;
	float homeRadius;
	float homeInnerRadius;
	iPoint home;
	iPoint moveDirection;
	float stompAngle;

	EnemyState state = EnemyState::IDLE;
	List<iPoint> pathToPlayer;
	Timer pfCooldown;

	SDL_Texture* texture = nullptr;
	Animation* currentAnimation = nullptr;
	List<Animation*> animationList;
	int deathsound;

};

