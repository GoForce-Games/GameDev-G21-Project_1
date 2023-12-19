#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Physics.h"
#include "Reload.h"
#include "Timer.h"
#include "SDL\include\SDL_timer.h"

#include "Camera.h"

#include "Defs.h"
#include "Log.h"
#include "Enemy.h"

Scene::Scene() : Module()
{
	name.Create("scene");
	needsAwaking = true;
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	// iterate all objects in the scene
	// Check https://pugixml.org/docs/quickstart.html#access
	for (pugi::xml_node itemNode = config.child("item"); itemNode; itemNode = itemNode.next_sibling("item"))
	{
		Item* item = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		item->parameters = itemNode;
	}

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player"); // Sobreescribe los datos del preset con los que hay en la escena
	}

	if (config.child("camera")) {
		Camera* cam = app->entityManager->CreateCamera(player);
		cam->parameters = config.child("camera"); // Sobreescribe los datos del preset con los que hay en la escena
	}

	if (config.child("map")) {
		//Get the map name from the config file and assigns the value in the module
		app->map->name = config.child("map").attribute("name").as_string();
		app->map->path = config.child("map").attribute("path").as_string();
	}

	if (config.child("gameover")) {
		gameOverScreenTexturePath = config.child("gameover").attribute("path").as_string();
	}

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	//img = app->tex->Load("Assets/Textures/test.png");
	img = app->tex->Load(gameOverScreenTexturePath.GetString());
	
	//Music is commented so that you can add your own music
	//app->audio->PlayMusic("Assets/Audio/Music/music_spy.ogg");

	// Set mapSize to 0 in case the map was changed
	mapSize.Create(0, 0);

	//Get the size of the window
	app->win->GetWindowSize(windowW, windowH);

	//Get the size of the texture
	app->tex->GetSize(img, texW, texH);

	textPosX = (float)windowW / 2 - (float)texW / 2;
	textPosY = (float)windowH / 2 - (float)texH / 2;

	SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d",
		app->map->mapData.width,
		app->map->mapData.height,
		app->map->mapData.tileWidth,
		app->map->mapData.tileHeight,
		app->map->mapData.tilesets.Count());

	if (app->entityManager->mainCamera != nullptr)
		app->entityManager->mainCamera->SetTarget(player);

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if (mapSize == iPoint{0,0}) {
		mapSize = app->map->mapData.GetMapSize();
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) app->reload->reload = true; // TODO cambiar cuando haya segundo nivel
	else if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) app->reload->reload = true;

	//If player is out of the map, kill them
	if (player->position.x<-player->pbody->width || player->position.x>mapSize.x + player->pbody->width || player->position.y > mapSize.y + player->pbody->height) {
		player->OnDeath();
	}


	
	
	
	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

	//DEBUG
	if (app->debug)
		DebugActions();


	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (player->alive == false) {

		const uint32 waitTimeInSec = 2;

		if (timer.ReadSec() >= waitTimeInSec) {

			app->render->DrawTexture(img, /*(int)textPosX + */ app->entityManager->mainCamera->position.x, /*(int)textPosY + */app->entityManager->mainCamera->position.y);
		}
	}

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");
	player = nullptr;
	app->tex->UnLoad(img);

	return true;
}
bool Scene::LoadState(pugi::xml_node node)
{
	b2Vec2 posMeters = b2Vec2(PIXEL_TO_METERS(node.child("player").attribute("x").as_int()), 
		                      PIXEL_TO_METERS(node.child("player").attribute("y").as_int()));

	player->pbody->body->SetTransform(posMeters, 0);
	return true;
}

bool Scene::SaveState(pugi::xml_node node)
{
	pugi::xml_node playerNode = node.append_child("player");
	playerNode.append_attribute("x").set_value(player->position.x);
	playerNode.append_attribute("y").set_value(player->position.y);
	return true;
}


void Scene::DebugActions()
{
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY_GROUNDED);
		e->SetPosition(app->render->cam->position-app->render->cam->offset, true);
		
	}

}
