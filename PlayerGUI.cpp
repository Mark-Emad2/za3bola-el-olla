#include <JuceHeader.h>
#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h> // لملفات MP3
#include <taglib/flacfile.h> // لملفات FLAC
#include <taglib/id3v2tag.h>
#include <taglib/attachedpictureframe.h>

using namespace std;
using namespace juce;
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

// تغيير لون الزر مع تغيير النص 
void PlayerGUI::safeButton_Colour(TextButton& btn, const String& text, const Colour& col)
{
    btn.setButtonText(text);
    btn.setColour(TextButton::buttonColourId, col);
    btn.repaint();
}
void PlayerGUI::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);

    playerAudio.getNextAudioBlock(bufferToFill);
    const float* channelData = bufferToFill.buffer->getReadPointer(0);
    wave_form_visualiser.pushBuffer(*bufferToFill.buffer);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

PlayerGUI::PlayerGUI(const juce::String& sessionFileName)
    : thumbnailCache(5),// number of thumbnails to store
    //num in fron for the details heiger lower detals lower heiger detais and slower generation
    //audioThumbnail wave form reginerator
    audioThumbnail(512, formatManager, thumbnailCache),
    fileLoaded(false),
    waveformVisualiser(1)// 1 chanel waves don't overlap(two waves)
{
    addAndMakeVisible(waveformVisualiser);

    addAndMakeVisible(albumArtComponent);
    albumArtComponent.setImagePlacement(juce::RectanglePlacement::centred);

    waveformVisualiser.setRepaintRate(60);
    waveformVisualiser.setBufferSize(512);//controles the size of the wave

    // (اللون الأول هو الخلفية، والثاني هو الموجة)
// (اللون الأول هو الخلفية، والثاني هو الموجة)
    waveformVisualiser.setColours(juce::Colour(46, 28, 64), juce::Colours::cyan);
    //waveformVisualiser.setColours(juce::Colour(0xFF2E1C40), juce::Colours::cyan);
    // Add buttons
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &restart_PreviousButton , &stopButton ,&Pause_PlayButton,&EndButton,&muteButton,&addToPlaylistButton,&loopButton,&forwardButton ,&backButton , &A_B_LOOP , &addMarkerButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    The_bar_pos.setLookAndFeel(this);
    loopOffImage = ImageCache::getFromMemory(BinaryData::reload_png, BinaryData::reload_pngSize);
    loopOnImage = ImageCache::getFromMemory(BinaryData::reload_1_png, BinaryData::reload_1_pngSize);

    muteOnImage = ImageCache::getFromMemory(BinaryData::mute_1_png, BinaryData::mute_1_pngSize);
    muteOffImage = ImageCache::getFromMemory(BinaryData::volume_1_png, BinaryData::volume_1_pngSize);

    loadImage = ImageCache::getFromMemory(BinaryData::plus_png, BinaryData::plus_pngSize);
    addToPlaylistImage = ImageCache::getFromMemory(BinaryData::down_png, BinaryData::down_pngSize);
    stopImage = ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);
    restartPreviousImage = ImageCache::getFromMemory(BinaryData::backward_2_png, BinaryData::backward_2_pngSize);
    endImage = ImageCache::getFromMemory(BinaryData::forward_1_png, BinaryData::forward_1_pngSize);
    playImage = ImageCache::getFromMemory(BinaryData::playbutton_png, BinaryData::playbutton_pngSize);
    pauseImage = ImageCache::getFromMemory(BinaryData::pausebutton_png, BinaryData::pausebutton_pngSize);
    forwardImage = ImageCache::getFromMemory(BinaryData::ten_png, BinaryData::ten_pngSize);
    backImage = ImageCache::getFromMemory(BinaryData::ten_1_png, BinaryData::ten_1_pngSize);
    Aloop = ImageCache::getFromMemory(BinaryData::lettera_1_png, BinaryData::lettera_1_pngSize);
    Bloop = ImageCache::getFromMemory(BinaryData::letterb_1_png, BinaryData::letterb_1_pngSize);
    A_B_LOOP_Image = ImageCache::getFromMemory(BinaryData::ab_png, BinaryData::ab_pngSize);
    ALOOP_slider = ImageCache::getFromMemory(BinaryData::lettera_png, BinaryData::lettera_pngSize);
    BLOOP_slider = ImageCache::getFromMemory(BinaryData::letterb_png, BinaryData::letterb_pngSize);

    addMarkerImage = ImageCache::getFromMemory(BinaryData::mark_png, BinaryData::mark_pngSize);
    backgroundImage = ImageCache::getFromMemory(BinaryData::_3dnetworkcommunicationsdatatechnologybackgroundwithflowingparticles_1_png, BinaryData::_3dnetworkcommunicationsdatatechnologybackgroundwithflowingparticles_1_pngSize);
    //i have to change the image because it's not suitable for add marker  

    addMarkerButton.setImages(false, true, true,
        addMarkerImage, 1.0f, Colours::transparentBlack,
        addMarkerImage, 1.0f, Colours::white,
        addMarkerImage, 1.0f, Colours::transparentBlack);


    // 3. ظبط الـ playlistBox (زي ما هي)
    playlistBox.setModel(this); // <-- دي بتستخدم الكلاس الكبير
    addAndMakeVisible(playlistBox);

    // 4. ظبط الـ markerBox (ده الجديد)
    markerModel = std::make_unique<MarkerListBoxModel>(playerAudio, markerBox, The_bar_pos);
    markerBox.setModel(markerModel.get()); // <-- بنربط الليست بوكس بالمودل بتاعها
    addAndMakeVisible(markerBox);
    markerBox.setOutlineThickness(1);



    loadButton.setImages(false, true, true,
        loadImage, 1.0f, Colours::transparentBlack,
        loadImage, 1.0f, Colours::white,
        loadImage, 1.0f, Colours::transparentBlack);
    addToPlaylistButton.setImages(false, true, true,
        addToPlaylistImage, 1.0f, Colours::transparentBlack,
        addToPlaylistImage, 1.0f, Colours::white,
        addToPlaylistImage, 1.0f, Colours::transparentBlack);
    restart_PreviousButton.setImages(false, true, true,
        restartPreviousImage, 1.0f, Colours::transparentBlack,
        restartPreviousImage, 1.0f, Colours::white,
        restartPreviousImage, 1.0f, Colours::transparentBlack);
    stopButton.setImages(false, true, true,
        stopImage, 1.0f, Colours::transparentBlack,
        stopImage, 1.0f, Colours::white,
        stopImage, 1.0f, Colours::transparentBlack);
    EndButton.setImages(false, true, true,
        endImage, 1.0f, Colours::transparentBlack,
        endImage, 1.0f, Colours::white,
        endImage, 1.0f, Colours::transparentBlack);
    Pause_PlayButton.setImages(false, true, true,
        playImage, 1.0f, Colours::transparentBlack,
        playImage, 1.0f, Colours::white,
        playImage, 1.0f, Colours::transparentBlack);
    forwardButton.setImages(false, true, true,
        forwardImage, 1.0f, Colours::transparentBlack,
        forwardImage, 1.0f, Colours::white,
        forwardImage, 1.0f, Colours::transparentBlack);
    backButton.setImages(false, true, true,
        backImage, 1.0f, Colours::transparentBlack,
        backImage, 1.0f, Colours::white,
        backImage, 1.0f, Colours::transparentBlack);
    A_B_LOOP.setImages(false, true, true,
        Aloop, 1.0f, Colours::transparentBlack,
        Aloop, 1.0f, Colours::white,
        Aloop, 1.0f, Colours::transparentBlack);

    loopButton.setImages(false, true, true,
        loopOffImage, 1.0f, Colours::transparentBlack,
        loopOffImage, 1.0f, Colours::white,
        loopOffImage, 1.0f, Colours::transparentBlack);

    muteButton.setImages(false, true, true,
        muteOffImage, 1.0f, Colours::transparentBlack,
        muteOffImage, 1.0f, Colours::white,
        muteOffImage, 1.0f, Colours::transparentBlack);




    addAndMakeVisible(infoLabel);
    infoLabel.setJustificationType(Justification::centredLeft);
    infoLabel.setText("No File Loaded", dontSendNotification);
    infoLabel.setFont(Font("Arial", 16.0f, Font::bold));


    addAndMakeVisible(poslabel);
    addAndMakeVisible(endPos);


    volume_label.setText("Volume:", dontSendNotification);
    volume_label.setJustificationType(Justification::centredLeft);
    addAndMakeVisible(volume_label);

    // Volume slider

    volumeSlider.setRange(0, 100, 1);
    volumeSlider.setValue(50);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, // <--- تم التغيير
        false,
        60,
        20);
    volumeSlider.setTextValueSuffix("%");
    volumeSlider.setSliderStyle(Slider::LinearHorizontal);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
    // Speed slider

    speed_slider.setRange(0.25, 2.0, 0.25);
    speed_slider.setValue(1.0);
    speed_slider.setTextBoxStyle(Slider::TextBoxRight, false, 60, 20); // <-- اتعدلت أهي
    speed_slider.setTextValueSuffix("x");
    speed_slider.setSliderStyle(Slider::LinearHorizontal); // <-- أفقي
    speed_slider.addListener(this);
    addAndMakeVisible(speed_slider);

    // Speed label
    speed_label.setText("Speed:", dontSendNotification);
    speed_label.setJustificationType(Justification::centredRight);
    addAndMakeVisible(speed_label);

    // Position slider
    The_bar_pos.addListener(this);
    addAndMakeVisible(The_bar_pos);
    The_bar_pos.setTextBoxStyle(juce::Slider::TextBoxLeft,
        false,
        0,
        0);

    //loop button
    //loopButton.setClickingTogglesState(true);

    //دي تخص  التايمر
    startTimerHz(10);

    // session file location
    sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile(sessionFileName);

    // playlist box
    playlistBox.setModel(this);
    addAndMakeVisible(playlistBox);

    loadLastState();

    volumeSlider.setColour(Slider::thumbColourId, Colours::white);
    volumeSlider.setColour(Slider::trackColourId, Colours::purple);
    volumeSlider.setColour(Slider::textBoxTextColourId, Colours::white);

    speed_slider.setColour(Slider::thumbColourId, Colours::white);
    speed_slider.setColour(Slider::trackColourId, Colours::purple);
    speed_slider.setColour(Slider::textBoxTextColourId, Colours::white);

}
PlayerGUI::~PlayerGUI()
{
    saveLastState();
}

