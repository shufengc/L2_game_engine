#ifndef __DIALOGUE_H__
#define __DIALOGUE_H__

#include "Text.h"
#include "Utils.h"
#include <SDL_render.h>
#include <iostream>
#include <rapidjson/document.h>
#include <string>

enum DialogueType { NEARBY, CONTACT };

class Dialogue {
  public:
    // Set the dialogue string.
    void set_dialogue(const std::string &dialogue) {
        this->text.set_text(dialogue);

        //? Is it possible that the dialogue contains multiple phrases?
        if (dialogue.find("health down") != std::string::npos) {
            health_down = true;
        } else if (dialogue.find("score up") != std::string::npos) {
            score_up = true;
        } else if (dialogue.find("you win") != std::string::npos) {
            you_win = true;
        } else if (dialogue.find("game over") != std::string::npos) {
            game_over = true;
        } else if (dialogue.find("proceed to") != std::string::npos) {
            proceed_to = true;
            proceed_to_name = obtain_word_after_phrase(dialogue, "proceed to");
        }
    }

    // Get the dialogue string.
    const std::string &get() const { return text.get(); }

    // Check if the dialogue is empty.
    bool empty() const { return text.get().empty(); }

    // Getters.
    bool is_health_down() const { return health_down; }
    bool is_score_up() const { return score_up; }
    bool is_you_win() const { return you_win; }
    bool is_game_over() const { return game_over; }
    bool is_proceed_to() const { return proceed_to; }
    const std::string &get_proceed_to_name() const { return proceed_to_name; }

    // Get the dialogue text object.
    Text &get_text() { return text; }

    // Return the dialogue as a string.
    operator const std::string &() const { return text; }
    operator const char *() const { return text; }

    // Print the dialogue string.
    friend std::ostream &operator<<(std::ostream &os, const Dialogue &dialogue) {
        os << dialogue.text;
        return os;
    }

  private:
    bool health_down = false;
    bool score_up = false;
    bool you_win = false;
    bool game_over = false;
    bool proceed_to = false;
    std::string proceed_to_name;

    Text text;
};

// Specializations for `Dialogue` type;
template <> inline Dialogue &json_get<Dialogue>(const rapidjson::Value &value, Dialogue &target) {
    target.set_dialogue(value.GetString());
    return target;
}

#endif // __DIALOGUE_H__
