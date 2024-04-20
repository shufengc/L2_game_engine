#ifndef __MAIN_STAGE_VIEW_H__
#define __MAIN_STAGE_VIEW_H__

#include "Map.h"
#include "Player.h"
#include "Scene.h"
#include "StageView.h"
#include <limits>
#include <set>

class MainStageView : public StageView {
  public:
    MainStageView(TextAdventure &adventure);

    // Destructor.
    ~MainStageView();

    // Update the stage.
    virtual bool update() override;

    // Render the stage.
    virtual void render() override;

  private:
    // Process mouse/keyboard events.
    void process_input();

    void update_move_actors();
    void update_nearby_dialogues();
    void update_contact_dialogues();
    void update_non_move_actors();

    // Update camera position to follow the player.
    bool update_camera_position();

    // Proceed to a new scene.
    void proceed_to(const std::string &scene_name);

    // Move the actor to a new position.
    bool move_actor(Actor &actor, float x, float y);

    // Check and apply nearby or contact dialogues.
    void apply_dialogue(Actor &npc, Dialogue &dialogue, Player &player);

    void update_render_set(Actor *actor);

    // Check if the game has a player actor.
    bool has_player() const;

    // Reset the scene.
    void reset_scene() {
        Resources &resources = Resources::get_instance();

        player.reset();
        scene.reset();
        map[NEARBY].reset();
        map[CONTACT].reset();
        resources.reset();

        actors_to_render.clear();
        dialogues_to_render.clear();
    }

  private:
    Player player; // The player.
    Scene scene; // The scene, contains actors.
    RangeMap map[2] = {RangeMap(2.0, 2.0), RangeMap(2.0, 2.0)}; // Collision maps for NEARBY and CONTACT.

    bool proceed_to_next_scene = false; // If true, the game will proceed to the next scene.
    std::string next_scene_name; // The name of the next scene to proceed to.

    double camera_x = 0.0; // The camera position x.
    double camera_y = 0.0; // The camera position y.

    // The renderer will render the actors within the camera collider.
    Collider camera_collider;
    // Extra padding for the camera collider. The camera collider is larger than the screen for robust and for not
    // updating the non-move actors frequently.
    const double camera_collider_padding = 0.5;

    // The camera region. The non-move actors will not be updated if the camera region is unchanged.
    // The camera region size is `screen_size * camera_region_size`.
    int camera_region_x = std::numeric_limits<int>::max();
    int camera_region_y = std::numeric_limits<int>::max();
    const double camera_region_size = camera_collider_padding / 2.0;

    // Actors with view images to render.
    std::set<Actor *, ActorPtrsCmpForRendering> actors_to_render;

    // The dialogues to apply. Dialogues may be unchanged for a few frames, so we need to store
    // the dialogues to apply.
    std::vector<std::pair<int, Actor *>> dialogues_to_render;

    bool go_to_next_stage = false;

#ifdef DEBUG
    size_t dbg_not_removed_before_updating_for_y_not_changed_cnt = 0;
    size_t dbg_total_move_actor_cnt = 0;
    size_t dbg_total_contact_check_cnt = 0;
    size_t dbg_skiped_contact_check_cnt = 0;
#endif
};

#endif // __MAIN_STAGE_VIEW_H__
