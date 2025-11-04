#pragma once // PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"
using namespace juce;
//using namespace std;

class MarkerListBoxModel : public juce::ListBoxModel
{
public:
    // 1. تعديل: ضيفنا ListBox& box
    // عشان نقدر نخليه يعمل update لما نمسح
    MarkerListBoxModel(PlayerAudio& audio, juce::ListBox& box, juce::Slider& bar)
        : playerAudio(audio), ownerBox(box), positionBar(bar) {
    }

    int getNumRows() override
    {
        return (int)playerAudio.get_markers().size();
    }

    void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::cyan.withAlpha(0.5f));

        double markerTime = playerAudio.get_marker_position(row);
        int mins = (int)(markerTime / 60);
        int secs = (int)(round(markerTime)) % 60;
        juce::String timeText = juce::String(mins).paddedLeft('0', 2) + ":" + juce::String(secs).paddedLeft('0', 2);

        g.setColour(juce::Colours::white);
        g.drawText("Marker " + juce::String(row + 1) + " @ " + timeText,
            5, 0, width - 10, height,
            juce::Justification::centredLeft, true);
    }

    // 2. تعديل: ده الكود الجديد اللي بيفرق بين الكليكات
    // الدالة دي هي اللي بتعمل كل الشغل
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu()) // <-- لو المستخدم داس كليك يمين
        {
            juce::PopupMenu menu;
            menu.addItem(1, "Jump to this marker");
            menu.addItem(2, "Remove this marker");
            menu.addSeparator();
            menu.addItem(3, "Cancel");

            // بنشغل القايمة
           // السطر الصح
            menu.showMenuAsync(juce::PopupMenu::Options().withTargetScreenArea(juce::Rectangle<int>(e.getScreenX(), e.getScreenY(), 1, 1)),
                [this, row](int result)
                {
                    if (result == 1) // اختار "Jump"
                    {
                        playerAudio.jump_to_marker(row);
                    }
                    else if (result == 2) // اختار "Remove"
                    {
                        playerAudio.remove_marker(row);
                        ownerBox.updateContent();
                        positionBar.repaint();
                    }
                    // لو 3 أو 0 (Cancel) مش بنعمل حاجة
                });
        }
        else // <-- لو المستخدم داس كليك شمال (عادي)
        {
            playerAudio.jump_to_marker(row); // اعمل jump علطول
        }
    }

private:
    PlayerAudio& playerAudio;
    juce::ListBox& ownerBox; // 3. تعديل: ضيفنا الريفرنس ده
    juce::Slider& positionBar;
};







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
    PlayerGUI(const juce::String& sessionFileName);
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



    //void listBoxItemClicked(int row, const MouseEvent& e) override;

private:
    PlayerAudio playerAudio;

    // for album image
    juce::ImageComponent albumArtComponent;
    juce::Image currentAlbumArt;

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
    ImageButton addMarkerButton;
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


    ListBox markerBox;
    std::unique_ptr<MarkerListBoxModel> markerModel;
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
    Image addMarkerImage;

    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;




    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};