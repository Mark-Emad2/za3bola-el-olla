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


    addAndMakeVisible(poslabel);
    addAndMakeVisible(endPos);




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

    // Position slider
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);
    positionSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
        false,
        0,
        0);

    //loop button
    loopButton.setButtonText("Loop\nOOF");
    loopButton.setColour(TextButton::buttonColourId, Colours::red);
    loopButton.repaint();
    loopButton.setClickingTogglesState(true);

    //دي تخص  التايمر
    startTimerHz(10);

    sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("player_session.xml");
    loadLastState();


    Pause_PlayButton.setButtonText("Play");
    Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
    Pause_PlayButton.repaint();

}
PlayerGUI::~PlayerGUI()
{
    saveLastState();
}


void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    loadButton.setColour(TextButton::buttonColourId, Colours::blue);

    loopButton.setBounds(250, y, 80, 40);

    volumeSlider.setBounds(getWidth() - 110, 100, 100, 200);
    positionSlider.setBounds(20, 450, getWidth() - 40, 30);

    //volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
    volumeSlider.setColour(Slider::trackColourId, Colours::darkgrey);


    restartButton.setBounds(250, 350, 80, 40);
    Pause_PlayButton.setBounds(350, 350, 80, 40);
    stopButton.setBounds(450, 350, 80, 40);
    EndButton.setBounds(550, 350, 80, 40);

    //prevButton.setBounds(340, y, 80, 40);
    muteButton.setBounds(440, y, 80, 40);

    infoLabel.setBounds(20, 100, getWidth() - 40, 60);
    poslabel.setBounds(20, 500, 100, 30);
    endPos.setBounds(getWidth() - 120, 500, 100, 30);


}

void PlayerGUI::updateLabel(const File& file) {
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


void PlayerGUI::saveLastState()
{
    File currentFile = playerAudio.getCurrentFile();
    appState.setProperty("lastFile", currentFile.getFullPathName(), nullptr);
    appState.setProperty("lastPosition", playerAudio.getPosition(), nullptr);
    appState.setProperty("lastVolume", volumeSlider.getValue(), nullptr);
    appState.setProperty("lastPositionSlider", positionSlider.getValue(), nullptr);

    unique_ptr<XmlElement> xml(appState.createXml());

    if (xml) {
        xml->writeTo(sessionFile);
    }

}

void PlayerGUI::loadLastState() {

    if (sessionFile.existsAsFile()) {

        //playerAudio.stop();

        unique_ptr<XmlElement> xml(parseXML(sessionFile));
        if (xml) {
            appState = ValueTree::fromXml(*xml);
            String lastFilePath = appState.getProperty("lastFile").toString();
            double lastPosition = (double)appState.getProperty("lastPosition");
            double lastVolume = (double)appState.getProperty("lastVolume", 50);
            double lastPositionSlider = (double)appState.getProperty("lastPositionSlider", 0.0);

            if (!lastFilePath.isEmpty()) {

                playerAudio.loadFile(File(lastFilePath));
                double lengthInSeconds = playerAudio.getLength();
                positionSlider.setRange(0.0, lengthInSeconds);
                playerAudio.setPosition(lastPosition);
                volumeSlider.setValue(lastVolume, dontSendNotification);
                positionSlider.setValue(lastPositionSlider, dontSendNotification);
                updateLabel(File(lastFilePath));
            }
        }
    }
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
                    double lengthInSeconds = playerAudio.getLength();
                    positionSlider.setRange(0.0, lengthInSeconds);
                    Pause_PlayButton.setButtonText("pause ||");
                    Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::orange);
                    Pause_PlayButton.repaint();
                    playerAudio.start();
                    saveLastState();
                    updateLabel(file);


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
    else if (slider == &positionSlider)
    {
        playerAudio.setPosition((float)slider->getValue());
    }
}



void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);
}


void PlayerGUI::timerCallback()
{
    positionSlider.setValue(playerAudio.getPosition(), dontSendNotification);

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



    // label of the position in minutes and seconds 

    int mins = (int)(currentPos / 60);
    int secs = (int)(round(currentPos)) % 60;
    String positionText = String(mins).paddedLeft('0', 2) + ":" + String(secs).paddedLeft('0', 2);
    poslabel.setText(positionText, dontSendNotification);


    int mi = (int)(totalLength / 60);
    int se = (int)(round(totalLength)) % 60;
    String endPositionText = String(mi).paddedLeft('0', 2) + ":" + String(se).paddedLeft('0', 2);
    endPos.setText(endPositionText, dontSendNotification);
}
