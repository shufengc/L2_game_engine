#include "MainStageView.h"
#include "Modding.h"
#include "Renderer.h"
#include "TextAdventure.h"
#include <string>

using namespace std;

MainStageView::MainStageView(TextAdventure &adventure) : StageView(adventure) {
    camera_collider.set(
        (float(config.x_resolution) / Const::PIXELS_PER_UNIT) * (1.0 + camera_collider_padding) / config.zoom_factor,
        (float(config.y_resolution) / Const::PIXELS_PER_UNIT) * (1.0 + camera_collider_padding) / config.zoom_factor);

    // Load the initial scene
    proceed_to(config.initial_scene_name);

    // Load the player's hp image
    if (!config.hp_image.empty()) {
        player.set_image(resources.get_image(config.hp_image));
    }

    // Play the gameplay bgm
    if (!config.gameplay_audio.empty()) {
        audio.play(resources.get_audio(config.gameplay_audio), 0, -1);
    }
}

MainStageView::~MainStageView() {
#ifdef DEBUG
    std::cout << "\n";
    std::cout << "=================================================================================================\n";
    int nearby_total = map[NEARBY].dbg_cnt_moved + map[NEARBY].dbg_cnt_not_moved;
    int contact_total = map[CONTACT].dbg_cnt_moved + map[CONTACT].dbg_cnt_not_moved;
    std::cout << "RangeMap[NEARBY] not_moved/Total/Percentage: " << map[NEARBY].dbg_cnt_not_moved << "/" << nearby_total
              << "/" << (map[NEARBY].dbg_cnt_not_moved * 100.0 / nearby_total) << "%\n";
    std::cout << "RangeMap[CONTACT] not_moved/Total/Percentage: " << map[CONTACT].dbg_cnt_not_moved << "/"
              << contact_total << "/" << (map[CONTACT].dbg_cnt_not_moved * 100.0 / contact_total) << "%\n";
    std::cout << "not_removed_before_updating_for_y_not_changed/Total/Percentage: "
              << dbg_not_removed_before_updating_for_y_not_changed_cnt << "/" << dbg_total_move_actor_cnt << "/"
              << (dbg_not_removed_before_updating_for_y_not_changed_cnt * 100.0 / dbg_total_move_actor_cnt) << "%\n";
    std::cout << "skiped_contact_check/Total/Percentage: " << dbg_skiped_contact_check_cnt << "/"
              << dbg_total_contact_check_cnt << "/"
              << (dbg_skiped_contact_check_cnt * 100.0 / dbg_total_contact_check_cnt) << "%\n";
    std::cout << "=================================================================================================\n";
#endif
}

// Proceed to a new scene.
void MainStageView::proceed_to(const std::string &scene_name) {
#ifdef DEBUG
    cout << "proceeding to " << scene_name << endl;
#endif

    // Reset the game for the new scene
    reset_scene();

    // Load the initial scene
    scene.load_scene(scene_name);

    // Initialize the player
    if (has_player()) {
        // If an actor with the name player exists but no hp_image is specified, print error and exit
        if (config.hp_image.empty()) {
            std::cout << "error: player actor requires an hp_image be defined";
            exit(0);
        }
        player.set_actor(&scene.actors[scene.player_actor_index]); // Using the address of a vector element may be
                                                                   // problematic if the vector is resized.

        // update_camera_position();
        camera_x = player.get_actor()->transform_position_x + config.cam_offset_x;
        camera_y = player.get_actor()->transform_position_y + config.cam_offset_y;
    } else { // No player actor
        camera_x = 0.0;
        camera_y = 0.0;
    }

    // Initialize the actors' view images and pivot offsets
    for (auto &actor : scene.actors) {
        auto view_image = resources.get_image(actor.view_image);
        auto view_image_back = resources.get_image(actor.view_image_back);
        auto view_image_damage = resources.get_image(actor.view_image_damage);
        auto view_image_attack = resources.get_image(actor.view_image_attack);
        actor.init(view_image, view_image_back, view_image_damage, view_image_attack);
    }

    // Add actors to the map
    for (auto &actor : scene.actors) {
        if (!actor.dialogues[NEARBY].empty() && !actor.is_player()) {
            map[NEARBY].add_actor(&actor, actor.box_trigger);
        }
        map[CONTACT].add_actor(&actor, actor.box_collider);

        // Add actors with view images to the render_actors vector
        update_render_set(&actor);
    }

#ifdef DEBUG
    cout << "\nCONTACT MAP:" << endl;
    map[CONTACT].print();
    cout << "\nNEARBY MAP:" << endl;
    map[NEARBY].print();
#endif
}

