#include "AudioHelper.h"
#include "Constants.h"
#include "TextAdventure.h"
#include <SDL.h>
#include <iostream>

//#include "lua/lua.hpp"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBridge/LuaBridge.h"
#include "keycode_to_scancode.h"

#include <glm/glm.hpp>

#include <thread>
#include <rapidjson/document.h>
#include "Audio.h"

#include "box2d/box2d.h"
#include "list"

using namespace rapidjson;
using namespace std;
using namespace luabridge;

struct DrawRequest
{
    std::string image_name;
    float x, y;
    float r, g, b, a;
    int rotation_degrees;
    float scale_x, scale_y;
    float pivot_x, pivot_y;
    int sorting_order;
    string font_name;
    int fontSize;
};

std::vector<DrawRequest> drawPixRequests;
std::vector<DrawRequest> drawTextRequests;
std::vector<DrawRequest> drawExRequests;
std::vector<DrawRequest> drawUIExRequests;

lua_State* g_lua_state = NULL;
Input g_input;
GameConfig g_config;
Renderer g_renderer(g_config);
Audio g_audio;

class LuaActor;

std::vector<LuaActor*> g_actors;
std::vector<LuaActor*> g_actors_newly_added;
std::vector<string> g_scene_newly_added;
vector<LuaActor*> g_stayActorList;
string g_cur_scene;

//box2d
b2Vec2 g_gravity(0.0f, 9.8f);
b2World g_world(g_gravity);

void init_SDL2() {
    int ret;

    // Initialize SDL2
    ret = SDL_Init(/* SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER */
                   SDL_INIT_EVERYTHING);
    if (ret != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(0);
    }

    // Initialize SDL2_ttf for .ttf files
    ret = TTF_Init();
    if (ret != 0) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        exit(0);
    }

    // Initialize SDL2_image for .png files
    ret = IMG_Init(IMG_INIT_PNG);
    if (ret != IMG_INIT_PNG) {
        std::cout << "IMG_Init Error: " << IMG_GetError() << std::endl;
        exit(0);
    }

    // Initialize SDL_mixer for .wav and .ogg files
    ret = Mix_Init(MIX_INIT_OGG);
    if (ret != MIX_INIT_OGG) {
        std::cout << "Mix_Init Error: " << Mix_GetError() << std::endl;
        exit(0);
    }

    // Open the audio device
    ret = AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    if (ret != 0) {
        std::cout << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        exit(0);
    }

    // Allocate the number of channels
    ret = AudioHelper::Mix_AllocateChannels498(Const::NUM_MIX_CHANNELS);
    if (ret != Const::NUM_MIX_CHANNELS) {
        std::cout << "Mix_AllocateChannels Error: " << Mix_GetError() << std::endl;
        exit(0);
    }
}

void quit_SDL2() {
    exit(0);
    // Mix_CloseAudio();
    // Mix_Quit();
    // IMG_Quit();
    // TTF_Quit();
    // SDL_Quit();
}

void start() {
    TextAdventure text_adventure;

    text_adventure.start();
}

void parse_lua_actor(const rapidjson::Value& actor_obj) {
    // Parse actor name.
    // Note: the player actor must have the name "player".
    // If the actor name is not set, it is assumed to be a non-player actor.
    // Non-player actors' name are not important.
    /*
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
    }
    else { // Extract NON-PLAYER-ONLY properties
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
    */
}


void load_lua_scene(const std::string& scene_name) {
    std::string path = Const::SCENES_PATH + scene_name + ".scene";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }

    Document document;
    ReadJsonFile(path, document);

    assert(document.HasMember("actors") && "scene must have actors");

    const Value& actors_array = document["actors"];
    assert(actors_array.IsArray() && "actors must be an array");


    // Load actors
    for (SizeType i = 0; i < actors_array.Size(); i++) {
        const Value& actor = actors_array[i];

        //parse_lua_actor(actor);
    }
}

void my_log(const std::string& s) {
    std::cout << s << std::endl;
}

void my_error_log(const std::string& s) {
    std::cerr << s << std::endl;
}

void EstablishInheritance(lua_State* lua_state, luabridge::LuaRef & instance_table, luabridge::LuaRef& parent_table)
{
    /* We must create a metatable to establish inheritance in lua. */
    luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
    new_metatable["__index"] = parent_table;
    /* We must use the raw lua C-AI (lua stack) to perform a "setmetatable" operation. */
    instance_table.push(lua_state) ;
    new_metatable.push(lua_state);
    lua_setmetatable(lua_state, -2);
    lua_pop(lua_state, 1);
}


using event_callback = std::pair<LuaRef, LuaRef>;

class EventSubscriber
{
public:
    explicit EventSubscriber(event_callback callback) : callback_(std::move(callback)) {}

    void execute(LuaRef event_object)
    {
        callback_.second(callback_.first, event_object);
    }
public:
    event_callback callback_;
};


class Event
{
public:
    static void Publish(std::string event_type, LuaRef event_object)
    {
        auto& subscribers = m_subscribers[event_type];
        for (auto& subs : subscribers)
        {
            subs.execute(event_object);
        }
    }

    static void DoSubscribe(std::string event_type, LuaRef component, LuaRef func)
    {
        EventSubscriber subscriber({ component, func });
        m_subscribers[event_type].push_back(subscriber);
    }

    static void Subscribe(std::string event_type, LuaRef component, LuaRef func)
    {
        m_eventRequest.push_back(std::make_tuple(0, event_type, component, func));
    }

    static void DoUnsubscribe(std::string event_type, LuaRef component, LuaRef func)
    {
        auto it = std::find_if(m_subscribers[event_type].begin(), m_subscribers[event_type].end(), [&](EventSubscriber& s) {
            return s.callback_.first == component && s.callback_.second == func;
            });
        if (it != m_subscribers[event_type].end())
        {
            m_subscribers[event_type].erase(it);
        }
    }

    static void Unsubscribe(std::string event_type, LuaRef component, LuaRef func)
    {
        m_eventRequest.push_back(std::make_tuple(1, event_type, component, func));
    }

    static void updateSubscriberEvent()
    {
        for (size_t i = 0; i < m_eventRequest.size(); i++)
        {
            int myInt = std::get<0>(m_eventRequest[i]);
            std::string myString = std::get<1>(m_eventRequest[i]);
            LuaRef myLuaRef1 = std::get<2>(m_eventRequest[i]);
            LuaRef myLuaRef2 = std::get<3>(m_eventRequest[i]);
            if (myInt == 0)
            {
                DoSubscribe(myString, myLuaRef1, myLuaRef2);
            }
            else
            {
                DoUnsubscribe(myString, myLuaRef1, myLuaRef2);
            }
        }
        m_eventRequest.clear();
    }

    using SubscribersContainer = std::list<EventSubscriber>;

private:
    static std::map<std::string, SubscribersContainer> m_subscribers;
    static std::vector<std::tuple<int, string, LuaRef, LuaRef>> m_eventRequest;
};

std::map<std::string, Event::SubscribersContainer> Event::m_subscribers;
std::vector<std::tuple<int, string, LuaRef, LuaRef>> Event::m_eventRequest;

