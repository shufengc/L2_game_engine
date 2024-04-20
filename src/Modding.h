#ifndef __MODDING_H__
#define __MODDING_H__

#include "Constants.h"
#include "Sfx.h"
#include "Text.h"
#include <string>
#include <vector>

// #define NOT_PRESENT_STR "@NOT_PRESENT_STR$"

class GameConfig {
  public:
    // resources/game.config
    // std::string game_start_message = NOT_PRESENT_STR;
    // std::string game_over_good_message;
    // std::string game_over_bad_message;
    std::string initial_scene_name;
    std::string game_title;
    std::vector<std::string> intro_images;
    std::vector<Text> intro_texts;
    std::string font;
    std::string intro_bgm;
    std::string hp_image;
    std::string gameplay_audio;
    std::string game_over_good_image;
    std::string game_over_bad_image;
    std::string game_over_good_audio;
    std::string game_over_bad_audio;
    double player_movement_speed = 0.02;
    Sfx score_sfx; // a. Played on the frame in which the player's score increases. b. Channel : 1

    // resources/rendering.config
    int x_resolution = Const::DEFAULT_WINDOW_WIDTH;
    int y_resolution = Const::DEFAULT_WINDOW_HEIGHT;
    int clear_color_r = 255;
    int clear_color_g = 255;
    int clear_color_b = 255;
    double cam_offset_x = 0.0;
    double cam_offset_y = 0.0;
    double zoom_factor = 1.0;
    double cam_ease_factor = 1.0;
    bool x_scale_actor_flipping_on_movement = false;

    void load_game_config();

  private:
    void load_game_config_file(const std::string &path);
    void load_rendering_config_file(const std::string &path);
};

#endif // __MODDING_H__