// Process an SDL event, updating the game state if necessary.
// An SDL event can be a key press, mouse click, etc.
void MainStageView::process_input() {
    // When an arrow key or WASD SDL_KEYDOWN event occurs, move the player actor one unit in that direction

    int dx = 0, dy = 0;

    // TODO: optimize this
    if (input.GetKey(SDL_SCANCODE_UP) || input.GetKey(SDL_SCANCODE_W)) {
        dy += -1;
    }
    if (input.GetKey(SDL_SCANCODE_DOWN) || input.GetKey(SDL_SCANCODE_S)) {
        dy += 1;
    }
    if (input.GetKey(SDL_SCANCODE_RIGHT) || input.GetKey(SDL_SCANCODE_D)) {
        dx += 1;
    }
    if (input.GetKey(SDL_SCANCODE_LEFT) || input.GetKey(SDL_SCANCODE_A)) {
        dx += -1;
    }

    Actor &player_actor = *player.get_actor();

    player_actor.up_or_down = dy > 0 ? Dir::DIR_DOWN : dy < 0 ? Dir::DIR_UP : player_actor.up_or_down;

    if (dx || dy) {
        // Play the step_sfx (without looping) on every 20th frame if the actor is moving
        if (Helper::GetFrameNumber() % 20 == 0) {
            int channel = (Helper::GetFrameNumber()) % 48 + 2;
            audio.play(player_actor.step_sfx, channel, 0);
        }

        if (dx && dy) { // If the player is moving diagonally, the speed should be divided by sqrt(2)
            static const double player_movement_speed_div_sqrt = config.player_movement_speed / sqrt(2);
            static const double player_movement[] = {-player_movement_speed_div_sqrt, 0,
                                                     player_movement_speed_div_sqrt};

            player_actor.vel_x = player_movement[dx + 1];
            player_actor.vel_y = player_movement[dy + 1];
        } else {
            static const double player_movement[] = {-config.player_movement_speed, 0, config.player_movement_speed};

            player_actor.vel_x = player_movement[dx + 1];
            player_actor.vel_y = player_movement[dy + 1];
        }
    } else {
        player_actor.vel_x = 0;
        player_actor.vel_y = 0;
    }

#ifdef DEBUG
    // Pressing ESC key should quit the game.
    // This is an extension of the original game.
    if (input.GetKeyDown(SDL_SCANCODE_ESCAPE)) {
        go_to_next_stage = true;
    }
#endif
}

