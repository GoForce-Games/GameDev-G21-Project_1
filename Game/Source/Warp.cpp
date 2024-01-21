#include "Warp.h"

Warp::Warp() : Entity(EntityType::WARP)
{
	name.Create("warp");
}

Warp::~Warp()
{
}

bool Warp::Awake()
{
	return true;
}

bool Warp::Update(float dt)
{
	return true;
}

bool Warp::CleanUp()
{
	return true;
}

void Warp::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
}
