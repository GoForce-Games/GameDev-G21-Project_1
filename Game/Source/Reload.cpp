#include "Reload.h"
#include "App.h"

Reload::Reload() {

}

Reload::~Reload() {

}

bool Reload::Awake(pugi::xml_node& conf)
{
	return true;
}

bool Reload::Start()
{
	// TODO A�adir los m�dulos a recargar
	moduleList.Add((Module*)app->tex);
	moduleList.Add((Module*)app->audio);
	moduleList.Add((Module*)app->physics);
	moduleList.Add((Module*)app->scene);
	moduleList.Add((Module*)app->map);
	moduleList.Add((Module*)app->entityManager);

	return true;
}

bool Reload::Update(float dt)
{

	if (reload) {

		for (ListItem<Module*>* item = moduleList.end; item != nullptr; item = item->prev)
		{
			if (item->data != nullptr) item->data->Disable();
		}

		for (ListItem<Module*>* item = moduleList.start; item != nullptr; item = item->next)
		{
			if (item->data != nullptr && item->data->needsAwaking) { 
				pugi::xml_node config = app->GetConfig(*(item->data));
				item->data->Awake(config);
			}
		}
		
		for (ListItem<Module*>* item = moduleList.start; item != nullptr; item = item->next)
		{
			if (item->data != nullptr) item->data->Enable();
		}

		reload = false;
	}

	return true;
}

bool Reload::CleanUp()
{
	return true;
}
