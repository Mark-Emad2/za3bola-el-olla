#include <JuceHeader.h>
#include "PlayerGUI.h"
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

PlayerGUI::PlayerGUI()
    : thumbnailCache(5),
    audioThumbnail(512, formatManager, thumbnailCache),
    fileLoaded(false),
	waveformVisualiser(1) // number of channels
{
    //waveform visualiser
    addAndMakeVisible(waveformVisualiser);
    waveformVisualiser.setRepaintRate(60);
    waveformVisualiser.setBufferSize(512);
    waveformVisualiser.setSamplesPerBlock(256);
    waveformVisualiser.setColours(Colours::black, Colours::cyan);

    // Add buttons
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &restart_PreviousButton , &stopButton , &loopButton,&Pause_PlayButton,&EndButton,&muteButton,&addToPlaylistButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    addAndMakeVisible(infoLabel);
    infoLabel.setJustificationType(Justification::centredLeft);
    infoLabel.setText("No File Loaded", dontSendNotification);
    infoLabel.setFont(Font("Arial", 16.0f, Font::bold));

    addAndMakeVisible(poslabel);
    addAndMakeVisible(endPos);

    // Volume slider
    volumeSlider.setRange(0, 100, 1);
    volumeSlider.setValue(50);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 60, 20);
    volumeSlider.setTextValueSuffix("%");
    volumeSlider.setSliderStyle(Slider::LinearVertical);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Speed slider
    speed_slider.setRange(0.25, 2.0, 0.25);
    speed_slider.setValue(1.0);
    speed_slider.setTextBoxStyle(Slider::TextBoxRight, false, 60, 20);
    speed_slider.setTextValueSuffix("x");
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

    // Loop button
    loopButton.setButtonText("Loop\nOFF");
    loopButton.setColour(TextButton::buttonColourId, Colours::red);
    loopButton.setClickingTogglesState(true);

    //دي تخص  التايمر
    startTimerHz(30);

    // Session file location
    sessionFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("player_session.xml");

    // Playlist box
    playlistBox.setModel(this);
    addAndMakeVisible(playlistBox);

    // Listen to thumbnail changes
    audioThumbnail.addChangeListener(this);

    loadLastState();

    Pause_PlayButton.setButtonText("Play");
    Pause_PlayButton.setColour(TextButton::buttonColourId, Colours::green);
}

PlayerGUI::~PlayerGUI()
{
    saveLastState();
}
//void PlayerGUI::resized()
//{
//    int y = 20;
//    loadButton.setBounds(20, y, 100, 40);
//    loadButton.setColour(TextButton::buttonColourId, Colours::blue);
//
//    loopButton.setBounds(250, y, 80, 40);
//
//    volumeSlider.setBounds(getWidth() - 110, 100, 100, 200);
//    positionSlider.setBounds(20, 450, getWidth() - 40, 30);
//
//    //volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
//    volumeSlider.setColour(Slider::trackColourId, Colours::darkgrey);
//
//
//    restart_PreviousButton.setBounds(250, 350, 80, 40);
//    Pause_PlayButton.setBounds(350, 350, 80, 40);
//    stopButton.setBounds(450, 350, 80, 40);
//    EndButton.setBounds(550, 350, 80, 40);
//
//    //prevButton.setBounds(340, y, 80, 40);
//    muteButton.setBounds(440, y, 80, 40);
//
//    infoLabel.setBounds(20, 100, getWidth() - 40, 60);
//    poslabel.setBounds(20, 500, 100, 30);
//    endPos.setBounds(getWidth() - 120, 500, 100, 30);
//
//    //speed slider and label
//    speed_label.setBounds(20, 400, 80, 30);
//    speed_slider.setBounds(110, 400, 200, 30);
//
//	// playlist button, box and remove button
//    addToPlaylistButton.setBounds(20, 70, 140, 28);
//   // removeButton.setBounds(170, 70, 80, 28);
//    playlistBox.setBounds(20, 110, getWidth() - 160, 200);
//
//
//
//
//}

