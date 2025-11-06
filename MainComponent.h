#pragma once						// MainComponent.h
#include <JuceHeader.h>
#include "PlayerGUI.h"
using namespace juce;
using namespace std;
class MainComponent : public AudioAppComponent,
    public Button::Listener,
    public Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void resized() override;

    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;


    bool keyPressed(const KeyPress& key);

private:
    PlayerGUI player1;
    PlayerGUI player2;

    float player1_mix_level = 0.5f;
    float player2_mix_level = 0.5f;

    ToggleButton mixerToggleButton;
    bool mixer_enabled = true;
    Slider player1_mix_slider;
    Slider player2_mix_slider;
    Label player1_mix_label;
    Label player2_mix_label;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};