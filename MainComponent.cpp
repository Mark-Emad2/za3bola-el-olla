#include "MainComponent.h"
using namespace juce;
using namespace std;
//==============================================================================
MainComponent::MainComponent() {

    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(1200, 800);
    setAudioChannels(0, 2);

    mixerToggleButton.setButtonText("Mixer: ON");
    mixerToggleButton.setToggleState(true, dontSendNotification);
    mixerToggleButton.addListener(this);
    mixerToggleButton.setColour(ToggleButton::textColourId, Colours::white);
    mixerToggleButton.setColour(ToggleButton::tickColourId, Colours::lime);
    mixerToggleButton.setColour(ToggleButton::tickDisabledColourId, Colours::red);
    addAndMakeVisible(mixerToggleButton);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    if (mixer_enabled) {
        // clear buffer first
        bufferToFill.clearActiveBufferRegion();

        // make temp buffer
        AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);

		// player 1 directly to output
        player1.getNextAudioBlock(bufferToFill);

		// player 2 to temp buffer
        player2.getNextAudioBlock(tempInfo);

		// mix two palyers together
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            auto* output = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            auto* player2Data = tempBuffer.getReadPointer(channel);

            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                output[sample] += player2Data[sample] * 0.5f; // 50% volume
            }
        }
    }
    else {
        player1.getNextAudioBlock(bufferToFill);
    }

}

void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}
void MainComponent::resized()
{
    auto area = getLocalBounds();

    // but mixeer on top
    auto mixerArea = area.removeFromTop(40);
    mixerToggleButton.setBounds(mixerArea.reduced(10, 5));

    // split players
    auto player1Area = area.removeFromLeft(getWidth() / 2);
    auto player2Area = area;

    player1.setBounds(player1Area.reduced(10));
    player2.setBounds(player2Area.reduced(10));
}
void MainComponent::button_clicked(Button* button) {
    if (button == &mixerToggleButton) {
        mixer_enabled = mixerToggleButton.getToggleState();

        if (mixer_enabled) {
            mixerToggleButton.setButtonText("Mixer: ON");
            mixerToggleButton.setColour(ToggleButton::textColourId, Colours::lime);
        }
        else {
            mixerToggleButton.setButtonText("Mixer: OFF");
            mixerToggleButton.setColour(ToggleButton::textColourId, Colours::red);
        }
    }
}



