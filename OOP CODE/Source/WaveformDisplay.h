#pragma once

#include <JuceHeader.h>

//This class is responsible for displaying a visual waveform of an audio file.
class WaveformDisplay  : public juce::Component,
                         public ChangeListener
{
public:
    //Constructor: Initializes the waveform display using an AudioFormatManager and AudioThumbnailCache
    WaveformDisplay(AudioFormatManager & formatManagerToUse,
                    AudioThumbnailCache & cacheToUse);
    //Destructor: Cleans up resources
    ~WaveformDisplay() override;

    //This function is responsible for rendering the waveform on the component.
    void paint (juce::Graphics&) override;
    
    //This function is called when the component is resized
    void resized() override;
    
    //Callback function that gets triggered when the ChangeBroadcaster (e.g., an audio thumbnail) sends an update
    void changeListenerCallback(ChangeBroadcaster *source) override;
    
    //Function to updates the position of the playhead relative to the waveform's length
    void setPositionRelative(double pos);
    
    //Function to loads an audio file from a given URL and updates the waveform display accordingl
    void loadURL(URL audioURL);
    
    //Function to set the color of the waveform and repaints it when a new track is added
    void setWaveformColour(juce::Colour newColour)
        {
            waveformColour = newColour;
            //Repaint the component to apply the new color
            repaint();
        }

    private:
    
    //AudioThumbnail object that stores the waveform data and allows drawing
    AudioThumbnail audioThumb;
    
    //Boolean flag to check whether a file has been successfully loaded
    bool fileLoaded;
    
    //Stores the current playback position relative to the waveform
    double position;
    
    //Color used to render the waveform.
    juce::Colour waveformColour { juce::Colours::orange };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WaveformDisplay)
};
