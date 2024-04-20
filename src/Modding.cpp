#include "Modding.h"
#include "Utils.h"
#include <filesystem>
#include <iostream>

void GameConfig::load_game_config_file(const std::string &path) {
    rapidjson::Document document;
    ReadJsonFile(path, document);

    // json_get_val(document, "game_start_message", game_start_message);
    // json_get_val(document, "game_over_good_message", game_over_good_message);
    // json_get_val(document, "game_over_bad_message", game_over_bad_message);
    json_get_val(document, "game_title", game_title);
    json_get_val(document, "intro_image", intro_images);
    json_get_val(document, "intro_text", intro_texts);
    json_get_val(document, "intro_bgm", intro_bgm);
    json_get_val(document, "hp_image", hp_image);
    json_get_val(document, "gameplay_audio", gameplay_audio);
    json_get_val(document, "game_over_good_image", game_over_good_image);
    json_get_val(document, "game_over_bad_image", game_over_bad_image);
    json_get_val(document, "game_over_good_audio", game_over_good_audio);
    json_get_val(document, "game_over_bad_audio", game_over_bad_audio);
    json_get_val(document, "player_movement_speed", player_movement_speed);

    std::string score_sfx;
    json_get_val(document, "score_sfx", score_sfx);
    this->score_sfx.set_file_path(score_sfx);

    if (!json_get_val(document, "initial_scene", initial_scene_name)) {
        std::cout << "error: initial_scene unspecified";
        exit(0);
    }

    // If intro_text is specified in game.config but no font is also specified...
    // a. Print error: text render failed. No font configured
    // b. Exit the program
    if (!json_get_val(document, "font", font) && !intro_texts.empty()) {
        std::cout << "error: text render failed. No font configured";
        exit(0);
    }
}

void GameConfig::load_rendering_config_file(const std::string &path) {
    rapidjson::Document document;
    ReadJsonFile(path, document);

    json_get_val(document, "x_resolution", x_resolution);
    json_get_val(document, "y_resolution", y_resolution);
    json_get_val(document, "clear_color_r", clear_color_r);
    json_get_val(document, "clear_color_g", clear_color_g);
    json_get_val(document, "clear_color_b", clear_color_b);
    json_get_val(document, "cam_offset_x", cam_offset_x);
    json_get_val(document, "cam_offset_y", cam_offset_y);
    json_get_val(document, "zoom_factor", zoom_factor);
    json_get_val(document, "cam_ease_factor", cam_ease_factor);
    json_get_val(document, "x_scale_actor_flipping_on_movement", x_scale_actor_flipping_on_movement);

    assert(x_resolution > 0 && "x_resolution must be greater than 0");
    assert(y_resolution > 0 && "y_resolution must be greater than 0");
    assert(clear_color_r >= 0 && clear_color_r <= 255 && "clear_color_r must be in the range [0, 255]");
    assert(clear_color_g >= 0 && clear_color_g <= 255 && "clear_color_g must be in the range [0, 255]");
    assert(clear_color_b >= 0 && clear_color_b <= 255 && "clear_color_b must be in the range [0, 255]");
}

void GameConfig::load_game_config() {
    // Load game.config anyway
    if (std::filesystem::exists(Const::GAME_CONFIG_PATH)) {
        load_game_config_file(Const::GAME_CONFIG_PATH);
    } else {
        std::cout << "error: " << Const::GAME_CONFIG_PATH << " missing";
        exit(0);
    }

    // Load rendering.config, if it exists
    if (std::filesystem::exists(Const::RENDERING_CONFIG_PATH)) {
        load_rendering_config_file(Const::RENDERING_CONFIG_PATH);
    }
}