void MainStageView::update_move_actors() {
    vector<Actor> &actors = scene.actors;
    const vector<size_t> &movable_actors = scene.movable_actors;

    int curframe = Helper::GetFrameNumber();

    // for (auto &actor_id : movable_actors) {
    //     Actor &actor = actors[actor_id];
    //     actor.contact_actors_this_frame = false;
    // }
    player.clear_contact_actors();

    // Update all actors in the order of their appearance
    for (auto &actor_pid : movable_actors) { // TODO: optimize this!!!
        Actor &actor = actors[actor_pid];

        // If the actor is not moving, skip it, mainly for the player actor
        // ! The player actor may collides with other actors at the very beginning
        // ! So we need to check the contact_actors_this_frame even if the player is not moving
        // if (actor.vel_x == 0 && actor.vel_y == 0) {
        //     actor.moving = false;
        //     continue;
        // }

        if (config.x_scale_actor_flipping_on_movement) {
            // i. If the actor attempts to move east, the x component should become positive
            // (regardless of its original value).
            // ii. If the actor attempts to move west, the x component should become negative
            // (regardless of its original value.
            if (actor.vel_x < 0) {
                actor.horizontal_flip = true;
            } else if (actor.vel_x > 0) {
                actor.horizontal_flip = false;
            }
        }

        float old_x = actor.transform_position_x;
        float old_y = actor.transform_position_y;
        float new_x = actor.transform_position_x + actor.vel_x;
        float new_y = actor.transform_position_y + actor.vel_y;

        // Move the actor
        actor.transform_position_x = new_x;
        actor.transform_position_y = new_y;

        // Check if the actor collides with another actor
        for (auto &cell : map[CONTACT].get_cells(actor, actor.box_collider)) {
            for (auto other_id : cell.get().get_actors()) {
                Actor &other = actors[other_id];

                if (actor.get_id() == other.get_id()) {
                    //* CAN'T only check with actors with higher id. For example, if the player is moving, it may
                    //* collide with non-movable actors with lower id.
                    continue;
                }

#ifdef DEBUG
                dbg_total_contact_check_cnt++;
#endif

                if (!actor.is_player() && !other.is_player() && actor.contact_actors_this_frame == curframe &&
                    other.contact_actors_this_frame == curframe) {
#ifdef DEBUG
                    dbg_skiped_contact_check_cnt++;
#endif
                    continue;
                }

                if (actor.contact_with(other)) {
                    actor.contact_actors_this_frame = curframe;
                    other.contact_actors_this_frame = curframe;

                    // If one of the actors is the player, add the other actor to the player's contact actors.
                    // Note that only the player's contact actors will be used to apply contact dialogues.
                    if (actor.is_player()) {
                        player.add_contact_actor(other_id);
                    } else if (other.is_player()) {
                        player.add_contact_actor(actor.get_id());
                    }
                }
            }
        }

        // Reset movement
        actor.transform_position_x = old_x;
        actor.transform_position_y = old_y;

        // If the actor does not collide with another actor, move it
        if (actor.contact_actors_this_frame != curframe && (actor.vel_x != 0 || actor.vel_y != 0)) {
            actor.moving = true;
            actor.up_or_down = actor.vel_y > 0 ? Dir::DIR_DOWN : actor.vel_y < 0 ? Dir::DIR_UP : actor.up_or_down;
            move_actor(actor, new_x, new_y);
        }
        // If the actor collides with another actor, reset its position and reverse its velocity
        else {
            actor.moving = false;

            // Reverse the velocity
            // if (!actor.is_player()) {
            actor.vel_x = -actor.vel_x;
            actor.vel_y = -actor.vel_y;
            // }

            //* Blocked actor should call update_render_set() as well.
            //* It is called in update_non_move_actors() later.
            // update_render_set(&actor);
        }
    }
}

void MainStageView::update_non_move_actors() {
    vector<Actor> &actors = scene.actors;
    // const vector<size_t> &non_movable_actors = scene.non_movable_actors;

    for (auto &actor : actors) { // TODO: check for only actors with view images
        if (!actor.moving) {
            update_render_set(&actor);
        }
    }
}

