#pragma once
#include <JuceHeader.h>

class TrackPad
{
public:
    //Constructor: Initializes the TrackPad object with a given file
    TrackPad(juce::File _file);

    //File object representing the track file
    juce::File file;
    //URL representation of the track
    juce::URL trackURL;
    //Title of the track
    juce::String title;
    //Length of the track (as a string)
    juce::String length;

    /** Compare object's title for searching */
    bool operator==(const juce::String& other) const;
};

