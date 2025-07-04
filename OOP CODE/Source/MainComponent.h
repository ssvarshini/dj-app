#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioplayer.h"
#include "DeckGUI.h"
#include "PlaylistComponent.h"

//A custom LookAndFeel class for styling the crossfader slider
class CrossFaderLookAndFeel : public LookAndFeel_V4
{
public:
    //Constructor: Sets initial slider thumb color
    CrossFaderLookAndFeel()
    {
        setColour(Slider::thumbColourId, Colours::black);  
    }

    void drawLinearSlider(Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const Slider::SliderStyle style, Slider& slider) override
    {
        //Set vertical center of the slider
        int centerY = y + (height / 2);
        int sliderX = static_cast<int>(sliderPos);

        //Draw the left side of the crossfader in deck 1
        g.setColour(Colour(97, 132, 216));
        g.drawLine(x, centerY, sliderX, centerY, 4);

        //Draw the left side of the crossfader in deck 2
        g.setColour(Colour(80, 162, 167));
        g.drawLine(sliderX, centerY, x + width, centerY, 4);

        //Draw the tick marks along the slider
        g.setColour(Colours::lightgrey);
        const int numTicks = 20;
        const int tickHalfHeight = 10;
        const int tickStartY = centerY - tickHalfHeight;
        const int tickEndY = centerY + tickHalfHeight;

        for (int i = 0; i <= numTicks; ++i)
        {
            int tickX = x + (i * width) / numTicks;
            g.drawLine(tickX, tickStartY, tickX, tickEndY, 1);
        }

        //Draw slider thumb
        g.setColour(Colours::black);
        const int thumbWidth = 12;
        const int thumbHeight = 30;
        const int thumbX = sliderX - (thumbWidth / 2);
        const int thumbY = centerY - (thumbHeight / 2);

        //Draw the slider handle
        g.fillRect(thumbX, thumbY, thumbWidth, thumbHeight);
        g.setColour(Colours::white);
        g.fillRect(thumbX + 5, thumbY, thumbWidth - 10, thumbHeight);
    }
};


//The main audio component containing two decks, a playlist, and a crossfader
class MainComponent : public AudioAppComponent,
                      public Slider::Listener //Listens for slider changes
{
public:
    //Constructor: Initializes audio components and UI elements
    MainComponent();
    
    //Destructor: Cleans up resources
    ~MainComponent();

    //AudioAppComponent overrides:
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //Handles UI painting
    void paint(Graphics& g) override;
    //Adjusts UI component sizes when resized
    void resized() override;

    //Implement Slider::Listener
    void sliderValueChanged(Slider* slider) override;


private:
    //Manages audio file formats
    AudioFormatManager formatManager;
    //Caches waveforms for fast drawing
    AudioThumbnailCache thumbCache{100};

    //File chooser for selecting audio files
    juce::FileChooser fChooser{"Select a file..."};

    //Two DJ audio players
    DJAudioplayer player1{formatManager};
    DJAudioplayer player2{formatManager};

    //Two deck GUIs for controlling the players
    DeckGUI deckGUI1{&player1, formatManager, thumbCache, true};
    DeckGUI deckGUI2{&player2, formatManager, thumbCache, false};

    //Mixer to combine audio from both decks
    MixerAudioSource mixerSource;

    //Playlist component for managing tracks
    PlaylistComponent playlistComponent;

    //Horizontal slider for balancing between decks
    Slider crossFaderSlider;
    //Custom look for the crossfader
    CrossFaderLookAndFeel crossFaderLookAndFeel;

    //Label for crossfader control
    juce::Label crossFaderLabel{"crossFaderLabel", "CROSSFADER"};

    //Prevents accidental copying of the component
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

