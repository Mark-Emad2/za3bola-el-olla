#include <JuceHeader.h>
#include "MainComponent.h"
using namespace std;
using namespace juce;
// Our application class
class SimpleAudioPlayer : public JUCEApplication
{
public:
    const String getApplicationName() override { return "Simple Audio Player"; }
    const String getApplicationVersion() override { return "1.0"; }

    void initialise(const String&) override
    {
        // Create and show the main window
        mainWindow = make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        mainWindow = nullptr; // Clean up
    }

private:
    // The main window of the app
    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow(String name)
            : DocumentWindow(name,
                Colour(0xFF1A1A2A),
                DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true);
            setContentOwned(new MainComponent(), true); // MainComponent = our UI + logic
            centreWithSize(1000, 700);
            setVisible(true);
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
    };

    unique_ptr<MainWindow> mainWindow;
};

// This macro starts the app
START_JUCE_APPLICATION(SimpleAudioPlayer)
//جديد