void PlayerGUI::resized()
{
    int margin = 20;
    int buttonWidth = 100;
    int buttonHeight = 40;

    int rightPanelWidth = 260;// عرض جزء البلاي ليست
    int contentWidth = getWidth() - rightPanelWidth - (3 * margin);

    // ====== الأزرار العليا ======
    int topY = margin;
    loadButton.setBounds(margin, topY, buttonWidth, buttonHeight);
    addToPlaylistButton.setBounds(loadButton.getRight() + 10, topY, 140, buttonHeight);
    loopButton.setBounds(addToPlaylistButton.getRight() + 10, topY, 80, buttonHeight);
    muteButton.setBounds(loopButton.getRight() + 10, topY, 80, buttonHeight);

    // ====== metadata label (في النص) ======
    int infoY = loadButton.getBottom() + 20;
    infoLabel.setBounds(margin, infoY, contentWidth, 60);
    infoLabel.setColour(Label::backgroundColourId, Colours::darkgrey.withAlpha(0.2f));
    infoLabel.setColour(Label::textColourId, Colours::whitesmoke);
    infoLabel.setJustificationType(Justification::centredLeft);

    // ====== waveform visualiser ======
    int waveformY = infoLabel.getBottom() + 10;
    int waveformHeight = 120; // ارتفاع أكبر للـ waveform
    waveformVisualiser.setBounds(margin, waveformY, contentWidth, waveformHeight);

    // ====== Position slider (hidden but functional) ======
    positionSlider.setBounds(margin, waveformY, contentWidth, waveformHeight);

    // ====== الليبلات بتاعة الوقت ======
    int posSliderY = waveformY + waveformHeight + 10;
    poslabel.setBounds(margin, posSliderY, 100, 25);
    endPos.setBounds(contentWidth - 100, posSliderY, 100, 25);

    // ====== السلايدر بتاع السرعة ======
    int speedY = posSliderY + 30;
    speed_label.setBounds(margin, speedY, 80, 25);
    speed_slider.setBounds(speed_label.getRight() + 10, speedY, 200, 25);

    // ====== Volume slider ======
    volumeSlider.setBounds(contentWidth - 70, infoLabel.getBottom() + 10, 60, 200);

    // ====== playlist على اليمين ======
    int playlistX = getWidth() - rightPanelWidth - margin;
    int playlistY = topY;
    playlistBox.setBounds(playlistX, playlistY, rightPanelWidth, getHeight() - (2 * margin) - 60);
    playlistBox.setColour(ListBox::backgroundColourId, Colours::darkslategrey.withAlpha(0.4f));
    playlistBox.setOutlineThickness(1);
    playlistBox.setColour(ListBox::outlineColourId, Colours::lightgrey.withAlpha(0.4f));

    // ====== الأزرار السفلية ======
    int bottomY = getHeight() - buttonHeight - margin;
    restart_PreviousButton.setBounds(margin, bottomY, buttonWidth, buttonHeight);
    Pause_PlayButton.setBounds(restart_PreviousButton.getRight() + 10, bottomY, buttonWidth, buttonHeight);
    stopButton.setBounds(Pause_PlayButton.getRight() + 10, bottomY, buttonWidth, buttonHeight);
    EndButton.setBounds(stopButton.getRight() + 10, bottomY, buttonWidth, buttonHeight);

    // ====== تحسين الأزرار ======
    for (auto* btn : { &loadButton, &addToPlaylistButton, &loopButton, &muteButton,
                       &restart_PreviousButton, &Pause_PlayButton, &stopButton, &EndButton })
    {
        btn->setColour(TextButton::buttonColourId, Colours::darkcyan.withAlpha(0.7f));
        btn->setColour(TextButton::textColourOffId, Colours::white);
        btn->setColour(TextButton::buttonOnColourId, Colours::deepskyblue);
        btn->setConnectedEdges(0);
    }
}

