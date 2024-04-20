#ifndef __ACTOR_H__
#define __ACTOR_H__

#include "Constants.h"
#include "Dialogue.h"
#include "Helper.h"
#include "Image.h"
#include "Sfx.h"
#include <SDL_render.h>
#include <limits>
#include <optional>
#include <string>

class Collider {
  public:
    float w = 0;
    float h = 0;
    float half_w = 0.0;
    float half_h = 0.0;

  public:
    Collider() = default;
    Collider(float w, float h) : w(w), h(h), half_w(w * 0.5), half_h(h * 0.5), is_empty(w == 0 && h == 0) {}

    void set(float w, float h) {
        this->w = w;
        this->h = h;
        half_w = w * 0.5;
        half_h = h * 0.5;
        is_empty = (w == 0 && h == 0);
    }

    void set(float w, float h, float scale_x, float scale_y) {
        this->w = w * scale_x;
        this->h = h * scale_y;
        half_w = w * 0.5;
        half_h = h * 0.5;
        is_empty = (w == 0 && h == 0);
    }

    void scale(float scale_x, float scale_y) {
        w *= scale_x;
        h *= scale_y;
        half_w = w * 0.5;
        half_h = h * 0.5;
        is_empty = (w == 0 && h == 0);
    }

    bool empty() const { return is_empty; }

    // Note: two colliders that share a border exactly are not considered colliding / overlapping.
    bool is_inside(float x, float y) const { return x >= 0 && x <= w && y >= 0 && y <= h; }

  private:
    bool is_empty = true;
};

/*
a. view_image : The image that represents this actor during gameplay.
  i. There is no default value. An actor without a view_image property is invisible.
b. transform_scale_x : The x-axis scale factor for this actor’s image.
  i. Default value is 1.0 (double data type)
c. transform_scale_y : The y-axis scale factor for this actor’s image.
  i. Default value is 1.0 (double data type)
d. transform_rotation_degrees : The rotation of this actor’s image in degrees clockwise.
  i. Default value is 0.0 (double data type)
e. view_pivot_offset_x : pivot location (x) for the view image, offset from top-left corner.
  i. Default is actor_view.w * 0.5 (double data type)
f. view_pivot_offset_y : pivot location (y) for the view image, offset from top-left corner.
  i. Default is actor_view.h * 0.5 (double data type)
 */
struct Actor {
  public:
    // *********************** ACTOR PROPERTIES *****************************

    float transform_position_x = 0.0;
    float transform_position_y = 0.0;
    float vel_x = 0.0;
    float vel_y = 0.0;
    // Note: two colliders that share a border exactly are not considered colliding / overlapping.
    Collider box_collider; // Contact box
    Collider box_trigger; // Nearby box
    std::string view_image;
    float transform_scale_x = 1.0;
    float transform_scale_y = 1.0;
    double transform_rotation_degrees = 0.0;
    float view_pivot_offset_x = std::numeric_limits<float>::infinity();
    float view_pivot_offset_y = std::numeric_limits<float>::infinity();
    std::optional<int> render_order;
    std::string view_image_back;
    bool movement_bounce_enabled = false;

    // NON-PLAYER ACTORS ONLY.
    Dialogue dialogues[2];

    // PLAYER ACTOR ONLY.
    std::string view_image_damage;

    // NON-PLAYER ACTORS ONLY.
    std::string view_image_attack;

    // a. Played on the frame in which the player actor takes damage (no looping).
    // b. Channel : (current_frame) % 48 + 2.
    //   i. (the +2 prevents us from clobbering channel 0 or 1).
    // PLAYER ACTOR ONLY.
    Sfx damage_sfx;

    // a. This file will play on (without looping) on every 20th frame if the actor is moving.
    // i. (use Helper::GetFrameNumber() % 20 == 0 to perform the check).
    // b. Channel : (current_frame) % 48 + 2.
    // PLAYER ACTOR ONLY.
    Sfx step_sfx;

    // a. Played on the first frame in which nearby_dialogue is processed (no looping).
    // i. (do not repeat the sound if the nearby_dialogue is triggered again).
    // b. Channel : (current_frame) % 48 + 2.
    // NON-PLAYER ACTORS ONLY.
    Sfx nearby_dialogue_sfx;

    // Used by early versions of the game.
    // char view = '?';

    // If true, other actors cannot move through this actor.
    //* HW6: a. The blocking actor property no longer exists.
    // bool blocking = false;

#ifdef DEBUG
    // We don't really care about the actor's name.
    std::string name;
#endif

    // *********************** RUN-TIME PROPERTIES *****************************

