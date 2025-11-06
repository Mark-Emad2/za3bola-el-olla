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
    mixerToggleButton.setColour(ToggleButton::textColourId, Colours::cyan);
    mixerToggleButton.setColour(ToggleButton::tickColourId, Colours::cyan);
    mixerToggleButton.setColour(ToggleButton::tickDisabledColourId, Colours::mediumpurple);
    addAndMakeVisible(mixerToggleButton);

    // Player 1 Mix Slider
    player1_mix_slider.setRange(0.0, 1.0, 0.01);
    player1_mix_slider.setValue(0.5);
    player1_mix_slider.setTextBoxStyle(Slider::TextBoxRight, false, 60, 20);
    player1_mix_slider.setSliderStyle(Slider::LinearHorizontal);
    player1_mix_slider.addListener(this);
    addAndMakeVisible(player1_mix_slider);

    // Player 1 Mix Label
    player1_mix_label.setText("Player 1 Volume:", dontSendNotification);
    player1_mix_label.setJustificationType(Justification::centredRight);
    player1_mix_label.attachToComponent(&player1_mix_slider, true);
    addAndMakeVisible(player1_mix_label);

    player2_mix_slider.setRange(0.0, 1.0, 0.01);
    player2_mix_slider.setValue(0.5);
    player2_mix_slider.setTextBoxStyle(Slider::TextBoxRight, false, 60, 20);
    player2_mix_slider.setSliderStyle(Slider::LinearHorizontal);
    player2_mix_slider.addListener(this);
    addAndMakeVisible(player2_mix_slider);

    player2_mix_label.setText("Player 2 Volume:", dontSendNotification);
    player2_mix_label.setJustificationType(Justification::centredRight);
    player2_mix_label.attachToComponent(&player2_mix_slider, true);
    addAndMakeVisible(player2_mix_slider);
    player1_mix_slider.setColour(Slider::thumbColourId, Colours::cyan);
    player1_mix_slider.setColour(Slider::trackColourId, Colours::purple);
    player1_mix_slider.setColour(Slider::textBoxTextColourId, Colours::white);
    player2_mix_slider.setColour(Slider::thumbColourId, Colours::cyan);
    player2_mix_slider.setColour(Slider::trackColourId, Colours::purple);
    player2_mix_slider.setColour(Slider::textBoxTextColourId, Colours::white);

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
        if (mixer_enabled) {
            // clear the buffer
            bufferToFill.clearActiveBufferRegion();

            // make temp buffers for palyers
            AudioBuffer<float> tempBuffer1(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
            AudioBuffer<float> tempBuffer2(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);

            AudioSourceChannelInfo tempInfo1(&tempBuffer1, 0, bufferToFill.numSamples);
            AudioSourceChannelInfo tempInfo2(&tempBuffer2, 0, bufferToFill.numSamples);

            // get audio from players
            player1.getNextAudioBlock(tempInfo1);
            player2.getNextAudioBlock(tempInfo2);

            // audio mixing
            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
            {


                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    float p1_sample = tempBuffer1.getSample(channel, sample);  // Function call + copy
                    float p2_sample = tempBuffer2.getSample(channel, sample);  // Function call + copy
                    float result = (p1_sample * player1_mix_level) +
                        (p2_sample * player2_mix_level);
                    bufferToFill.buffer->setSample(channel, sample, result);

                }
            }
        }
    }
    //disable mixer
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

    // Mixer button and sliders at the top
    auto mixerArea = area.removeFromTop(60); // Even more compact

    // Mixer toggle button
    auto buttonArea = mixerArea.removeFromTop(25);
    mixerToggleButton.setBounds(buttonArea.reduced(10, 2));

    // Mixer sliders area - side by side
    auto slidersArea = mixerArea.reduced(10, 5);

    // Smaller dimensions
    int sliderHeight = 18;
    int labelWidth = 70;
    int sliderWidth = 120;

    // Split area for two sliders side by side
    auto player1Area = slidersArea.removeFromLeft(getWidth() / 2);
    auto player2Area = slidersArea;

    // Player 1 slider (left side)
    auto player1SliderArea = player1Area.withSizeKeepingCentre(labelWidth + sliderWidth + 10, sliderHeight);
    player1_mix_label.setBounds(player1SliderArea.removeFromLeft(labelWidth));
    player1_mix_slider.setBounds(player1SliderArea.reduced(2, 0));

    auto player2SliderArea = player2Area.withSizeKeepingCentre(labelWidth + sliderWidth + 10, sliderHeight);
    player2_mix_label.setBounds(player2SliderArea.removeFromLeft(labelWidth));
    player2_mix_slider.setBounds(player2SliderArea.reduced(2, 0));

    auto player1PlayerArea = area.removeFromLeft(getWidth() / 2);
    auto player2PlayerArea = area;

    player1.setBounds(player1PlayerArea.reduced(10));
    player2.setBounds(player2PlayerArea.reduced(10));
}
void MainComponent::buttonClicked(Button* button) {
    if (button == &mixerToggleButton) {
        mixer_enabled = mixerToggleButton.getToggleState();

        if (mixer_enabled) {
            mixerToggleButton.setButtonText("Mixer: ON");
            mixerToggleButton.setColour(ToggleButton::textColourId, Colours::cyan);

            // Show mixer sliders when enabled
            player1_mix_slider.setVisible(true);
            player2_mix_slider.setVisible(true);
            player1_mix_label.setVisible(true);
            player2_mix_label.setVisible(true);
        }
        else {
            mixerToggleButton.setButtonText("Mixer: OFF");
            mixerToggleButton.setColour(ToggleButton::textColourId, Colours::hotpink);

            // Hide mixer sliders when disabled
            player1_mix_slider.setVisible(false);
            player2_mix_slider.setVisible(false);
            player1_mix_label.setVisible(false);
            player2_mix_label.setVisible(false);

        }

        // Trigger resizing to adjust layout
        resized();
    }
}
void MainComponent::sliderValueChanged(Slider* slider)
{
    if (slider == &player1_mix_slider)
    {
        player1_mix_level = (float)player1_mix_slider.getValue();
    }
    else if (slider == &player2_mix_slider)
    {
        player2_mix_level = (float)player2_mix_slider.getValue();
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
    // Next song (N key)
    if (key.getKeyCode() == 'n' || key.getKeyCode() == 'N')
    {
        if (ctrlPressed) {
            player1.EndButton.triggerClick();
            player2.EndButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.EndButton.triggerClick();
        }
        else {
            player1.EndButton.triggerClick();
        }
        return true;
    }
    // Previous song (P key)
    if (key.getKeyCode() == 'p' || key.getKeyCode() == 'P')
    {
        if (ctrlPressed) {
            player1.restart_PreviousButton.triggerClick();
            player2.restart_PreviousButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.restart_PreviousButton.triggerClick();
        }
        else {
            player1.restart_PreviousButton.triggerClick();
        }
        return true;
    }
    if (key.getKeyCode() == 'k' || key.getKeyCode() == 'K') // marker (k)
    {
        if (ctrlPressed) {
            player1.addMarkerButton.triggerClick();
            player2.addMarkerButton.triggerClick();
        }
        else if (shiftPressed) {
            player2.addMarkerButton.triggerClick();
        }
        else {
            player1.addMarkerButton.triggerClick();
        }
        return true;
    }

    return false;
    //جديد

}