class Rigidbody {
public:
    Rigidbody()
    {

    }
    virtual ~Rigidbody() {
        //todo: release
    }

    void OnDestroy()
    {
        g_world.DestroyBody(m_body);
    }

    void CreateBody()
    {
        if ( m_body )
            return;

        b2BodyDef bodyDef;
        bodyDef.position.Set(x, y);
        if (body_type == "dynamic")
            bodyDef.type = b2_dynamicBody;
        if (body_type == "static")
            bodyDef.type = b2_staticBody;
        if (body_type == "kinematic")
            bodyDef.type = b2_kinematicBody;        
        bodyDef.bullet = precise;
        bodyDef.gravityScale = gravity_scale;
        bodyDef.angularDamping = angular_friction;
        bodyDef.angle = rotation * (b2_pi / 180.0f);
        m_body = g_world.CreateBody(&bodyDef);

        if (!has_collider && !has_trigger)
        {
            b2PolygonShape phan_shape;
            phan_shape.SetAsBox(width * 0.5f, height * 0.5f);
            b2FixtureDef phan_fixture_def;
            phan_fixture_def.shape = &phan_shape;
            phan_fixture_def.density = density;
            phan_fixture_def.isSensor = true;
            // Because it is a sensor (with no callback even), no collisions will ever occur
            phan_fixture_def.isSensor = true;
            phan_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(m_pActorPtr);
            m_body->CreateFixture(&phan_fixture_def);
        }
        
        if (has_collider == true)
        {
            b2PolygonShape my_box_shape;
            my_box_shape.SetAsBox(width * 0.5f, height * 0.5f);
            b2CircleShape  my_circle_shape;
            my_circle_shape.m_radius = radius;

            b2FixtureDef fixture_def;

            if (collider_type == "circle")
            {
                fixture_def.shape = &my_circle_shape;
            }
            else // (collider_type == "box")
            {
                fixture_def.shape = &my_box_shape;
            }
#define BOX2D_CATEGORY_COLLIDER 0x0001 
#define BOX2D_CATEGORY_SENSOR   0x0002 
            fixture_def.filter.categoryBits = BOX2D_CATEGORY_COLLIDER;
            fixture_def.filter.maskBits = ~BOX2D_CATEGORY_SENSOR;
            fixture_def.restitution = bounciness;
            fixture_def.friction = friction;
            fixture_def.density = density;
            fixture_def.isSensor = false;           //false
            fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(m_pActorPtr);
            m_body->CreateFixture(&fixture_def);
        }
        
        if (has_trigger == true)
        {
            b2PolygonShape my_box_shape;
            my_box_shape.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
            b2CircleShape  my_circle_shape;
            my_circle_shape.m_radius = trigger_radius;


            b2FixtureDef fixture_def;

            if (trigger_type == "circle")
            {
                fixture_def.shape = &my_circle_shape;
            
            }
            else // (collider_type == "box")
            {
                fixture_def.shape = &my_box_shape;
            }
            fixture_def.filter.categoryBits = BOX2D_CATEGORY_SENSOR;
            fixture_def.filter.maskBits = ~BOX2D_CATEGORY_COLLIDER;
            fixture_def.restitution = bounciness;
            fixture_def.friction = friction;
            fixture_def.density = density;
            fixture_def.isSensor = true;               //true
            fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(m_pActorPtr);
            m_body->CreateFixture(&fixture_def);
        }
/*
        {
            b2PolygonShape my_shape;
            my_shape.SetAsBox(width * 0.5f, height * 0.5f);
            b2FixtureDef fixture;
            fixture.shape = &my_shape;
            fixture.density = 1.0f;
            fixture.userData.pointer = reinterpret_cast<uintptr_t>(m_pActorPtr);
            m_body->CreateFixture(&fixture);
        }
*/
    }

    b2Vec2 GetPosition() { 
        if ( m_body )
            return m_body->GetPosition();
        else
        {
            return b2Vec2(x, y);
        }
    }; // use b2Body->GetPosition();
    float GetRotation() { return m_body->GetAngle() * (180.0f / b2_pi); }; // Use b2Body->GetAngle()
    void AddForce(b2Vec2 f) { m_body->ApplyForceToCenter(f, true); };
    void SetVelocity(b2Vec2 vec2) { m_body->SetLinearVelocity(vec2); };
    void SetPosition(b2Vec2 v) {
        if (m_body)
         m_body->SetTransform(v, m_body->GetAngle());
        else
        {
            x = v.x;
            y = v.y;
        }
    };
    void SetRotation(float degrees_clockwise) { m_body->SetTransform(m_body->GetPosition(), degrees_clockwise * (b2_pi / 180.0f)); };
    void SetAngularVelocity(float degrees_clockwise) { m_body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f)); };
    void SetGravityScale(float g) { m_body->SetGravityScale(g); };

    b2Vec2 GetVelocity() { return m_body->GetLinearVelocity(); };// use b2Body
    float GetAngularVelocity() { return m_body->GetAngularVelocity() * (180.0f / b2_pi); };
    float GetGravityScale() { return m_body->GetGravityScale(); };

    void SetUpDirection(b2Vec2 direction) {
        direction.Normalize();
        float angle = glm::atan(direction.x, -direction.y);
        m_body->SetTransform(m_body->GetPosition(), angle);
    };                         
    void SetRightDirection(b2Vec2 direction) {
        direction.Normalize();
        float angle = glm::atan(direction.x, -direction.y);
        m_body->SetTransform(m_body->GetPosition(), angle - b2_pi / 2.0f);
    };
    b2Vec2 GetUpDirection() {
        float angle = m_body->GetAngle();
        b2Vec2 upDirection = b2Vec2(glm::sin(angle), -glm::cos(angle));
        upDirection.Normalize();
        return upDirection;
     };
    b2Vec2 GetRightDirection() {
        float angle = m_body->GetAngle() - b2_pi / 2.0f;
        b2Vec2 upDirection = b2Vec2(-glm::sin(angle), glm::cos(angle));
        upDirection.Normalize();
        return upDirection;
    };// Return the normalized ¡°right¡± vector for this body

public:
    float x = 0.0f; // Use b2BodyDef.position (in c++)
    float y = 0.0f;
    string body_type = "dynamic";
    //dynamic : use b2_dynamicBody with b2BodyDef.type
    //static : use b2_staticBody with b2BodyDef.type
    //kinematic : use b2_kinematicBody with b2BodyDef.type
    bool precise = true; // Use b2BodyDef.bullet
    float gravity_scale = 1.0f; // Use b2BodyDef.gravityScale
    float density = 1.0f; // see below to set density on a fixture.
    float angular_friction = 0.3f; // Use b2BodyDef.angularDamping
    float rotation = 0.0f; // Units of degrees (not radians).
    bool has_collider = true; // Not used for anything just yet, but please add it.
    bool has_trigger = true; // Not used for anything just yet, but please add it.
    string collider_type = "box"; // define the shape of the collider.
    float width = 1.0f;
    float height = 1.0f;
    float radius = 0.5f;
    float friction = 0.3f;
    float bounciness = 0.3f;
    float trigger_width = 1.0f;
    float trigger_height = 1.0f;
    float trigger_radius = 0.5f;
    string trigger_type = "box";
    void* m_pActorPtr;