    bool score_up_triggered = false; //? For actor or actor's dialogue?
    bool nearby_dialogue_triggered = false;
    Dir up_or_down = Dir::DIR_NONE;
    Dir left_or_right = Dir::DIR_NONE;
    bool moving = false;
    bool in_render_set = false;

    int damage_view_frame = -1; // Last frame to show the damage image.
    int attack_view_frame = -1; // Last frame to show the attack image.

    bool horizontal_flip = false;
    bool vertical_flip = false;
    // std::unordered_set<int> contact_actors_this_frame; //? Or unordered_set<Actor*>?
    int contact_actors_this_frame = -1;

    Collider box_view; // The collider for the view image.
    float render_offset_x = 0.0; // The image offset against the transform_position_x for rendering.
    float render_offset_y = 0.0; // The image offset against the transform_position_y for rendering.

  public:
    Actor() {}

    void init(SDL_Texture *view_image,
              SDL_Texture *view_image_back,
              SDL_Texture *view_image_damage,
              SDL_Texture *view_image_attack) {
        set_image(view_image);
        set_back_image(view_image_back);
        set_damage_image(view_image_damage);
        set_attack_image(view_image_attack);
        box_collider.scale(transform_scale_x, transform_scale_y);
        box_trigger.scale(transform_scale_x, transform_scale_y);

        // Set the view collider size to the maximum of the view image and the back view image.
        float max_w = std::max(si_view_image.width(), si_view_image_back.width());
        float max_h = std::max(si_view_image.height(), si_view_image_back.height());
        box_view.set(max_w / Const::PIXELS_PER_UNIT, max_h / Const::PIXELS_PER_UNIT); // Add some padding.

        render_offset_x = (get_view_image().width() * 0.5 - view_pivot_offset_x) / Const::PIXELS_PER_UNIT;
        render_offset_y = (get_view_image().height() * 0.5 - view_pivot_offset_y) / Const::PIXELS_PER_UNIT;
    }

    inline void damage(Actor &attacker) {
        (void)attacker;
        damage_view_frame = Helper::GetFrameNumber() + Const::DAMAGE_VIEW_FRAMES;
    }

    inline void attack(Actor &target) {
        (void)target;
        attack_view_frame = Helper::GetFrameNumber() + Const::ATTACK_VIEW_FRAMES;
    }

    // Setters
    inline void set_id(int id) { this->id = id; }
    void set_name(const std::string &name);
    void set_image(SDL_Texture *image);
    void set_back_image(SDL_Texture *image);
    void set_damage_image(SDL_Texture *image);
    void set_attack_image(SDL_Texture *image);

    // Getters
    SimpleImage &get_render_image();
    inline int get_id() const { return id; }
    inline SimpleImage &get_view_image() { return si_view_image; }
    inline SimpleImage &get_view_image_back() { return si_view_image_back; }
    inline SimpleImage &get_view_image_damage() { return si_view_image_damage; }
    inline SimpleImage &get_view_image_attack() { return si_view_image_attack; }
    inline const SimpleImage &get_view_image() const { return si_view_image; }
    inline const SimpleImage &get_view_image_back() const { return si_view_image_back; }
    inline const SimpleImage &get_view_image_damage() const { return si_view_image_damage; }
    inline const SimpleImage &get_view_image_attack() const { return si_view_image_attack; }
    inline bool is_player() const { return player; }
    // inline bool is_blocking() const { return blocking; }

    inline int get_dialogue_index(DialogueType type) const { return (id << 1) + int(type); }
    inline static DialogueType get_dialogue_type(int index) { return DialogueType(index & 1); }

    bool contact_with(const Actor &other) const;
    bool nearby_with(const Actor &other) const;

    static Actor &parse_actor(const rapidjson::Value &actor_obj, Actor &actor);

  private:
    int id = -1;
    SimpleImage si_view_image;
    SimpleImage si_view_image_back;
    SimpleImage si_view_image_damage;
    SimpleImage si_view_image_attack;

    bool player = false;
};

// Compare two actors based on their id.
bool cmp_actor_ptrs(const Actor *a, const Actor *b);

// Compare two actors based on their y position and render order or id.
bool cmp_actor_ptrs_for_rendering(const Actor *a, const Actor *b);

bool collide_with(float x1, float y1, const Collider &c1, float x2, float y2, const Collider &c2);

// Check if two actors are colliding.
bool collide_with(const Actor &a1, const Collider &c1, const Actor &a2, const Collider &c2);

struct ActorPtrsCmpForRendering {
    bool operator()(const Actor *a, const Actor *b) const { return cmp_actor_ptrs_for_rendering(a, b); }
};

#endif // __ACTOR_H__
