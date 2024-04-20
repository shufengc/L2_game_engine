#ifndef __SCENE_H__
#define __SCENE_H__

#include "Actor.h"
#include <cassert>
#include <map>
#include <rapidjson/document.h>
#include <string>
#include <vector>

class Scene {
  public:
    std::vector<Actor> actors;
    std::vector<size_t> talk_actors;
    std::vector<size_t> movable_actors;
    std::vector<size_t> non_movable_actors;
    int player_actor_index = -1;

    void load_scene(const std::string &scene_name);
    void reset();

    bool has_player() const;

  private:
    void parse_actor(const rapidjson::Value &actor_obj, Actor &actor, bool allow_template = true);
    void load_template_actor(const std::string &template_name, Actor &actor);
    template <typename ActorType> void add_actor(ActorType &&actor);

  private:
    std::map<std::string, Actor> template_actors;
};

template <typename ActorType> void Scene::add_actor(ActorType &&actor) {
    actors.push_back(std::forward<Actor>(actor));

    Actor &actor_ref = actors.back();

    if (actor_ref.is_player()) {
        assert(player_actor_index == -1 && "scene must have only one player");
        player_actor_index = actors.size() - 1;
        movable_actors.push_back(actors.size() - 1);
    } else {
        if (!actor_ref.dialogues[NEARBY].empty() || !actor_ref.dialogues[CONTACT].empty()) {
            talk_actors.push_back(actors.size() - 1);
        }

        if (actor_ref.vel_x != 0 || actor_ref.vel_y != 0) {
            movable_actors.push_back(actors.size() - 1);
        } else {
            non_movable_actors.push_back(actors.size() - 1);
        }
    }
}

#endif // __SCENE_H__
