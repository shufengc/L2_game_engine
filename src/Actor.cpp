#include "Actor.h"
#include "Constants.h"
#include "Utils.h"
#include <cmath>
#include <limits>

bool cmp_actor_ptrs(const Actor *a, const Actor *b) {
    return a->get_id() < b->get_id();
}

bool cmp_actor_ptrs_for_rendering(const Actor *a, const Actor *b) {
    // Sometimes, we may want to force a certain render order for an actor, and not use sorting on y-pos
    // If an actor has a render_order property defined, it will use that number for sorting.
    // i. (ie, render_order replaces transform_position_y during the sort)
    // 1. Tip: std::optional can make non-nullable types (int) take null as a value.
    // c. If two actors have the same render_order and the same y position, break ties based on
    // their actor id.

    float order1 = a->render_order.has_value() ? float(a->render_order.value()) : a->transform_position_y;
    float order2 = b->render_order.has_value() ? float(b->render_order.value()) : b->transform_position_y;

    return order1 < order2 || (order1 == order2 && a->get_id() < b->get_id());
}

// Parse an actor from a JSON object.
Actor &Actor::parse_actor(const rapidjson::Value &actor_obj, Actor &actor) {
    // Parse actor name.
    // Note: the player actor must have the name "player".
    // If the actor name is not set, it is assumed to be a non-player actor.
    // Non-player actors' name are not important.
    std::string name;
    if (json_get_val(actor_obj, "name", name)) {
        actor.set_name(name);
    }

    json_get_val(actor_obj, "transform_position_x", actor.transform_position_x);
    json_get_val(actor_obj, "transform_position_y", actor.transform_position_y);
    json_get_val(actor_obj, "vel_x", actor.vel_x);
    json_get_val(actor_obj, "vel_y", actor.vel_y);
    json_get_val(actor_obj, "view_image", actor.view_image);
    json_get_val(actor_obj, "transform_scale_x", actor.transform_scale_x);
    json_get_val(actor_obj, "transform_scale_y", actor.transform_scale_y);
    json_get_val(actor_obj, "transform_rotation_degrees", actor.transform_rotation_degrees);
    json_get_val(actor_obj, "view_pivot_offset_x", actor.view_pivot_offset_x);
    json_get_val(actor_obj, "view_pivot_offset_y", actor.view_pivot_offset_y);
    json_get_val(actor_obj, "render_order", actor.render_order);
    json_get_val(actor_obj, "view_image_back", actor.view_image_back);
    json_get_val(actor_obj, "movement_bounce_enabled", actor.movement_bounce_enabled);
    json_get_val(actor_obj, "box_collider_width", actor.box_collider.w);
    json_get_val(actor_obj, "box_collider_height", actor.box_collider.h);
    json_get_val(actor_obj, "box_trigger_width", actor.box_trigger.w);
    json_get_val(actor_obj, "box_trigger_height", actor.box_trigger.h);
    // json_get_val(actor_obj, "view", actor.view);
    // json_get_val(actor_obj, "blocking", actor.blocking);

    if (actor.is_player()) { // Extract PLAYER-ONLY properties
        json_get_val(actor_obj, "view_image_damage", actor.view_image_damage);
        json_get_val(actor_obj, "damage_sfx", actor.damage_sfx);
        json_get_val(actor_obj, "step_sfx", actor.step_sfx);
    } else { // Extract NON-PLAYER-ONLY properties
        json_get_val(actor_obj, "view_image_attack", actor.view_image_attack);
        json_get_val(actor_obj, "nearby_dialogue_sfx", actor.nearby_dialogue_sfx);

        std::string dialogue;

        if (json_get_val(actor_obj, "nearby_dialogue", dialogue)) {
            actor.dialogues[NEARBY].set_dialogue(dialogue);
        }

        json_get_val(actor_obj, "contact_dialogue", dialogue);
        actor.dialogues[CONTACT].set_dialogue(dialogue);
    }

    return actor;
}

void Actor::set_name(const std::string &name) {
#ifdef DEBUG
    this->name = name;
#endif

    // Note: the player actor must have the name "player".
    // If the actor name is not set, it is assumed to be a non-player actor.
    // Non-player actors' name are not important.
    if (name == Const::PLAYER_NAME) {
        player = true;
    }
}

