#include "EntityManager.h"
#include "Player.h"
#include "Camera.h"
#include "Item.h"
#include "NotAGoomba.h"

#include "App.h"
#include "Textures.h"
#include "Scene.h"


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
	Entity* entity = nullptr;

	// TODO add new entities here
	switch (type)
	{
	case EntityType::PLAYER:			entity = players.Add(new Player())->data; break;
	case EntityType::ITEM:				entity = new Item(); break;
	case EntityType::CAMERA:
		LOG("Use CreateCamera() to create cameras!");
		return CreateCamera(nullptr);
	case EntityType::ENEMY_GROUNDED:	entity = new NotAGoomba(); break;
	default:
		LOG("Invalid EntityType");
		break;
	}

	if (entity != nullptr) {
		entity->dataFromMap = objectData;
		entity->parameters = entityPresets.child(entity->name.GetString());
		// Si ya ha pasado la fase de inicializacion (Awake() y Start()), ejecuta manualmente la función correspondiente para esta entidad
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
	if (mainCamera == nullptr) {
		SetMainCamera(camera); // If there's no main camera, assign this camera as it
	}
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
	if (name == "player")
		entity = CreateEntity(EntityType::PLAYER, objectData);
	else if (strcmp(name.GetString(), "item") == 0)
		entity = CreateEntity(EntityType::ITEM, objectData);

	return entity;
}

// Removes the entity from the list and destroys it. Make sure not to have any pointers to it after running this
void EntityManager::DestroyEntity(Entity* entity)
{
	RemoveEntity(entity);
	entity->CleanUp();
	delete entity;
}

// Removes the entity from the list
void EntityManager::RemoveEntity(Entity* entity)
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
	if (entity != nullptr) entities.Add(entity);
}

void EntityManager::SetMainCamera(Camera* c)
{
	app->render->cam = mainCamera = c;
	if (c != nullptr && c->GetTarget() == nullptr && players.Count() > 0) {
		c->SetTarget(players[0]);
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
			DestroyEntity(pEntity);
			continue;
		}
		if (pEntity->active == false) continue;
		ret = item->data->Update(dt);
	}

	return ret;
}

bool EntityManager::LoadState(pugi::xml_node node) {
	/*Disable();
	Enable();*/
	
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
		else if (entityName == "Player") {
			/*Entity* entity = CreateEntity(EntityType::PLAYER, entityNode);
			if (entity != nullptr) {
				Player* player = dynamic_cast<Player*>(entity);
				if (player != nullptr) {
					player->LoadState(entityNode);
				}
			}*/
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
		// Repite el proceso para otras entidades según sea necesario
	}
	
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