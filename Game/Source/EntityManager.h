#ifndef __ENTITYMANAGER_H__
#define __ENTITYMANAGER_H__

#include "Module.h"
#include "Entity.h"
#include "List.h"

class Player;
class Camera;

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

	Camera* CreateCamera(Entity* target = nullptr);

	void DestroyEntity(Entity* entity);

	void AddEntity(Entity* entity);

	void SetMainCamera(Camera* c);

public:

	List<Entity*> entities;

	List<Player*> players;

	List<Camera*> cameras;

	Camera* mainCamera;

};

#endif // __ENTITYMANAGER_H__
