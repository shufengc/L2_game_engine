#pragma once
#include <unordered_map>
#include <string>
#include "SDL.h"

const std::unordered_map<std::string, SDL_Scancode> __keycode_to_scancode = {
	// joystick player1 Keys
	{"joy_p1_up", JOY_P1_UP},
	{"joy_p1_down", JOY_P1_DOWN},
	{"joy_p1_right", JOY_P1_RIGHT},
	{"joy_p1_left", JOY_P1_LEFT},

	{"joy_p1_button0", JOY_P1_B1},
	{"joy_p1_button1", JOY_P1_B2},
	{"joy_p1_button2", JOY_P1_B3},
	{"joy_p1_button3", JOY_P1_B4},
	{"joy_p1_button4", JOY_P1_B5},
	{"joy_p1_button5", JOY_P1_B6},
	{"joy_p1_button6", JOY_P1_B7},
	{"joy_p1_button7", JOY_P1_B8},

	// joystick player2 Keys
	{"joy_p2_up", JOY_P2_UP},
	{"joy_p2_down", JOY_P2_DOWN},
	{"joy_p2_right", JOY_P2_RIGHT},
	{"joy_p2_left", JOY_P2_LEFT},

	{"joy_p2_button0", JOY_P2_B1},
	{"joy_p2_button1", JOY_P2_B2},
	{"joy_p2_button2", JOY_P2_B3},
	{"joy_p2_button3", JOY_P2_B4},
	{"joy_p2_button4", JOY_P2_B5},
	{"joy_p2_button5", JOY_P2_B6},
	{"joy_p2_button6", JOY_P2_B7},
	{"joy_p2_button7", JOY_P2_B8},

	// Directional (arrow) Keys
	{"up", SDL_SCANCODE_UP},
	{"down", SDL_SCANCODE_DOWN},
	{"right", SDL_SCANCODE_RIGHT},
	{"left", SDL_SCANCODE_LEFT},

	// Misc Keys
	{"escape", SDL_SCANCODE_ESCAPE},

	// Modifier Keys
	{"lshift", SDL_SCANCODE_LSHIFT},
	{"rshift", SDL_SCANCODE_RSHIFT},
	{"lctrl", SDL_SCANCODE_LCTRL},
	{"rctrl", SDL_SCANCODE_RCTRL},
	{"lalt", SDL_SCANCODE_LALT},
	{"ralt", SDL_SCANCODE_RALT},

	// Editing Keys
	{"tab", SDL_SCANCODE_TAB},
	{"return", SDL_SCANCODE_RETURN},
	{"enter", SDL_SCANCODE_RETURN},
	{"backspace", SDL_SCANCODE_BACKSPACE},
	{"delete", SDL_SCANCODE_DELETE},
	{"insert", SDL_SCANCODE_INSERT},

	// Character Keys
	{"space", SDL_SCANCODE_SPACE},
	{"a", SDL_SCANCODE_A},
	{"b", SDL_SCANCODE_B},
	{"c", SDL_SCANCODE_C},
	{"d", SDL_SCANCODE_D},
	{"e", SDL_SCANCODE_E},
	{"f", SDL_SCANCODE_F},
	{"g", SDL_SCANCODE_G},
	{"h", SDL_SCANCODE_H},
	{"i", SDL_SCANCODE_I},
	{"j", SDL_SCANCODE_J},
	{"k", SDL_SCANCODE_K},
	{"l", SDL_SCANCODE_L},
	{"m", SDL_SCANCODE_M},
	{"n", SDL_SCANCODE_N},
	{"o", SDL_SCANCODE_O},
	{"p", SDL_SCANCODE_P},
	{"q", SDL_SCANCODE_Q},
	{"r", SDL_SCANCODE_R},
	{"s", SDL_SCANCODE_S},
	{"t", SDL_SCANCODE_T},
	{"u", SDL_SCANCODE_U},
	{"v", SDL_SCANCODE_V},
	{"w", SDL_SCANCODE_W},
	{"x", SDL_SCANCODE_X},
	{"y", SDL_SCANCODE_Y},
	{"z", SDL_SCANCODE_Z},
	{"0", SDL_SCANCODE_0},
	{"1", SDL_SCANCODE_1},
	{"2", SDL_SCANCODE_2},
	{"3", SDL_SCANCODE_3},
	{"4", SDL_SCANCODE_4},
	{"5", SDL_SCANCODE_5},
	{"6", SDL_SCANCODE_6},
	{"7", SDL_SCANCODE_7},
	{"8", SDL_SCANCODE_8},
	{"9", SDL_SCANCODE_9},
	{"/", SDL_SCANCODE_SLASH},
	{";", SDL_SCANCODE_SEMICOLON},
	{"=", SDL_SCANCODE_EQUALS},
	{"-", SDL_SCANCODE_MINUS},
	{".", SDL_SCANCODE_PERIOD},
	{",", SDL_SCANCODE_COMMA},
	{"[", SDL_SCANCODE_LEFTBRACKET},
	{"]", SDL_SCANCODE_RIGHTBRACKET},
	{"\\", SDL_SCANCODE_BACKSLASH},
	{"'", SDL_SCANCODE_APOSTROPHE}
};