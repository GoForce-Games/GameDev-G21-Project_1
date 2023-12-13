#include "Entity.h"
#include "Camera.h"

bool Entity::CleanUp()
{
	

	if (boundCam != nullptr)
		boundCam->SetTarget(nullptr);
	renderable = false;

	return true;
}
