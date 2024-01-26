#ifndef __GUIVALUEBOX_H__
#define __GUIVALUEBOX_H__

#include "GuiControl.h"

#include "Point.h"
#include "SString.h"

class GuiValuebox : public GuiControl
{

public:

	GuiValuebox(uint32 id, SDL_Rect bounds, const char* text);
	virtual ~GuiValuebox();

	// Called each loop iteration
	bool Update(float dt);

private:

	int mouseX, mouseY;
	unsigned int click;

	bool canClick = true;
	bool drawBasic = false;
};

#endif // __GUIVALUEBOX_H__