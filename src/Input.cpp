#include "Input.h"
#include "Helper.h"

Input::Input() : keyboard_states(SDL_NUM_SCANCODES), mouse_button_states(6), joystick_states(SDL_NUM_SCANCODES){
    // TODO: Add only the keys we care about:
    // W, A, S, D, Space, Enter, Escape, and Arrow keys.
    // Also add left mouse button.
    mouse_scroll_delta = 0;
}

void Input::ProcessEvent(const SDL_Event &e) {
    int curframe = Helper::GetFrameNumber();
    //printf("Event:%d\n", e.type);
    if (e.type == SDL_JOYAXISMOTION) {
        printf("JoyStickAxisMotion, which: %d, button:%d, axis:%d\n", e.jaxis.which, e.jaxis.value, e.jaxis.axis);
        if (e.jaxis.value == 32767 && e.jaxis.axis == 0 && e.jaxis.which == 0 )
        {    //right
            joystick_states[JOY_P1_RIGHT].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P1_RIGHT].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == -32768 && e.jaxis.axis == 0 && e.jaxis.which == 0 )
        {    //left
            joystick_states[JOY_P1_LEFT].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P1_LEFT].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == 32767 && e.jaxis.axis == 1 && e.jaxis.which == 0 )
        {   //down
            joystick_states[JOY_P1_DOWN].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P1_DOWN].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == -32768 && e.jaxis.axis == 1 && e.jaxis.which == 0 )
        {   //up
            joystick_states[JOY_P1_UP].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P1_UP].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == 0 && e.jaxis.which == 0 )
        {
            joystick_states[JOY_P1_RIGHT].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P1_RIGHT].up_or_down_frame = curframe;
            joystick_states[JOY_P1_LEFT].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P1_LEFT].up_or_down_frame = curframe;
            joystick_states[JOY_P1_DOWN].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P1_DOWN].up_or_down_frame = curframe;
            joystick_states[JOY_P1_UP].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P1_UP].up_or_down_frame = curframe;
        }

        if (e.jaxis.value == 32767 && e.jaxis.axis == 0 && e.jaxis.which == 1)
        {    //right
            joystick_states[JOY_P2_RIGHT].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P2_RIGHT].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == -32768 && e.jaxis.axis == 0 && e.jaxis.which == 1)
        {    //left
            joystick_states[JOY_P2_LEFT].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P2_LEFT].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == 32767 && e.jaxis.axis == 1 && e.jaxis.which == 1)
        {   //down
            joystick_states[JOY_P2_DOWN].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P2_DOWN].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == -32768 && e.jaxis.axis == 1 && e.jaxis.which == 1)
        {   //up
            joystick_states[JOY_P2_UP].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[JOY_P2_UP].up_or_down_frame = curframe;
        }
        else if (e.jaxis.value == 0 && e.jaxis.which == 1)
        {
            joystick_states[JOY_P2_RIGHT].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P2_RIGHT].up_or_down_frame = curframe;
            joystick_states[JOY_P2_LEFT].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P2_LEFT].up_or_down_frame = curframe;
            joystick_states[JOY_P2_DOWN].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P2_DOWN].up_or_down_frame = curframe;
            joystick_states[JOY_P2_UP].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[JOY_P2_UP].up_or_down_frame = curframe;
        }
    }
    else if (e.type == SDL_JOYBUTTONDOWN ) {
        printf("JoyStickButtonDown, which:%d, button:%d, state:%d\n", e.jbutton.which, e.jbutton.button, e.jbutton.state);
        if (e.jbutton.which == 0)
        {
            joystick_states[e.jbutton.button  + JOY_P1_B1].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[e.jbutton.button  + JOY_P1_B1].up_or_down_frame = curframe;
        }
        else
        {
            joystick_states[e.jbutton.button + JOY_P2_B1].state = INPUT_STATE::INPUT_STATE_DOWN;
            joystick_states[e.jbutton.button + JOY_P2_B1].up_or_down_frame = curframe;
        }
    }
    else if (e.type == SDL_JOYBUTTONUP) {
        printf("JoyStickButtonDown, which:%d, button:%d, state:%d\n", e.jbutton.which, e.jbutton.button, e.jbutton.state);
        if (e.jbutton.which == 0)
        {
            joystick_states[e.jbutton.button + JOY_P1_B1].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[e.jbutton.button + JOY_P1_B1].up_or_down_frame = curframe;
        }
        else
        {
            joystick_states[e.jbutton.button + JOY_P2_B1].state = INPUT_STATE::INPUT_STATE_UP;
            joystick_states[e.jbutton.button + JOY_P2_B1].up_or_down_frame = curframe;
        }
    }
    else if (e.type == SDL_KEYDOWN) {
        keyboard_states[e.key.keysym.scancode].state = INPUT_STATE::INPUT_STATE_DOWN;
        keyboard_states[e.key.keysym.scancode].up_or_down_frame = curframe;
    } else if (e.type == SDL_KEYUP) {
        keyboard_states[e.key.keysym.scancode].state = INPUT_STATE::INPUT_STATE_UP;
        keyboard_states[e.key.keysym.scancode].up_or_down_frame = curframe;
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        mouse_button_states[e.button.button].state = INPUT_STATE::INPUT_STATE_DOWN;
        mouse_button_states[e.button.button].up_or_down_frame = curframe;
    } else if (e.type == SDL_MOUSEBUTTONUP) {
        mouse_button_states[e.button.button].state = INPUT_STATE::INPUT_STATE_UP;
        mouse_button_states[e.button.button].up_or_down_frame = curframe;
    }

    if (e.type == SDL_MOUSEWHEEL)
    {
        mouse_scroll_delta = e.wheel.preciseY;
    }
    else
    {
        mouse_scroll_delta = 0;
    }

    if (e.type == SDL_MOUSEMOTION)
    {
        mouse_pos.x = e.motion.x;
        mouse_pos.y = e.motion.y;
    }
}

bool Input::GetKey(SDL_Scancode keycode) {
    return keyboard_states[keycode].state == INPUT_STATE::INPUT_STATE_DOWN;
}

bool Input::GetControllerKey(int keycode) {
    return joystick_states[keycode].state == INPUT_STATE::INPUT_STATE_DOWN;
}

bool Input::GetKeyDown(SDL_Scancode keycode) {
    return keyboard_states[keycode].state == INPUT_STATE::INPUT_STATE_DOWN &&
           keyboard_states[keycode].up_or_down_frame == Helper::GetFrameNumber();
}

bool Input::GetKeyUp(SDL_Scancode keycode) {
    return keyboard_states[keycode].state == INPUT_STATE::INPUT_STATE_UP &&
           keyboard_states[keycode].up_or_down_frame == Helper::GetFrameNumber();
}

bool Input::GetMouseButton(int button) {
    return mouse_button_states[button].state == INPUT_STATE::INPUT_STATE_DOWN;
}

bool Input::GetMouseButtonDown(int button) {
    return mouse_button_states[button].state == INPUT_STATE::INPUT_STATE_DOWN &&
           mouse_button_states[button].up_or_down_frame == Helper::GetFrameNumber();
}

bool Input::GetMouseButtonUp(int button) {
    return mouse_button_states[button].state == INPUT_STATE::INPUT_STATE_UP &&
           mouse_button_states[button].up_or_down_frame == Helper::GetFrameNumber();
}