private:
    b2Body* m_body = NULL;
};

class LuaActor {
public:
    LuaActor() : name_("") { dontDestroy = false; }

    LuaActor(const Value& actor, bool running_time)
    {
        string actorName = actor["name"].GetString();
        SetName(actorName);
        lua_state = g_lua_state;
        dontDestroy = false;

        if (actor.FindMember("template") != actor.MemberEnd())
        {
            std::string path = Const::ACTOR_TEMPLATE_PATH + actor["template"].GetString() + ".template";

            if (!std::filesystem::exists(path)) {
                std::cout << "error: template " << actor["template"].GetString() << " is missing";
                exit(0);
            }

            Document tplDocument;
            ReadJsonFile(path, tplDocument);
            const Value& components = tplDocument["components"];

            if (0 == InitLuaObject(g_lua_state, components, components_map, running_time))
            {
                exit(0);
            }

            if (actor.FindMember("components") != actor.MemberEnd())
            {
                const Value& components = actor["components"];

                if (0 == InitLuaObject(g_lua_state, components, components_map, running_time))
                {
                    exit(0);
                }
            }
        }
        else 
        {
            std::string path = Const::ACTOR_TEMPLATE_PATH + actorName + ".template";

            if (std::filesystem::exists(path)) { //lua Actor.Instantiate("falling_box")
                Document tplDocument;
                ReadJsonFile(path, tplDocument);
                const Value& components = tplDocument["components"];

                if (0 == InitLuaObject(g_lua_state, components, components_map, true))
                {
                    exit(0);
                }
            }
            else
            {
                if (actor.FindMember("components") != actor.MemberEnd())
                {
                    const Value& components = actor["components"];

                    if (0 == InitLuaObject(g_lua_state, components, components_map, running_time))
                    {
                        exit(0);
                    }
                }
            }
        }
    }

    void SetName(std::string& name) {
        name_ = name;
    }


    int InitLuaObject(lua_State* lua_state, const Value& components, std::map<std::string, luabridge::LuaRef>& components_map, bool running_time)
    {
        int ret = 1;
        std::vector<std::string> component_list;

        if (components.IsObject()) {
            for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin();
                itr != components.MemberEnd(); ++itr) {
                component_list.push_back(itr->name.GetString());
            }
        }

        std::sort(component_list.begin(), component_list.end());

        for (const auto& sub_component_name : component_list) {
            luabridge::LuaRef outputMessageClass = luabridge::newTable(lua_state);
            std::string lua_class_name;
            auto it = components_map.find(sub_component_name);
            if (it != components_map.end()) {
                outputMessageClass = it->second;
            }
            else
            {
                lua_class_name = components.FindMember(sub_component_name.c_str())->value["type"].GetString();
                if ("Rigidbody" == lua_class_name)
                {
                    outputMessageClass = LoadRBComponent(lua_class_name, sub_component_name, lua_state);
                }
                else
                {
                    outputMessageClass = LoadComponent(lua_class_name, sub_component_name, lua_state);
                }

                components_map.insert(std::make_pair(sub_component_name, outputMessageClass));
            }

            if ( components.HasMember(sub_component_name.c_str())) {

                const rapidjson::Value& componentData = components[sub_component_name.c_str()];

                for (rapidjson::Value::ConstMemberIterator itr = componentData.MemberBegin(); itr != componentData.MemberEnd(); ++itr) {
                    //std::cout << itr->name.GetString() << " : " << itr->value.GetString() << std::endl;
                    if ("Rigidbody" != lua_class_name)
                    {
                        if (itr->value.IsDouble())
                            outputMessageClass[itr->name.GetString()] = itr->value.GetDouble();
                        if (itr->value.IsString())
                            outputMessageClass[itr->name.GetString()] = itr->value.GetString();
                        if (itr->value.IsInt())
                            outputMessageClass[itr->name.GetString()] = itr->value.GetInt();
                        if (itr->value.IsInt64())
                            outputMessageClass[itr->name.GetString()] = itr->value.GetInt64();
                        if (itr->value.IsBool())
                            outputMessageClass[itr->name.GetString()] = itr->value.GetBool();
                    }
                    else
                    {
                        Rigidbody* myObj = (Rigidbody*)outputMessageClass;
                        string name = itr->name.GetString();
                        if (name == "x")
                            myObj->x = itr->value.GetDouble();
                        if (name == "y")
                            myObj->y = itr->value.GetDouble();
                        if (name == "body_type")
                            myObj->body_type = itr->value.GetString();
                        if (name == "gravity_scale")
                            myObj->gravity_scale = itr->value.GetDouble();
                        if (name == "rotation")
                            myObj->rotation = itr->value.GetInt();
                        if (name == "density")
                            myObj->density = itr->value.GetDouble();
                        if (name == "gravity_scale")
                            myObj->gravity_scale = itr->value.GetDouble();
                        if (name == "angular_friction")
                            myObj->angular_friction = itr->value.GetDouble();
                        if (name == "has_collider")
                            myObj->has_collider = itr->value.GetBool();
                        if (name == "has_trigger")
                            myObj->has_trigger = itr->value.GetBool();
                        if (name == "collider_type")
                            myObj->collider_type = itr->value.GetString();
                        if (name == "width")
                            myObj->width = itr->value.GetDouble();
                        if (name == "height")
                            myObj->height = itr->value.GetDouble();
                        if (name == "radius")
                            myObj->radius = itr->value.GetDouble();
                        if (name == "friction")
                            myObj->friction = itr->value.GetDouble();
                        if (name == "bounciness")
                            myObj->bounciness = itr->value.GetDouble();
                        if (name == "trigger_height")
                            myObj->trigger_height = itr->value.GetDouble();
                        if (name == "trigger_width")
                            myObj->trigger_width = itr->value.GetDouble();
                        if (name == "trigger_radius")
                            myObj->trigger_radius = itr->value.GetDouble();
                        if (name == "trigger_type")
                            myObj->trigger_type = itr->value.GetString();

                    }
                }
            }
            else {
                //std::cout << "Component " << str << " not found!" << std::endl;
            }

            if (running_time == false && "Rigidbody" == lua_class_name)
            {
                Rigidbody* myObj = (Rigidbody*)outputMessageClass;
                myObj->CreateBody();
            }
        }
        return ret;
    }

    luabridge::LuaRef LoadRBComponent(string lua_class_name, string sub_component_name, lua_State* lua_state)
    {
        Rigidbody* myObj =  new Rigidbody();
        luabridge::push(lua_state, myObj);
        lua_setglobal(lua_state, sub_component_name.c_str());
        luabridge::LuaRef outputMessageClass = luabridge::getGlobal(lua_state, sub_component_name.c_str());
        //outputMessageClass["type"] = "Rigidbody";       
        myObj->m_pActorPtr = this;
        return outputMessageClass;
    }

    luabridge::LuaRef LoadComponent(string lua_class_name, string sub_component_name, lua_State* lua_state)
    {
        std::string lua_path = Const::COMP_PATH + lua_class_name + ".lua";

        if (!std::filesystem::exists(lua_path)) {
            std::cout << "error: failed to locate component " << lua_class_name;
            exit(0);
        }

        if (luaL_dofile(lua_state, lua_path.c_str()) != LUA_OK) {
            std::cout << "problem with lua file " << lua_class_name;
            exit(0);
        }

        luabridge::LuaRef outputMessageClass = luabridge::newTable(lua_state);
        luabridge::LuaRef parentTable = luabridge::getGlobal(lua_state, lua_class_name.c_str());
        EstablishInheritance(lua_state, outputMessageClass, parentTable);
        outputMessageClass["key"] = sub_component_name; //actor["name"].GetString() + type_str
        outputMessageClass["actor"] = this;
        outputMessageClass["enabled"] = true;
        outputMessageClass["type"] = lua_class_name;
        return outputMessageClass;
    }

    LuaRef GetComponent(std::string name)
    {       
        for (size_t i = 0; i < components_to_be_deleted.size(); i++)
        {
            for (auto& pair : components_map) {
                std::string key = pair.first;
                luabridge::LuaRef obj = pair.second;

                string type = obj["type"];
                if (components_to_be_deleted.at(i) == key) {
                    return luabridge::getGlobal(lua_state, "EMPTY_LUA_REF");
                }
            }
        }
       

        for (auto& pair : components_map) {
            std::string key = pair.first;
            luabridge::LuaRef obj = pair.second;

            if (obj.isTable())
            {
                string type = obj["type"];
                if (type == name) {
                    return obj;
                }
            }
            else if (name == "Rigidbody")   //RB obj
            {
                return obj;
            }
        }

        return luabridge::getGlobal(lua_state, "EMPTY_LUA_REF");
    }

    LuaRef AddComponent(std::string name)
    {
        static int id = 0;
        string rn = "r" + std::to_string(id++);

        if ( name == "Rigidbody")
        {
            Rigidbody* myObj = new Rigidbody();
            luabridge::push(lua_state, myObj);
            lua_setglobal(lua_state, rn.c_str());
            luabridge::LuaRef outputMessageClass = luabridge::getGlobal(lua_state, rn.c_str());      
            myObj->m_pActorPtr = this;
            components_map_newly_added.insert(std::make_pair(rn, outputMessageClass));
            return outputMessageClass;
        }
        else
        {
            luabridge::LuaRef obj = LoadComponent(name, rn, g_lua_state);
            components_map_newly_added.insert(std::make_pair(rn, obj));
            return obj;
        }
    }

    void RemoveComponent(luabridge::LuaRef target_obj)
    {       
        for (auto& pair : components_map) {
            std::string key = pair.first;

            luabridge::LuaRef obj = pair.second;

            if (target_obj == obj)
            {
                if (!obj.isTable())
                {
                    Rigidbody* p = (Rigidbody*)obj;
                    p->OnDestroy();
                    components_to_be_deleted.push_back(key);
                }
                else
                {
                    obj["enabled"] = false;
                    components_to_be_deleted.push_back(key);
                }
            }
        }
    }

    LuaRef GetComponents(std::string name)
    {
        luabridge::LuaRef luaNums = luabridge::newTable(lua_state);
        int i = 1;
        for (auto& pair : components_map) {
            //std::string key = pair.first;

            luabridge::LuaRef obj = pair.second;

            if (obj["type"] == name) {
                luaNums[i++] = obj;
            }
        }
        return luaNums;
    }

    LuaRef GetComponentByKey(std::string name)
    {
        auto it = components_map.find(name);
        if (it != components_map.end()) {
            return it->second;
        }
        else {
            return luabridge::getGlobal(lua_state, "EMPTY_LUA_REF");;
        }
    }

    std::string GetName() {
        return name_;
    }

    std::string GetID() {
        return "id"; //todo
    }

