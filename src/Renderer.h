#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "Actor.h"
#include "Modding.h"
#include "Player.h"
#include <SDL.h>
#include <string>

class Renderer {
  public:
    Renderer(GameConfig &config) : config(config) {}

    // Create the window.
    void create_window(const std::string &title, int x_resolution, int y_resolution);

    // Render a texture at a given position.
    void render_texture(SDL_Texture *texture, int x, int y, int w, int h);

    // Render an image at a given position.
    void render_image(SDL_Texture *texture, int x, int y, int w, int h);

    // Render text at a given position.
    void render_text(const std::string &text, int x, int y);

    void TextDraw(const std::string &str_content, int xi, int yi, const std::string& font_name, int fontSize
        , int ri, int gi, int bi, int ai);

    void DrawUI(std::string image_name, int x, int y);

    void Draw(std::string image_name, float x, float y);

    void DrawUIEx(std::string image_name, int x, int y, int r, int g, int b, int a);

    void DrawEx(std::string image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y,
        float pivot_x, float pivot_y, int r, int g, int b, int a);

    void DrawPixelBegin();

    void DrawPixel(int x, int y, int r, int g, int b, int a);

    void DrawPixelEnd();

    // Render an actor.
    void render_actor(Actor &actor, double camera_x, double camera_y);


    // Render the player's hp and score.
    void render_hp_and_score(Player &player);

    // Render a dialogue.
    void render_dialogue(Dialogue &dialogue, size_t total_dialogues, size_t index);

    // Present the window.
    void present();

    // Clear the window.
    void clear();

    // Set the scale of the renderer.
    void set_scale(double zoom_factor);

    // Destructor.
    ~Renderer() {
        /*
        No need to call SDL_DestroyRenderer and SDL_DestroyWindow here.
        They will also destroy the associated textures and surfaces which are managed by the resources database.

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
        */
    }

  private:
    GameConfig &config;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Surface* m_pix_surface;
};

#endif // __RENDERER_H__
