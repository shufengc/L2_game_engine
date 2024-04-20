#include "Player.h"
#include <cassert>

void Player::set_image(SDL_Texture *hp_image) {
    this->hp_image = hp_image;
    SDL_QueryTexture(hp_image, NULL, NULL, &hp_image_width, &hp_image_height);
}

bool Player::damage(Actor &attacker, int amount) {
    assert(amount > 0 && "amount must be positive");

    int curframe = Helper::GetFrameNumber();

    if (curframe < cooldown_frame) {
        return false;
    }

#ifdef DEBUG
    std::cout << "Player damaged: " << amount << " at frame: " << curframe << std::endl;
#endif

    health -= amount;
    cooldown_frame = curframe + Const::PLAYER_COOLDOWN_FRAMES;

    actor->damage(attacker);

    return true;
}

void Player::gain_score(int amount) {
    assert(amount > 0 && "amount must be positive");
    score += amount;
    set_score_text();
}