void Actor::set_image(SDL_Texture *image) {
    int image_width, image_height;
    SDL_QueryTexture(image, nullptr, nullptr, &image_width, &image_height);

    if (transform_scale_x < 0) { // If the scale is negative, flip the image.
        horizontal_flip = true;
        transform_scale_x = -transform_scale_x;
    }
    if (transform_scale_y < 0) { // If the scale is negative, flip the image.
        vertical_flip = true;
        transform_scale_y = -transform_scale_y;
    }

    // offset.x = std::abs(std::round(imageSize.x / 2.0) * this->scale.x);

    // Apply the scale to the pivot offset
    if (view_pivot_offset_x == std::numeric_limits<double>::infinity()) {
        // view_pivot_offset_x = std::round(image_width * 0.5);
        view_pivot_offset_x = std::round(image_width * 0.5) * transform_scale_x;
    } else {
        view_pivot_offset_x = std::round(view_pivot_offset_x * transform_scale_x);
    }
    if (view_pivot_offset_y == std::numeric_limits<double>::infinity()) {
        // view_pivot_offset_y = std::round(image_height * 0.5);
        view_pivot_offset_y = std::round(image_height * 0.5) * transform_scale_y;
    } else {
        view_pivot_offset_y = std::round(view_pivot_offset_y * transform_scale_y);
    }

    // Apply the scale to the image width and height
    image_width = round_int(image_width * transform_scale_x);
    image_height = round_int(image_height * transform_scale_y);

    this->si_view_image.set_texture(image, image_width, image_height);
}

void Actor::set_back_image(SDL_Texture *image) {
    if (image == nullptr) {
        return;
    }

    int image_width, image_height;
    SDL_QueryTexture(image, nullptr, nullptr, &image_width, &image_height);

    // Apply the scale to the image width and height
    image_width = round_int(image_width * transform_scale_x);
    image_height = round_int(image_height * transform_scale_y);

    this->si_view_image_back.set_texture(image, image_width, image_height);
}

void Actor::set_damage_image(SDL_Texture *image) {
    if (image == nullptr) {
        return;
    }

    int image_width, image_height;
    SDL_QueryTexture(image, nullptr, nullptr, &image_width, &image_height);

    // Apply the scale to the image width and height
    image_width = round_int(image_width * transform_scale_x);
    image_height = round_int(image_height * transform_scale_y);

    this->si_view_image_damage.set_texture(image, image_width, image_height);
}

void Actor::set_attack_image(SDL_Texture *image) {
    if (image == nullptr) {
        return;
    }

    int image_width, image_height;
    SDL_QueryTexture(image, nullptr, nullptr, &image_width, &image_height);

    // Apply the scale to the image width and height
    image_width = round_int(image_width * transform_scale_x);
    image_height = round_int(image_height * transform_scale_y);

    this->si_view_image_attack.set_texture(image, image_width, image_height);
}

bool Actor::contact_with(const Actor &other) const {
    // box_collider with other.box_collider
    return collide_with(*this, box_collider, other, other.box_collider);
}

bool Actor::nearby_with(const Actor &other) const {
    // b. If the player actor's box trigger overlaps with another actor's box trigger...
    //! 8. If the player actor's box collider overlaps with another actor's trigger. THIS IS CLARIFIED AS AN ERROR.
    assert(is_player());
    return collide_with(*this, box_trigger, other, other.box_trigger);
}

bool collide_with(float x1, float y1, const Collider &c1, float x2, float y2, const Collider &c2) {
    if (c1.empty() || c2.empty()) {
        return false;
    }

    float x1min = x1 - c1.half_w;
    float x1max = x1 + c1.half_w;
    float x2min = x2 - c2.half_w;
    float x2max = x2 + c2.half_w;

    if (x1min < x2max && x1max > x2min) {
        float y1min = y1 - c1.half_h;
        float y1max = y1 + c1.half_h;
        float y2min = y2 - c2.half_h;
        float y2max = y2 + c2.half_h;

        return y1min < y2max && y1max > y2min;
    }

    return false;
}

bool collide_with(const Actor &a1, const Collider &c1, const Actor &a2, const Collider &c2) {
    return collide_with(a1.transform_position_x, a1.transform_position_y, c1, a2.transform_position_x,
                        a2.transform_position_y, c2);
}

SimpleImage &Actor::get_render_image() {
    int cur_frame = Helper::GetFrameNumber();

    // If the actor is attacking, return the attack image.
    if (cur_frame < attack_view_frame) {
        return !si_view_image_attack.empty() ? si_view_image_attack : si_view_image;
    }

    // If the actor is damaged, return the damage image.
    if (cur_frame < damage_view_frame) {
        return !si_view_image_damage.empty() ? si_view_image_damage : si_view_image;
    }

    // a. When the actor moves upwards, this image (view_image_back) shows. It reverts back upon moving downwards.
    if (up_or_down == Dir::DIR_UP) {
        return !si_view_image_back.empty() ? si_view_image_back : si_view_image;
    }

    return si_view_image;
}
