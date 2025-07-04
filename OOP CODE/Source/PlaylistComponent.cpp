#include "../JuceLibraryCode/JuceHeader.h"
#include "PlaylistComponent.h"

//Constructor: Initializes the playlist component with references to two deck GUIs and a DJ audio player
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1,
                                     DeckGUI* _deckGUI2,
                                     DJAudioplayer* _playerForParsingMetaData)
      //Store reference to Deck 1
    : deckGUI1(_deckGUI1),
      //Store reference to Deck 2
      deckGUI2(_deckGUI2),
      //Store reference to the player for metadata parsing
      playerForParsingMetaData(_playerForParsingMetaData)
{
    //Set the height of each row in the table
    libraryTable.setRowHeight(30);
    //Make the table visible
    addAndMakeVisible(libraryTable);
    //Add columns to the library table
    libraryTable.getHeader().addColumn("Track title", 1, 400); //Column 1: Track title
    libraryTable.getHeader().addColumn("Length", 2, 200); //Column 2: Track length
    libraryTable.getHeader().addColumn("", 3, 100);  //Column 3: Empty
    libraryTable.setModel(this);
    loadLibrary();

    //Make the search field visible
    addAndMakeVisible(searchField);
    //Set placeholder text for the search field
    searchField.setTextToShowWhenEmpty("Search (press enter to submit)", juce::Colour::fromRGB(140, 172, 218));
    //Set a lambda function to trigger the search when the enter key is pressed
    searchField.onReturnKey = [this] { searchLibrary(searchField.getText()); };

    //Loop through all buttons and apply the same settings
    for (auto* btn : { &importButton, &addToPlayer1Button, &addToPlayer2Button, &playSnippetButton })
    {
        //Register this class as the listener for button clicks
        btn->addListener(this);
        //Applt the styling
        btn->setLookAndFeel(&playlistButtonLookAndFeel);
        //Make the button visible
        addAndMakeVisible(*btn);
    }
}

//Destructor: Cleans up resources
PlaylistComponent::~PlaylistComponent()
{
    //Remove custom look-and-feel settings before destruction to avoid dangling pointers
    for (auto* btn : { &importButton, &addToPlayer1Button, &addToPlayer2Button, &playSnippetButton })
        btn->setLookAndFeel(nullptr);
    //Save the library data
    saveLibrary();
}

//Paints the background and the elements
void PlaylistComponent::paint(juce::Graphics& g)
{
    //Fill the entire background with dark grey
    g.fillAll(juce::Colour(31, 31, 31));
    
    //Set the drawing color to white.
    g.setColour(juce::Colours::white);

    //Set the font size to 14.
    g.setFont(14.0f);

    //Draw the text "PlaylistComponent" centered in the component.
    g.drawText("PlaylistComponent", getLocalBounds(),
               juce::Justification::centred, true);
}

//Handles component resizing and positions child components accordingly
void PlaylistComponent::resized()
{
    //Get the total area of this component
    auto area = getLocalBounds();

    //Allocate 80% width for the library and 20% for the buttons.
    auto libraryArea = area.removeFromLeft(getWidth() * 0.8);
    auto buttonsArea = area.reduced(5);

    //Allocate space for the space area
    auto searchBarArea = libraryArea.removeFromTop(40);
    searchField.setBounds(searchBarArea.reduced(5));
    libraryTable.setBounds(libraryArea.reduced(5));

    //Set position for buttons
    int x = buttonsArea.getX(), y = buttonsArea.getY();
    importButton.setBounds(x + 20, y + 40, 200, 30);
    addToPlayer1Button.setBounds(x + 20, y + 83, 200, 30);
    addToPlayer2Button.setBounds(x + 20, y + 127, 200, 30);
    playSnippetButton.setBounds(x + 20, y + 170, 200, 30);
}


//Returns the total number of rows in the playlist table
int PlaylistComponent::getNumRows()
{
    //Convert the track list size to an integer
    return static_cast<int>(tracks.size());
}