public:
    std::map<std::string, LuaRef> components_map;
    std::map<std::string, LuaRef> components_map_newly_added;
    std::vector<string> components_to_be_deleted;
    lua_State*   lua_state;
    bool dontDestroy;
private:
    std::string name_;
};

struct collision_st {
    LuaActor* other; // The other actor involved in the collision.
    b2Vec2 point; // First point of collision (use GetWorldManifold())
    b2Vec2 relative_velocity; // Use this calculation
    b2Vec2 normal; // (use GetWorldManifold().normal
};

class ContactListener : public b2ContactListener {
    /// Called when two fixtures begin to touch.
    virtual void BeginContact(b2Contact* contact) 
    {
        struct collision_st co;
        LuaActor* pActorA = (LuaActor*)(contact->GetFixtureA()->GetUserData().pointer);
        LuaActor* pActorB = (LuaActor*)(contact->GetFixtureB()->GetUserData().pointer);

        for (auto& pair : pActorA->components_map) {
            luabridge::LuaRef component = pair.second;
            string type_name = component["type"].tostring();
            if (type_name == "CollisionResponder") {
                if (component.isTable())
                {
                    co.other = pActorB;
                    b2WorldManifold mani;
                    contact->GetWorldManifold(&mani);
                    co.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();
                    co.point = mani.points[0];
                    co.normal = mani.normal;
                    if (component["OnCollisionEnter"].isFunction()) {
                        component["OnCollisionEnter"](component, co);
                    }
                    if (component["OnTriggerEnter"].isFunction()) {
                        co.point = b2Vec2(-999.0f, -999.0f);
                        co.normal = b2Vec2(-999.0f, -999.0f);
                        component["OnTriggerEnter"](component, co);
                    }
                }
            }
        }

        for (auto& pair : pActorB->components_map) {
            luabridge::LuaRef component = pair.second;
            string type_name = component["type"].tostring();
            if (type_name == "CollisionResponder") {
                if (component.isTable())
                {
                    co.other = pActorA;
                    co.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();
                    b2WorldManifold mani;
                    contact->GetWorldManifold(&mani);
                    co.point = mani.points[0];
                    co.normal = mani.normal;
                    if (component["OnCollisionEnter"].isFunction()) {
                        component["OnCollisionEnter"](component, &co);
                    }
                    if (component["OnTriggerEnter"].isFunction()) {
                        co.point = b2Vec2(-999.0f, -999.0f);
                        co.normal = b2Vec2(-999.0f, -999.0f);
                        component["OnTriggerEnter"](component, co);
                    }
                }
            }
        }
    };


