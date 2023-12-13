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

	EntityManager(bool startEnabled = true);

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
	Entity* CreateEntity(EntityType type, pugi::xml_node objectData = pugi::xml_node());

	Camera* CreateCamera(Entity* target = nullptr);

	Entity* CreateEntityFromMapData(SString name, pugi::xml_node objectData = pugi::xml_node());

	void DestroyEntity(Entity* entity);

	void RemoveEntity(Entity* entity);

	void AddEntity(Entity* entity);

	void SetMainCamera(Camera* c);

	bool LoadState(pugi::xml_node node);

	bool SaveState(pugi::xml_node node);

public:

	List<Entity*> entities;

	List<Player*> players;
	List<Camera*> cameras;

	pugi::xml_node entityPresets;
	Camera* mainCamera;

};

#endif // __ENTITYMANAGER_H__