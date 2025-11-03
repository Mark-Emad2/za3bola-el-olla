#pragma once // PlayerGUI.h
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
    , public LookAndFeel_V4
    , public ChangeListener
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
    void changeListenerCallback(ChangeBroadcaster* source) override;

    int getNumRows() override;
    void paintListBoxItem(int row, Graphics& graph, int width, int height, bool rowIsSelected);
    void listBoxItemDoubleClicked(int row, const MouseEvent&) override;
    void selectedRowsChanged(int lastRow) override;
    void listBoxItemClicked(int row, const MouseEvent& e)override;
    void resetLoopPoints();
    void drawLinearSlider(Graphics& g,
        int	x,
        int	y,
        int	width,
        int	height,
        float	sliderPos,
        float	minSliderPos,
        float	maxSliderPos,
        Slider::SliderStyle style,
        Slider& slider) override;


private:
    PlayerAudio playerAudio;

    // Waveform components
    AudioFormatManager formatManager;
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail audioThumbnail;
    bool fileLoaded;

    //waveform visualiser
    AudioVisualiserComponent waveformVisualiser;
    // GUI elements
    ImageButton loadButton;
    ImageButton restart_PreviousButton;
    ImageButton stopButton;
    ImageButton loopButton;
    ImageButton Pause_PlayButton;
    ImageButton EndButton;
    ImageButton muteButton;
    ImageButton addToPlaylistButton;
    ImageButton forwardButton;
    ImageButton backButton;
    ImageButton A_B_LOOP;
    int loopState = 0;
    bool AbLOOPING = false;
    double lengthInSeconds;


    //TextButton removeButton{ "Remove" };
    Slider positionSlider;
    Slider volumeSlider;
    Slider The_bar_pos;
    Slider speed_slider;
    Label speed_label;
    Label poslabel;
    Label endPos;
    Label volume_label;


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

    // الصور
    Image loopOnImage;
    Image loopOffImage;
    Image playImage;
    Image pauseImage;
    Image stopImage;
    Image loadImage;
    Image restartPreviousImage;
    Image endImage;
    Image muteOnImage;
    Image muteOffImage;
    Image addToPlaylistImage;
    Image forwardImage;
    Image backImage;
    Image A_B_LOOP_Image;
    Image Aloop;
    Image Bloop;
    Image ALOOP_slider;
    Image BLOOP_slider;


    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};