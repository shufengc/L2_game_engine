#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "Resources.h"
#include "Utils.h"
#include <SDL.h>
#include <string>

class SimpleImage {
  public:
    SimpleImage() {}
    SimpleImage(SDL_Texture *texture, int w, int h) : texture(texture), w(w), h(h) {}

    void set_texture(SDL_Texture *texture, int w, int h) {
        this->texture = texture;
        this->w = w;
        this->h = h;
    }

    SDL_Texture *get_texture() { return texture; }
    int width() const { return w; }
    int height() const { return h; }

    bool empty() const { return texture == nullptr; }

  private:
    SDL_Texture *texture = nullptr;
    int w = 0;
    int h = 0;
};

class Image {
  public:
    Image() {}
    Image(const std::string &file_path);

    void set_file_path(const std::string &file_path, double scale_x, double scale_y) {
        this->file_path = file_path;
        this->scale_x = scale_x;
        this->scale_y = scale_y;
        texture = nullptr;
    }

    int width() const { return w; }
    int height() const { return h; }

    bool empty() const { return file_path.empty(); }

    SDL_Texture *get_texture(int &width, int &height) const {
        if (texture == nullptr) {
            texture = Resources::get_instance().get_image(file_path.c_str());
            SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

            bool horizontal_flip = false;
            bool vertical_flip = false;

            if (scale_x < 0) { // If the scale is negative, flip the image.
                horizontal_flip = true;
                scale_x = -scale_x;
            }
            if (scale_y < 0) { // If the scale is negative, flip the image.
                vertical_flip = true;
                scale_y = -scale_y;
            }
            renderer_flip = GetRendererFlip(horizontal_flip, vertical_flip);

            // Apply the scale to the image width and height
            w = round_int(w * scale_x);
            h = round_int(h * scale_y);
        }
        width = w;
        height = h;
        return texture;
    }

    SDL_RendererFlip get_renderer_flip() const { return renderer_flip; }

  private:
    std::string file_path;
    mutable double scale_x = 1.0;
    mutable double scale_y = 1.0;
    mutable SDL_Texture *texture = nullptr;
    mutable int w = 0;
    mutable int h = 0;
    mutable SDL_RendererFlip renderer_flip = SDL_FLIP_NONE;
};

#endif // __IMAGE_H__
