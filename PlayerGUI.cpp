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

    // send audio data to waveform visualiser
    if (bufferToFill.buffer->getNumChannels() > 0)
    {
        const float* channelData = bufferToFill.buffer->getReadPointer(0);

        waveformVisualiser.pushBuffer(*bufferToFill.buffer);
    }
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

PlayerGUI::PlayerGUI(const juce::String& sessionFileName)
    : thumbnailCache(5),
    audioThumbnail(512, formatManager, thumbnailCache),
    fileLoaded(false),
    waveformVisualiser(1)
{
    addAndMakeVisible(waveformVisualiser);

    addAndMakeVisible(albumArtComponent);
    albumArtComponent.setImagePlacement(juce::RectanglePlacement::centred);

    waveformVisualiser.setRepaintRate(60);
    waveformVisualiser.setBufferSize(512);
    waveformVisualiser.setSamplesPerBlock(256);
    waveformVisualiser.setColours(Colours::black, Colours::cyan);
    // Add buttons
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &restart_PreviousButton , &stopButton ,&Pause_PlayButton,&EndButton,&muteButton,&addToPlaylistButton,&loopButton,&forwardButton ,&backButton , &A_B_LOOP })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    The_bar_pos.setLookAndFeel(this);
    loopOffImage = ImageCache::getFromMemory(BinaryData::loop_2_png, BinaryData::loop_2_pngSize);
    loopOnImage = ImageCache::getFromMemory(BinaryData::LLOOP_png, BinaryData::LLOOP_pngSize);

    muteOnImage = ImageCache::getFromMemory(BinaryData::mute_png, BinaryData::mute_pngSize);
    muteOffImage = ImageCache::getFromMemory(BinaryData::volume_png, BinaryData::volume_pngSize);

    loadImage = ImageCache::getFromMemory(BinaryData::arrow_png, BinaryData::arrow_pngSize);
    addToPlaylistImage = ImageCache::getFromMemory(BinaryData::add_png, BinaryData::add_pngSize);

    stopImage = ImageCache::getFromMemory(BinaryData::stop_png, BinaryData::stop_pngSize);
    restartPreviousImage = ImageCache::getFromMemory(BinaryData::rewindsign_png, BinaryData::rewindsign_pngSize);
    endImage = ImageCache::getFromMemory(BinaryData::fastforwardbutton_png, BinaryData::fastforwardbutton_pngSize);
    playImage = ImageCache::getFromMemory(BinaryData::playbuttonarrowhead_png, BinaryData::playbuttonarrowhead_pngSize);

    pauseImage = ImageCache::getFromMemory(BinaryData::pause_png, BinaryData::pause_pngSize);
    forwardImage = ImageCache::getFromMemory(BinaryData::forward_png, BinaryData::forward_pngSize);
    backImage = ImageCache::getFromMemory(BinaryData::backward_png, BinaryData::backward_pngSize);
    Aloop = ImageCache::getFromMemory(BinaryData::lettera_1_png, BinaryData::lettera_1_pngSize);
    Bloop = ImageCache::getFromMemory(BinaryData::letterb_1_png, BinaryData::letterb_1_pngSize);
    A_B_LOOP_Image = ImageCache::getFromMemory(BinaryData::abtesting_png, BinaryData::abtesting_pngSize);
    ALOOP_slider = ImageCache::getFromMemory(BinaryData::lettera_png, BinaryData::lettera_pngSize);
    BLOOP_slider = ImageCache::getFromMemory(BinaryData::letterb_png, BinaryData::letterb_pngSize);





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


    // Position slider - invisible but functional
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);
    positionSlider.setSliderStyle(Slider::LinearBar);
    positionSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    positionSlider.setAlpha(0.0f); // Make invisible


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





}
PlayerGUI::~PlayerGUI()
{
    saveLastState();
}
void PlayerGUI::resized()
{
    const int margin = 15;
    const int spacing = 10;
    const int w = getWidth();
    const int h = getHeight();
    const int innerW = w - margin * 2;
    const int innerH = h - margin * 2;

    // نسب للرأس / الفوتر / المنتصف
    const float headerRatio = 0.12f;
    const float footerRatio = 0.22f;

    const int headerH = (int)(innerH * headerRatio);
    const int footerH = (int)(innerH * footerRatio);
    const int middleH = innerH - headerH - footerH - spacing * 2;

    // ---------- Header ----------
    const int headerX = margin;
    const int headerY = margin;
    const int buttonW = 100;
    const int headerInnerH = jmax(32, headerH - spacing);

    infoLabel.setBounds(headerX, headerY, innerW - (buttonW * 2) - spacing * 2, headerInnerH);
    infoLabel.setJustificationType(Justification::centredLeft);
    loadButton.setBounds(infoLabel.getRight() + spacing, headerY, buttonW, headerInnerH);
    addToPlaylistButton.setBounds(loadButton.getRight() + spacing, headerY, buttonW, headerInnerH);

    // ---------- Middle area (waveform + album + playlist) ----------
    const int middleY = headerY + headerH + spacing;
    const int waveformH = jmax(44, (int)(middleH * 0.22f));
    waveformVisualiser.setBounds(margin, middleY, innerW, waveformH);
    positionSlider.setBounds(margin, middleY, innerW, waveformH);

    const int contentY = waveformVisualiser.getBottom() + spacing;
    const int contentH = middleH - waveformH - spacing;

    // قسم الصورة و playlist: نسبة عرض صورة = 62%
    const float albumRatio = 0.62f;
    const int albumSectionW = (int)(innerW * albumRatio);
    const int playlistW = innerW - albumSectionW - spacing;

    const int albumMaxSize = jmin(contentH, albumSectionW - spacing * 2);
    const int albumX = margin + (albumSectionW - albumMaxSize) / 2;
    const int albumY = contentY + (contentH - albumMaxSize) / 2;
    albumArtComponent.setBounds(albumX, albumY, albumMaxSize, albumMaxSize);

    playlistBox.setBounds(margin + albumSectionW + spacing, contentY, playlistW, contentH);
    playlistBox.setOutlineThickness(1);

    // ---------- Footer ----------
    const int footerY = margin + headerH + middleH + spacing * 2;
    const int timeLabelW = 60;
    const int barH = 26;
    const int barW = innerW - timeLabelW * 2 - spacing * 2;

    poslabel.setBounds(margin, footerY + spacing, timeLabelW, barH);
    The_bar_pos.setBounds(poslabel.getRight() + spacing, footerY + spacing, barW, barH);
    endPos.setBounds(The_bar_pos.getRight() + spacing, footerY + spacing, timeLabelW, barH);

    // ======= منطقة الصف السفلي مع الأزرار =======
    const int controlsY = footerY + spacing + barH + spacing;
    const int availableControlsH = footerH - barH - spacing * 3;
    const int controlsAreaTop = controlsY;
    const int controlsAreaH = jmax(48, availableControlsH);

    // --- تصميم مطابق للصورة:
    // على الشمال: شبكة دائرية 2 صف × 5 أعمدة (أزرار صغيرة دائرية)
    const int leftGridCols = 5;
    const int leftGridRows = 2;
    const int leftGridW = jmin((int)(innerW * 0.45f), 520); // عرض منطقي للشبكة
    const int leftGridX = margin;
    const int leftGridY = controlsAreaTop;

    // نحسب حجم زر صغير بحيث يتسع 5 أعمدة مع spacing
    const int smallBtnSize = jmin(48, (leftGridW - (leftGridCols - 1) * spacing) / leftGridCols);
    const int leftGridActualW = smallBtnSize * leftGridCols + spacing * (leftGridCols - 1);

    // الأزرار الصغيرة — صفين (ترتيب من الصورة: Back, Restart, Restart/Previous, Previous, (maybe) repeat icons ...)
    // عدّل أسماء الأزرار هنا لتتطابق مع أزرارك الفعلية.
    // Row 0
    backButton.setBounds(leftGridX + 0 * (smallBtnSize + spacing), leftGridY + 0 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    restart_PreviousButton.setBounds(leftGridX + 1 * (smallBtnSize + spacing), leftGridY + 0 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    restart_PreviousButton.setBounds(leftGridX + 2 * (smallBtnSize + spacing), leftGridY + 0 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    //rewindButton.setBounds(leftGridX + 3 * (smallBtnSize + spacing), leftGridY + 0 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    //trackMenuButton.setBounds(leftGridX + 4 * (smallBtnSize + spacing), leftGridY + 0 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);

    // Row 1
    loopButton.setBounds(leftGridX + 0 * (smallBtnSize + spacing), leftGridY + 1 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    restart_PreviousButton.setBounds(leftGridX + 1 * (smallBtnSize + spacing), leftGridY + 1 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    EndButton.setBounds(leftGridX + 2 * (smallBtnSize + spacing), leftGridY + 1 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    muteButton.setBounds(leftGridX + 3 * (smallBtnSize + spacing), leftGridY + 1 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);
    A_B_LOOP.setBounds(leftGridX + 4 * (smallBtnSize + spacing), leftGridY + 1 * (smallBtnSize + spacing), smallBtnSize, smallBtnSize);

    // --- الزر الكبير للـ Play/Pause في مركز الفوتر (أكبر وأمامي)
    const int centerPlaySize = jmax(64, smallBtnSize + 20);
    const int centerX = margin + (innerW / 2) - (centerPlaySize / 2);
    const int centerY = controlsAreaTop - (centerPlaySize / 6); // يطلع شوية للأعلى ليظهر أكبر كما في الصورة
    Pause_PlayButton.setBounds(centerX, centerY, centerPlaySize, centerPlaySize);

    // --- يمين المركز: أزرار تشغيل/تقديم/تأخير أصغر (دائماً على نفس خط الزر الكبير)
    const int rightHelpersX = centerX + centerPlaySize + spacing * 2;
    const int smallHelperSize = smallBtnSize;
    EndButton.setBounds(rightHelpersX, centerY + (centerPlaySize - smallHelperSize) / 2, smallHelperSize, smallHelperSize);
    forwardButton.setBounds(EndButton.getRight() + spacing, centerY + (centerPlaySize - smallHelperSize) / 2, smallHelperSize, smallHelperSize);
    // skipButton.setBounds(forwardButton.getRight() + spacing, centerY + (centerPlaySize - smallHelperSize) / 2, smallHelperSize, smallHelperSize);

     // --- أقصى اليمين: سلايدرات الصوت و السرعة كما قبل
    const int slidersW = (int)(innerW * 0.25f);
    const int slidersX = margin + innerW - slidersW;
    const int labelW = 60;
    const int sliderH = 22;

    volume_label.setBounds(slidersX, controlsAreaTop + (centerPlaySize / 2) - sliderH / 2, labelW, sliderH);
    volumeSlider.setBounds(volume_label.getRight() + spacing, controlsAreaTop + (centerPlaySize / 2) - sliderH / 2, slidersW - labelW - spacing, sliderH);

    speed_label.setBounds(slidersX, volume_label.getBottom() + spacing, labelW, sliderH);
    speed_slider.setBounds(speed_label.getRight() + spacing, volume_label.getBottom() + spacing, slidersW - labelW - spacing, sliderH);
}


//void PlayerGUI::resized()
//{
//    int margin = 10;
//    int spacing = 8;
//    int fullWidth = getWidth() - (2 * margin);
//
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
    g.fillAll(Colours::grey);
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
        graph.fillAll(Colours::lightblue);
    }
    String filename = File(playlist[row]).getFileName();
    graph.setColour(Colours::black);
    graph.drawText(filename, 4, 0, width - 4, height, Justification::centredLeft, true);
}

//عشان اما تغغط مرتين يغير رقم الاندكس و يشغل الاغنيه
void PlayerGUI::mouseDown(const MouseEvent& event)
{
    // handle clicking on waveform to seek
    auto waveformArea = positionSlider.getBounds();
    if (waveformArea.contains(event.getPosition()))
    {
        double relativePos = (event.getPosition().x - waveformArea.getX()) / (double)waveformArea.getWidth();
        double newPosition = relativePos * playerAudio.getLength();
        playerAudio.setPosition(newPosition);
        positionSlider.setValue(newPosition);
    }
}


void PlayerGUI::mouseDrag(const MouseEvent& event)
{
    // handle dragging on waveform to seek
    auto waveformArea = positionSlider.getBounds();
    if (waveformArea.contains(event.getPosition()))
    {
        double relativePos = (event.getPosition().x - waveformArea.getX()) / (double)waveformArea.getWidth();
        relativePos = jlimit(0.0, 1.0, relativePos);
        double newPosition = relativePos * playerAudio.getLength();
        playerAudio.setPosition(newPosition);
        positionSlider.setValue(newPosition);
    }
}
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


void PlayerGUI::drawLinearSlider(Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, Slider::SliderStyle style, Slider& slider)
{
    LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);

    // 2. اتأكد إننا بنرسم على الـ positionSlider بس (مش الـ volumeSlider)
    if (&slider == &The_bar_pos)
    {
        auto range = The_bar_pos.getRange();

        auto valueToPixel = [&](double value) -> float
            {
                double proportion = The_bar_pos.valueToProportionOfLength(value);
                return jmap((float)proportion, 0.0f, 1.0f, (float)x, (float)(x + width));
            };

        // نعرف المتغيرات هنا بحيث تكون متاحة لأي بلوك داخل هذه الدالة
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
            int imageX = (int)(a_x_pos - (float)imageWidth / 2.0f); // عشان الصورة تتوسط عند النقطة

            g.drawImage(ALOOP_slider,
                imageX, y, imageWidth, height, // المكان والحجم اللي هيترسم فيه
                0, 0, ALOOP_slider.getWidth(), ALOOP_slider.getHeight() // المكان اللي هيتاخد من الصورة (الصورة كلها)
            );
        }

        if (loopState >= 2)
        {
            double bPointTime = playerAudio.getBLoopPoint();
            b_x_pos = valueToPixel(bPointTime);
            hasB = true;

            float aspectRatio = (float)BLOOP_slider.getWidth() / (float)BLOOP_slider.getHeight();
            int imageWidth = (int)((float)height * aspectRatio);
            int imageX = (int)(b_x_pos - (float)imageWidth / 2.0f);
            g.drawImage(BLOOP_slider,
                imageX, y, imageWidth, height, // المكان والحجم اللي هيترسم فيه
                0, 0, BLOOP_slider.getWidth(), BLOOP_slider.getHeight() // المكان اللي هيتاخد من الصورة (الصورة كلها)
            );
        }

        // لو كلتا النقطتين موجودتين — نرسم التظليل بينهما
        if (hasA && hasB)
        {
            float left = jmin(a_x_pos, b_x_pos);
            float right = jmax(a_x_pos, b_x_pos);
            float widthRect = right - left;

            if (widthRect > 0.0f) // تأكد من أن العرض موجب
            {
                g.setColour(Colours::cyan.withAlpha(0.15f));
                g.fillRect(left, (float)y, widthRect, (float)height);
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
        {// عشان لو مفيش بلاي ليست
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

}