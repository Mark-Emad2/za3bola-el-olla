#include <JuceHeader.h>
#include "PlayerGUI.h"
using namespace std;
using namespace juce;
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}
PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton , &loopButton,&Pause_ResumeButton,&EndButton,&muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setSliderStyle(Slider::LinearVertical);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    loopButton.setButtonText("Loop\nOOF");
    loopButton.setColour(TextButton::buttonColourId, Colours::red);
    loopButton.repaint();
    startTimerHz(10);
    loopButton.setClickingTogglesState(true);

    Pause_ResumeButton.addListener(this);
    Pause_ResumeButton.setColour(TextButton::buttonColourId, Colours::green);
    Pause_ResumeButton.repaint();
}
PlayerGUI::~PlayerGUI()
{
}
void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    loadButton.setColour(TextButton::buttonColourId, Colours::blue);

    loopButton.setBounds(250, y, 80, 40);

    volumeSlider.setBounds(getWidth() - 50, 100, 30, 200);
    //volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    volumeSlider.setColour(Slider::trackColourId, Colours::darkgrey);


    restartButton.setBounds(250, 350, 80, 40);
    Pause_ResumeButton.setBounds(350, 350, 80, 40);
    stopButton.setBounds(450, 350, 80, 40);
    EndButton.setBounds(550, 350, 80, 40);

    //prevButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);

}
void PlayerGUI::buttonClicked(Button* button)
{

    if (button == &loadButton)
    {
        FileChooser chooser("Select audio files...",
            File{},
            "*.wav;*.mp3");

        fileChooser = make_unique<FileChooser>(
            "Select an audio file...",
            File{},
            "*.wav;*.mp3;*.dat");

        fileChooser->launchAsync(
            FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
            [this](const FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile()) {
                    playerAudio.loadFile(file);
                }

            });
    }

    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &loopButton) {
        if (loopButton.getToggleState())
        {
            loopButton.setButtonText("Loop\nON");
            loopButton.setColour(TextButton::buttonOnColourId, Colours::green);
            loopButton.repaint();
        }
        else {
            loopButton.setButtonText("Loop\nOFF");
            loopButton.setColour(TextButton::buttonColourId, Colours::red);
            loopButton.repaint();
        }
    }
    else if (button == &muteButton) {
        playerAudio.mute();

        if (playerAudio.muted()) {
            muteButton.setButtonText("unmute");
        }
        else {
            muteButton.setButtonText("mute");
        }
    }
    else if (button == &Pause_ResumeButton) {
        if (playerAudio.isPlaying()) {
            playerAudio.stop();
            Pause_ResumeButton.setButtonText("Resume");
            Pause_ResumeButton.setColour(TextButton::buttonColourId, Colours::orange);
        }
        else {
            playerAudio.start();
            Pause_ResumeButton.setButtonText("Pause");
            Pause_ResumeButton.setColour(TextButton::buttonColourId, Colours::green);
        }
    }//⏸️⏯️
    else if (button == &EndButton) {
        playerAudio.setPosition(playerAudio.getLength());
    }




}

void PlayerGUI::sliderValueChanged(Slider* slider)
{
    /*volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
        false,
        0,
        0);*/ // هنا المربع القيمة اللي جواه مش ظاهره عندي معرفش ليه رغم اني مغيرتش حاجه المهم ده كود يشيل الصنةدق كله


    if (slider == &volumeSlider)
    {
        float gainValue = (float)slider->getValue();
        playerAudio.setGain(gainValue);
    }
}


void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}

void PlayerGUI::timerCallback()
{
    if (loopButton.getToggleState())
    {
        double currentPos = playerAudio.getPosition();
        double totalLength = playerAudio.getLength();
        if (totalLength > 0 && currentPos >= totalLength)
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
        }
    }

}