﻿#pragma once						// PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"
using namespace juce;
//using namespace std;

class PlayerGUI : public Component,
    public Button::Listener,
    public Slider::Listener,
    public Timer,
    public ListBoxModel // فيه الفنكشنز الى هي
    //getNumRows و paintListBoxItem و listBoxItemDoubleClicked
      // و selectedRowsChanged
      // كل حاجه عاوزها فى البوكس بتاعة البلاي لست


{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void updateLabel(const File& file);
    //void updateLabelUsingFFprobe(const File& file);// ما هذا


    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void paint(Graphics& g) override;
    void timerCallback() override;

    int getNumRows() override;
    void paintListBoxItem(int row, Graphics& graph, int width, int height, bool rowIsSelected);
    void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
    void selectedRowsChanged(int lastRow) override;
    void listBoxItemClicked(int row, const MouseEvent& e)override;


private:
    PlayerAudio playerAudio;

    // GUI elements
    TextButton loadButton{ "Load File" };
    TextButton restart_PreviousButton{ "|< Previous" };
    TextButton stopButton{ "Stop" };
    TextButton loopButton;
    TextButton Pause_PlayButton{ "Pause ||" };
    TextButton EndButton{ "Next >|" };
    TextButton muteButton{ "Mute" };
    TextButton addToPlaylistButton{ "Add To Playlist +_+" };
    //TextButton removeButton{ "Remove" };

    Slider volumeSlider;
    Slider positionSlider;
    Slider speed_slider;
    Label speed_label;
    Label poslabel;
    Label endPos;
    AudioFormatManager formatManager;



    unique_ptr<FileChooser> fileChooser;
    std::unique_ptr<juce::Drawable> playIconDrawable;


    // Event handlers
    void buttonClicked(Button* button) override;
    void sliderValueChanged(Slider* slider) override;

    Label infoLabel;

    String displayText; //cpp على فكره ممكن احطه عادى ف ملف ال 



    File sessionFile;
    ValueTree appState{ "AppState" };
    void saveLastState();
    void loadLastState();

    ListBox playlistBox;
    StringArray playlist;
    int currentIndex{ -1 };
    void playIndex(int row);
    static void safeButton_Colour(TextButton& btn, const String& text, const Colour& col);



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};