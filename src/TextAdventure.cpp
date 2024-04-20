#include "TextAdventure.h"
#include "Constants.h"
#include "EndStageView.h"
#include "IntroStageView.h"
#include "MainStageView.h"
#include <iostream>
#include <string>

using namespace std;

TextAdventure::TextAdventure() : renderer(config) {
    // Check if the RESOURCES_PATH directory exists
    if (!std::filesystem::exists(Const::RESOURCES_PATH)) {
        std::cout << "error: " << Const::RESOURCES_PATH << " missing";
        exit(0);
    }

    // Load the game config
    config.load_game_config();

    // Load the font before creating the window
    if (!config.font.empty()) {
        Resources::get_instance().get_font(config.font);
    }
}

// Text adventure game.
void TextAdventure::start() {
    // Create the window
    renderer.create_window(config.game_title, config.x_resolution, config.y_resolution);

    // If intro_image is defined
    if (config.intro_images.size() > 0) {
        stage = GAME_STAGE_INTRO;
        view = new IntroStageView(*this);
    }
    // If no intro_image is defined, do not render or process any intro_text
    else {
        stage = GAME_STAGE_MAIN;
        view = new MainStageView(*this);
    }

#ifdef DEBUG
    size_t rendered_frames = 0; // The number of frames that were rendered
    // size_t skiped_frames = 0; // The number of frames that were not rendered
#endif

    // Main game loop
    while (state != GAME_STAT_QUIT) {
        SDL_Event event;

        // Process SDL events
        while (Helper::SDL_PollEvent498(&event)) {
            if (event.type == SDL_QUIT) {
                state = GAME_STAT_QUIT;
                break;
            }

            // Pass events to the input manager
            input.ProcessEvent(event);
        }

        // Update the game state
        bool go_to_next_stage = view->update();

        // Clear the renderer
        renderer.clear();

        // Render the game state
        view->render();

#ifdef DEBUG
        rendered_frames++;
#endif

        if (go_to_next_stage) {
            if (stage == GAME_STAGE_INTRO) {
                delete view;
                view = new MainStageView(*this);
                stage = GAME_STAGE_MAIN;
            } else if (stage == GAME_STAGE_MAIN) {
                delete view;
                view = new EndStageView(*this);
                stage = GAME_STAGE_END;
            } else if (stage == GAME_STAGE_END) {
                delete view;
                view = nullptr;
                state = GAME_STAT_QUIT;
            }
        }

        // Show the renderer
        renderer.present();
    }

#ifdef DEBUG
    std::cout << "\n";
    std::cout << "=================================================================================================\n";
    std::cout << "rendered frames: " << rendered_frames << "\n";
    std::cout << "=================================================================================================\n";
#endif
}

// Destructor.
TextAdventure::~TextAdventure() {
    delete view;
}
