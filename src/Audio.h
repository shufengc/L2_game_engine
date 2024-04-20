#ifndef __AUDIO_H__
#define __AUDIO_H__

#include "AudioHelper.h"
#include "Constants.h"
#include "Sfx.h"
#include <vector>

class Audio {
  public:
    Audio() = default;

    // Play the audio on the given channel with the given number of loops.
    // If the audio is already playing on the channel, it will be halted and
    // restarted.
    // loop = -1 means loop forever.
    // loop = 0 means play once.
    void play(Mix_Chunk *chunk, int channel, int loops) {
        // Halt the channel before playing
        // halt(channel);

        // Play the audio
        if (chunk) {
            AudioHelper::Mix_PlayChannel498(channel, chunk, loops);
            playing[channel] = true;
        }
    }

    // Play the `Sfx` audio on the given channel with the given number of loops.
    // If the audio is already playing on the channel, it will be halted and
    // restarted.
    // loop = -1 means loop forever.
    // loop = 0 means play once.
    void play(const Sfx &sfx, int channel, int loops) { play(sfx.get_chunk(), channel, loops); }

    // Play the audio with the given name on the given channel with the given number of loops.
    // If the audio is already playing on the channel, it will be halted and
    // restarted.
    // loop = -1 means loop forever.
    // loop = 0 means play once.
    void play(const std::string &audio_name, int channel, int loops) {
        play(Resources::get_instance().get_audio(audio_name), channel, loops);
    }

    // Halt the audio on the given channel.
    // If the audio is not playing on the channel, do nothing.
    void halt(int channel) {
        // If this channel is not playing, do nothing.
        if (!playing[channel]) {
            return;
        }

        // Halt the channel
        AudioHelper::Mix_HaltChannel498(channel);
        playing[channel] = false;
    }

  private:
    std::vector<bool> playing = std::vector<bool>(Const::NUM_MIX_CHANNELS, false);
};

#endif // __AUDIO_H__
