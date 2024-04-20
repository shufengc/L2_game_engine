#include "Utils.h"
#include <iostream>
#include <rapidjson/filereadstream.h>

std::string &strip(std::string &str) {
    // Remove trailing whitespace
    while (str.size() && isspace(str.back())) {
        str.pop_back();
    }

    // Remove leading whitespace
    size_t i = 0;
    while (i < str.size() && isspace(str[i])) {
        i++;
    }
    str = str.substr(i);

    return str;
}

void ReadJsonFile(const std::string &path, rapidjson::Document &out_document) {
    FILE *file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    std::fclose(file_pointer);

    if (out_document.HasParseError()) {
        rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
        std::cout << "error parsing json at [" << path << "]"
                  << " " << errorCode << std::endl;
        exit(0);
    }
}

std::string obtain_word_after_phrase(const std::string &input, const std::string &phrase) {
    // Find the position of the phrase in the string
    size_t pos = input.find(phrase);

    // If the phrase is not found, return an empty string
    if (pos == std::string::npos) {
        return "";
    }

    // Find the starting position of the next word (skip spaces after the phrase)
    pos += phrase.length();
    while (pos < input.length() && std::isspace(input[pos])) {
        ++pos;
    }

    // If we're at the end of the string, return an empty string
    if (pos == input.length()) {
        return "";
    }

    // Find the end position of the word (until a space or the end of the string)
    size_t end = pos;
    while (end < input.length() && !std::isspace(input[end])) {
        ++end;
    }

    // Extract and return the word
    return input.substr(pos, end - pos);
}

SDL_RendererFlip GetRendererFlip(bool horizontalFlip, bool verticalFlip) {
    int flip = 0;
    if (horizontalFlip) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (verticalFlip) {
        flip |= SDL_FLIP_VERTICAL;
    }
    return static_cast<SDL_RendererFlip>(flip);
}
