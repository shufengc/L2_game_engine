#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <SDL_pixels.h>
#include <string>
#include <vector>

enum Dir {
    DIR_UP = 0x1,
    DIR_RIGHT = 0x2,
    DIR_DOWN = 0x4,
    DIR_LEFT = 0x8,
    DIR_NONE = 0x0,
    DIR_ALL = 0xF,

    // Diagonal directions.
    DIR_UP_RIGHT = DIR_UP | DIR_RIGHT,
    DIR_DOWN_RIGHT = DIR_DOWN | DIR_RIGHT,
    DIR_DOWN_LEFT = DIR_DOWN | DIR_LEFT,
    DIR_UP_LEFT = DIR_UP | DIR_LEFT
};

struct DirStep {
    std::string dir;
    int dx;
    int dy;
};

#ifdef DEBUG
// Move through w, a, s, d in DEBUG mode which is more intuitive.
const std::vector<DirStep> DIR_STEPS = {
    {"w", 0, -1},
    {"d", 1, 0},
    {"s", 0, 1},
    {"a", -1, 0},
};
#else
const std::vector<DirStep> DIR_STEPS = {
    {"n", 0, -1},
    {"e", 1, 0},
    {"s", 0, 1},
    {"w", -1, 0},
};
#endif

class Const {
  public:
    inline static const int PIXELS_PER_UNIT = 100;
    inline static const int DEFAULT_WINDOW_WIDTH = 640;
    inline static const int DEFAULT_WINDOW_HEIGHT = 360;
    inline static const int PLAYER_HEALTH = 3;
    inline static const int PLAYER_COOLDOWN_FRAMES = 180;

    inline static const char *PLAYER_NAME = "player";

    inline static const SDL_Color TEXT_COLOR = {255, 255, 255, 255};

    inline static const std::string RESOURCES_PATH = "resources/";
    inline static const std::string GAME_CONFIG_PATH = RESOURCES_PATH + "game.config";
    inline static const std::string RENDERING_CONFIG_PATH = RESOURCES_PATH + "rendering.config";
    inline static const std::string FONT_PATH = RESOURCES_PATH + "fonts/";
    inline static const std::string IMAGE_PATH = RESOURCES_PATH + "images/";
    inline static const std::string AUDIO_PATH = RESOURCES_PATH + "audio/";
    inline static const std::string COMP_PATH = RESOURCES_PATH + "component_types/";
    inline static const std::string SCENES_PATH = RESOURCES_PATH + "scenes/";
    inline static const std::string ACTOR_TEMPLATE_PATH = RESOURCES_PATH + "actor_templates/";

    inline static const int NUM_MIX_CHANNELS = 50;

    inline static const int DAMAGE_VIEW_FRAMES = 30;
    inline static const int ATTACK_VIEW_FRAMES = 30;
};

#endif // __CONSTANTS_H__