    /// Called when two fixtures cease to touch.
    virtual void EndContact(b2Contact* contact) {
        struct collision_st co;
        LuaActor* pActorA = (LuaActor*)(contact->GetFixtureA()->GetUserData().pointer);
        LuaActor* pActorB = (LuaActor*)(contact->GetFixtureB()->GetUserData().pointer);

        for (auto& pair : pActorA->components_map) {
            luabridge::LuaRef component = pair.second;
            string type_name = component["type"].tostring();
            if (type_name == "CollisionResponder") {
                if (component.isTable())
                {
                    co.other = pActorB;
                    b2WorldManifold mani;
                    contact->GetWorldManifold(&mani);
                    co.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();
                    co.point = b2Vec2(-999.0f, -999.0f);
                    co.normal = b2Vec2(-999.0f, -999.0f);
                    if (component["OnCollisionExit"].isFunction()) {
                        component["OnCollisionExit"](component, co);
                    }
                    if (component["OnTriggerExit"].isFunction()) {
                        component["OnTriggerExit"](component, co);
                    }
                }
            }
        }

        for (auto& pair : pActorB->components_map) {
            luabridge::LuaRef component = pair.second;
            string type_name = component["type"].tostring();
            if (type_name == "CollisionResponder") {
                if (component.isTable())
                {
                    co.other = pActorA;
                    co.relative_velocity = contact->GetFixtureA()->GetBody()->GetLinearVelocity() - contact->GetFixtureB()->GetBody()->GetLinearVelocity();
                    co.point = b2Vec2(-999.0f, -999.0f);
                    co.normal = b2Vec2(-999.0f, -999.0f);
                    if (component["OnCollisionExit"].isFunction()) {
                        component["OnCollisionExit"](component, &co);
                    }
                    if (component["OnTriggerExit"].isFunction()) {
                        component["OnTriggerExit"](component, co);
                    }
                }
            }
        }
    };
};

LuaActor* ActorFind(string name)
{
    for (size_t i = 0; i < g_actors.size(); i++)
    {
        if (g_actors.at(i)->GetName() == name)
            return g_actors.at(i);
    }

    for (size_t i = 0; i < g_actors_newly_added.size(); i++)
    {
        if (g_actors_newly_added.at(i)->GetName() == name)
            return g_actors_newly_added.at(i);
    }


    return nullptr;
}

LuaRef ActorFindAll(string name)
{
    luabridge::LuaRef luaNums = luabridge::newTable(g_lua_state);
    int k = 1;
    for (size_t i = 0; i < g_actors.size(); i++)
    {
        if (g_actors.at(i)->GetName() == name)
        {
            luaNums[k++] = g_actors.at(i);
        }
    }

    for (size_t i = 0; i < g_actors_newly_added.size(); i++)
    {
        if (g_actors_newly_added.at(i)->GetName() == name)
            luaNums[k++] = g_actors_newly_added.at(i);
    }

    return luaNums;
}

LuaActor* ActorInstantiate(string template_name)
{
    std::string path = Const::ACTOR_TEMPLATE_PATH + template_name + ".template";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: actor template " << template_name << " is missing";
        exit(0);
    }

    Document document;
    ReadJsonFile(path, document);

    //assert(document.HasMember("actors") && "scene must have actors");
    const Value& actor = document;
    LuaActor* pActor = new LuaActor(actor, true);
    g_actors_newly_added.push_back(pActor);
    return pActor;
}

void ActorDestroy(LuaActor* pActor)
{
    //todo: release pActor??
    auto it = std::find(g_actors.begin(), g_actors.end(), pActor);

    for (auto& pair : pActor->components_map) {

        luabridge::LuaRef obj = pair.second;
        if (obj.isTable())
        {
            LuaRef OnDestroy = obj["OnDestroy"];
            if (!OnDestroy.isNil())
                OnDestroy(obj);
            obj["enabled"] = false;
        }
        else
        {
            Rigidbody* p = (Rigidbody*)obj;
            p->OnDestroy();
        }
    }

    if (it != g_actors.end())
    {
       g_actors.erase(it);
    }

    it = std::find(g_actors_newly_added.begin(), g_actors_newly_added.end(), pActor);

    if (it != g_actors_newly_added.end())
    {
        g_actors_newly_added.erase(it);
    }
}

void ApplicationQuit()
{
    exit(0);
}

void ApplicationSleep(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int ApplicationGetFrame()
{
    return Helper::GetFrameNumber();
}

void ApplicationOpenURL(std::string url)
{
    string cmd = "start " + url;
    std::system(cmd.c_str());
}

bool InputGetKey(string keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it != __keycode_to_scancode.end())
        return g_input.GetKey(it->second);
    else
        return false;
}

bool InputGetControllerKey(string keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it != __keycode_to_scancode.end())
        return g_input.GetControllerKey(it->second);
    else
        return false;
}

bool InputGetKeyDown(string keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it != __keycode_to_scancode.end())
        return g_input.GetKeyDown(it->second);
    else
        return false;
}

bool InputGetKeyUp(string keycode)
{
    auto it = __keycode_to_scancode.find(keycode);
    if (it != __keycode_to_scancode.end())
        return g_input.GetKeyUp(it->second);
    else
        return false;
}

glm::vec2 InputGetMousePosition()
{
    return g_input.mouse_pos;
}

float InputGetMouseScrollDelta()
{
    int f = Helper::GetFrameNumber();
    
    if (f >= 24 && f <= 26 )
        return -1;
    if (f >= 27 && f <= 28)
        return -2;
    if (f == 29 || f == 30 || f == 32 )
        return -1;
    if ( (f >= 44 && f <= 47) )
        return 1;
    if (f == 48)
        return 2;
    if ((f >= 49 && f <= 51))
        return 1;
    if (f == 53)
        return 1;
    if (f == 56)
        return 1;

    return 0;

    //return g_input.mouse_scroll_delta;
}

bool InputGetMouseButton(int btn_num)
{
    return g_input.GetMouseButton(btn_num);
}

bool InputGetMouseButtonDown(int btn_num)
{
    return g_input.GetMouseButtonDown(btn_num);
}

bool InputGetMouseButtonUp(int btn_num)
{
    return g_input.GetMouseButtonUp(btn_num);
}

void AudioPlay(int channel, string clip_name, bool does_loop)
{
    g_audio.play(clip_name, channel, does_loop ? -1 : 0);
}

void AudioHalt(int channel)
{
    g_audio.halt(channel);
}

void AudioSetVolume(int channel, int volume)
{
    AudioHelper::Mix_Volume498(channel, volume);
}

void TextDraw(string str_content, float x, float y
    , string font_name, int fontSize
    , float r, float g, float b, float a)
{
    DrawRequest request = { 
        str_content, x, y, r, g, b, a, 0
        , 1.0f, 1.0f, 0.5f, 0.5f, 0
        , font_name, fontSize };

    drawTextRequests.push_back(request);
}

void ImageDrawUI(string image_name, float x, float y)
{
    DrawRequest request = {
        image_name, x, y, 255, 255, 255, 255, 0
        , 1.0f, 1.0f, 0, 0, 0
        , "", 0 };
    drawUIExRequests.push_back(request);
}

void ImageDrawUIEx(string image_name, 
    float x, float y, float r,
    float g, float b, float a, int sorting_order)
{
    DrawRequest request = {
        image_name, x, y, r, g, b, a, 0
        , 1.0f, 1.0f, 0.5f, 0.5f, sorting_order
        , "", 0 };
    drawUIExRequests.push_back(request);
}

void ImageDraw(string image_name, float x, float y)
{
    DrawRequest request = {
        image_name, x, y, 255, 255, 255, 255, 0
        , 1.0f, 1.0f, 0.5f, 0.5f, 0
        , "", 0 };
    drawExRequests.push_back(request);
}

