#pragma once						// MainComponent.h
#include <JuceHeader.h>
#include "PlayerGUI.h"
using namespace juce;
using namespace std;
class MainComponent : public AudioAppComponent,
    public Button::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void resized() override;

    void buttonClicked(Button* button) override;

   
    bool keyPressed(const KeyPress& key) override;

private:
    PlayerGUI player1;
    PlayerGUI player2;

    float player1_mixLevel = 0.5f;
    float player2_mixLevel = 0.5f;

    ToggleButton mixerToggleButton;
    bool mixer_enabled = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};