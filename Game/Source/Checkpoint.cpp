#include "Checkpoint.h"

Checkpoint::Checkpoint() : Entity(EntityType::CHECKPOINT)
{
	name.Create("checkpoint");
}

Checkpoint::~Checkpoint()
{
}

bool Checkpoint::Awake()
{
	return true;
}

bool Checkpoint::Update(float dt)
{
	return true;
}

bool Checkpoint::CleanUp()
{
	return true;
}

void Checkpoint::OnCollision(PhysBody* physA, PhysBody* physB, b2Contact* contactInfo)
{
}