void MainStageView::update_nearby_dialogues() {
    vector<Actor> &actors = scene.actors;
    Actor &player_actor = *player.get_actor();

    player.clear_nearby_actors();

    //* NOW, ALL ACTORS HAVE MOVED. WE CAN CHECK FOR PLAYER'S NEARBY ACTORS.

    for (auto &cell : map[NEARBY].get_cells(player_actor, player_actor.box_trigger)) {
        for (auto other_id : cell.get().get_actors()) {
            Actor &other = actors[other_id];
            // NOTE: We do not add the player or actors with no nearby dialogues to the map[NEARBY].
            //       So no need to check.
            if (/* !other.is_player() && !other.dialogues[NEARBY].empty() && */ player_actor.nearby_with(other)) {
                player.add_nearby_actor(other.get_id());
            }
        }
    }

    dialogues_to_render.clear();

    for (auto &actor_id : player.get_nearby_actors()) {
        Actor &actor = actors[actor_id];
        dialogues_to_render.emplace_back(actor.get_dialogue_index(NEARBY), &actor);
    }

    // APPLY THE DIALOGUES IN THE ORDER OF THEIR APPEARANCE IN THE ACTORS VECTOR.

    // Sort the dialogues based on the actor id and the type of dialogue.
    // Note that std::pair has a built-in comparison operator that compares the first element first.
    sort(dialogues_to_render.begin(), dialogues_to_render.end()); // TODO: optimize

    // Apply the dialogues
    for (auto &pair : dialogues_to_render) {
        size_t d_index = pair.first;
        Actor &npc = *pair.second;
        DialogueType type = Actor::get_dialogue_type(d_index);
        Dialogue &dialogue = pair.second->dialogues[type];
        apply_dialogue(npc, dialogue, player);
    }

    // Played on the first frame in which nearby_dialogue is processed (no looping).
    //   i. (do not repeat the sound if the nearby_dialogue is triggered again).
    if (dialogues_to_render.size() > 0) {
        for (auto &pair : dialogues_to_render) {
            if (!pair.second->nearby_dialogue_triggered) {
                int channel = (Helper::GetFrameNumber()) % 48 + 2;
                audio.play(dialogues_to_render[0].second->nearby_dialogue_sfx, channel, 0);
                pair.second->nearby_dialogue_triggered = true;
            }
        }
    }
}

void MainStageView::update_contact_dialogues() {
    vector<Actor> &actors = scene.actors;
    Actor &player_actor = *player.get_actor();

    // Apply contact dialogues
    std::vector<std::pair<int, Actor *>> contact_dialogues;
    for (auto &actor_id : player.get_contact_actors()) {
        Actor &actor = actors[actor_id];
        if (!actor.dialogues[CONTACT].empty()) {
            contact_dialogues.emplace_back(actor.get_dialogue_index(CONTACT), &actor);
        }
    }

    // Sort the dialogues based on the actor id and the type of dialogue.
    // Note that std::pair has a built-in comparison operator that compares the first element first.
    sort(contact_dialogues.begin(), contact_dialogues.end()); // TODO: optimize

    // Apply the dialogues
    for (auto &pair : contact_dialogues) {
        size_t d_index = pair.first;
        Actor &npc = *pair.second;
        DialogueType type = Actor::get_dialogue_type(d_index);
        Dialogue &dialogue = pair.second->dialogues[type];
        apply_dialogue(npc, dialogue, player);
    }

    // If there is no player actor, do not update the dialogues.
    // If the player has moved or an NPC has moved, update the dialogues.
    // If there is a health_down dialogue, the dialogue should be applied every frame.
}

// Update the stage.
bool MainStageView::update() {
    /*
    1) Begin iterating through actors, calling Update() on each one.
            1.1) Determine the "desired new position" of the actor (logic different if player vs npc).
            1.2) If the actor has a valid box collider, collect a vector of all other actors colliding with it at
                 "desired new position" (AABB).
                1.2.1) For each of these actors we collide with at "desired new position", add them to our
                       "colliding_actors_this_frame" set...
                1.2.2) ...and add ourselves to the other actor's set.
            1.3) If the colliding_actors_this_frame set has a size larger than 0 (IE, we collided with something)...
                1.3.1) Do not move this frame.
                1.3.2) Invert our velocity (only meaningful for NPCs).
            1.4) If the colliding_actors_this_frame set has a size of 0, move this actor to "desired new position".


    2) Collect and render dialogue (and process dialogue commands). // All actors have been updated at this point.
            2.1) Find any non-player actors in the player's region (or neighboring regions).
            2.2) Perform an AABB test on these "nearby" actors to collect actors whose triggers are overlapping the
                 player trigger.
            2.3) Process and render the nearby_dialogue of these actors.
            2.4) Process the contact_dialogue of any actors that collided (box colliders) with the player earlier during
                 the frame.
    */

    process_input();

#ifdef DEBUG
//     int curframe = Helper::GetFrameNumber();
//     if (curframe >= 40 && curframe < 43) {
//         cout << "\n\nCURFRAME: " << curframe << endl;
//         cout << "\nCONTACT MAP:" << endl;
//         map[CONTACT].print();
//         cout << "\nNEARBY MAP:" << endl;
//         map[NEARBY].print();
//     }
//
//     if (curframe > 100) {
//         exit(0);
//     }
#endif

    // ****************** CHECK COLLISIONS AND MOVE ACTORS *********************

    update_move_actors();

    // ****************** PERFORM NEARBY DIALOGUES *****************************

    update_nearby_dialogues();

    // ****************** PERFORM CONTACT DIALOGUES ****************************

    update_contact_dialogues();

    // ****************** PROCEED TO NEW SCENE IF NECESSARY ********************

    if (proceed_to_next_scene) {
        proceed_to(next_scene_name);
        proceed_to_next_scene = false;
    }

    // ****************** UPDATE CAMERA POSITION *******************************

    // b. Every frame, after updating all actors, update the camera's location with a smooth ease.
    //   i. Jump cam_ease_factor of the way from cam's current position to the player pos.
    //   ii. Consider using new_cam_pos = glm::mix(current_cam_pos, player_current_pos, cam_ease_factor);
    if (update_camera_position()) {
        update_non_move_actors();
    }

#ifdef DEBUG
    // cout << "\n\nCURFRAME: " << curframe << endl;
    // cout << "\nRENDER MAP:" << endl;
    // render_map.print();
#endif

    return go_to_next_stage;
}