void PlayerGUI::resized()
{
    // --- 1. ثوابت التصميم (كلاسيك نضيف - متأمن ضد الإيرور) ---
    const int margin = 10;
    const int spacing = 8;

    // --- الارتفاعات ---
    const int headerHeight = 109;
    const int footerHeight = 100;
    const int timeSliderHeight = 35;
    const int mainButtonHeight = 45;      // الأزرار المهمة
    const int utilItemHeight = 30;        // الزراير الفرعية

    // --- النسب ---
    const float waveformHeightPercent = 0.35f;
    const float rightColumnWidthPercent = 0.30f;
    const float playlistHeightPercent = 0.60f;
    const float minRightColumnWidth = 200.0f;
    const float minWaveformHeight = 80.0f;
    const float footerLeftPercent = 0.30f;
    const float footerRightPercent = 0.33f;

    // ---------------------------------------------------------
    // --- 2. التقسيم الرئيسي ---
    // ---------------------------------------------------------
    auto bounds = getLocalBounds().reduced(margin);

    // --- 3. الهيدر ---
    auto headerArea = bounds.removeFromTop(headerHeight);
    const int headerButtonWidth = 60;
    auto addPlaylistButtonArea = headerArea.removeFromRight(headerButtonWidth);
    headerArea.removeFromRight(spacing);
    auto loadButtonArea = headerArea.removeFromRight(headerButtonWidth);
    loadButton.setBounds(loadButtonArea);
    addToPlaylistButton.setBounds(addPlaylistButtonArea);
    infoLabel.setBounds(headerArea.reduced(5, 0));

    bounds.removeFromTop(spacing * 2);

    // --- 4. الفوتر ---
    auto footerArea = bounds.removeFromBottom(footerHeight); // 100px

    // أ. الصف الأول: سلايدر الوقت
    auto timeSliderRow = footerArea.removeFromTop(timeSliderHeight); // 30px
    const int timeLabelWidth = 60;
    poslabel.setBounds(timeSliderRow.removeFromLeft(timeLabelWidth));
    endPos.setBounds(timeSliderRow.removeFromRight(timeLabelWidth));
    The_bar_pos.setBounds(timeSliderRow.reduced(spacing, 0));

    footerArea.removeFromTop(spacing * 2); // مسافة 16px (الباقي 54px)

    // ب. الصف الثاني: الأزرار
    auto mainControlsRow = footerArea; // ارتفاعه 54px

    auto leftControlsArea = mainControlsRow.removeFromLeft(mainControlsRow.getWidth() * footerLeftPercent);
    mainControlsRow.removeFromLeft(spacing);
    auto rightSlidersArea = mainControlsRow.removeFromRight(mainControlsRow.getWidth() * (footerRightPercent / (1.0f - footerLeftPercent)));
    mainControlsRow.removeFromRight(spacing);
    auto centerPlaybackArea = mainControlsRow;

    // -- 1. رص الأزرار المركزية (المهمة) --
    const int playPauseButtonWidth = (int)(mainButtonHeight * 1.5f);

    const int otherMainButtonWidth = juce::jmax(1, (int)((centerPlaybackArea.getWidth() - playPauseButtonWidth - (spacing * 4)) / 4));

    int x = centerPlaybackArea.getX();
    int y = centerPlaybackArea.getY() + (centerPlaybackArea.getHeight() - mainButtonHeight) / 2;

    backButton.setBounds(x, y, otherMainButtonWidth, mainButtonHeight);
    x += otherMainButtonWidth + spacing;
    restart_PreviousButton.setBounds(x, y, otherMainButtonWidth, mainButtonHeight);
    x += otherMainButtonWidth + spacing;
    Pause_PlayButton.setBounds(x, y, playPauseButtonWidth, mainButtonHeight);
    x += playPauseButtonWidth + spacing;
    EndButton.setBounds(x, y, otherMainButtonWidth, mainButtonHeight);
    x += otherMainButtonWidth + spacing;
    forwardButton.setBounds(x, y, otherMainButtonWidth, mainButtonHeight);

    // -- 2. رص الأزرار الشمالية (الفرعية) --
    const int numSmallButtons = 5;

    // *********** التصليح الثاني ***********
    // (juce::jmax) عشان العرض ميبقاش سالب أبداً
    const int smallButtonWidth = juce::jmax(1, (leftControlsArea.getWidth() - (spacing * (numSmallButtons - 1))) / numSmallButtons);

    x = leftControlsArea.getX();
    y = leftControlsArea.getY() + (leftControlsArea.getHeight() - utilItemHeight) / 2;

    muteButton.setBounds(x, y, smallButtonWidth, utilItemHeight);
    x += smallButtonWidth + spacing;
    loopButton.setBounds(x, y, smallButtonWidth, utilItemHeight);
    x += smallButtonWidth + spacing;
    A_B_LOOP.setBounds(x, y, smallButtonWidth, utilItemHeight);
    x += smallButtonWidth + spacing;
    stopButton.setBounds(x, y, smallButtonWidth, utilItemHeight);
    x += smallButtonWidth + spacing;
    addMarkerButton.setBounds(x, y, smallButtonWidth, utilItemHeight);

    // -- 3. رص السلايدرز اليمين (الفرعية) --
    const int sliderLabelWidth = 55;
    const int sliderHeight = (rightSlidersArea.getHeight() - spacing) / 2; // (ارتفاعه 23)

    auto volumeSliderArea = rightSlidersArea.removeFromTop(sliderHeight);
    volume_label.setBounds(volumeSliderArea.removeFromLeft(sliderLabelWidth));
    volumeSlider.setBounds(volumeSliderArea.reduced(spacing, 0));

    rightSlidersArea.removeFromTop(spacing);

    auto speedSliderArea = rightSlidersArea.removeFromTop(sliderHeight); // الباقي (23)
    speed_label.setBounds(speedSliderArea.removeFromLeft(sliderLabelWidth));
    speed_slider.setBounds(speedSliderArea.reduced(spacing, 0));


    // --- 5. الجزء الأوسط ---
    auto middleArea = bounds;

    const int waveformHeight = (int)juce::jlimit(minWaveformHeight, 200.0f, middleArea.getHeight() * waveformHeightPercent);
    waveformVisualiser.setBounds(middleArea.removeFromTop(waveformHeight));

    positionSlider.setBounds(waveformVisualiser.getBounds());

    middleArea.removeFromTop(spacing);

    auto contentArea = middleArea;
    const int rightColumnWidth = (int)juce::jlimit(minRightColumnWidth, 300.0f, contentArea.getWidth() * rightColumnWidthPercent);
    auto rightColumnArea = contentArea.removeFromRight(rightColumnWidth);
    contentArea.removeFromRight(spacing);

    playlistBox.setBounds(rightColumnArea.removeFromTop(rightColumnArea.getHeight() * playlistHeightPercent));
    rightColumnArea.removeFromTop(spacing);
    markerBox.setBounds(rightColumnArea);

    albumArtComponent.setBounds(contentArea);
}


