#include "EntityManager.h"
#include "Player.h"
#include "Camera.h"
#include "Item.h"
#include "NotAGoomba.h"
#include "FlyingEnemy.h"
#include "HealerItem.h"
#include "Warp.h"
#include "Checkpoint.h"

#include "App.h"
#include "Textures.h"
#include "Scene.h"
#include "Map.h"


#include "Defs.h"
#include "Log.h"

EntityManager::EntityManager(bool startEnabled) : Module(startEnabled)
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
	Module::Awake(config); //Call base module config (general behavior)
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

	entityPresets = config.child("entitypresets"); // TODO crear presets en el config.xml

	return ret;

}

bool EntityManager::Start() {
	Module::Start(); //Call base module config (general behavior)
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

	return ret;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	Module::CleanUp();
	bool ret = true;
	ListItem<Entity*>* item;
	item = entities.end;

	while (item != NULL && ret == true)
	{
		ret = item->data->CleanUp();
		delete item->data;
		entities.Del(item);
		item = item->prev;
	}

	entities.Clear();
	players.Clear();
	cameras.Clear();

	SetMainCamera(nullptr);

	return ret;
}

Entity* EntityManager::CreateEntity(EntityType type, pugi::xml_node objectData)
{
	Entity* entity = GetEntityFromCache(type);

	// If there's no available cached entity, create a new one
	if (entity == nullptr) {

		switch (type)
		{// TODO add new entities here
		case EntityType::PLAYER:			entity = players.Add(new Player())->data; break;
		case EntityType::ITEM:				entity = new Item(); break;
		case EntityType::HEALERITEM:        entity = new HealerItem(); break;
		case EntityType::CAMERA:
			LOG("Use CreateCamera() to create cameras!");
			return CreateCamera(nullptr);
		case EntityType::ENEMY_GROUNDED:	entity = new NotAGoomba(); break;
		case EntityType::ENEMY_FLYING:	entity = new FlyingEnemy(); break;
		case EntityType::WARP: entity = new Warp(); break;
		case EntityType::CHECKPOINT: entity = new Checkpoint(); break;
		default:
			LOG("Invalid EntityType");
			break;
		}
	}

	if (entity != nullptr) {
		entity->dataFromMap = objectData;
		entity->parameters = entityPresets.child(entity->name.GetString());
		// Si ya ha pasado la fase de inicializacion (Awake() y Start()), ejecuta manualmente la función correspondiente para esta entidad
		// IMPORTANTE: para reutilizar entidades hace falta que no se repita instanciaciones innecesarias en estas funciones
		if (awoken) entity->Awake();
		if (started) entity->Start();

		entities.Add(entity);
	}

	return entity;
}

Camera* EntityManager::CreateCamera(Entity* target)
{
	Camera* camera = new Camera(target);
	camera->parameters = entityPresets.child(camera->name.GetString());
	entities.Add(camera);
	cameras.Add(camera);
	if (app->render->cam == nullptr) {
		SetMainCamera(camera); // If there's no main camera, assign this camera as it
	}
	if (target != nullptr)
		camera->SetTarget(target);

	//If module is already initialized, enable the camera
	if (awoken) camera->Awake();
	if (started) camera->Start();

	return camera;
}

Entity* EntityManager::CreateEntityFromMapData(SString name, pugi::xml_node objectData)
{
	Entity* entity = nullptr;

	// TODO add new entities here
	if (name == "grounded")
		entity = CreateEntity(EntityType::ENEMY_GROUNDED, objectData);
	else if (name == "flying")
		entity = CreateEntity(EntityType::ENEMY_FLYING, objectData);
	else if (name == "player")
		entity = CreateEntity(EntityType::PLAYER, objectData);
	else if (name == "item")
		entity = CreateEntity(EntityType::ITEM, objectData);
	else if (name == "healeritem")
		entity = CreateEntity(EntityType::HEALERITEM, objectData);
	else if (name == "warp")
		entity = CreateEntity(EntityType::WARP, objectData);
	else if (name == "checkpoint")
		entity = CreateEntity(EntityType::CHECKPOINT, objectData);

	return entity;
}

// Removes the entity from the list and destroys it. Make sure not to have any pointers to it after running this
void EntityManager::DestroyEntity(Entity* entity)
{
	RemoveEntity(entity);
	entity->CleanUp();
	delete entity;
}

// Removes the entity from the main entity list
void EntityManager::RemoveEntity(Entity* entity)
{
	ListItem<Entity*>* item;

	for (item = entities.start; item != NULL; item = item->next)
	{
		if (item->data == entity)
		{
			entities.Del(item);
			break;
		}
	}
}

