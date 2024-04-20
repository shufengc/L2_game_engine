#ifndef __IMAGEDB_H__
#define __IMAGEDB_H__

#include "AudioHelper.h"
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>

// A class to manage resources such as images, fonts, and audio.
// Singleton pattern.
class Resources {
  public:
    static Resources &get_instance() {
        static Resources instance;
        return instance;
    }

    void init(SDL_Renderer *renderer) { this->renderer = renderer; }

    // Load an image from the file system.
    SDL_Texture *get_image(const std::string &name);

    TTF_Font *get_font(const std::string &name);

    Mix_Chunk *get_audio(const std::string &name);

    SDL_Texture *get_text_texture(const std::string &text, SDL_Color color);

    void reset() {
        //? Maybe we should free the resources of a scene when we switch to another scene.
    }

    ~Resources() {
        // for (auto &pair : images) {
        //     SDL_DestroyTexture(pair.second);
        // }
        // for (auto &pair : audio) {
        //     Mix_FreeChunk(pair.second);
        // }
        // for (auto &pair : texts) {
        //     SDL_DestroyTexture(pair.second);
        // }
        // TTF_CloseFont(font);
    }

  private:
    Resources(SDL_Renderer *renderer = nullptr) : renderer(renderer) {}

  private:
    SDL_Renderer *renderer = nullptr; // The renderer is needed to create textures.
    std::unordered_map<std::string, SDL_Texture *> images;
    std::unordered_map<std::string, Mix_Chunk *> audio;
    std::unordered_map<std::string, SDL_Texture *> texts;
    TTF_Font *font = nullptr;
};

#endif // __IMAGEDB_H__