//void PlayerGUI::resized()
//{
//    int margin = 10;
//    int spacing = 8;
//    int fullWidth = getWidth() - (2 * margin);
//    // --- 1. تعريف ارتفاعات الأقسام ---
//    int headerHeight = 60;
//    int footerHeight = 140; // فوتر كبير عشان يشيل كل الزراير
//    int liveWaveformHeight = 120; // ارتفاع الـ Waveform اللي شغالة
//
//    // --- 2. الهيدر (المعلومات وأزرار الملفات) ---
//    int headerY = margin;
//    int buttonWidth = 120;
//
//    infoLabel.setBounds(margin, headerY, fullWidth - (2 * buttonWidth) - (2 * spacing), headerHeight - (2 * margin));
//    infoLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withAlpha(0.2f));
//    infoLabel.setColour(Label::textColourId, Colours::whitesmoke);
//    infoLabel.setJustificationType(Justification::centred);
//
//    loadButton.setBounds(infoLabel.getRight() + spacing, headerY, buttonWidth, headerHeight - (2 * margin));
//    addToPlaylistButton.setBounds(loadButton.getRight() + spacing, headerY, buttonWidth, headerHeight - (2 * margin));
//
//    // --- 4. الفوتر (كل أزرار التحكم والسلايدرز) ---
//    int footerY = getHeight() - footerHeight - margin;
//    int currentY_inFooter = footerY;
//    int sliderHeight = 20;
//    int buttonHeight = 35;
//
//    // الصف الأول في الفوتر: سلايدر الوقت
//    int timeLabelWidth = 60; // وسعنا الليبل شوية عشان "00:00"
//    int posSliderWidth = fullWidth - (2 * timeLabelWidth) - (2 * spacing);
//
//    poslabel.setBounds(margin, currentY_inFooter, timeLabelWidth, sliderHeight);
//    The_bar_pos.setBounds(poslabel.getRight() + spacing, currentY_inFooter, posSliderWidth, sliderHeight);
//    endPos.setBounds(The_bar_pos.getRight() + spacing, currentY_inFooter, timeLabelWidth, sliderHeight);
//
//    currentY_inFooter += sliderHeight + spacing; // انزل للصف التاني
//
//    // الصف الثاني في الفوتر: الأزرار والسلايدرز
//    int footerControlsY = currentY_inFooter;
//
//    // الجزء اليمين (سلايدرز الصوت والسرعة)
//    int slidersAreaWidth = fullWidth * 0.3; // 30% للـ Sliders
//    int slidersAreaX = getWidth() - margin - slidersAreaWidth;
//    int labelWidth = 50;
//    int horizSliderWidth = slidersAreaWidth - labelWidth - spacing;
//
//    speed_label.setBounds(slidersAreaX, footerControlsY, labelWidth, sliderHeight);
//    speed_slider.setBounds(speed_label.getRight() + spacing, footerControlsY, horizSliderWidth, sliderHeight);
//
//    volume_label.setBounds(slidersAreaX, footerControlsY + sliderHeight + spacing, labelWidth, sliderHeight);
//    volumeSlider.setBounds(volume_label.getRight() + spacing, footerControlsY + sliderHeight + spacing, horizSliderWidth, sliderHeight);
//
//    // الجزء اللي في النص (أزرار التشغيل الرئيسية - 5 زراير)
//    int mainControlsWidth = fullWidth * 0.4; // 40% للأزرار الرئيسية
//    int mainControlsX = margin + (fullWidth - mainControlsWidth) / 2; // توسيط
//    int numMainButtons = 5;
//    int mainButtonWidth = (mainControlsWidth - (numMainButtons - 1) * spacing) / numMainButtons;
//
//    backButton.setBounds(mainControlsX, footerControlsY, mainButtonWidth, buttonHeight);
//    restart_PreviousButton.setBounds(backButton.getRight() + spacing, footerControlsY, mainButtonWidth, buttonHeight);
//    Pause_PlayButton.setBounds(restart_PreviousButton.getRight() + spacing, footerControlsY, mainButtonWidth, buttonHeight + 10); // زرار التشغيل أكبر
//    EndButton.setBounds(Pause_PlayButton.getRight() + spacing, footerControlsY, mainButtonWidth, buttonHeight);
//    forwardButton.setBounds(EndButton.getRight() + spacing, footerControlsY, mainButtonWidth, buttonHeight);
//
//    // الجزء الشمال (الأزرار المساعدة - 4 زراير)
//    int utilControlsWidth = fullWidth * 0.3; // الـ 30% الباقية
//    int numUtilButtons = 4;
//    int utilButtonWidth = (utilControlsWidth - (numUtilButtons - 1) * spacing) / numUtilButtons;
//
//    loopButton.setBounds(margin, footerControlsY, utilButtonWidth, buttonHeight);
//    muteButton.setBounds(loopButton.getRight() + spacing, footerControlsY, utilButtonWidth, buttonHeight);
//    A_B_LOOP.setBounds(muteButton.getRight() + spacing, footerControlsY, utilButtonWidth, buttonHeight);
//    stopButton.setBounds(A_B_LOOP.getRight() + spacing, footerControlsY, utilButtonWidth, buttonHeight);
//
//
//    // --- 3. المحتوى (الويف فورم والبلاي ليست) ---
//    int middleY = headerY + (headerHeight - (2 * margin)) + spacing; // ابدأ بعد الهيدر
//    int middleTotalHeight = (footerY - spacing) - middleY; // كل المساحة المتاحة في النص
//
//    // الـ Waveform الحية (والسلايدر المخفي فوقها للضغط)
//    waveformVisualiser.setBounds(margin, middleY, fullWidth, liveWaveformHeight);
//    positionSlider.setBounds(margin, middleY, fullWidth, liveWaveformHeight); // فوقها بالظبط
//
//    // قائمة التشغيل (تاخد باقي المساحة)
//    int playlistY = waveformVisualiser.getBottom() + spacing;
//    int playlistHeight = middleTotalHeight - liveWaveformHeight - spacing;
//
//    playlistBox.setBounds(margin, playlistY, fullWidth, playlistHeight);
//    playlistBox.setColour(ListBox::backgroundColourId, Colours::darkslategrey.withAlpha(0.4f));
//    playlistBox.setOutlineThickness(1);
//    playlistBox.setColour(ListBox::outlineColourId, Colours::lightgrey.withAlpha(0.4f));
//
//
//    // أنا شلت اللوب بتاعة تلوين الأزرار اللي كانت في الآخر
//    // لأنك دلوقتي بتستخدم ImageButtons واللوب دي كانت لـ TextButtons
//}



