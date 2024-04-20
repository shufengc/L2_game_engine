#ifndef __STAGE_VIEW_H__
#define __STAGE_VIEW_H__

#include "Audio.h"
#include "Input.h"
#include "Modding.h"
#include "Resources.h"
#include "TextAdventure.h"

class TextAdventure;

class StageView {
  public:
    StageView(TextAdventure &adventure)
        : adventure(adventure), config(adventure.get_config()), renderer(adventure.get_renderer()),
          input(adventure.get_input()), audio(adventure.get_audio()), resources(Resources::get_instance()) {}

    // Destructor.
    virtual ~StageView() {}

    // Update the stage.
    virtual bool update() = 0;

    // Render the stage.
    virtual void render() = 0;

  protected:
    TextAdventure &adventure;
    const GameConfig &config;
    Renderer &renderer;
    Input &input;
    Audio &audio;
    Resources &resources;
};

#endif // __STAGE_VIEW_H__
