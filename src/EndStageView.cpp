#include "EndStageView.h"

// Update the stage.
bool EndStageView::update() {
    bool go_to_next_stage = false;

#ifdef DEBUG
    // Pressing ESC key should quit the game.
    // This is an extension of the original game.
    if (input.GetKeyDown(SDL_SCANCODE_ESCAPE)) {
        go_to_next_stage = true;
    }
#endif

    return go_to_next_stage;
}

// Render the stage.
void EndStageView::render() {
    SDL_Texture *texture = nullptr;

    GameResult result = adventure.get_game_result();

    if (result == GAME_RESULT_WIN) {
        texture = resources.get_image(config.game_over_good_image);
    } else if (result == GAME_RESULT_LOSE) {
        texture = resources.get_image(config.game_over_bad_image);
    }

    if (texture) {
        renderer.render_image(texture, 0, 0, config.x_resolution, config.y_resolution);
    } else {
        // If neither game_over_good_image or game_over_bad_image exists in game.config, you should end the game
        // immediately.
        // state = GAME_STAT_QUIT;
        adventure.set_game_state(GAME_STAT_QUIT);
    }
}
