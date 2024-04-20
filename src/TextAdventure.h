#ifndef __TEXT_ADVENTURE_H__
#define __TEXT_ADVENTURE_H__

#include "Audio.h"
#include "Input.h"
#include "Modding.h"
#include "Renderer.h"

class StageView;

// Game state enum.
enum GameState { GAME_STAT_RUNNING, GAME_STAT_QUIT };

// Game stage enum.
enum GameStage { GAME_STAGE_INTRO, GAME_STAGE_MAIN, GAME_STAGE_END };

// Game result enum.
enum GameResult { GAME_RESULT_WIN, GAME_RESULT_LOSE, GAME_RESULT_NONE };

class TextAdventure {
  public:
    TextAdventure();

    // Delete copy constructor and assignment operator.
    TextAdventure(const TextAdventure &) = delete;
    TextAdventure &operator=(const TextAdventure &) = delete;

    // Start the game.
    void start();

    const GameConfig &get_config() const { return config; }
    Renderer &get_renderer() { return renderer; }
    Audio &get_audio() { return audio; }
    Input &get_input() { return input; }
    GameResult get_game_result() const { return result; }

    void set_game_state(GameState state) { this->state = state; }
    void set_game_result(GameResult result) { this->result = result; }

    // Destructor.
    ~TextAdventure();

  protected:
    GameConfig config; // The game configuration.
    Audio audio; // The audio manager.
    Renderer renderer; // The renderer.
    Input input; // The input manager for mouse/keyboard events.

    StageView *view = nullptr;

    GameState state = GAME_STAT_RUNNING; // The game state. Only two states: running and quit. quit
                                         // means the game is about to exit.
    GameStage stage = GAME_STAGE_INTRO; // The game stage. There are three stages: intro, main, and
                                        // end. The `end` stage does not mean the game is about to
                                        // exit.
    GameResult result = GAME_RESULT_NONE; // The game result. There are three results: win, lose, and
                                          // none. The `none` result means the player quits the game before
                                          // the game ends. For example, the player closes the game window.
};

#endif // __TEXT_ADVENTURE_H__