Image loadCoverArt(const juce::File& file)
{
    TagLib::FileName path(file.getFullPathName().toWideCharPointer());


    String ext = file.getFileExtension().toLowerCase();
    TagLib::ByteVector imageBytes;
    Image art;

    if (ext == ".mp3")
    {
        TagLib::MPEG::File mp3File(path);
        if (mp3File.isValid() && mp3File.ID3v2Tag())
        {
            TagLib::ID3v2::FrameList apicFrames = mp3File.ID3v2Tag()->frameList("APIC");
            if (!apicFrames.isEmpty())
            {
                auto picFrame = static_cast<TagLib::ID3v2::AttachedPictureFrame*>(apicFrames.front());
                imageBytes = picFrame->picture();
            }
        }
    }


    if (!imageBytes.isEmpty())
    {
        art = ImageFileFormat::loadFrom(imageBytes.data(), (size_t)imageBytes.size());
    }

    if (art.isValid())
    {
        return art;
    }

    // هبحث في الفولدر
    File audioFileDir = file.getParentDirectory();

    StringArray artFilenames;
    artFilenames.add("folder.jpg");
    artFilenames.add("cover.jpg");
    artFilenames.add("folder.png");
    artFilenames.add("cover.png");
    artFilenames.add("album.jpg");
    artFilenames.add("album.png");
    artFilenames.add(file.getFileNameWithoutExtension() + ".jpg");
    artFilenames.add(file.getFileNameWithoutExtension() + ".png");

    for (const auto& filename : artFilenames)
    {
        File artFile = audioFileDir.getChildFile(filename);
        if (artFile.existsAsFile())
        {
            art = ImageFileFormat::loadFrom(artFile);
            if (art.isValid())
            {
                return art;
            }
        }
    }

    // لو الخطتين فشلوا، رجع صورة فاضية
    return Image();
}

