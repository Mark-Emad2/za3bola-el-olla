#include <JuceHeader.h>
#include "PlayerAudio.h"
//using namespace std;
using namespace juce;

PlayerAudio::PlayerAudio()
    : resampleSource(&transportSource, false)
{
    formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
    readerSource.reset();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();

    resampleSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    resampleSource.releaseResources();
}

bool PlayerAudio::loadFile(const File& file)
{

    if (file.existsAsFile()) {

        currentFile = file;

        if (auto* reader = formatManager.createReaderFor(file))
        {

            // 🔑 Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = make_unique<AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            set_speed(current_speed);

            // transportSource.start();
        }
        return true;
    }
    else
        return false;
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
}
void PlayerAudio::setGain(float gain)
{
    last_value = gain;
    if (!ismuted)
        transportSource.setGain(gain);

}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}
double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}
void PlayerAudio::mute() {
    if (ismuted) {
        transportSource.setGain(last_value);
        ismuted = false;
    }
    else {
        last_value = transportSource.getGain();
        transportSource.setGain(0.0f);
        ismuted = true;
    }
}
bool PlayerAudio::muted() const
{
    return ismuted;
}
void PlayerAudio::loop()
{
    if (islooping) {
        islooping = false;
    }
    else {
        islooping = true;
    }

}
bool PlayerAudio::isLooping() const
{
    return islooping;
}
float PlayerAudio::get_current_gain() const
{
    return ismuted ? last_value : transportSource.getGain();
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}

void PlayerAudio::set_speed(float speed)
{

    speed = jlimit(0.25f, 2.0f, speed);
    current_speed = speed;
    resampleSource.setResamplingRatio(speed);
}

float PlayerAudio::get_speed() const
{
    return current_speed;
}
File PlayerAudio::getCurrentFile() const { return currentFile; };


void PlayerAudio::setALoopPoint(double pos)
{
    aLoopPoint = pos;

}
void PlayerAudio::setBLoopPoint(double pos)
{
    bLoopPoint = pos;
}

double PlayerAudio::getALoopPoint() const
{

    return aLoopPoint;
}
double PlayerAudio::getBLoopPoint() const
{
    return bLoopPoint;
}
void PlayerAudio::aad_marker(double pos) {
    markers.push_back(pos);
}
void PlayerAudio::remove_marker(int index) {
    if (index >= 0 && index < markers.size()) {
        markers.erase(markers.begin() + index);
    }
}
void PlayerAudio::clear_markers() {
    markers.clear();
}
void PlayerAudio::jump_to_marker(int index) {
    if (index >= 0 && index < markers.size()) {
        setPosition(markers[index]);
    }
}
const std::vector<double>& PlayerAudio::get_markers() const {
    return markers;
}
double PlayerAudio::get_marker_position(int index) const {
    if (index >= 0 && index < markers.size()) {
        return markers[index];
    }
    return -1.0;
}