// Render the stage.
// INTRO STAGE: Show the intro image
void MainStageView::render() {
    // TODO: optimize this
    //* Maybe use a priority queue to sort the actors based on their render_order.
    //* Maybe separate the actors into 2 vectors, one for movable actors and one for non-movable actors. Then sort
    //    only the movable actors. Then merge the 2 vectors.
    //* Maybe remove actors far from the camera.
    // sort(actors_to_render.begin(), actors_to_render.end(), cmp_actor_ptrs_for_rendering);

    // Render the actors
    renderer.set_scale(config.zoom_factor);
    for (auto &actor_ptr : actors_to_render) {
        Actor &actor = *actor_ptr;
        renderer.render_actor(actor, camera_x, camera_y);
    }
    renderer.set_scale(1.0);

    // Render the player's hp and score
    if (has_player()) {
        renderer.render_hp_and_score(player);
    }

    // Render the dialogues
    for (size_t i = 0; i < dialogues_to_render.size(); ++i) {
        auto &pair = dialogues_to_render[i];

        int d_index = pair.first;
        DialogueType type = Actor::get_dialogue_type(d_index);
        auto &dialogue = pair.second->dialogues[type];

        renderer.render_dialogue(dialogue, dialogues_to_render.size(), i);
    }
}

// Update the camera position based on the player's position.
bool MainStageView::update_camera_position() {
    // HW5: Camera position = player_actor.position + (cam_offset_x, cam_offset_y)
    // if (has_player()) {
    //     camera_x = player.get_actor()->transform_position_x + config.cam_offset_x;
    //     camera_y = player.get_actor()->transform_position_y + config.cam_offset_y;
    // }

    // HW6: new_cam_pos = glm::mix(current_cam_pos, player_current_pos, cam_ease_factor);
    if (has_player()) {
        Actor &player_actor = *player.get_actor();

        camera_x = glm::mix(camera_x, player_actor.transform_position_x + config.cam_offset_x, config.cam_ease_factor);
        camera_y = glm::mix(camera_y, player_actor.transform_position_y + config.cam_offset_y, config.cam_ease_factor);

        static const double x_resolution_mul_camera_collider_padding = config.x_resolution * camera_region_size /
                                                                       Const::PIXELS_PER_UNIT;
        static const double y_resolution_mul_camera_collider_padding = config.y_resolution * camera_region_size /
                                                                       Const::PIXELS_PER_UNIT;

        int new_camera_pos_x = round_int(camera_x / x_resolution_mul_camera_collider_padding);
        int new_camera_pos_y = round_int(camera_y / y_resolution_mul_camera_collider_padding);

        if (new_camera_pos_x != camera_region_x || new_camera_pos_y != camera_region_y) {
            camera_region_x = new_camera_pos_x;
            camera_region_y = new_camera_pos_y;
            return true;
        }
    }

    return false;
}