void PlayerGUI::updateLabel(const File& file)
{
    TagLib::FileRef f(file.getFullPathName().toWideCharPointer());

    if (!f.isNull() && f.tag() != nullptr)
    {
        TagLib::Tag* tag = f.tag();
        String title = tag->title().isEmpty() ? "Unknown" : tag->title().toCString(true);
        String artist = tag->artist().isEmpty() ? "Unknown" : tag->artist().toCString(true);
        String album = tag->album().isEmpty() ? "Unknown" : tag->album().toCString(true);
        String year = (tag->year() != 0) ? String(tag->year()) : "Unknown";

        double duration = 0.0;
        if (f.audioProperties() != nullptr)
            duration = f.audioProperties()->length();

        int mins = static_cast<int>(duration / 60);
        int secs = static_cast<int>(std::round(duration)) % 60;

        displayText = "Title: " + title +
            "\nArtist: " + artist +
            "\nAlbum: " + album +
            "\nYear: " + year +
            "\nDuration: " + String(mins).paddedLeft('0', 2) + ":" + String(secs).paddedLeft('0', 2);
    }
    else
    {
        displayText = "File: " + file.getFileName() + "\n(No metadata found)";
        DBG("Failed to read text metadata (TagLib::FileRef failed).");
    }

    infoLabel.setText(displayText, dontSendNotification);

    currentAlbumArt = loadCoverArt(file);

    if (currentAlbumArt.isValid())
    {
        albumArtComponent.setImage(currentAlbumArt);
    }
    else
    {
        albumArtComponent.setImage(juce::Image());
    }


    audioThumbnail.clear();
    audioThumbnail.setSource(new FileInputSource(file));
    fileLoaded = true;
    waveformVisualiser.clear();
}

// save last session state
void PlayerGUI::saveLastState()
{

    File currentFile = playerAudio.getCurrentFile();
    appState.setProperty("lastFile", currentFile.getFullPathName(), nullptr);

    appState.setProperty("lastPosition", playerAudio.getPosition(), nullptr);

    appState.setProperty("lastVolume", volumeSlider.getValue(), nullptr);
    appState.setProperty("lastPositionSlider", The_bar_pos.getValue(), nullptr);
    appState.setProperty("currentIndex", currentIndex, nullptr);



    // بمسح البلاي ليست القديمه عشان يضيف الجديد
    for (int i = appState.getNumChildren() - 1; i >= 0; --i)
    {
        if (appState.getChild(i).getType().toString() == "Playlist")
            appState.removeChild(i, nullptr);
    }

    ValueTree playlistState("Playlist");
    for (auto& path : playlist) {

        ValueTree filetree("File");
        filetree.setProperty("path", path, nullptr);
        playlistState.addChild(filetree, -1, nullptr);// -1 عشان يضيف فى الاخر

    }
    appState.addChild(playlistState, -1, nullptr);


    unique_ptr<XmlElement> xml(appState.createXml());

    if (xml) {
        xml->writeTo(sessionFile);
    }

}
// load last session state

void PlayerGUI::loadLastState() {

    if (sessionFile.existsAsFile()) {

        //playerAudio.stop();

        unique_ptr<XmlElement> xml(parseXML(sessionFile));
        if (xml) {

            playlist.clear();
            appState = ValueTree::fromXml(*xml);

            ValueTree playlistTree = appState.getChildWithName("Playlist");

            for (int i = 0; i < playlistTree.getNumChildren(); ++i) {

                ValueTree fileTree = playlistTree.getChild(i);
                String path = fileTree.getProperty("path").toString();

                if (File(path).existsAsFile()) {
                    playlist.add(path);
                }

            }
            playlistBox.updateContent();


            String lastFilePath = appState.getProperty("lastFile").toString();
            double lastPosition = (double)appState.getProperty("lastPosition");
            double lastVolume = (double)appState.getProperty("lastVolume", 50);
            double lastPositionSlider = (double)appState.getProperty("lastPositionSlider", 0.0);

            currentIndex = (int)appState.getProperty("currentIndex", -1);

            if (!lastFilePath.isEmpty()) {

                playerAudio.loadFile(File(lastFilePath));
                double lengthInSeconds = playerAudio.getLength();
                The_bar_pos.setRange(0.0, lengthInSeconds);
                playerAudio.setPosition(lastPosition);
                volumeSlider.setValue(lastVolume, dontSendNotification);
                The_bar_pos.setValue(lastPositionSlider, dontSendNotification);

                updateLabel(File(lastFilePath));
            }
            if (currentIndex >= 0 && currentIndex < playlist.size())
                playlistBox.selectRow(currentIndex);
        }
    }
}


void PlayerGUI::sliderValueChanged(Slider* slider)
{
    if (slider == &volumeSlider)
    {
        float gainValue = (float)slider->getValue();
        playerAudio.setGain(gainValue / 100);

    }
    else if (slider == &The_bar_pos)
    {
        playerAudio.setPosition((float)slider->getValue());
    }
    else if (slider == &speed_slider)
    {
        float speedValue = (float)speed_slider.getValue();
        playerAudio.set_speed(speedValue);
    }
    else if (slider == &positionSlider)
    {
        playerAudio.setPosition((float)slider->getValue());
    }
}



void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A2A));
    g.setColour(Colours::white);
    g.setFont(14.0f);
    //String speedText = String(playerAudio.get_speed(), 2) + "x";
    //g.drawText(speedText, speed_slider.getRight() + 10, speed_slider.getY(), 50, speed_slider.getHeight(), Justification::centredLeft);
}


