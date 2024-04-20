#ifndef __SFX_H__
#define __SFX_H__

#include "Resources.h"
#include <rapidjson/document.h>
#include <string>

class Sfx {
  public:
    Sfx() = default;
    Sfx(const std::string &file_path) : file_path(file_path) {}

    // Set the file path.
    void set_file_path(const std::string &file_path) {
        this->file_path = file_path;
        chunk = nullptr;
    }

    const std::string &get_file_path() const { return file_path; }

    // Check if the file path is empty.
    bool empty() const { return file_path.empty(); }

    // Get the chunk and load it if it's not loaded.
    Mix_Chunk *get_chunk() const {
        if (chunk == nullptr) {
            chunk = Resources::get_instance().get_audio(file_path);
        }
        return chunk;
    }

  private:
    std::string file_path;
    mutable Mix_Chunk *chunk = nullptr;
};

#endif // __SFX_H__
