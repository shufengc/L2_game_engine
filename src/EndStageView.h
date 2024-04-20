#ifndef __END_STAGE_VIEW_H__
#define __END_STAGE_VIEW_H__

#include "StageView.h"

class EndStageView : public StageView {
  public:
    EndStageView(TextAdventure &adventure) : StageView(adventure) {}

    // Destructor.
    ~EndStageView() {}

    // Update the stage.
    virtual bool update() override;

    // Render the stage.
    virtual void render() override;

  private:
};

#endif // __END_STAGE_VIEW_H__