void ImageDrawEx(string image_name,
    float x, float y, float rotation_degrees,
    float scale_x, float scale_y,
    float pivot_x, float pivot_y, 
    float r, float g, float b, float a, int sorting_order)
{
    DrawRequest request = {
        image_name, x, y, r, g, b, a, (int)rotation_degrees
        , scale_x, scale_y, pivot_x, pivot_y, sorting_order
        , "", 0 };
    drawExRequests.push_back(request);
}

void ImageDrawPixel(float x, float y, float r,
    float g, float b, float a)
{
    DrawRequest request = {
        "", x, y, r, g, b, a, 0
        , 1.0f, 1.0f, 0.5f, 0.5f, 0
        , "", 0 };
    drawPixRequests.push_back(request);
}

void CameraSetPosition(float x, float y)
{
    g_config.cam_offset_x = x;
    g_config.cam_offset_y = y;
}

float CameraGetPositionX()
{
    return g_config.cam_offset_x;
}

float CameraGetPositionY()
{
    return g_config.cam_offset_y;
}

void CameraSetZoom(float zoom)
{
    g_config.zoom_factor = zoom;
    g_renderer.set_scale(zoom);
}

float CameraGetZoom()
{
    return g_config.zoom_factor;
}

void SceneLoad(string name)
{
    //ondestory??
    g_scene_newly_added.clear();
    g_scene_newly_added.push_back(name);
}

string SceneGetCurrent()
{
    return g_cur_scene;
}

void SceneDontDestroy(LuaActor* pActor)
{
    pActor->dontDestroy = true;
}

void LoadScene(string scene_name)
{
    std::string path = Const::SCENES_PATH + scene_name + ".scene";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: scene " << scene_name << " is missing";
        exit(0);
    }

    Document document;

    try {
        ReadJsonFile(path, document);
    }
    catch (const LuaException& e)
    {
        cout << "\033[31m" << scene_name << " : " << e.what() << "\033[0m" << endl;
    }
 
    assert(document.HasMember("actors") && "scene must have actors");

    const Value& actors_list = document["actors"];
    assert(actors_list.IsArray() && "actors must be an array");

    for (size_t i = 0; i < g_actors.size(); i++)
    {
        if (g_actors.at(i)->dontDestroy)
        {
            g_stayActorList.push_back(g_actors.at(i));
       }
    }

    g_actors.clear();
    // Load actors
    for (SizeType i = 0; i < actors_list.Size(); i++) {
        const Value& actor = actors_list[i];
        LuaActor* pActor = new LuaActor(actor, false);
        g_actors.push_back(pActor);
    }
}

void ActorsOnstart(vector<LuaActor*> &actors)
{
    for (size_t i = 0; i < actors.size(); i++)
    {
        for (auto& pair : actors.at(i)->components_map) {
            //std::string key = pair.first;

            luabridge::LuaRef obj = pair.second;

            if (obj["enabled"] == false)
            {
                continue;
            }

            try {
                if (obj["OnStart"].isFunction()) {
                    obj["OnStart"](obj);
                }
            }
            catch (const LuaException& e)
            {
                cout << "\033[31m" << actors.at(i)->GetName() << " : " << e.what() << "\033[0m" << endl;
            }
        }
    }
}

/*
class b2Vec2 {

public:
    b2Vec2(int x, int y)
    {
        _x = x;
        _y = y;
    };
    virtual ~b2Vec2() {};
    void Normalize() { };
    float Length() { return 0; };
    void operator_add() {};
    void operator_sub() {};
    void operator_mul() {};
    void   setX(float x) { _x = x; }
    float  getX() const { return _x; }
    void   setY(float y) { _y = y; }
    float  getY() const { return _y; }

private:
    float _x,_y;
};
*/

struct HitResult
{
    LuaActor* actor;
    b2Vec2 point;
    b2Vec2 normal;
    bool is_trigger;
};

class MyRayCastCallback : public b2RayCastCallback
{
public:
    MyRayCastCallback()
    {

    }

    float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
    {
        HitResult hitResult;
        hitResult.point = point;
        hitResult.normal = normal;
        hitResult.is_trigger = fixture->IsSensor();
        hitResult.actor = (LuaActor*)fixture->GetUserData().pointer;
        m_hitResult.push_back(hitResult);
        return 1.0f;
    }

    std::vector<HitResult>  m_hitResult;
};

LuaRef PhysicsRaycast( b2Vec2 pos, b2Vec2 dir, float dist)
{
    b2Vec2 endPos = pos + dist * dir;

    MyRayCastCallback callback;
    g_world.RayCast(&callback, pos, endPos);

    if ( callback.m_hitResult.size() > 0 )
    {
        luabridge::LuaRef result = luabridge::newTable(g_lua_state);
        result["point"] = callback.m_hitResult.at(0).point;
        result["normal"] = callback.m_hitResult.at(0).normal;
        result["is_trigger"] = callback.m_hitResult.at(0).is_trigger;
        result["actor"] = callback.m_hitResult.at(0).actor;

        return result;
    }

    // if no hit was reported, return nil
    return luabridge::getGlobal(g_lua_state, "EMPTY_LUA_REF");
}

luabridge::LuaRef PhysicsRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
    b2Vec2 endPos = pos + dist * dir;

    MyRayCastCallback callback;
    g_world.RayCast(&callback, pos, endPos);

    luabridge::LuaRef luaNums = luabridge::newTable(g_lua_state);
    int k = 1;
    for (size_t i = 0; i < callback.m_hitResult.size(); i++)
    {
        if (callback.m_hitResult.at(i).actor)
           luaNums[k++] = callback.m_hitResult.at(i);
    }

    return luaNums;
}

