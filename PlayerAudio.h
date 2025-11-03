#pragma once							// PlayerAudio.h
#include <JuceHeader.h>
using namespace std;
using namespace juce;
class PlayerAudio
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    bool loadFile(const File& file);
    void start();
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;
    void mute();
    bool muted()const;
    void loop();
    bool isLooping() const;
    float get_current_gain()const;
    bool isPlaying() const;

    File getCurrentFile() const;
    void set_speed(float speed);
    float get_speed() const;
    void setALoopPoint(double pos);
    void setBLoopPoint(double pos);
    double getALoopPoint() const;
    double getBLoopPoint() const;




private:
    AudioFormatManager formatManager;
    unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    ResamplingAudioSource resampleSource;
    float last_value = 1.0f;
    bool ismuted = false;
    bool islooping = false;
    float current_speed = 1.0f;
    double aLoopPoint = 0.0;
    double bLoopPoint = 0.0;

    File currentFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};