void PlayerGUI::timerCallback()
{
    The_bar_pos.setValue(playerAudio.getPosition(), dontSendNotification);

    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();
    positionSlider.setValue(currentPos, dontSendNotification); //------------>

    if (totalLength > 0 && currentPos >= totalLength)
    {
        playerAudio.stop();

        Pause_PlayButton.setImages(false, true, true,
            playImage, 1.0f, Colours::transparentBlack,
            playImage, 1.0f, Colours::white,
            playImage, 1.0f, Colours::transparentBlack);
        if (playerAudio.isLooping())
        {

            playerAudio.setPosition(0.0);
            playerAudio.start();
            Pause_PlayButton.setImages(false, true, true,
                pauseImage, 1.0f, Colours::transparentBlack,
                pauseImage, 1.0f, Colours::white,
                pauseImage, 1.0f, Colours::transparentBlack);

        }
        else {
            if (playlist.size() > 0 && currentIndex >= 0) {
                int nextIndex = currentIndex + 1;
                if (nextIndex >= playlist.size()) {
                    nextIndex = 0;
                }
                playIndex(nextIndex);
            }
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

    repaint();


    // بتاع اللوب A-B
    if (AbLOOPING) {
        double currentPos = playerAudio.getPosition();
        if (currentPos >= playerAudio.getBLoopPoint() || currentPos <= playerAudio.getALoopPoint()) {
            playerAudio.stop();
            playerAudio.setPosition(playerAudio.getALoopPoint());
            playerAudio.start();
            if (playerAudio.isPlaying()) {
                Pause_PlayButton.setImages(false, true, true,
                    pauseImage, 1.0f, Colours::transparentBlack,
                    pauseImage, 1.0f, Colours::white,
                    pauseImage, 1.0f, Colours::transparentBlack);
            }
            else {
                Pause_PlayButton.setImages(false, true, true,
                    playImage, 1.0f, Colours::transparentBlack,
                    playImage, 1.0f, Colours::white,
                    playImage, 1.0f, Colours::transparentBlack);
            }
        }

    }
    if (loopState == 1) {
        double currentPos = playerAudio.getPosition();
        double aPoint = playerAudio.getALoopPoint();
        if (currentPos <= aPoint) {
            resetLoopPoints();



        }
    }
}

void PlayerGUI::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &audioThumbnail)
    {
        repaint(); // Redraw when waveform data is loaded
    }
}

int PlayerGUI::getNumRows()
{
    return playlist.size();
}

void PlayerGUI::paintListBoxItem(int row, Graphics& graph, int width, int height, bool rowIsSelected) {
    if (row < 0 || row >= playlist.size()) return;
    if (rowIsSelected) {
        graph.fillAll(Colours::cyan);
    }
    String filename = File(playlist[row]).getFileName();
    graph.setColour(Colours::black);
    graph.drawText(filename, 4, 0, width - 4, height, Justification::centredLeft, true);
}

//عشان اما تغغط مرتين يغير رقم الاندكس و يشغل الاغنيه


void PlayerGUI::listBoxItemDoubleClicked(int row, const MouseEvent&)
{
    playIndex(row);
}

void PlayerGUI::listBoxItemClicked(int row, const MouseEvent& e)
{
    // صححنا شرط التحقق من الـ row
    if (row < 0 || row >= playlist.size()) return;

    if (e.mods.isPopupMenu()) { // to check if user press right click

        PopupMenu menu;
        menu.addItem(1, "Play");
        menu.addItem(2, "Remove");

        // capture a copy of row (already captured in lambda) -- fine
        menu.showMenuAsync(PopupMenu::Options().withTargetScreenArea(Rectangle<int>(e.getScreenPosition().x, e.getScreenPosition().y, 1, 1)), [this, row](int click)
            {
                if (click == 1)
                {
                    playIndex(row);
                    return;
                }

                if (click == 2)
                {

                    if (row < 0 || row >= playlist.size())
                        return;

                    String currentFilePath;
                    File curFile = playerAudio.getCurrentFile();
                    if (curFile.existsAsFile())
                        currentFilePath = curFile.getFullPathName();

                    // إذا المستخدم حذف نفس الملف المشغول الآن
                    bool deletingCurrent = (currentIndex == row);

                    String removedPath = playlist[row];

                    playlist.remove(row);
                    playlistBox.updateContent();

                    int newIndex = -1;
                    if (currentFilePath.isNotEmpty())
                    {
                        for (int i = 0; i < playlist.size(); ++i)
                        {
                            if (playlist[i] == currentFilePath)
                            {
                                newIndex = i;
                                break;
                            }
                        }
                    }

                    if (newIndex != -1)
                    {

                        currentIndex = newIndex;
                        playlistBox.selectRow(currentIndex);
                    }
                    else

                    {
                        if (playlist.size() > 0)
                        {

                            int Play = jmin(row, playlist.size() - 1);


                            playIndex(Play);
                        }
                        else
                        {
                            // البلاي لست بقت فارغة إيقاف وتشغيل واجهة المستخدم
                            playerAudio.stop();
                            Pause_PlayButton.setImages(false, true, true,
                                playImage, 1.0f, Colours::transparentBlack,
                                playImage, 1.0f, Colours::white,
                                playImage, 1.0f, Colours::transparentBlack);
                            infoLabel.setText("No File Loaded", dontSendNotification);
                            currentIndex = -1;
                        }
                    }

                    saveLastState();
                }
            });

    }
    else {
        playlistBox.selectRow(row);
        currentIndex = row;
    }
}

void PlayerGUI::resetLoopPoints()
{
    playerAudio.setALoopPoint(0.0);
    playerAudio.setBLoopPoint(0.0);
    loopState = 0;
    AbLOOPING = false;
    A_B_LOOP.setImages(false, true, true,
        Aloop, 1.0f, Colours::transparentBlack,
        Aloop, 1.0f, Colours::white,
        Aloop, 1.0f, Colours::transparentBlack);
}



void PlayerGUI::selectedRowsChanged(int lastRow)
{
    currentIndex = lastRow;
    //playIndex(currentIndex);
}




//الاغنيه اللى الشغاله من البلاي ليست و رقم الاندكس بتاعها
void PlayerGUI::playIndex(int row) {
    resetLoopPoints();
    if (row >= 0 && row < playlist.size()) { // بتاكد ان الرو بتاع الاندكس جوه الرينج

        File fileToPlay(playlist[row]);
        if (fileToPlay.existsAsFile()) {

            playerAudio.loadFile(fileToPlay);
            double lengthInSeconds = playerAudio.getLength();

            The_bar_pos.setRange(0.0, lengthInSeconds);

            Pause_PlayButton.setImages(false, true, true,
                pauseImage, 1.0f, Colours::transparentBlack,
                pauseImage, 1.0f, Colours::white,
                pauseImage, 1.0f, Colours::transparentBlack);
            playerAudio.start();
            currentIndex = row;


            playerAudio.clear_markers(); // <-- امسح الماركرز القديمة
            markerBox.updateContent(); // بتاع الماركرز



            playlistBox.selectRow(currentIndex);

            updateLabel(fileToPlay);
            saveLastState();

        }
        else {          // if file not exist remove it from playlist
            playlist.remove(row);
            playlistBox.updateContent();
            saveLastState();

        }
    }
}


