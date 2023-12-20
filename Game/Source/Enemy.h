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
	//TODO poner los métodos que hace falta para una entidad (copia de Item por ahora)
	Enemy(EntityType _type = EntityType::ENEMY) : Entity(_type) {}
	~Enemy() {}

	virtual bool Awake() = 0;

	virtual bool Start() = 0;

	virtual bool Update(float dt);

	virtual void OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo) {}

	virtual bool CleanUp(bool reuse = false)
	{
		if (!reuse) {
			for (ListItem<Animation*>* item = animationList.start; item; item = item->next)
			{
				RELEASE(item->data);
			}
			animationList.Clear();
			app->tex->UnLoad(texture);
			texture = nullptr;
		}
		else {
			currentAnimation = GetAnimation("Idle");
		}
		return Entity::CleanUp();
	}

	void Disable() override {
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

	void SetPosition(const iPoint& newPos, bool newHome = false) {
		Entity::SetPosition(newPos);
		if (newHome)
			home = newPos;
	}

	// General pathfinding
	virtual bool FindPath(iPoint& destination);

	// Specific enemy behaviour. Must be defined
	virtual bool EnemyBehaviour(float dt) = 0;

	void LoadAllAnimations();

	Animation* GetAnimation(SString name);

public:

	float actionRadius;
	float speed;
	float homeRadius;
	iPoint home;
	iPoint moveDirection;

	EnemyState state = EnemyState::IDLE;
	List<iPoint> pathToPlayer;

	SDL_Texture* texture = nullptr;
	Animation* currentAnimation = nullptr;
	List<Animation*> animationList;

};

