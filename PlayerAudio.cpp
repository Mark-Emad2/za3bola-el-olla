#include <JuceHeader.h>
#include "PlayerAudio.h"
//using namespace std;
using namespace juce;

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
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
    //gain = jlimit(0.0f, 1.0f, gain);
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
float PlayerAudio::get_current_gain() const
{
    return ismuted ? last_value : transportSource.getGain();
}

bool PlayerAudio::isPlaying() const
{
    return transportSource.isPlaying();
}

File PlayerAudio::getCurrentFile() const { return currentFile; };

