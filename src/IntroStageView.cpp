#include "IntroStageView.h"
#include "Modding.h"
#include "Renderer.h"
#include "Text.h"
#include <string>

using namespace std;

// Update the stage.
bool IntroStageView::update() {
    bool go_to_next_stage = false;

    // If the user left-clicks, presses spacebar, or presses enter, advance to the next image
    if (input.GetMouseButtonDown(SDL_MOUSEBUTTONDOWN) || input.GetKeyDown(SDL_SCANCODE_SPACE) ||
        input.GetKeyDown(SDL_SCANCODE_RETURN)) {
        intro_img_index++;
        if (intro_img_index >= config.intro_images.size() && intro_img_index >= config.intro_texts.size()) {
            go_to_next_stage = true;

            // Stop playing the intro bgm and start playing the gameplay_audio
            audio.halt(0);
            if (!config.gameplay_audio.empty()) {
                audio.play(resources.get_audio(config.gameplay_audio), 0, -1);
            }
        }
    }

    return go_to_next_stage;
}

// Render the stage.
// INTRO STAGE: Show the intro image
void IntroStageView::render() {
    //* It is guaranteed that one of the intro_images or intro_texts is not used up at this point.

    // If intro_images are used up, render the last intro image
    const string &img_name = intro_img_index < config.intro_images.size() ? config.intro_images[intro_img_index]
                                                                          : config.intro_images.back();
    renderer.render_image(resources.get_image(img_name), 0, 0, config.x_resolution, config.y_resolution);

    // If intro_texts are used up, do not render text // TODO: optimize this
    if (intro_img_index < config.intro_texts.size()) {
        const Text &text = config.intro_texts[intro_img_index];

        int w, h;
        SDL_Texture *texture = text.get_texture(w, h);
        renderer.render_texture(texture, 25, config.y_resolution - 50, w, h);
    }
}
