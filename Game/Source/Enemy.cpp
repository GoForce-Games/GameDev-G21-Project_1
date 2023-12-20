#include "Enemy.h"
#include "App.h"

#include "Map.h"

#include "Pathfinding.h"

bool Enemy::Update(float dt)
{
    // TODO: General enemy behaviour (recalculate path, roam home, etc.)
    bool ret = true;

    ret |= EnemyBehaviour(dt);

    //Update enemy position in pixels
    position.x = METERS_TO_PIXELS(pbody->body->GetTransform().p.x);
    position.y = METERS_TO_PIXELS(pbody->body->GetTransform().p.y);

	if (app->debug) {
		for (ListItem<iPoint>* item = pathToPlayer.start; item; item=item->next)
		{
			iPoint worldPos = app->map->MapToWorld(item->data.x, item->data.y);
			SDL_Rect rect = { worldPos.x, worldPos.y, app->map->mapData.tileWidth, app->map->mapData.tileHeight };
			app->render->DrawRectangle(rect, 0, 0, 255, 150);
		}
	}
	if (currentAnimation != nullptr) {
		iPoint offset = GetOrigin();
		app->render->DrawTexture(texture, position.x - offset.x, position.y - offset.y, &currentAnimation->GetCurrentFrame());
		currentAnimation->Update();
	}

    return ret;
}

bool Enemy::FindPath(iPoint& destination)
{
	if (pfCooldown.ReadMSec() < 500) {
		return false;
	}
		pfCooldown.Start();

    // TODO: Define pathfinding from center of mass instead of top-left corner
	iPoint thisPos = app->map->WorldToMap(position.x, position.y); thisPos.y;
	iPoint destPos = app->map->WorldToMap(destination.x, destination.y); destPos.y;
	bool ret = app->map->pathfinding->CreatePath(thisPos, destPos) > 0;
    pathToPlayer.Clear();
	if (!ret) return ret;
    const DynArray<iPoint>* d = app->map->pathfinding->GetLastPath();
    for (size_t i = 0; i < d->Count(); i++)
    {
        pathToPlayer.Add(*d->At(i));
    }

    return true;
}

void Enemy::LoadAllAnimations()
{
	for (pugi::xml_node animNode = parameters.child("animation"); animNode; animNode = animNode.next_sibling())
	{
		Animation* anim = new Animation();

		anim->name = animNode.attribute("name").as_string();
		anim->speed = animNode.attribute("speed").as_float();
		anim->loop = animNode.attribute("loop").as_bool();
		anim->pingpong = animNode.attribute("pingpong").as_bool();

		for (pugi::xml_node frameNode = animNode.child("frame"); frameNode; frameNode = frameNode.next_sibling())
		{
			int x = frameNode.attribute("x").as_int();
			int y = frameNode.attribute("y").as_int();
			int w = frameNode.attribute("w").as_int();
			int h = frameNode.attribute("h").as_int();
			anim->PushBack({ x,y,w,h });
		}
		animationList.Add(anim);
	}

}

Animation* Enemy::GetAnimation(SString name)
{
	for (ListItem<Animation*>* item = animationList.start; item != nullptr; item = item->next)
	{
		if (item->data != nullptr) {
			if (item->data->name == name) return item->data;
		}
	}
	return nullptr;
}
