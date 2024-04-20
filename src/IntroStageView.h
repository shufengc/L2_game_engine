#ifndef __INTRO_STAGE_VIEW_H__
#define __INTRO_STAGE_VIEW_H__

#include "StageView.h"

class IntroStageView : public StageView {
  public:
    IntroStageView(TextAdventure &adventure) : StageView(adventure) {
        // Start playing the intro bgm
        if (!config.intro_bgm.empty()) {
            audio.play(resources.get_audio(config.intro_bgm), 0, -1);
        }
    }

    // Destructor.
    ~IntroStageView() {}

    // Update the stage.
    virtual bool update() override;

    // Render the stage.
    virtual void render() override;

  private:
    unsigned int intro_img_index = 0; // The index of the intro image to render.
};

#endif // __INTRO_STAGE_VIEW_H__
