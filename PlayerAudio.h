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
    float get_current_gain()const;
    bool isPlaying() const;

    File getCurrentFile() const;



private:
    AudioFormatManager formatManager;
    unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource;
    float last_value = 1.0f;
    bool ismuted = false;

    File currentFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};