void EntityManager::AddEntity(Entity* entity)
{
	if (entity != nullptr) entities.Add(entity);
}

// Instead of deleting the entity, caches it for later use
void EntityManager::CacheEntity(Entity* entity)
{
	RemoveEntity(entity);
	entityCache.Add(entity);
	entity->setToDestroy = false;
	entity->SetActive(false);
}

//Accessed by CreateEntity ONLY
Entity* EntityManager::GetEntityFromCache(EntityType type)
{
	Entity* ret = nullptr;

	for (ListItem<Entity*>* item = entityCache.start; item != nullptr; item = item->next)
	{
		if (item->data->type == type) {
			ret = item->data;
			ret->SetActive(true);
			item->data = nullptr;
			entityCache.Del(item);
			break;
		}
	}

	return ret;
}

void EntityManager::SetMainCamera(Camera* c)
{
	app->render->SetCamera(c);
	if (c != nullptr) {
		LOG("Main camera set. Render camera will move");
		if (c->GetTarget() == nullptr && players.Count() > 0) {
			c->SetTarget(players[0]);
		}
	}
	else {
		LOG("Main camera set to NULL. Render camera will not move");
	}
}

bool EntityManager::Update(float dt)
{
	bool ret = true;
	ListItem<Entity*>* item;
	Entity* pEntity = NULL;

	for (item = entities.start; item != NULL && ret == true; item = item->next)
	{
		pEntity = item->data;
		if (pEntity->setToDestroy) {
			CacheEntity(pEntity);
			continue;
		}
		if (pEntity->active == false) continue;
		
		ret = item->data->Update(dt);
	}

	for (item = entityCache.start; item != NULL && ret == true; item = item->next) {
		if (item->data->active) {
			item->data->SetActive(false);
			item->data->SetPosition({ -10000,10000 });
		}
	}

	if (app->render->cam == nullptr) {
		Camera* cam = CreateCamera();
		if (players.Count() > 0)
			cam->SetTarget(players[0]);
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node node) {
	// TODO: cambiar esto para no necesitar desactivar y reactivar modulos
	app->map->Disable();
	this->Disable();
	app->physics->Disable();
	Awake(app->GetConfig(*this));
	app->physics->Enable();
	this->Enable();
	app->map->Enable();
	
	pugi::xml_node managerNode = node.child("entitymanager");

	for (pugi::xml_node entityNode = managerNode.first_child(); entityNode; entityNode = entityNode.next_sibling()) {
		SString entityName(entityNode.name());

		if (entityName == "item") {
			Entity* entity = CreateEntity(EntityType::ITEM, entityNode);
			if (entity != nullptr) {
				Item* item = dynamic_cast<Item*>(entity);
				if (item != nullptr) {
					item->LoadState(entityNode);
				}
			}
		}
		else if (entityName == "player") {
			Entity* entity = CreateEntity(EntityType::PLAYER, entityNode);
			if (entity != nullptr) {
				Player* player = dynamic_cast<Player*>(entity);
				if (player != nullptr) {
					player->LoadState(entityNode);
				}
			}
		}		
		else if (entityName == "notGoomba") {
			Entity* entity = CreateEntity(EntityType::ENEMY_GROUNDED, entityNode);
			if (entity != nullptr) {
				NotAGoomba* notAGoomba = dynamic_cast<NotAGoomba*>(entity);
				if (notAGoomba != nullptr) {
					notAGoomba->LoadState(entityNode);
				}
			}
		}
		else if (entityName == "flyingEnemy") {
			Entity* entity = CreateEntity(EntityType::ENEMY_FLYING, entityNode);
			if (entity != nullptr) {
				FlyingEnemy* flyingEnemy = dynamic_cast<FlyingEnemy*>(entity);
				if (flyingEnemy != nullptr) {
					flyingEnemy->LoadState(entityNode);
				}
			}
		}
		else if (entityName == "healeritem") {
			Entity* entity = CreateEntity(EntityType::HEALERITEM, entityNode);
			if (entity != nullptr) {
				HealerItem* item = dynamic_cast<HealerItem*>(entity);
				if (item != nullptr) {
					item->LoadState(entityNode);
				}
			}
		}
	}
	CreateCamera();

	return true;
}

bool EntityManager::SaveState(pugi::xml_node node) {
	pugi::xml_node managerNode = node.append_child("entitymanager");
	for (ListItem<Entity*>* item = entities.start; item != NULL; item = item->next) {
		pugi::xml_node entityNode = managerNode.append_child(item->data->name.GetString());
		item->data->SaveState(entityNode);
	}
	return true;
}