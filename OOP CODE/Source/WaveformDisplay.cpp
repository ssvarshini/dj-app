#include <JuceHeader.h>
#include "WaveformDisplay.h"

//Constructor: Initializes the waveform display with an AudioFormatManager and AudioThumbnailCache
WaveformDisplay::WaveformDisplay(AudioFormatManager & formatManagerToUse,
                                 AudioThumbnailCache & cacheToUse) :
                                 audioThumb(1000, formatManagerToUse, cacheToUse),
                                 fileLoaded(false),
                                 position(0)
{
    //Add this class as a listener for changes in the AudioThumbnail
    audioThumb.addChangeListener(this);
}

//Destructor: Cleans up resources if needed
WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{
    //Fill the background with a dark grey color
    g.fillAll(Colour(31, 31, 31));
    
    //Set the color for the waveform and UI elements
    g.setColour (juce::Colours::grey);
    
    //Use the configurable waveform colour here:
    g.setColour(waveformColour);
    
    //If an audio file is loaded, draw the waveform
    if(fileLoaded){
        audioThumb.drawChannel(g, //Graphics context
                               getLocalBounds(), //Area to draw within
                               0, //Start time in seconds
                               audioThumb.getTotalLength(), //End time in seconds
                               0, //Channel index
                               1.0f //Vertical scaling
                               );
        //Draw a position marker as a light blue rectangle
        g.setColour(Colours::lightblue);
        g.drawRect(position * getWidth(), 0, getWidth() /20, getHeight());
    }else{
        //If no file is loaded, display a placeholder message
        g.setFont (juce::FontOptions (20.0f));
        g.drawText ("File not loaded...", getLocalBounds(),
                    juce::Justification::centred, true);   // draw some placeholder text
   }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

//Function to load audio file from s given URL and updates the waveform
void WaveformDisplay::loadURL(URL audioURL) {
    
    //Clear any existing waveform data
    audioThumb.clear();
    
    //Attempt to load the audio file from the provided URL
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    
    //Check if the file was successfully loaded
    if (fileLoaded){
        std::cout<<"wfd: loaded!" <<std::endl;
        repaint();
    }
    else{
        std::cout<<"wfd: not loaded!" <<std::endl;
    }
}

//Called when the waveform data changes
void WaveformDisplay::changeListenerCallback (ChangeBroadcaster *source){
    std::cout << "wtd:change received" <<std::endl;
    repaint();
}

//Function to update the position of the playhead
void WaveformDisplay::setPositionRelative(double pos){
    
    //Only update if the new position is different from the current one
    if (pos != position)
    {
        position = pos;
        //Repaint to reflect the new position
        repaint();
    }
}
