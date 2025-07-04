
#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
class OtoDecksApplication  : public JUCEApplication
{
public:
    //==============================================================================
    OtoDecksApplication() {}
    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        //Method for initialising application
        mainWindow.reset (new MainWindow (getApplicationName()));
    }

    void shutdown() override
    {
        //Application's shutdown code
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public DocumentWindow
    {
    public:
        //Constructor: Creates the main application window with a title and default settings
        MainWindow (String name)  : DocumentWindow (name,
                                                    Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                    //Enables minimize, maximize, and close buttons
                                                    DocumentWindow::allButtons)
        {
            setUsingNativeTitleBar(true); //Use the operating system's default title bar
            setContentOwned(new MainComponent(), true); //Set MainComponent as the window's content

            //Handle fullscreen/resizable settings for different platforms
            #if JUCE_IOS || JUCE_ANDROID
                setFullScreen(true); //On mobile, make the window full screen
            #else
                setResizable(true, true); //Allow window resizing
                centreWithSize(getWidth(), getHeight()); // enter the window on the screen
            #endif

            setVisible(true); //Make the window visible
        }

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (OtoDecksApplication)