//Paints the background of each row in the table
void PlaylistComponent::paintRowBackground(juce::Graphics& g,
                                           int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected)
{
    //If the row is selected, paint it orange; otherwise, use a light color
    g.fillAll(rowIsSelected ? juce::Colours::orange : juce::Colours::blanchedalmond);
}

//Paints the text inside each table cell
void PlaylistComponent::paintCell(juce::Graphics& g,
                                  int rowNumber,
                                  int columnId,
                                  int width,
                                  int height,
                                  bool /*rowIsSelected*/)
{
    //Ensure the row number is within the valid range
    if (rowNumber < getNumRows())
    {
        if (columnId == 1) //Column 1: Track Title
        {
            g.drawText(tracks[rowNumber].title, 2, 0, width - 4, height,
                       juce::Justification::centredLeft, true);
        }
        else if (columnId == 2) //Column 2: Track Length
        {
            g.drawText(tracks[rowNumber].length, 2, 0, width - 4, height,
                       juce::Justification::centred, true);
        }
    }
}

//Creates or updates a component for a specific table cell.
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
                                                            int columnId,
                                                            bool /*isRowSelected*/,
                                                            juce::Component* existingComponentToUpdate)
{
    //Check if this is the third column, which contains buttons
    if (columnId == 3)
    {
        //If there is no existing component, create a new button
        if (existingComponentToUpdate == nullptr)
        {
            auto* btn = new juce::TextButton("X"); //Create a delete button
            btn->setComponentID(juce::String(rowNumber)); //Store the row number as the component ID
            btn->addListener(this); //Make this component handle button clicks.
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

//Handles button click events and performs actions
void PlaylistComponent::buttonClicked(juce::Button* button)
{
    //Import button
    if (button == &importButton)
    {
        //Debug message
        DBG("Load button clicked");
        //Import tracks into the library
        importToLibrary();
        //Refreash the table display
        libraryTable.updateContent();
    }
    //Handle add to player 1 button
    else if (button == &addToPlayer1Button)
    {
        DBG("Add to Player 1 clicked");
        //Load the selected track into Deck 1
        loadInPlayer(deckGUI1);
    }
    //Handle add to player 2 button
    else if (button == &addToPlayer2Button)
    {
        DBG("Add to Player 2 clicked");
        //Load the selected track into Deck 2
        loadInPlayer(deckGUI2);
    }
    //Handle the play snippet button
    else if (button == &playSnippetButton)
    {
        //Get selected row index
        int selectedRow = libraryTable.getSelectedRow();
        //If track is selected, play a %-second snippet
        if (selectedRow != -1)
        {
            juce::URL audioURL = tracks[selectedRow].trackURL;
            playerForParsingMetaData->loadURL(audioURL);
            playerForParsingMetaData->start();

            //Stop playback after 5 secons
            juce::Timer::callAfterDelay(5000, [this] { playerForParsingMetaData->stop(); });
            DBG("Playing snippet of: " << tracks[selectedRow].title);
        }
        else
        {
            //Show alert if no track is selected
            juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                                              "Play Snippet",
                                              "Please select a track to play a snippet.",
                                              "OK");
        }
    }
    //Handles the delete button
    else
    {
        //Get track index form the button ID
        int id = button->getComponentID().getIntValue();
        DBG(tracks[id].title + " removed from Library");
        //Remove track from library
        deleteFromTracks(id);
        //Refresh table view
        libraryTable.updateContent();
    }
}

//Loads the selected track into the specified Deck GUI
void PlaylistComponent::loadInPlayer(DeckGUI* deckGUI)
{
    //Get selected row index
    int selectedRow = libraryTable.getSelectedRow();

    // If a track is selected, load it into the deck player
    if (selectedRow != -1)
    {
        DBG("Adding: " << tracks[selectedRow].title << " to Player");
        //Load the track into the deck
        deckGUI->loadFile(tracks[selectedRow].trackURL);
    }
    else
    {
        //Show an alert if no track is selected
        juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                                          "Add to Deck Information:",
                                          "Please select a track to add to deck",
                                          "OK");
    }
}

//Opens a file chooser dialog to select and import tracks into the playlist
void PlaylistComponent::importToLibrary()
{
    DBG("PlaylistComponent::importToLibrary called");

    //Open file dialog
    juce::FileChooser chooser("Select files");

    //If the user selects files
    if (chooser.browseForMultipleFilesToOpen())
    {
        for (const auto& file : chooser.getResults())
        {
            juce::String fileName = file.getFileNameWithoutExtension();

            //Check if the track is already in the library
            if (!isInTracks(fileName))
            {
                //Create a new track object
                TrackPad newTrack(file);
                //Get track length
                newTrack.length = getLength(juce::URL(file));
                //Add to track list
                tracks.push_back(newTrack);

                DBG("Loaded file: " << newTrack.title);
            }
            else
            {
                //Show an alert if the track is already loaded
                juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                                                  "Load information:",
                                                  fileName + " already loaded",
                                                  "OK");
            }
        }
    }
}