// تحديث الليبل من الميتاداتا
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


        // Load waveform for the file
        audioThumbnail.clear();
        audioThumbnail.setSource(new FileInputSource(file));
        fileLoaded = true;

		//delete previous waveform data
        waveformVisualiser.clear();
    }
    else {
        infoLabel.setText("Failed to load audio file.", dontSendNotification);
        fileLoaded = false;
    }
}

// save last session state
void PlayerGUI::saveLastState()
{
    File currentFile = playerAudio.getCurrentFile();
    appState.setProperty("lastFile", currentFile.getFullPathName(), nullptr);
    appState.setProperty("lastPosition", playerAudio.getPosition(), nullptr);
    appState.setProperty("lastVolume", volumeSlider.getValue(), nullptr);
    appState.setProperty("lastPositionSlider", positionSlider.getValue(), nullptr);
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
        playlistState.addChild(filetree, -1, nullptr); // -1 عشان يضيف فى الاخر
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
                positionSlider.setRange(0.0, lengthInSeconds);
                playerAudio.setPosition(lastPosition);
                volumeSlider.setValue(lastVolume, dontSendNotification);
                positionSlider.setValue(lastPositionSlider, dontSendNotification);
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
    else if (slider == &positionSlider)
    {
        playerAudio.setPosition((float)slider->getValue());
    }
    else if (slider == &speed_slider)
    {
        float speedValue = (float)speed_slider.getValue();
        playerAudio.set_speed(speedValue);
    }
}

void PlayerGUI::paint(Graphics& g)
{
    g.fillAll(Colours::black);

    // Draw speed value
    g.setColour(Colours::white);
    g.setFont(14.0f);
    String speedText = String(playerAudio.get_speed(), 2) + "x";
    g.drawText(speedText, speed_slider.getRight() + 10, speed_slider.getY(), 50, speed_slider.getHeight(), Justification::centredLeft);

    // باقي الـ paint للـ waveform القديم ممكن تمسحه أو تتركه حسب احتياجك
}

