#pragma once						// PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"
using namespace juce;
using namespace std;

class PlayerGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void paint(Graphics& g) override;
    void timerCallback() override;

private:
    PlayerAudio playerAudio;

    // GUI elements
    TextButton loadButton{ "Load File" };
    TextButton restartButton{ "Restart" };
    TextButton stopButton{ "Stop" };
    TextButton loopButton;
    Slider volumeSlider;
    Slider positionSlider;



    unique_ptr<FileChooser> fileChooser;

    // Event handlers
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};
