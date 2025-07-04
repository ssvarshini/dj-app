#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <algorithm>
#include <fstream>
#include "TrackList.h"
#include "DeckGUI.h"
#include "DJAudioplayer.h"

//Class to define a custom looks for the button
class PlaylistButtonLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //Constructor: Initializes custom button colors
    PlaylistButtonLookAndFeel()
    {
        setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        //Light blue text when not pressed
        setColour(juce::TextButton::textColourOffId, juce::Colour::fromRGB(0, 240, 255));
        //Darker blue text when pressed
        setColour(juce::TextButton::textColourOnId, juce::Colour::fromRGB(0, 150, 170));
    }
    
    //Overrides the default button background rendering
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                              const juce::Colour&, bool isMouseOverButton, bool isButtonDown) override
    {
        //Get button area
        auto buttonArea = button.getLocalBounds().toFloat();
        //Light blue outline
        auto outlineColour = juce::Colour::fromRGB(0, 240, 255);
        //Slightly filled when clicked
        auto fillColour = isButtonDown ? outlineColour.withAlpha(0.1f) :
                          //Less filled when hovered
                          isMouseOverButton ? outlineColour.withAlpha(0.05f) :
                          //Transparent when idle
                          juce::Colours::transparentBlack;

        //Draw the background
        g.setColour(fillColour);
        g.fillRoundedRectangle(buttonArea, 10.0f);

        //Draw the outline
        g.setColour(outlineColour);
        g.drawRoundedRectangle(buttonArea, 10.0f, 2.0f);
    }

    //Function to override the default button text rendering
    void drawButtonText(juce::Graphics& g, juce::TextButton& button, bool, bool shouldDrawButtonAsDown) override
    {
        g.setColour(button.findColour(shouldDrawButtonAsDown ? juce::TextButton::textColourOnId
                                                             : juce::TextButton::textColourOffId));
        g.drawFittedText(button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, 1);
    }
};

//This class represents a playlist component that displays a list of tracks and allow the users to import, search, and load tracks
class PlaylistComponent : public juce::Component,
                          //Provides row-based data
                          public juce::TableListBoxModel,
                          //Handles button click events
                          public juce::Button::Listener,
                          //Handles text input events
                          public juce::TextEditor::Listener
{
public:
    //Constructor: Initializes the playlist component with references to two deck GUIs and a DJ audio player
    PlaylistComponent(DeckGUI* _deckGUI1,
                      DeckGUI* _deckGUI2,
                      DJAudioplayer* _playerForParsingMetaData);
    
    //Destructor: Cleans up resources
    ~PlaylistComponent() override;

    //Returns the number of rows in the playlist table
    int getNumRows() override;
    
    //Paints the background of each row
    void paintRowBackground(juce::Graphics& g, int rowNumber, int width, int height, bool rowIsSelected) override;
    //Paints the contents of each cell
    void paintCell(juce::Graphics& g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;
    //Updates the cell components dynamically
    juce::Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, juce::Component* existingComponentToUpdate) override;

    //Paints the components.s background and UI elements
    void paint(juce::Graphics& g) override;
    //Called when the component us resized to adjuxt layout
    void resized() override;

    //Called when a button is clicked
    void buttonClicked(juce::Button* button) override;
    //Called when the enter key is pressed in the search field
    void textEditorReturnKeyPressed(juce::TextEditor& editor) override { searchLibrary(editor.getText()); }

private:
    
    //Opens a file chooser dialog and imports selected tracks into the library
    void importToLibrary();
    
    //Loads the selected track into a specified deck GUI
    void loadInPlayer(DeckGUI* deckGUI);
    
    //Saves the current library state to a file
    void saveLibrary();
    
    //Loads the library state from a file
    void loadLibrary();
    
    //Deletes a track from the list based on its ID
    void deleteFromTracks(int id);
    
    //Checks if a track is already in the library
    bool isInTracks(const juce::String& fileNameWithoutExtension);
    
    //Finds the index of a track based on search input
    int findTracksIndex(const juce::String& searchText);
    
    //Retrieves the length of an audio file as a formatted string
    juce::String getLength(const juce::URL& audioURL);
    
    //Converts seconds into a minutes:seconds format string
    juce::String secondsToMinutes(double seconds);
    
    //Searches the library for tracks matching the given text
    void searchLibrary(const juce::String& searchText);
    
    //Stores the list of tracks
    std::vector<TrackPad> tracks;
    
    //Custom styling for buttons
    PlaylistButtonLookAndFeel playlistButtonLookAndFeel;

    //UI element for displaying the playlist
    juce::TableListBox libraryTable;
    //Search box for filtering the playlist
    juce::TextEditor searchField;
    
    //Buttons for importing, loading, and playing tracks
    juce::TextButton importButton{ "IMPORT TRACKS" };
    juce::TextButton addToPlayer1Button{ "ADD TO DECK 1" };
    juce::TextButton addToPlayer2Button{ "ADD TO DECK 2" };
    juce::TextButton playSnippetButton{ "PLAY SNIPPET" };

    //References to the two deck players for loading tracks
    DeckGUI* deckGUI1;
    DeckGUI* deckGUI2;
    DJAudioplayer* playerForParsingMetaData;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};


