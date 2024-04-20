#include "Resources.h"
#include "Constants.h"
#include <cassert>

SDL_Texture *Resources::get_image(const std::string &name) {
    if (name.empty()) {
        return nullptr;
    }

    auto it = images.find(name);
    if (it != images.end()) {
        return it->second;
    }

    std::string path = Const::IMAGE_PATH + name + ".png";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: missing image " << name;
        exit(0);
    }

#ifdef DEBUG
    std::cout << "Loading image: \"" << path << "\"\n";
#endif

    SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());

    if (texture == NULL) {
        std::cout << SDL_GetError() << std::endl;
        exit(0);
    }

    images[name] = texture;
    return texture;
}

TTF_Font *Resources::get_font(const std::string &name) {
    if (font != nullptr) {
        return font;
    }

    std::string path = Const::FONT_PATH + name + ".ttf";

    if (!std::filesystem::exists(path)) {
        std::cout << "error: font " << name << " missing";
        exit(0);
    }

#ifdef DEBUG
    std::cout << "Loading font: \"" << path << "\"\n";
#endif

    font = TTF_OpenFont(path.c_str(), 16);

    if (font == NULL) {
        std::cout << TTF_GetError() << std::endl;
        exit(0);
    }

    return font;
}

Mix_Chunk *Resources::get_audio(const std::string &name) {
    if (name.empty()) {
        return nullptr;
    }

    auto it = audio.find(name);
    if (it != audio.end()) {
        return it->second;
    }

    std::string path = Const::AUDIO_PATH + name + ".ogg";

    if (!std::filesystem::exists(path)) {
        path = Const::AUDIO_PATH + name + ".wav";

        if (!std::filesystem::exists(path)) {
            std::cout << "error: failed to play audio clip " << name;
            exit(0);
        }
    }

#ifdef DEBUG
    std::cout << "Loading audio: \"" << path << "\"\n";
#endif

    Mix_Chunk *m = AudioHelper::Mix_LoadWAV498(path.c_str());

    if (m == nullptr) {
        std::cout << Mix_GetError() << std::endl;
        exit(0);
    }

    audio[name] = m;
    return m;
}

SDL_Texture *Resources::get_text_texture(const std::string &text, SDL_Color color) {
    auto it = texts.find(text);
    if (it != texts.end()) {
        return it->second;
    }

#ifdef DEBUG
    std::cout << "Creating texture for: \"" << text << "\"\n";
#endif

    assert(font != nullptr);
    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture == nullptr) {
        std::cout << SDL_GetError() << std::endl;
        exit(0);
    }

    texts[text] = texture;

    return texture;
}
