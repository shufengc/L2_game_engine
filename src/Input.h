#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>
#include <vector>
#include <glm/vec2.hpp>

class Input {
    enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_DOWN };

    struct KeyState {
        INPUT_STATE state = INPUT_STATE_UP;
        int up_or_down_frame = -1; // The frame number when the key was pressed or released.
    };

  public:
    Input(); // Call before main loop begins.

    void ProcessEvent(const SDL_Event &e); // Call every frame at start of event loop.

    bool GetKey(SDL_Scancode keycode);
    bool GetControllerKey(int keycode);
    bool GetKeyDown(SDL_Scancode keycode);
    bool GetKeyUp(SDL_Scancode keycode);

    bool GetMouseButton(int button);
    bool GetMouseButtonDown(int button);
    bool GetMouseButtonUp(int button);
  public:
    float mouse_scroll_delta;
    glm::vec2 mouse_pos;
  private:
    std::vector<KeyState> keyboard_states;
    std::vector<KeyState> joystick_states;
    std::vector<KeyState> mouse_button_states;
};

#endif