// Apply a dialogue, update the player state according to the Dialogue Command.
void MainStageView::apply_dialogue(Actor &npc, Dialogue &dialogue, Player &player) {
    assert(!dialogue.empty() && "dialogue is empty");

    if (dialogue.is_health_down()) {
        if (player.damage(npc)) {
            npc.attack(*player.get_actor());

            // Play the damage sfx
            int channel = (Helper::GetFrameNumber()) % 48 + 2;
            audio.play(player.get_actor()->damage_sfx, channel, 0);
        }
        if (player.is_dead()) {
            // result = GAME_RESULT_LOSE;
            // stage = GAME_STAGE_END;
            adventure.set_game_result(GAME_RESULT_LOSE);
            go_to_next_stage = true;
        }
    } else if (dialogue.is_score_up()) {
        // A score up dialogue should only be applied once
        if (!npc.score_up_triggered) {
            player.gain_score();
            npc.score_up_triggered = true;

            // Play the score sfx at channel 1
            audio.play(config.score_sfx, 1, 0);
        }
    } else if (dialogue.is_you_win()) {
        // result = GAME_RESULT_WIN;
        // stage = GAME_STAGE_END;
        adventure.set_game_result(GAME_RESULT_WIN);
        go_to_next_stage = true;
    } else if (dialogue.is_game_over()) {
        // If the player is "in cooldown", do not execute the game over command either
        if (!player.is_cooldown()) {
            // result = GAME_RESULT_LOSE;
            // stage = GAME_STAGE_END;
            adventure.set_game_result(GAME_RESULT_LOSE);
            go_to_next_stage = true;
        }
    } else if (dialogue.is_proceed_to()) {
        proceed_to_next_scene = true;
        next_scene_name = dialogue.get_proceed_to_name();
    }
}

// Move an actor to a new position.
// Note that this function does not check blocking actors.
// Return true if the actor successfully moved, false otherwise.
bool MainStageView::move_actor(Actor &actor, float x, float y) {
    float old_x = actor.transform_position_x;
    float old_y = actor.transform_position_y;

    //* `actors_to_render` is a set of pointers to actors. Actors are sorted based on their y position.
    //* So, if the actor is in the render set, and its y position changes, we need to remove it from the render set
    //* before updating its position.
    if (actor.in_render_set && old_y != y) {
        actors_to_render.erase(&actor);
        actor.in_render_set = false;
    }

#ifdef DEBUG
    else {
        dbg_not_removed_before_updating_for_y_not_changed_cnt++;
    }
    dbg_total_move_actor_cnt++;
#endif

    // Update the actor's position
    actor.transform_position_x = x;
    actor.transform_position_y = y;

    // Update the actor's position in the maps
    map[CONTACT].update_actor(old_x, old_y, &actor, actor.box_collider);
    if (!actor.dialogues[NEARBY].empty() && !actor.is_player()) {
        map[NEARBY].update_actor(old_x, old_y, &actor, actor.box_trigger);
    }

    // Update the actor's render set after moving
    update_render_set(&actor);

    return true;
}

void MainStageView::update_render_set(Actor *actor) {
    if (actor->view_image.empty()) {
        return;
    }

    bool in_render_range = false;

    //* If the actor is rotating, it should always be in the render set, since it's hard to determine if it's in the
    //* camera's view. Maybe we can optimize this later.
    if (actor->transform_rotation_degrees != 0) {
        in_render_range = true;
    } else { // TODO: check correctness
        // Check if the actor is in the camera's view
        in_render_range = collide_with(camera_x, camera_y, camera_collider,
                                       actor->transform_position_x + actor->render_offset_x,
                                       actor->transform_position_y + actor->render_offset_y, actor->box_view);
    }

    if (in_render_range && !actor->in_render_set) {
        actors_to_render.insert(actor);
        actor->in_render_set = true;
    } else if (!in_render_range && actor->in_render_set) {
        actors_to_render.erase(actor);
        actor->in_render_set = false;
    }
}

// Check if the game has a player actor.
bool MainStageView::has_player() const {
    return scene.has_player();
}