//Checks if a track is already in the playlist.
bool PlaylistComponent::isInTracks(const juce::String& fileNameWithoutExtension)
{
    return std::find(tracks.begin(), tracks.end(), fileNameWithoutExtension) != tracks.end();
}


//Deletes a track from the library based on its index.
void PlaylistComponent::deleteFromTracks(int id)
{
    //Ensure index is valid
    if (id >= 0 && id < static_cast<int>(tracks.size()))
        //Remove track at given index
        tracks.erase(tracks.begin() + id);
}

//Returns the length of an audio file as a formatted string
juce::String PlaylistComponent::getLength(const juce::URL& audioURL)
{
    //Load audio file into player
    playerForParsingMetaData->loadURL(audioURL);
    //Get length in seconds
    double seconds = playerForParsingMetaData->getLengthInSeconds();
    //Convert the time
    return secondsToMinutes(seconds);
}

//Converts a time in seconds to a mm:ss format string
juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    //Round seconds to the nearest integer
    int secondsRounded = static_cast<int>(std::round(seconds));
    //Calculate minutes
    juce::String minutes = std::to_string(secondsRounded / 60);
    //Get remaining seconds
    juce::String sec = std::to_string(secondsRounded % 60);

    //Ensure seconds are always displayed as two digits
    if (sec.length() < 2)
        sec = sec.paddedLeft('0', 2);

    //Return formatted time
    return minutes + ":" + sec;
}

//Searches for a track in the library and selects the first matching row
void PlaylistComponent::searchLibrary(const juce::String& text)
{
    DBG("Searching library for: " << text);
    //If there is a search term
    if (text.isNotEmpty())
    {
        //Find track index
        int rowNumber = findTracksIndex(text);
        //Select row if found
        libraryTable.selectRow(rowNumber);
    }
    else
    {
        //Deselect all if search is empty
        libraryTable.deselectAllRows();
    }
}

//Finds the index of a track that matches the search text
int PlaylistComponent::findTracksIndex(const juce::String& searchText)
{
    auto it = std::find_if(tracks.begin(), tracks.end(),
                           [&searchText](const TrackPad& track) { return track.title.contains(searchText); });
    
    return (it != tracks.end()) ? static_cast<int>(std::distance(tracks.begin(), it)) : -1;
}

//Saves the current playlist to a CSV file
void PlaylistComponent::saveLibrary()
{
    //Open file for writing
    std::ofstream myLibrary("MusicLibrary.csv");

    for (const auto& t : tracks)
        //Write each track to file
        myLibrary << t.file.getFullPathName() << "," << t.length << "\n";
}

//Loads tracks from a previously saved CSV file into the library
void PlaylistComponent::loadLibrary()
{
    //Open file for reading
    std::ifstream myLibrary("MusicLibrary.csv");
    std::string filePath, length;

    //Ensure the file is open
    if (myLibrary.is_open())
    {
        // Read file path
        while (std::getline(myLibrary, filePath, ','))
        {
            juce::File file(filePath);
            TrackPad newTrack(file);

            //Read track length
            std::getline(myLibrary, length);
            newTrack.length = length;
            //Add track to library
            tracks.push_back(newTrack);
        }
    }
    //Close the file after reading
    myLibrary.close();
}





