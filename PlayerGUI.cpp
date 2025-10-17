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
    for (auto* btn : { &loadButton, &restartButton , &stopButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);



}
PlayerGUI::~PlayerGUI()
{
}
void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    restartButton.setBounds(140, y, 80, 40);
    stopButton.setBounds(240, y, 80, 40);
    //prevButton.setBounds(340, y, 80, 40);
    //nextButton.setBounds(440, y, 80, 40);

    volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
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
            "*.wav;*.mp3");

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



}

void PlayerGUI::sliderValueChanged(Slider* slider)
{
    /*volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
        false,
        0,
        0);*/ // هنا المربع القيمة اللي جواه مش ظاهره عندي معرفش ليه رغم اني مغيرتش حاجه المهم ده كود يشيل الصنةدق كله

    if (slider == &volumeSlider)
        playerAudio.setGain((float)slider->getValue());
}
void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}