void PlayerGUI::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
    float sliderPos, float minSliderPos, float maxSliderPos,
    Slider::SliderStyle style, Slider& slider)
{
    if (&slider == &The_bar_pos)
    {
        float trackHeight = 6.0f;
        float trackY = (float)y + ((float)height - trackHeight) * 0.5f; 
        juce::Rectangle<float> trackBounds((float)x, trackY, (float)width, trackHeight);

        g.setColour(juce::Colours::darkgrey);
        g.fillRoundedRectangle(trackBounds, trackHeight * 0.5f);

        float filledWidth = sliderPos - (float)x;
        if (filledWidth > 0)
        {
            juce::Rectangle<float> filledBounds((float)x, trackY, filledWidth, trackHeight);
            g.setColour(juce::Colours::cyan);
            g.fillRoundedRectangle(filledBounds, trackHeight * 0.5f);
        }

        auto valueToPixel = [&](double value) -> float
            {
                double proportion = The_bar_pos.valueToProportionOfLength(value);
                return juce::jmap((float)proportion, 0.0f, 1.0f, (float)x, (float)(x + width));
            };

        float a_x_pos = 0.0f;
        float b_x_pos = 0.0f;
        bool hasA = false;
        bool hasB = false;

        if (loopState >= 1)
        {
            double aPointTime = playerAudio.getALoopPoint();
            a_x_pos = valueToPixel(aPointTime);
            hasA = true;
            float aspectRatio = (float)ALOOP_slider.getWidth() / (float)ALOOP_slider.getHeight();
            int imageWidth = (int)((float)height * aspectRatio);
            int imageX = (int)(a_x_pos - (float)imageWidth / 2.0f);
            g.drawImage(ALOOP_slider, imageX, y, imageWidth, height, 0, 0, ALOOP_slider.getWidth(), ALOOP_slider.getHeight());
        }

        if (loopState >= 2)
        {
            double bPointTime = playerAudio.getBLoopPoint();
            b_x_pos = valueToPixel(bPointTime);
            hasB = true;
            float aspectRatio = (float)BLOOP_slider.getWidth() / (float)BLOOP_slider.getHeight();
            int imageWidth = (int)((float)height * aspectRatio);
            int imageX = (int)(b_x_pos - (float)imageWidth / 2.0f);
            g.drawImage(BLOOP_slider, imageX, y, imageWidth, height, 0, 0, BLOOP_slider.getWidth(), BLOOP_slider.getHeight());
        }

        if (hasA && hasB)
        {
            float left = juce::jmin(a_x_pos, b_x_pos);
            float right = juce::jmax(a_x_pos, b_x_pos);
            float widthRect = right - left;
            if (widthRect > 0.0f)
            {
                g.setColour(juce::Colours::cyan.withAlpha(0.15f));
                g.fillRect(left, (float)y, widthRect, (float)height);
            }
        }

        g.setColour(juce::Colours::cyan.withAlpha(0.7f)); // تغير لونmark

        float dotDiameter = 7.0f; 
        float dotMargin = 7.0f;   

        float dotY = trackY + trackHeight + dotMargin;

        for (const auto& markerTime : playerAudio.get_markers())
        {
            float markerX = valueToPixel(markerTime);

            g.fillEllipse(markerX - (dotDiameter / 2.0f), 
                dotY,                         
                dotDiameter,                 
                dotDiameter);              
        }

        float thumbWidth = 16.0f;
        float thumbHeight = 16.0f;
        float thumbX = sliderPos - (thumbWidth * 0.5f);
        float thumbY = (float)y + ((float)height - thumbHeight) * 0.5f;
        juce::Rectangle<float> thumbBounds(thumbX, thumbY, thumbWidth, thumbHeight);

        g.setColour(juce::Colours::white);
        g.fillEllipse(thumbBounds);
        g.setColour(juce::Colours::black.withAlpha(0.5f));
        g.drawEllipse(thumbBounds, 1.0f);
    }
    else
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
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
                    resetLoopPoints();
                    lengthInSeconds = playerAudio.getLength();
                    The_bar_pos.setRange(0.0, lengthInSeconds);

                    Pause_PlayButton.setImages(false, true, true,
                        pauseImage, 1.0f, Colours::transparentBlack,
                        pauseImage, 1.0f, Colours::white,
                        pauseImage, 1.0f, Colours::transparentBlack);
                    playerAudio.start();

                    updateLabel(file);
                    saveLastState();
                }

            });

    }

    else if (button == &addToPlaylistButton) {
        FileChooser chooser("Select audio files...",
            File{},
            "*.wav;*.mp3");

        fileChooser = make_unique<FileChooser>(
            "Select an audio file...",
            File{},
            "*.wav;*.mp3");
        fileChooser->launchAsync(
            FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::canSelectMultipleItems, // <--- هو ده السطر الجديد
            [this](const FileChooser& fc)
            {
                auto results = fc.getResults();
                if (results.size() > 0) {
                    for (auto& file : results) {
                        playlist.add(file.getFullPathName());
                        playlistBox.updateContent();
                        if (currentIndex == -1 && playlist.size() > 0) {
                            playIndex(0);
                        }
                    }
                    saveLastState();

                }

            });

    }


    else if (button == &restart_PreviousButton)
    {
        resetLoopPoints();
        if (playerAudio.getPosition() > 1.0) {

            playerAudio.setPosition(0.0);
        }
        else if (playlist.size() > 0) {

            int prevIndex;
            if (currentIndex > 0) {
                playIndex(currentIndex - 1);
            }
            else {
                playIndex(playlist.size() - 1);
            }

        }
        Pause_PlayButton.setImages(false, true, true,
            pauseImage, 1.0f, Colours::transparentBlack,
            pauseImage, 1.0f, Colours::white,
            pauseImage, 1.0f, Colours::transparentBlack);

        playerAudio.start();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
        Pause_PlayButton.setImages(false, true, true,
            playImage, 1.0f, Colours::transparentBlack,
            playImage, 1.0f, Colours::white,
            playImage, 1.0f, Colours::transparentBlack);
    }
    else if (button == &loopButton) {
        playerAudio.loop();
        if (playerAudio.isLooping()) {
            loopButton.setImages(false, true, true,
                loopOnImage, 1.0f, Colours::transparentBlack,
                loopOnImage, 1.0f, Colours::white,
                loopOnImage, 1.0f, Colours::transparentBlack);
        }
        else {
            loopButton.setImages(false, true, true,
                loopOffImage, 1.0f, Colours::transparentBlack,
                loopOffImage, 1.0f, Colours::white,
                loopOffImage, 1.0f, Colours::transparentBlack);
        }


    }
    else if (button == &muteButton) {
        playerAudio.mute();

        if (playerAudio.muted()) {

            muteButton.setImages(false, true, true,
                muteOnImage, 1.0f, Colours::transparentBlack,
                muteOnImage, 1.0f, Colours::white,
                muteOnImage, 1.0f, Colours::transparentBlack);
        }
        else {
            muteButton.setImages(false, true, true,
                muteOffImage, 1.0f, Colours::transparentBlack,
                muteOffImage, 1.0f, Colours::white,
                muteOffImage, 1.0f, Colours::transparentBlack);
        }
    }
    else if (button == &Pause_PlayButton) {
        if (playerAudio.isPlaying()) {
            playerAudio.stop();

            Pause_PlayButton.setImages(false, true, true,
                playImage, 1.0f, Colours::transparentBlack,
                playImage, 1.0f, Colours::white,
                playImage, 1.0f, Colours::transparentBlack);
        }
        else {
            playerAudio.start();

            Pause_PlayButton.setImages(false, true, true,
                pauseImage, 1.0f, Colours::transparentBlack,
                pauseImage, 1.0f, Colours::white,
                pauseImage, 1.0f, Colours::transparentBlack);
        }
        Pause_PlayButton.repaint();
    }//⏸️⏯️
    else if (button == &EndButton)
    {
        if (playlist.size() > 0 && currentIndex >= 0)
        {
            int nextIndex = currentIndex + 1;
            if (nextIndex >= playlist.size()) {
                nextIndex = 0;
            }

            playIndex(nextIndex);
            if (!playerAudio.isPlaying())
            {
                playerAudio.stop();
                Pause_PlayButton.setImages(false, true, true,
                    playImage, 1.0f, Colours::transparentBlack,
                    playImage, 1.0f, Colours::white,
                    playImage, 1.0f, Colours::transparentBlack);
            }
        }
        else
        {
            playerAudio.setPosition(playerAudio.getLength());
        }
    }
    else if (button == &A_B_LOOP)
    {
        if (loopState >= 2) {
            resetLoopPoints();
        }
        else if (loopState == 0)
        {
            A_B_LOOP.setImages(false, true, true,
                Bloop, 1.0f, Colours::transparentBlack,
                Bloop, 1.0f, Colours::white,
                Bloop, 1.0f, Colours::transparentBlack);
            playerAudio.setALoopPoint(playerAudio.getPosition());
            The_bar_pos.repaint();
            loopState++;

        }
        else if (loopState == 1)
        {
            A_B_LOOP.setImages(false, true, true,
                A_B_LOOP_Image, 1.0f, Colours::transparentBlack,
                A_B_LOOP_Image, 1.0f, Colours::white,
                A_B_LOOP_Image, 1.0f, Colours::transparentBlack);
            playerAudio.setBLoopPoint(playerAudio.getPosition());
            AbLOOPING = true;
            The_bar_pos.repaint();
            loopState++;

        }
    }
    else if (button == &forwardButton) {
        lengthInSeconds = playerAudio.getLength();
        double newPos = playerAudio.getPosition() + 10.0;
        if (newPos > lengthInSeconds) {
            newPos = lengthInSeconds;
        }
        playerAudio.setPosition(newPos);
    }
    else if (button == &backButton) {
        double newPos = playerAudio.getPosition() - 10.0;
        if (newPos < 0.0) {
            newPos = 0.0;
        }
        playerAudio.setPosition(newPos);
    }
    else if (button == &addMarkerButton)
    {
        double currentPos = playerAudio.getPosition();
        playerAudio.aad_marker(currentPos); 
        markerBox.updateContent(); 
        markerBox.scrollToEnsureRowIsOnscreen(playerAudio.get_markers().size() - 1); 
        The_bar_pos.repaint(); 
    }

}

