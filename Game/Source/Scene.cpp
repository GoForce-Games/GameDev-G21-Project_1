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
#include "GuiControl.h"
#include "GuiManager.h"
#include "GuiControlButton.h"

#include "Camera.h"

#include "Defs.h"
#include "Log.h"
#include "Enemy.h"
#include "HealerItem.h"
#include "Player.h"

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
	musicsound = config.child("map").attribute("musicsound").as_string();
	if (false && config.child("map")) {
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
	player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
	app->entityManager->CacheEntity(player); // NOTE: This may cause instability depending on future implementation

	// NOTE: We have to avoid the use of paths in the code, we will move it later to a config file
	//img = app->tex->Load("Assets/Textures/test.png");
	img = app->tex->Load(gameOverScreenTexturePath.GetString());
	
	//Music is commented so that you can add your own music
	app->audio->PlayMusic(musicsound.GetString());

	/*app->audio->PlayMusic("Assets/Audio/Music/Algo.ogg");*/ //No descomentar esto si amas tu capacidad auditiva, no entiendo porque se reproduce asi

	/*falldeath = app->audio->LoadFx("Assets/Audio/Fx/buzzer.wav");*/

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

	//if (app->entityManager->mainCamera != nullptr)
		//app->entityManager->mainCamera->SetTarget(player);
	SDL_Rect btPos = { windowW / 25, windowH / 25, 120,30 };
	pauseButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::BUTTON, 1, "Pause", btPos, this);
	
	SDL_Rect btPos1 = { windowW - 125, windowH - 735, 30,20 };
	pauseButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::VALUEBOX, 1, "3" , btPos1, this);

	
	SDL_Rect btPos2 = { windowW - 55, windowH - 735, 50,20 };
	pauseButtom = (GuiControlButton*)app->guiManager->CreateGuiControl(GuiControlType::VALUEBOX, 1, "1000"  , btPos2, this);

	
	
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

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		app->map->ChangeMap(0); // Load level 1
	}
	else if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		app->map->ChangeMap(1); // Load level 2
	}
	else if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		app->map->ChangeMap(-1); // Reload current level
	}

	//If player is out of the map, kill them
	if (player->active && (player->position.x<-player->pbody->width || player->position.x>mapSize.x + player->pbody->width || player->position.y > mapSize.y + player->pbody->height)) {
		player->OnDeath();
		app->audio->PlayFx(falldeath);
	}

	
	
	
	// Renders the image in the center of the screen 
	//app->render->DrawTexture(img, (int)textPosX, (int)textPosY);

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) app->SaveRequest();
	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) app->LoadRequest();

	//DEBUG
	if (app->debug)
		DebugActions();

	if (pauseButtom->state == GuiControlState::PRESSED) {
		pause == true;
	}
	
	

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

			app->render->DrawTexture(img, /*(int)textPosX + */ app->render->cam->position.x, /*(int)textPosY + */app->render->cam->position.y);
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
	
    //delete live;
	
	

	

	return true;
}
bool Scene::LoadState(pugi::xml_node node)
{
	
	return true;
}

bool Scene::SaveState(pugi::xml_node node)
{
	
	return true;
}


void Scene::DebugActions()
{
	if (app->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY_GROUNDED);
		e->SetPosition(app->render->cam->position-app->render->cam->offset, true);
	}
	else if (app->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) {
		Enemy* e = (Enemy*)app->entityManager->CreateEntity(EntityType::ENEMY_FLYING);
		e->SetPosition(app->render->cam->position - app->render->cam->offset, true);
	}

	if (app->input->GetKey(SDL_SCANCODE_3) == KEY_REPEAT) {
		Item* e = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
		e->SetPosition(app->render->cam->position - app->render->cam->offset);
	}

	if (app->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) {
		HealerItem* e = (HealerItem*)app->entityManager->CreateEntity(EntityType::HEALERITEM);
		e->SetPosition(app->render->cam->position - app->render->cam->offset);
	}

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
		app->map->navigationLayer->properties.GetProperty("Draw")->boolVal ^= true; // XOR eXclusive OR
	}

}
