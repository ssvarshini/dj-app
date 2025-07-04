#include "TrackList.h"
#include <JuceHeader.h>

//Constructor: Initializes a TrackPad object with a given file
TrackPad::TrackPad(juce::File _file)
    //Store the file object
    : file(_file),
      //Extract the file name as the track title
      title(_file.getFileNameWithoutExtension()),
      //Create a URL from the file
      trackURL(juce::URL{ _file })
{
    DBG("Track Added: " << title);
}

bool TrackPad::operator==(const juce::String& other) const
{
    //Returns true if the track title matches the input string
    return title == other;
}

