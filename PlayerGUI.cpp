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

    for (auto* btn : { &loadButton, &restartButton , &stopButton , &loopButton,&Pause_PlayButton,&EndButton,&muteButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    formatManager.registerBasicFormats();

    addAndMakeVisible(infoLabel);
    infoLabel.setJustificationType(Justification::centredLeft);
    infoLabel.setText("No File Loaded", dontSendNotification);
    infoLabel.setFont(Font("Arial", 16.0f, Font::bold));


    Pause_PlayButton.setButtonText("Play");
    Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
    Pause_PlayButton.repaint();
    // Volume slider

    volumeSlider.setRange(0, 100, 1);
    volumeSlider.setValue(50);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
        false,
        60,
        20);
    volumeSlider.setTextValueSuffix("%");
    volumeSlider.setSliderStyle(Slider::LinearVertical);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    loopButton.setButtonText("Loop\nOOF");
    loopButton.setColour(TextButton::buttonColourId, Colours::red);
    loopButton.repaint();
    startTimerHz(10);
    loopButton.setClickingTogglesState(true);

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

    volumeSlider.setBounds(getWidth() - 110, 100, 100, 200);
    //volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    volumeSlider.setColour(Slider::trackColourId, Colours::darkgrey);


    restartButton.setBounds(250, 350, 80, 40);
    Pause_PlayButton.setBounds(350, 350, 80, 40);
    stopButton.setBounds(450, 350, 80, 40);
    EndButton.setBounds(550, 350, 80, 40);

    //prevButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);

    infoLabel.setBounds(20, 100, getWidth() - 40, 60);


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
                    Pause_PlayButton.setButtonText("pause ||");
                    Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::orange);
                    Pause_PlayButton.repaint();
                    std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
                    if (reader != nullptr) {
                        auto metadata = reader->metadataValues;
                        if (metadata.size() > 0) {

                            String title = metadata.getValue("title", metadata.getValue("TITLE", "unKnown"));
                            // السطر ده معمول عشان لو الميتاداتا جواها العنوان مكتوب بكابيتال او سمول 
                            // ممكن اعملى زيه الى تحت نفس الفكره
                            /*string title = metadata.getValue("title","");
                            if (title.empty()) {
                                title = metadata.getValue("TITLE","unKnown");
                            }*/
                            String artist = metadata.getValue("artist", metadata.getValue("ARTIST", "unknown"));

                            double duration = reader->lengthInSamples / reader->sampleRate;
                            int mins = static_cast<int>(duration / 60);
                            int secs = static_cast<int>(round(duration)) % 60;

                            displayText = "Title: " + title +
                                "\nArtist: " + artist +
                                "\nDuration: " + String(mins).paddedLeft('0', 2) + ":" + String(secs).paddedLeft('0', 2);


                        }
                        else {
                            displayText = "File: " + file.getFileName();
                        }

                        infoLabel.setText(displayText, dontSendNotification);
                        // مترسلش نوتفيكشن ده معمول عشان مش عاوز اعرف اليوزر ان فيه تغير حصل فى التيكست بتاع الليبل

                    }
                    else {
                        infoLabel.setText("Failed to load audio file.", dontSendNotification);
                    }
                }

            });

    }

    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        Pause_PlayButton.setButtonText("pause ||");
        Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::orange);
        Pause_PlayButton.repaint();

        playerAudio.start();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
        Pause_PlayButton.setButtonText("Play");
        Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
        Pause_PlayButton.repaint();
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
    else if (button == &Pause_PlayButton) {
        if (playerAudio.isPlaying()) {
            playerAudio.stop();
            Pause_PlayButton.setButtonText("Play");
            Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
        }
        else {
            playerAudio.start();
            Pause_PlayButton.setButtonText("Pause ||");
            Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::orange);
        }
        Pause_PlayButton.repaint();
    }//⏸️⏯️
    else if (button == &EndButton) {
        playerAudio.setPosition(playerAudio.getLength());

        Pause_PlayButton.setButtonText("Play");
        Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
        Pause_PlayButton.repaint();
    }




}

void PlayerGUI::sliderValueChanged(Slider* slider)
{




    if (slider == &volumeSlider)
    {
        float gainValue = (float)slider->getValue();
        playerAudio.setGain(gainValue / 100);

    }
}



void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}

//void PlayerGUI::timerCallback()
//{
//    if (loopButton.getToggleState())
//    {
//        double currentPos = playerAudio.getPosition();
//        double totalLength = playerAudio.getLength();
//
//        if (totalLength > 0 && currentPos >= totalLength)
//        {
//            playerAudio.setPosition(0.0);
//            playerAudio.start();
//        }
//    }
//
//}
void PlayerGUI::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();

    if (totalLength > 0 && currentPos >= totalLength)
    {
        playerAudio.stop();
        Pause_PlayButton.setButtonText("Play");
        Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
        Pause_PlayButton.repaint();

        if (loopButton.getToggleState())
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
            Pause_PlayButton.setButtonText("Pause");
            Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::orange);
            Pause_PlayButton.repaint();
        }
    }
}
