#ifndef __TEXT_H__
#define __TEXT_H__

#include "Constants.h"
#include "Resources.h"
#include <SDL.h>
#include <rapidjson/document.h>
#include <string>

// Text class to handle text strings along with their textures that are going to be rendered.
class Text {
  public:
    Text() = default;
    Text(const std::string &text) : str(text) {}

    // Set the text string.
    void set_text(const std::string &text) {
        str = text;
        textures = nullptr;
    }

    // Get the text string.
    const std::string &get() const { return str; }

    // Get the text texture and its dimensions.
    // If the texture is not created, create it using the `resources`.
    SDL_Texture *get_texture(int &width, int &height) const {
        if (textures == nullptr) {
            textures = Resources::get_instance().get_text_texture(str, Const::TEXT_COLOR);
            SDL_QueryTexture(textures, nullptr, nullptr, &textures_width, &textures_height);
        }
        width = textures_width;
        height = textures_height;
        return textures;
    }

    // Return the text as a string.
    operator const std::string &() const { return str; }
    operator const char *() const { return str.c_str(); }

    // Print the text string.
    friend std::ostream &operator<<(std::ostream &os, const Text &text) {
        os << text.str;
        return os;
    }

  private:
    std::string str;
    mutable SDL_Texture *textures = nullptr;
    mutable int textures_width = 0;
    mutable int textures_height = 0;
};

#endif // __TEXT_H__
