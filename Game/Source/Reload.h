#pragma once
#include "Module.h"
#include "List.h"

class Reload : public Module
{
public:
    Reload();

    // Destructor
    virtual ~Reload();

    // Called before render is available
    bool Awake(pugi::xml_node& conf);

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    // Called before quitting
    bool CleanUp();

public:

    List<Module*> moduleList;

    bool reload = false;

};

