#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "Actor.h"
#include "Constants.h"
#include "Helper.h"
#include "Text.h"
#include <unordered_set>

// The player.
class Player {
  public:
    Player() { set_score_text(); }

    void set_actor(Actor *actor) { this->actor = actor; }
    void set_image(SDL_Texture *hp_image);

    void reset() { actor = nullptr; }

    // Damage the player.
    // The player cannot take damage more than once every PLAYER_COOLDOWN_FRAMES frames.
    // The amount must be positive.
    bool damage(Actor &attacker, int amount = 1);

    // Add score to the player.
    // The amount must be positive.
    void gain_score(int amount = 1);

    bool is_dead() const { return health <= 0; }
    bool is_alive() const { return health > 0; }
    bool is_cooldown() const { return Helper::GetFrameNumber() < cooldown_frame; }

    int get_health() const { return health; }
    int get_score() const { return score; }
    Actor *get_actor() const { return actor; }
    SDL_Texture *get_hp_image() const { return hp_image; }
    int get_hp_image_width() const { return hp_image_width; }
    int get_hp_image_height() const { return hp_image_height; }
    Text &get_score_text() { return score_text; }

    void add_nearby_actor(int id) { nearby_actors.insert(id); }
    void add_contact_actor(int id) { contact_actors.insert(id); }

    void clear_nearby_actors() { nearby_actors.clear(); }
    void clear_contact_actors() { contact_actors.clear(); }

    std::unordered_set<int> &get_nearby_actors() { return nearby_actors; }
    std::unordered_set<int> &get_contact_actors() { return contact_actors; }

    const std::unordered_set<int> &get_nearby_actors() const { return nearby_actors; }
    const std::unordered_set<int> &get_contact_actors() const { return contact_actors; }

  private:
    void set_score_text() { score_text.set_text("score : " + std::to_string(score)); }

  private:
    int health = Const::PLAYER_HEALTH;
    int score = 0;
    Actor *actor = nullptr;
    SDL_Texture *hp_image = nullptr;
    int hp_image_width = 0;
    int hp_image_height = 0;

    int cooldown_frame = 0; // Cooldown: If the player takes damage on frame x, they cannot
                            // take more until frame x + 180

    Text score_text;
    std::unordered_set<int> nearby_actors; //? Or unordered_set<Actor*>?
    std::unordered_set<int> contact_actors; //? Or unordered_set<Actor*>?
};

#endif // __PLAYER_H__