void PlayerGUI::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();

    // Update position slider value
    positionSlider.setValue(currentPos, dontSendNotification);

    // Handle end of track and looping
    if (totalLength > 0 && currentPos >= totalLength)
    {
        playerAudio.stop();
        safeButton_Colour(Pause_PlayButton, "Play", Colours::green);

        if (loopButton.getToggleState())
        {
            playerAudio.setPosition(0.0);
            playerAudio.start();
            safeButton_Colour(Pause_PlayButton, "Pause ||", Colours::orange);
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

    // update position labels
    int mins = (int)(currentPos / 60);
    int secs = (int)(round(currentPos)) % 60;
    String positionText = String(mins).paddedLeft('0', 2) + ":" + String(secs).paddedLeft('0', 2);
    poslabel.setText(positionText, dontSendNotification);

    int totalMins = (int)(totalLength / 60);
    int totalSecs = (int)(round(totalLength)) % 60;
    String endPositionText = String(totalMins).paddedLeft('0', 2) + ":" + String(totalSecs).paddedLeft('0', 2);
    endPos.setText(endPositionText, dontSendNotification);

    // repaint to update waveform display
    repaint();
}

void PlayerGUI::changeListenerCallback(ChangeBroadcaster* source)
{
    if (source == &audioThumbnail)
    {
        repaint(); // Redraw when waveform data is loaded
    }
}

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

// ------------------------------ Playlist Methods ------------------------------

int PlayerGUI::getNumRows()
{
    return playlist.size();
}

void PlayerGUI::paintListBoxItem(int row, Graphics& graph, int width, int height, bool rowIsSelected) {
    
    if (row < 0 || row >= playlist.size()) return;
    if (rowIsSelected) {
        graph.fillAll(Colours::lightblue);
        graph.setColour(Colours::black);
    }
    else {
        graph.setColour(Colours::white);
    }
    String filename = File(playlist[row]).getFileName();
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

    if (e.mods.isPopupMenu()) {// to check if user press right click
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
                            safeButton_Colour(Pause_PlayButton, "Play", Colours::green);
                            infoLabel.setText("No File Loaded", dontSendNotification);
                            fileLoaded = false;
                            currentIndex = -1;
                            repaint();
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

void PlayerGUI::selectedRowsChanged(int lastRow)
{
    currentIndex = lastRow;
    //playIndex(currentIndex);
}

//الاغنيه اللى الشغاله من البلاي ليست و رقم الاندكس بتاعها
void PlayerGUI::playIndex(int row) {
    if (row >= 0 && row < playlist.size()) {
        File fileToPlay(playlist[row]);
        if (fileToPlay.existsAsFile()) {
            playerAudio.loadFile(fileToPlay);
            double lengthInSeconds = playerAudio.getLength();
            positionSlider.setRange(0.0, lengthInSeconds);
            safeButton_Colour(Pause_PlayButton, "Pause ||", Colours::orange);
            playerAudio.start();
            currentIndex = row;
            playlistBox.selectRow(currentIndex);
            updateLabel(fileToPlay);
            saveLastState();
        }
        else {   // if file not exist remove it from playlist
            playlist.remove(row);
            playlistBox.updateContent();
            saveLastState();
        }
    }
}

void PlayerGUI::buttonClicked(Button* button)
{
    if (button == &loadButton)
    {
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
                    safeButton_Colour(Pause_PlayButton, "Pause ||", Colours::orange);
                    playerAudio.start();
                    updateLabel(file);
                    saveLastState();
                }
            });
    }
    else if (button == &addToPlaylistButton) {
        fileChooser = make_unique<FileChooser>(
            "Select audio files...",
            File{},
            "*.wav;*.mp3");
        fileChooser->launchAsync(
            FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles,
            [this](const FileChooser& fc)
            {
                auto results = fc.getResults();
                if (results.size() > 0) {
                    for (auto& file : results) {
                        playlist.add(file.getFullPathName());
                    }
                    playlistBox.updateContent();
                    if (currentIndex == -1 && playlist.size() > 0) {
                        playIndex(0);
                    }
                    saveLastState();
                }
            });
    }
    else if (button == &restart_PreviousButton)
    {
        if (playerAudio.getPosition() > 1.0) {
            playerAudio.setPosition(0.0);
        }
        else if (playlist.size() > 0) {
            int prevIndex;
            if (currentIndex > 0)
            {
                playIndex(currentIndex - 1);
            }
            else {
                playIndex(playlist.size() - 1);
            }
        }
        safeButton_Colour(Pause_PlayButton, "Pause ||", Colours::orange);
        playerAudio.start();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
        safeButton_Colour(Pause_PlayButton, "Play", Colours::green);
    }
    else if (button == &loopButton) {
        if (loopButton.getToggleState())
        {
            loopButton.setButtonText("Loop\nON");
            loopButton.setColour(TextButton::buttonOnColourId, Colours::green);
        }
        else {
            loopButton.setButtonText("Loop\nOFF");
            loopButton.setColour(TextButton::buttonColourId, Colours::red);
        }
        loopButton.repaint();
    }
    else if (button == &muteButton) {
        playerAudio.mute();
        if (playerAudio.muted()) {
            muteButton.setButtonText("Unmute");
        }
        else {
            muteButton.setButtonText("Mute");
        }
    }
    else if (button == &Pause_PlayButton) {
        if (playerAudio.isPlaying()) {
            playerAudio.stop();
            safeButton_Colour(Pause_PlayButton, "Play", Colours::green);
        }
        else {
            playerAudio.start();
            safeButton_Colour(Pause_PlayButton, "Pause ||", Colours::orange);
        }
    }
    else if (button == &EndButton) {
        playerAudio.setPosition(playerAudio.getLength());
        if (playlist.size() == 0 || currentIndex == -1) {
            safeButton_Colour(Pause_PlayButton, "Play", Colours::green);
        }
    }
}