#include "Scene.h"
#include "Constants.h"
#include "Utils.h"
#include <filesystem>
#include <iostream>
#include <rapidjson/document.h>

using namespace rapidjson;

void Scene::load_scene(const std::string &scene_name) {
    std::string path = Const::SCENES_PATH + scene_name + ".scene";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }

    Document document;
    ReadJsonFile(path, document);

    assert(document.HasMember("actors") && "scene must have actors");

    const Value &actors_array = document["actors"];
    assert(actors_array.IsArray() && "actors must be an array");

    actors.reserve(actors_array.Size());

    // Load actors
    for (SizeType i = 0; i < actors_array.Size(); i++) {
        const Value &actor = actors_array[i];

        Actor new_actor;

        parse_actor(actor, new_actor);
        new_actor.set_id(int(i));

        add_actor(std::move(new_actor));
    }

#ifdef DEBUG
    for (size_t i = 0; i < actors.size(); i++) {
        Actor &actor = actors[i];
        assert(actor.get_id() != -1 && "actor id must be set");
        if (i > 0) {
            assert(actor.get_id() > actors[i - 1].get_id() && "actor id must be unique and increasing");
        }
    }
#endif
}

void Scene::reset() {
    actors.clear();
    talk_actors.clear();
    movable_actors.clear();
    player_actor_index = -1;
}

// Check if the scene has a player actor.
bool Scene::has_player() const {
    return player_actor_index != -1;
}

// Parse an actor from a JSON object.
void Scene::parse_actor(const Value &actor_obj, Actor &actor, bool allow_template) {
    // Deal with template
    if (allow_template && actor_obj.HasMember("template")) {
        std::string template_name = actor_obj["template"].GetString();
        auto it = template_actors.find(template_name);
        if (it == template_actors.end()) { // template not found, load it and save it in the map
            load_template_actor(template_name, actor);
            template_actors[template_name] = actor;
        } else { // template found, copy it
            actor = it->second;
        }
    }

    Actor::parse_actor(actor_obj, actor);
}

void Scene::load_template_actor(const std::string &template_name, Actor &actor) {
    std::string path = Const::ACTOR_TEMPLATE_PATH + template_name + ".template";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: template " << template_name << " is missing";
        exit(0);
    }

    Document document;
    ReadJsonFile(path, document);

    parse_actor(document, actor);
}
