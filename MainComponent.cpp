#include "MainComponent.h"
using namespace juce;
using namespace std;

MainComponent::MainComponent() {

    addAndMakeVisible(player1);
    setSize(1000, 700);
    setAudioChannels(0, 2);
}
MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    player1.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    player1.releaseResources();
}
void MainComponent::resized()
{
    player1.setBounds(getLocalBounds());
}