MarkerListBoxModel::MarkerListBoxModel(PlayerAudio& audio, juce::ListBox& box, juce::Slider& bar) : playerAudio(audio), ownerBox(box), positionBar(bar)
{
}

int MarkerListBoxModel::getNumRows()
{
    return (int)playerAudio.get_markers().size();
}

void MarkerListBoxModel::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.fillAll(juce::Colours::cyan);

    double markerTime = playerAudio.get_marker_position(row);
    int mins = (int)(markerTime / 60);
    int secs = (int)(round(markerTime)) % 60;
    juce::String timeText = juce::String(mins).paddedLeft('0', 2) + ":" + juce::String(secs).paddedLeft('0', 2);

    g.setColour(juce::Colours::black);
    g.drawText("Marker " + juce::String(row + 1) + "  " + timeText,
        5, 0, width - 10, height,
        juce::Justification::centredLeft, true);
}

void MarkerListBoxModel::listBoxItemClicked(int row, const juce::MouseEvent& e)
{
    if (e.mods.isPopupMenu()) 
    {
        juce::PopupMenu menu;
        menu.addItem(1, "Jump to this marker");
        menu.addItem(2, "Remove this marker");
        menu.addSeparator();
        menu.addItem(3, "Cancel");

        
        menu.showMenuAsync(juce::PopupMenu::Options().withTargetScreenArea(juce::Rectangle<int>(e.getScreenX(), e.getScreenY(), 1, 1)),
            [this, row](int result)
            {
                if (result == 1) 
                {
                    playerAudio.jump_to_marker(row);
                }
                else if (result == 2) 
                {
                    playerAudio.remove_marker(row);
                    ownerBox.updateContent();
                    positionBar.repaint();
                }
            });
    }
    else
    {
        playerAudio.jump_to_marker(row); // اعمل jump علطول
    }
}