int main(int argc, char *argv[]) {
    int ret = 1;
    init_SDL2();
    SDL_Joystick* joy1 = NULL;
    SDL_Joystick* joy2 = NULL;

    GameState state = GAME_STAT_RUNNING;
    g_lua_state = luaL_newstate();
    luaL_openlibs(g_lua_state);

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Physics")
        .addFunction("Raycast", PhysicsRaycast)
        .addFunction("RaycastAll", PhysicsRaycastAll)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Event")
        .addFunction("Publish", &Event::Publish)
        .addFunction("Subscribe", &Event::Subscribe)
        .addFunction("Unsubscribe", &Event::Unsubscribe)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginClass<HitResult>("HitResult")
        .addConstructor<void(*) (void)>()
        .addData("actor", &HitResult::actor)
        .addData("point", &HitResult::point)
        .addData("normal", &HitResult::normal)
        .addData("is_trigger", &HitResult::is_trigger)
        .endClass();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginClass<collision_st>("collision_st")
        .addConstructor<void(*) (void)>()
        .addData("other", &collision_st::other)
        .addData("point", &collision_st::point)
        .addData("relative_velocity", &collision_st::relative_velocity)
        .addData("normal", &collision_st::normal)
        .endClass();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginClass<Rigidbody>("Rigidbody")
        .addConstructor<void(*)()>()
        .addFunction("GetRotation", &Rigidbody::GetRotation)
        .addFunction("GetPosition", &Rigidbody::GetPosition)
        .addFunction("AddForce", &Rigidbody::AddForce)
        .addFunction("SetVelocity", &Rigidbody::SetVelocity)
        .addFunction("SetPosition", &Rigidbody::SetPosition)
        .addFunction("SetRotation", &Rigidbody::SetRotation)
        .addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
        .addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
        .addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
        .addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
        .addFunction("GetVelocity", &Rigidbody::GetVelocity)
        .addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
        .addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
        .addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
        .addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
        .addFunction("OnDestroy", &Rigidbody::OnDestroy)
        .addFunction("OnStart", &Rigidbody::CreateBody)
        .addData("gravity_scale", &Rigidbody::gravity_scale)
        .addData("x", &Rigidbody::x)
        .addData("y", &Rigidbody::y)
        .addData("rotation", &Rigidbody::rotation)
        .endClass();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginClass<b2Vec2>("Vector2")
        .addConstructor<void(*)(float, float)>()
        .addProperty("x", &b2Vec2::x, &b2Vec2::x)
        .addProperty("y", &b2Vec2::y, &b2Vec2::y)
        .addFunction("Normalize", &b2Vec2::Normalize)
        .addFunction("Length", &b2Vec2::Length)
        .addFunction("__add", &b2Vec2::operator_add)
        .addFunction("__sub", &b2Vec2::operator_sub)
        .addFunction("__mul", &b2Vec2::operator_mul)
        .addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .addStaticFunction("Distance", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Distance))
        .endClass();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Debug")
        .addFunction("LogError", my_error_log)
        .addFunction("Log", my_log)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Scene")
        .addFunction("Load", SceneLoad)
        .addFunction("GetCurrent", SceneGetCurrent)
        .addFunction("DontDestroy", SceneDontDestroy)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Camera")
        .addFunction("SetPosition", CameraSetPosition)
        .addFunction("GetPositionX", CameraGetPositionX)
        .addFunction("GetPositionY", CameraGetPositionY)
        .addFunction("SetZoom", CameraSetZoom)
        .addFunction("GetZoom", CameraGetZoom)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Text")
        .addFunction("Draw", TextDraw)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Image")
        .addFunction("DrawUI", ImageDrawUI)
        .addFunction("DrawUIEx", ImageDrawUIEx)
        .addFunction("Draw", ImageDraw)
        .addFunction("DrawEx", ImageDrawEx)
        .addFunction("DrawPixel", ImageDrawPixel)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Audio")
        .addFunction("Play", AudioPlay)
        .addFunction("Halt", AudioHalt)
        .addFunction("SetVolume", AudioSetVolume)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Application")
        .addFunction("Quit", ApplicationQuit)
        .addFunction("Sleep", ApplicationSleep)
        .addFunction("GetFrame", ApplicationGetFrame)
        .addFunction("OpenURL", ApplicationOpenURL)
        .endNamespace();
    
    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Input")
        .addFunction("GetKey", InputGetKey)
        .addFunction("GetControllerKey", InputGetControllerKey)
        .addFunction("GetKeyDown", InputGetKeyDown)
        .addFunction("GetKeyUp", InputGetKeyUp)
        .addFunction("GetMouseButton", InputGetMouseButton)
        .addFunction("GetMouseButtonDown", InputGetMouseButtonDown)
        .addFunction("GetMouseButtonUp", InputGetMouseButtonUp)
        .addFunction("GetMousePosition", InputGetMousePosition)
            .beginClass<glm::vec2>("vec2")
                .addConstructor<void(*)(float, float)>()
                .addProperty("x", &glm::vec2::x)
                .addProperty("y", &glm::vec2::y)
            .endClass()
        .addFunction("GetMouseScrollDelta", InputGetMouseScrollDelta)
        .endNamespace();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginClass<LuaActor>("Actor")
        .addFunction("GetName", &LuaActor::GetName)
        .addFunction("GetComponent", &LuaActor::GetComponent)
        .addFunction("GetComponents", &LuaActor::GetComponents)
        .addFunction("GetComponentByKey", &LuaActor::GetComponentByKey)
        .addFunction("AddComponent", &LuaActor::AddComponent)
        .addFunction("RemoveComponent", &LuaActor::RemoveComponent)
        .endClass();

    luabridge::getGlobalNamespace(g_lua_state)
        .beginNamespace("Actor")
        .addFunction("Find", ActorFind)
        .addFunction("FindAll", ActorFindAll)
        .addFunction("Instantiate", ActorInstantiate)
        .addFunction("Destroy", ActorDestroy)
        .endNamespace();

    ContactListener contactListener;
    g_world.SetContactListener(&contactListener);

    g_config.load_game_config();
    g_renderer.create_window(g_config.game_title, g_config.x_resolution, g_config.y_resolution);

    g_cur_scene = g_config.initial_scene_name;
    LoadScene(g_config.initial_scene_name);
  
    ActorsOnstart(g_actors);

    (void)argc;
    (void)argv;

    //SDL_GameControllerAddMapping
    SDL_InitSubSystem(SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);

    int numJoysticks = SDL_NumJoysticks();
    if ( numJoysticks > 0 ) {
        joy1 = SDL_JoystickOpen(0);
        const char* name = SDL_JoystickName(joy1);
        const int num_axes = SDL_JoystickNumAxes(joy1);
        const int num_buttons = SDL_JoystickNumButtons(joy1);
        const int num_hats = SDL_JoystickNumHats(joy1);

        printf("Now reading from joystick1 '%s' with:\n"
            "%d axes\n"
            "%d buttons\n"
            "%d hats\n\n",
            name,
            num_axes,
            num_buttons,
            num_hats);
    }

    if (numJoysticks > 1) {
        joy2 = SDL_JoystickOpen(1);
        const char* name = SDL_JoystickName(joy2);
        const int num_axes = SDL_JoystickNumAxes(joy2);
        const int num_buttons = SDL_JoystickNumButtons(joy2);
        const int num_hats = SDL_JoystickNumHats(joy2);

        printf("Now reading from joystick2 '%s' with:\n"
            "%d axes\n"
            "%d buttons\n"
            "%d hats\n\n",
            name,
            num_axes,
            num_buttons,
            num_hats);
    }

    while (state != GAME_STAT_QUIT) {
        SDL_Event event;

        // Process SDL events
        while (Helper::SDL_PollEvent498(&event)) {
            if (event.type == SDL_QUIT) {
                state = GAME_STAT_QUIT;
                break;
            }

            // Pass events to the input manager
            g_input.ProcessEvent(event);
        
            //printf("Event:%d\n", e.type);
        }


        // Update the game state
        //bool go_to_next_stage = view->update();
        
        // Clear the renderer
        g_renderer.clear();
        
        if (g_actors_newly_added.size())
        {
            for (size_t i = 0; i < g_actors_newly_added.size(); i++)
            {
                g_actors.push_back(g_actors_newly_added.at(i));
                if (g_actors_newly_added.at(i)->components_map.size() > 0)
                {
                    for (auto& pair : g_actors_newly_added.at(i)->components_map) {
                        luabridge::LuaRef obj = pair.second;

                        if (obj["enabled"] == false)
                        {
                            continue;
                        }


                        if (obj.isTable())
                        {
                            try {
                                if (obj["OnStart"].isFunction()) {
                                    obj["OnStart"](obj);
                                }
                            }
                            catch (const LuaException& e)
                            {
                                cout << "\033[31m" << pair.first << " : " << e.what() << "\033[0m" << endl;
                            }
                        }
                        else     //Rigidbody
                        {
                            Rigidbody* p = (Rigidbody*)obj;
                            p->CreateBody();
                        }
                    }
                }
            }
            g_actors_newly_added.clear();
        }

        for (size_t i = 0; i < g_actors.size(); i++)
        {
            for (size_t k = 0; k < g_actors.at(i)->components_to_be_deleted.size(); k++)
            {
                g_actors.at(k)->components_map.erase(g_actors.at(k)->components_to_be_deleted.at(k));
            }
            g_actors.at(i)->components_to_be_deleted.clear();

            if (g_actors.at(i)->components_map_newly_added.size() > 0)
            {
                for (auto& pair : g_actors.at(i)->components_map_newly_added) {
                    //std::string key = pair.first;

                    string name = pair.first;
                    luabridge::LuaRef obj = pair.second;

                    g_actors.at(i)->components_map.insert(std::make_pair(name, obj));  //sort???

                    if (obj["enabled"] == false)
                    {
                        continue;
                    }

                    if ( obj.isTable() )
                    {
                        try {
                            if (obj["OnStart"].isFunction()) {
                                obj["OnStart"](obj);
                            }
                        }
                        catch (const LuaException& e)
                        {
                            cout << "\033[31m" << name << " : " << e.what() << "\033[0m" << endl;
                        }
                    }
                    else     //Rigidbody
                    {
                        Rigidbody* p = (Rigidbody*)obj;
                        p->CreateBody();
                    }
                }
                g_actors.at(i)->components_map_newly_added.clear();
            }

            for (auto& pair : g_actors.at(i)->components_map) {
                //std::string key = pair.first;

                luabridge::LuaRef obj = pair.second;

                if (obj["enabled"] == false)
                {
                    continue;
                }

                try {
                    if (obj["OnUpdate"].isFunction()) {
                        obj["OnUpdate"](obj);
                    }
                }
                catch (const LuaException& e)
                {
                    cout << "\033[31m" << g_actors.at(i)->GetName() << " : " << e.what() << "\033[0m" << endl;
                }
            }
        }

        for (size_t i = 0; i < g_actors.size(); i++)
        {
            for (auto& pair : g_actors.at(i)->components_map) {
                //std::string key = pair.first;

                luabridge::LuaRef obj = pair.second;

                if (obj["enabled"] == false)
                {
                    continue;
                }

                LuaRef OnUpdate = obj["OnLateUpdate"];
                try {
                    if ( !OnUpdate.isNil() )
                        OnUpdate(obj);
                }
                catch (const luabridge::LuaException& e)
                {
                    cout << "\033[31m" << g_actors.at(i)->GetName() << " : " << e.what() << "\033[0m" << endl;
                }
            }
                     
            if (i >= g_actors.size())
                continue;

            for (size_t k = 0; k < g_actors.at(i)->components_to_be_deleted.size(); k++)
            {   //find  components to be deleted, call OnDestroy 
                auto it = g_actors.at(k)->components_map.find(g_actors.at(k)->components_to_be_deleted.at(k));
                if (it != g_actors.at(k)->components_map.end()) {
                    luabridge::LuaRef obj = it->second;

                    if (obj.isTable()) //not rigid body
                    {
                        LuaRef OnDestroy = obj["OnDestroy"];
                        if (!OnDestroy.isNil())
                            OnDestroy(obj);
                    }
                }
                //rigidbody?
            }
        }

        auto compareFunc = [](const DrawRequest& a, const DrawRequest& b)
            {
                return a.sorting_order < b.sorting_order;
            };

        std::sort(drawExRequests.begin(), drawExRequests.end(), compareFunc);
        for (size_t i = 0; i < drawExRequests.size(); i++)
        {
            DrawRequest request = drawExRequests.at(i);
            g_renderer.DrawEx(request.image_name, request.x, request.y, request.rotation_degrees, request.scale_x, request.scale_y,
                    request.pivot_x, request.pivot_y, request.r, request.g, request.b, request.a);
        }
        drawExRequests.clear();

        std::sort(drawUIExRequests.begin(), drawUIExRequests.end(), compareFunc);
        for (size_t i = 0; i < drawUIExRequests.size(); i++)
        {
            DrawRequest request = drawUIExRequests.at(i);
            g_renderer.DrawUIEx(request.image_name, request.x, request.y, request.r, request.g, request.b, request.a);
        }
        drawUIExRequests.clear();

        std::sort(drawTextRequests.begin(), drawTextRequests.end(), compareFunc);
        for (size_t i = 0; i < drawTextRequests.size(); i++)
        {
            DrawRequest request = drawTextRequests.at(i);
            g_renderer.TextDraw(request.image_name, request.x, request.y, request.font_name, request.fontSize, request.r, request.g, request.b, request.a);
        }
        drawTextRequests.clear();

        if ( drawPixRequests.size() > 0 )
        {
            std::sort(drawPixRequests.begin(), drawPixRequests.end(), compareFunc);
            g_renderer.DrawPixelBegin();
            for (size_t i = 0; i < drawPixRequests.size(); i++)
            {
                DrawRequest request = drawPixRequests.at(i);
                g_renderer.DrawPixel(request.x, request.y, request.r, request.g, request.b, request.a);
            }
            g_renderer.DrawPixelEnd();
            drawPixRequests.clear();
        }


        if (g_scene_newly_added.size())
        {
            LoadScene(g_scene_newly_added.at(0));
            ActorsOnstart(g_actors);
            for (size_t i = 0; i < g_stayActorList.size(); i++)
            {
                g_actors.push_back(g_stayActorList.at(i));
            }
            g_stayActorList.clear();
            g_scene_newly_added.clear();
        }
        // Render the game state
        //view->render();

        /*
        if (go_to_next_stage) {
            if (stage == GAME_STAGE_INTRO) {
                delete view;
                view = new MainStageView(*this);
                stage = GAME_STAGE_MAIN;
            }
            else if (stage == GAME_STAGE_MAIN) {
                delete view;
                view = new EndStageView(*this);
                stage = GAME_STAGE_END;
            }
            else if (stage == GAME_STAGE_END) {
                delete view;
                view = nullptr;
                state = GAME_STAT_QUIT;
            }
        }
        */        
        Event::updateSubscriberEvent();
        g_world.Step(1.0f / 60.0f, 8, 3);
        // Show the renderer
        g_renderer.present();
    }

_END:
    quit_SDL2();
    if (joy1)
        SDL_JoystickClose(joy1);
    if (joy2)
        SDL_JoystickClose(joy2);
    lua_close(g_lua_state);

    return ret;
}
