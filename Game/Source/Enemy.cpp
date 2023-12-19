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

    app->render->DrawTexture(texture, position.x, position.y);

    return ret;
}

bool Enemy::FindPath(iPoint& destination)
{
    // TODO: Define pathfinding
    app->map->pathfinding->CreatePath(position, destination);
    const DynArray<iPoint>* d = app->map->pathfinding->GetLastPath();
    pathToPlayer.Clear();
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
