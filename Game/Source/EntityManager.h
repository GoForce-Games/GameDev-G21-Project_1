#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"

class Player;

class EntityManager : public Module
{
public:

	EntityManager();

	// Destructor
	virtual ~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called after Awake
	bool Start();

	// Called every frame
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Additional methods
	Entity* CreateEntity(EntityType type);

	Camera* CreateCamera(Player* player);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

public:

	List<Entity*> entities;

	List<Player*> players;

};

#endif // __ENTITYMANAGER_H__
