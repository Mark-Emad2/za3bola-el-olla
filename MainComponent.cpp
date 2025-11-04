#include "MainComponent.h"
using namespace juce;
using namespace std;
//==============================================================================
MainComponent::MainComponent()
    : player1("player1_session.xml"),
    player2("player2_session.xml")
{

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
        // clear buffer
        bufferToFill.clearActiveBufferRegion();

		// make temp buffer for player2
        AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);

		// player1 to output buffer
        player1.getNextAudioBlock(bufferToFill);

        // player2 to temporary buffer
        player2.getNextAudioBlock(tempInfo);

        // mix audio
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            auto* output = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
            auto* player2Data = tempBuffer.getReadPointer(channel);

            for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
            {
                output[sample] += player2Data[sample] * 0.5f;
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

    // mixer button at the top
    auto mixerArea = area.removeFromTop(40);
    mixerToggleButton.setBounds(mixerArea.reduced(10, 5));

	//split screen for two players
    auto player1Area = area.removeFromLeft(getWidth() / 2);
    auto player2Area = area;

    player1.setBounds(player1Area.reduced(10));
    player2.setBounds(player2Area.reduced(10));
}
void MainComponent::buttonClicked(Button* button) {
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
bool MainComponent::keyPressed(const KeyPress& key)
{
    bool ctrlPressed = key.getModifiers().isCtrlDown();
    bool shiftPressed = key.getModifiers().isShiftDown();

    // pause (space)
    if (key.getKeyCode() == KeyPress::spaceKey)
    {
        if (ctrlPressed) {
            // both players (button +control)
            player1.Pause_PlayButton.triggerClick();
            player2.Pause_PlayButton.triggerClick();
        }
        else if (shiftPressed) {
            // player 2 (shift+button)
            player2.Pause_PlayButton.triggerClick();
        }
        else {
            // player 1 (button)
            player1.Pause_PlayButton.triggerClick();
        }
        return true;
    }

    // stop (escape)
    if (key.getKeyCode() == 's' || key.getKeyCode() == 'S')
    {
        if (ctrlPressed) {
            player1.stopButton.triggerClick();
            player2.stopButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.stopButton.triggerClick();
        }
        else {
            player1.stopButton.triggerClick();
        }
        return true;
    }

    // mute (m)
    if (key.getKeyCode() == 'm' || key.getKeyCode() == 'M')
    {
        if (ctrlPressed) {
            player1.muteButton.triggerClick();
            player2.muteButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.muteButton.triggerClick();
        }
        else {
            player1.muteButton.triggerClick();
        }
        return true;
    }

    // loop (l)
    if (key.getKeyCode() == 'l' || key.getKeyCode() == 'L')
    {
        if (ctrlPressed) {
            player1.loopButton.triggerClick();
            player2.loopButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.loopButton.triggerClick();
        }
        else {
            player1.loopButton.triggerClick();
        }
        return true;
    }

    // move back (left arrow)
    if (key.getKeyCode() == KeyPress::leftKey)
    {
        if (ctrlPressed) {
            player1.backButton.triggerClick();
            player2.backButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.backButton.triggerClick();
        }
        else {
            player1.backButton.triggerClick();
        }
        return true;
    }
    //move forward (right arrow)
    if (key.getKeyCode() == KeyPress::rightKey)
    {
        if (ctrlPressed) {
            player1.forwardButton.triggerClick();
            player2.forwardButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.forwardButton.triggerClick();
        }
        else {
            player1.forwardButton.triggerClick();
        }
        return true;
    }

    return false;
}