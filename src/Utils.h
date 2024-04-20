#ifndef __UTILS_H__
#define __UTILS_H__

#include "Sfx.h"
#include "Text.h"
#include <SDL.h>
#include <cmath>
#include <optional>
#include <rapidjson/document.h>
#include <string>
#include <vector>

// Remove leading and trailing whitespaces.
std::string &strip(std::string &str);

// Read a JSON file and parse it into a rapidjson::Document.
void ReadJsonFile(const std::string &path, rapidjson::Document &out_document);

// Obtain the word after a phrase in a string.
std::string obtain_word_after_phrase(const std::string &input, const std::string &phrase);

// Get the renderer flip based on the horizontal and vertical flip.
SDL_RendererFlip GetRendererFlip(bool horizontalFlip, bool verticalFlip);

// Round a double to the nearest integer using std::round() and cast it to an int.
inline int round_int(double val) {
    return static_cast<int>(std::round(val));
}

// Round a float to the nearest integer using std::round() and cast it to an int.
inline int round_int(float val) {
    return static_cast<int>(std::round(val));
}

/*==================================================================================================================*/

/*
 * JSON helper functions.
 *
 * These functions are used to get specific type values from a rapidjson::Value.
 */

// Get a value from a `rapidjson::Value` and store it in target.
template <typename T> inline T &json_get(const rapidjson::Value &value, T &target) {
    target = value.Get<T>();
    return target;
}

// Specializations for `char` type;
template <> inline char &json_get<char>(const rapidjson::Value &value, char &target) {
    target = value.GetString()[0];
    return target;
}

// Specializations for `std::string` type;
template <> inline std::string &json_get<std::string>(const rapidjson::Value &value, std::string &target) {
    target = value.GetString();
    return target;
}

// Specializations for `std::optional` type;
template <typename T> inline std::optional<T> &json_get(const rapidjson::Value &value, std::optional<T> &target) {
    T temp;
    target = json_get(value, temp);
    return target;
}

// Specializations for `Text` type;
template <> inline Text &json_get<Text>(const rapidjson::Value &value, Text &target) {
    target.set_text(value.GetString());
    return target;
}

// Specializations for `Sfx` type;
template <> inline Sfx &json_get<Sfx>(const rapidjson::Value &value, Sfx &target) {
    target.set_file_path(value.GetString());
    return target;
}

// Specializations for `std::vector` type;
template <typename T> inline std::vector<T> &json_get(const rapidjson::Value &value, std::vector<T> &target) {
    assert(value.IsArray() && "value must be an array");

    target.resize(value.Size());

    for (rapidjson::SizeType i = 0; i < value.Size(); i++) {
        json_get(value[i], target[i]);
    }

    return target;
}

/*==================================================================================================================*/

// Get the value of a key from a `rapidjson::Value` and store it in target.
// Returns true if the key is found, false otherwise.
template <typename T> inline bool json_get_val(const rapidjson::Value &value, const char *key, T &target) {
    auto it = value.FindMember(key);
    if (it != value.MemberEnd()) {
        json_get(it->value, target);
        return true;
    }
    return false;
}

#endif // __UTILS_H__
