#include "Reload.h"
#include "App.h"

#include "Box2D/Box2D/Box2D.h"
#include "Log.h"

Reload::Reload(bool startEnabled) : Module(startEnabled) {
	name.Create("reload");
}

Reload::~Reload() {

}

bool Reload::Awake(pugi::xml_node& conf)
{
	// TODO A�adir los m�dulos a recargar desde el xml
	for (pugi::xml_node& presetNode = conf.child("presets").child("preset"); presetNode != NULL; presetNode = presetNode.next_sibling("preset"))
	{
		SString presetName = presetNode.attribute("name").as_string();
		float fadeOut = presetNode.attribute("fadeOut").as_float();
		float fadeIn = presetNode.attribute("fadeIn").as_float();
		ReloadPreset* preset = presetList.Add(new ReloadPreset(presetName, fadeOut, fadeIn))->data;
		for (pugi::xml_node& moduleNode = presetNode.child("module"); moduleNode != NULL; moduleNode = moduleNode.next_sibling("module"))
		{
			SString type = moduleNode.attribute("type").as_string();
			SString moduleName = moduleNode.attribute("name").as_string();
			Module* mod = app->GetModule(moduleName.GetString());
			if (type == "r")
				preset->AddReload(mod);
			else if (type == "l")
				preset->AddLoad(mod);
			else if (type == "u")
				preset->AddUnload(mod);
		}
	}

	return true;
}

bool Reload::Start()
{
	// Enable blending mode for transparency
	SDL_SetRenderDrawBlendMode(app->render->renderer, SDL_BLENDMODE_BLEND);
	screenRect = app->render->camera;
	screenRect.x = screenRect.y = 0;

	return true;
}

bool Reload::PostUpdate()
{


	if (activePreset != nullptr) {

		// Steps in order: fade out -> reload modules -> fade in -> finished
		switch (currentStep)
		{
		case FADE_OUT: {
			FadeOut();
			break;
		}
		case RELOAD: {
			ReloadModules();
			break;
		}
		case FADE_IN: {
			FadeIn();
			break;
		}
		default:
			break;
		}
		//LOG("fadeRatio: %f", fadeRatio);
		//LOG("Alpha: %i", (uint8)(fadeRatio * 255.0f));
		app->render->DrawRectangle(screenRect, 0, 0, 0, (uint8)(fadeRatio * 255.0f), true, false);
	}

	return true;
}

bool Reload::CleanUp()
{
	for (ListItem<ReloadPreset*>* item = presetList.start; item != nullptr; item = item->next)
	{
		if (item->data != nullptr) {
			RELEASE(item->data);
		}
	}
	presetList.Clear();

	return true;
}

bool Reload::StartReload(SString presetName)
{
	bool ret = false;
	if (activePreset == nullptr) {

		for (size_t i = 0; i < presetList.Count(); i++)
		{
			ListItem<ReloadPreset*>* item = presetList.At(i);
			if (item->data->name == presetName) {
				ret = true;
				activePreset = item->data;
				currentStep = ReloadStep::FADE_OUT;
			}
		}
	}

	return true;
}

void Reload::FadeOut()
{
	if (!timerActive) {
		timer.Start();
		timerActive = true;
	}
	else {
		fadeRatio = (timer.ReadMSec() / 1000.0f) / activePreset->fadeOut;
		fadeRatio = b2Clamp(fadeRatio, 0.0f, 255.0f);
		if (fadeRatio >= 1.0f) {
			activePreset->fadeOut;
			timerActive = false;
			currentStep = ReloadStep::RELOAD;
		}
	}
}

void Reload::ReloadModules()
{


	for (ListItem<Module*>* item = activePreset->unload.end; item != nullptr; item = item->prev)
	{
		if (item->data != nullptr) item->data->Disable();
	}

	// Algunos modulos requieren recargar la configuraci�n al reiniciarse
	for (ListItem<Module*>* item = activePreset->load.start; item != nullptr; item = item->next)
	{
		if (item->data != nullptr && item->data->needsAwaking && !item->data->awoken) {
			pugi::xml_node config = app->GetConfig(*(item->data));
			item->data->Awake(config);
		}
	}

	for (ListItem<Module*>* item = activePreset->load.start; item != nullptr; item = item->next)
	{
		if (item->data != nullptr) item->data->Enable();
	}

	currentStep = ReloadStep::FADE_IN;
}

void Reload::FadeIn()
{
	if (!timerActive) {
		timer.Start();
		timerActive = true;
	}
	else {
		fadeRatio = 1 - ((timer.ReadMSec() / 1000.0f) / activePreset->fadeIn);
		fadeRatio = b2Clamp(fadeRatio, 0.0f, 255.0f);
		if (fadeRatio <= b2_epsilon) {
			activePreset = nullptr;
			timerActive = false;
			currentStep = ReloadStep::NONE;
			fadeRatio = 0.0f;
		}
	}
}
