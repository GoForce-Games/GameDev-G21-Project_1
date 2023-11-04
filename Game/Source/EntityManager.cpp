#include "EntityManager.h"
#include "Player.h"
#include "Camera.h"

#include "App.h"
#include "Textures.h"
#include "Scene.h"

#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager() : Module()
{
	name.Create("entitymanager");
	needsAwaking = true;
}

// Destructor
EntityManager::~EntityManager()
{}

// Called before render is available
bool EntityManager::Awake(pugi::xml_node& config)
{
	LOG("Loading Entity Manager");
	bool ret = true;

	//Iterates over the entities and calls the Awake
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Awake();
	}

	return ret;

}

bool EntityManager::Start() {

	bool ret = true; 

	//Iterates over the entities and calls Start
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;

		if (pEntity->active == false) continue;
		ret = item->data->Start();
	}

	//If cam is not following player, assign player as target
	//if (mainCamera->GetTarget() == nullptr && players.Count() > 0) {
		//mainCamera->SetTarget(players[0]);
		//app->render->cam = mainCamera;
	//}

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		DestroyEntity(item->data);
		item = item->prev;
	}

	entities.Clear();
	players.Clear();
	cameras.Clear();

	SetMainCamera(nullptr);

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type)
{
	Entity* entity = nullptr; 

	switch (type)
	{
	case EntityType::PLAYER:
		entity = new Player();
		players.Add(static_cast<Player*>(entity));
		break;
	case EntityType::ITEM:
		entity = new Item();
		break;
	case EntityType::CAMERA:
		LOG("Use CreateCamera() to create cameras!");
		CreateCamera(nullptr);
		break;
	default:
		break;
	}

	entities.Add(entity);

	return entity;
}

Camera* EntityManager::CreateCamera(Entity* target)
{
	Camera* camera = new Camera(target);
	entities.Add(camera);
	cameras.Add(camera);
	if (mainCamera == nullptr) {
		SetMainCamera(camera); // If there's no main camera, assign this camera as it
		
	}
	camera->SetTarget(target);

	return camera;
}

void EntityManager::DestroyEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity) 
		{
			entities.Del(item);
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if ( entity != nullptr) entities.Add(entity);
}

void EntityManager::SetMainCamera(Camera* c)
{
	app->render->cam = mainCamera = c;
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		if (pEntity->setToDestroy) DestroyEntity(pEntity);
		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}