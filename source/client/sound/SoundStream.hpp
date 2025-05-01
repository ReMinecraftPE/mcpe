#pragma once

#include <string>

class SoundStream
{
public:
    virtual ~SoundStream();

    virtual bool isStreaming() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isPlaying() const { return isStreaming() && !isPaused(); }

    virtual bool open(const std::string& fileName) = 0;
    virtual void close() = 0;
    virtual void update();

    virtual void setPausedState(bool state) = 0;
    virtual void setVolume(float vol) = 0;
};