#include <JuceHeader.h>
#include "DeckGUI.h"

DeckGUI::DeckGUI(DJAudioplayer* _player,
                 AudioFormatManager & formatManagerToUse,
                 AudioThumbnailCache & cacheToUse, bool isLeftDeck) :
                 player(_player),  //Store a reference to the associated DJAudioplayer
                 waveformDisplay(formatManagerToUse, cacheToUse), //Initialize the waveform display
                 isLeftDeck(isLeftDeck) //Determine if this deck is the left or right deck
{
    
    //BUTTONS//
    //Add and make buttons visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loopButton);
    addAndMakeVisible(forwardButton);
    addAndMakeVisible(backwardButton);
    
    //Add listeners for the button events
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    loopButton.addListener(this);
    forwardButton.addListener(this);
    backwardButton.addListener(this);
    
    //Apply LookAndFeel to Play and Stop buttons
    playButton.setLookAndFeel(&buttonLookAndFeel);
    stopButton.setLookAndFeel(&buttonLookAndFeel);
    loopButton.setLookAndFeel(&buttonLookAndFeel);
    forwardButton.setLookAndFeel(&buttonLookAndFeel);
    backwardButton.setLookAndFeel(&buttonLookAndFeel);
    loadButton.setLookAndFeel(&buttonLookAndFeel);
    
    //WAVEFORM//
    addAndMakeVisible(waveformDisplay);
    
    //SLIDER//
    //Configure speed slider
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);  // Changed from LinearVertical
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setRange(0.5, 2.0, 0.01); // Speed range (0.5x to 2x)
    speedSlider.setSkewFactorFromMidPoint(1.0);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);
    speedSlider.setLookAndFeel(&slidersLookAndFeel);

    //Configure volume Slider
    volSlider.setSliderStyle(juce::Slider::LinearVertical);
    volSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volSlider.setRange(0.0, 1.0);
    volSlider.setValue(1.0);
    volSlider.addListener(this);
    addAndMakeVisible(volSlider);
    volSlider.setLookAndFeel(&slidersLookAndFeel);

    //Configure position Slider
    posSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    posSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    posSlider.setRange(0.0, 1.0);
    posSlider.setValue(0.0);
    posSlider.addListener(this);
    addAndMakeVisible(posSlider);
    posSlider.setLookAndFeel(&slidersLookAndFeel);
    
    //Set colors for sliders based on their deckx
    if (isLeftDeck) {
    volSlider.setColour(juce::Slider::thumbColourId, juce::Colour(97, 132, 216));
    volSlider.setColour(juce::Slider::trackColourId, juce::Colour(121, 148, 209));

    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colour(97, 132, 216));
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colour(121, 148, 209));

    posSlider.setColour(juce::Slider::thumbColourId, juce::Colour(97, 132, 216));
    posSlider.setColour(juce::Slider::trackColourId, juce::Colour(121, 148, 209));
    } else {
    volSlider.setColour(juce::Slider::thumbColourId, juce::Colour(80, 162, 167));
    volSlider.setColour(juce::Slider::trackColourId, juce::Colour(122, 199, 190));

    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colour(80, 162, 167));
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colour(122, 199, 190));

    posSlider.setColour(juce::Slider::thumbColourId, juce::Colour(80, 162, 167));
    posSlider.setColour(juce::Slider::trackColourId, juce::Colour(122, 199, 190));
    }

    //JOGWHEEL//
    jogWheel.setLookAndFeel(&jogLookAndFeel);
    //Enable velocity-based mode for smooth interaction
    jogWheel.setVelocityBasedMode(true);
    //Set jog wheel rotation parameters
    jogWheel.setRotaryParameters(0.0f, juce::MathConstants<float>::twoPi, true);
    //Configure jog wheel settings
    jogWheel.setSliderStyle(Slider::Rotary);
    jogWheel.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    jogWheel.setRange(-1.0, 1.0, 0.01);
    jogWheel.setValue(0.0);
    jogWheel.addListener(this);
    addAndMakeVisible(jogWheel);

    //Audio Effects Control//
    //Configure bass Slider
    addAndMakeVisible(bassSlider);
    bassSlider.addListener(this);
    bassSlider.setRange(-1.0, 1.0);
    bassSlider.setSliderStyle(juce::Slider::Rotary);
    bassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bassSlider.setLookAndFeel(&eqKnobLookAndFeel);
    
    //Configure trebl Slider
    addAndMakeVisible(highSlider);
    highSlider.addListener(this);
    highSlider.setRange(-1.0, 1.0);
    highSlider.setSliderStyle(juce::Slider::Rotary);
    highSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highSlider.setLookAndFeel(&eqKnobLookAndFeel);
    
    //Configure wetDry Slider
    addAndMakeVisible(wetDrySlider);
    wetDrySlider.addListener(this);
    wetDrySlider.setRange(0.0, 1.0);
    wetDrySlider.setSliderStyle(juce::Slider::Rotary);
    wetDrySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    wetDrySlider.setLookAndFeel(&eqKnobLookAndFeel);
    
    //Configure mid Slider
    addAndMakeVisible(midSlider);
    midSlider.setRange(-1.0, 1.0, 0.01);  // Mid EQ range
    midSlider.setSliderStyle(juce::Slider::Rotary);
    midSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    midSlider.setLookAndFeel(&eqKnobLookAndFeel);
    midSlider.addListener(this);

    //Volume label
    volLabel.attachToComponent(&volSlider, false);
    volLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(volLabel);

    //Speed label
    speedLabel.attachToComponent(&speedSlider, false);
    speedLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(speedLabel);

    //Position label
    posLabel.attachToComponent(&posSlider, false);
    posLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(speedLabel);

    //Forward and Backward buttons labels
    forwardLabel.attachToComponent(&forwardButton, false);
    forwardLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(forwardLabel);
    backwardLabel.attachToComponent(&backwardButton, false);
    backwardLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(backwardLabel);
    
    //Sound Effects labels
    // Attach the labels to their respective components (sliders)
    MidLabel.attachToComponent(&midSlider, false);
    MidLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(MidLabel);

    wetdryLabel.attachToComponent(&wetDrySlider, false);
    wetdryLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(wetdryLabel);

    bassLabel.attachToComponent(&bassSlider, false);
    bassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassLabel);

    trebleLabel.attachToComponent(&highSlider, false);
    trebleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(trebleLabel);

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    bassSlider.setLookAndFeel(nullptr);
    highSlider.setLookAndFeel(nullptr);
    wetDrySlider.setLookAndFeel(nullptr);
    stopTimer();
    
}

void DeckGUI::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("DeckGUI", getLocalBounds(),
                juce::Justification::centred, true);
    
}

void DeckGUI::resized()
{
    //Sets the variables to use for setBounds
    //Height of each element
    double rowH = getHeight() / 10;
    //Variables for sliders
    int sliderWidth = 40;
    int verticalSliderHeight = 350;
    int horizontalSliderWidth = 300;
    //Variable for button
    double buttonSize = getWidth() * 0.12;
    //Variable for EQ
    int filterSliderWidth = 60;
    int filterSliderHeight = 60;

    //Position the elements in specific places depending on wheather they are on the right pr left deck
    if (isLeftDeck) {
        //Position the elements in left deck
        //Position for slider
        volSlider.setBounds(30, 250, sliderWidth, verticalSliderHeight);
        speedSlider.setBounds(550, 250, sliderWidth, verticalSliderHeight);
        posSlider.setBounds(147, 160, horizontalSliderWidth, 40);
        
        //Position for jogwheel
        jogWheel.setBounds(50, 220, getWidth() * 0.7, rowH * 5);
        
        //Position for buttons
        playButton.setBounds(115, 550, buttonSize, buttonSize);
        stopButton.setBounds(215, 550, buttonSize, buttonSize);
        loopButton.setBounds(315, 550, buttonSize, buttonSize);
        loadButton.setBounds(415, 550, buttonSize, buttonSize);
        backwardButton.setBounds(110, 200, buttonSize - 20, buttonSize - 20);
        forwardButton.setBounds(410, 200, buttonSize - 20, buttonSize - 20);
        
        //Positions for EQ
        midSlider.setBounds(620, 320, filterSliderWidth, filterSliderHeight);
        wetDrySlider.setBounds(620, 550, filterSliderWidth, filterSliderHeight);
        bassSlider.setBounds(620, 205, filterSliderWidth, filterSliderHeight);
        highSlider.setBounds(620, 435, filterSliderWidth, filterSliderHeight);
    }else {
        //Position the elements in right deck
        //Position for slider
        volSlider.setBounds(110, 250, sliderWidth, verticalSliderHeight);
        speedSlider.setBounds(630, 250, sliderWidth, verticalSliderHeight);
        posSlider.setBounds(227, 160, horizontalSliderWidth, 40);
        
        //Position for jogwheel
        jogWheel.setBounds(130, 220, getWidth() * 0.7, rowH * 5);
        
        //Position for buttons
        playButton.setBounds(195, 550, buttonSize, buttonSize);
        stopButton.setBounds(295, 550, buttonSize, buttonSize);
        loopButton.setBounds(395, 550, buttonSize, buttonSize);
        loadButton.setBounds(495, 550, buttonSize, buttonSize);
        backwardButton.setBounds(190, 200, buttonSize - 20, buttonSize - 20);
        forwardButton.setBounds(490, 200, buttonSize - 20, buttonSize - 20);
        
        //Positions for audio effects control
        midSlider.setBounds(20, 320, filterSliderWidth, filterSliderHeight);
        wetDrySlider.setBounds(20, 550, filterSliderWidth, filterSliderHeight);
        bassSlider.setBounds(20, 205, filterSliderWidth, filterSliderHeight);
        highSlider.setBounds(20, 435, filterSliderWidth, filterSliderHeight);
        
    }
   
    //Position the waveform Display in the center
    waveformDisplay.setBounds(0, 0, getWidth(), rowH * 2 - 20);
}

//Function for the button usage
void DeckGUI::buttonClicked(Button* button)
{
    //Music plays when button is clicked
    if (button == &playButton)
    {
        std::cout << "Play button was clicked " << std::endl;
        //Start playback
        player->start();
    }
    //Music is stopped when button is clicked
     if (button == &stopButton)
    {
        std::cout << "Stop button was clicked " << std::endl;
        //Stops playback
        player->stop();

    }
    //Music is loaded when button is clicked
    if (button == &loadButton)
    {
        FileChooser chooser ("Select a Wave file to play...");
        //Open file broswer
        if (chooser.browseForFileToOpen())
        {
            URL audioURL = URL{chooser.getResult()};
            //Load selected file into the player
            player->loadURL(audioURL);
            //Update waveform display
            waveformDisplay.loadURL(URL{chooser.getResult()});
        }
    }
    //Music is looped when button is clicked
    if (button == &loopButton) {
        //Toggle loop state
        isLoopEnabled = !isLoopEnabled;
        //Apply looping state to player
        player->setLooping(isLoopEnabled);
        //Update button appearance
        loopButton.repaint();
        std::cout << (isLoopEnabled ? "Loop enabled" : "Loop disabled") << std::endl;
    }
    //Music moves by 5 seconds forward when button is pressed
    if (button == &forwardButton) {
        double newPosition = player->getPositionRelative() + 0.05;
        //Keep within valid range
        player->setPositionRelative(jlimit(0.0, 1.0, newPosition));
    }
    //Music moves by 5 seconds backward when button is pressed
    if (button == &backwardButton) {
        double newPosition = player->getPositionRelative() - 0.05;
        //Keep within valid range
        player->setPositionRelative(jlimit(0.0, 1.0, newPosition));
    }
}

//Function to the slider value changes
void DeckGUI::sliderValueChanged (Slider *slider)
{
    //Handling the jog wheel slider for pitch bending
    if (slider == &jogWheel)
    {
        double jogAmount = jogWheel.getValue();
        if (jogAmount != 0)
        {
            //Apply pitch bending when jog wheel is moved
            player->pitchBend(jogAmount);
        }
    }
    //Handling the volume slider
    if (slider == &volSlider)
    {
        //Set the player volume
        player->setGain(slider->getValue());
        std::cout << "Volume changed: " << volSlider.getValue() << std::endl;
    }
    //Handling the speed slider
    if (slider == &speedSlider) {
        double speedValue = speedSlider.getValue();
        std::cout << "Speed Slider Changed: " << speedValue << std::endl;
        //Set playback speed
        player->setSpeed(speedValue);
    }
    //Handling the position slider
    if (slider == &posSlider)
    {
        //Set track position
        player->setPositionRelative(slider->getValue());
        std::cout << "Position changed: " << posSlider.getValue() << std::endl;
    }
    //Handling the audio effects sliders
    if (slider == &wetDrySlider)
        //Adjust the wet/dry mix of the audio effect
        player->setWetDry(slider->getValue());
    else if (slider == &highSlider)
        //Adjust the treble
        player->setTrebleGain(slider->getValue());
    else if (slider == &bassSlider)
        //Adjust the bass
        player->setBass(slider->getValue());
    else if (slider == &midSlider)
        //Adjust the mid
        player->setMid(slider->getValue());
}

//Function to check if the DeckGUI is interested in dragged files
bool DeckGUI::isInterestedInFileDrag (const StringArray &files){
    std::cout << "DeckGUI::isInterestedInFileDrag"<<std::endl;
    return true;
}

//Function to handle file drop event
void DeckGUI::filesDropped (const StringArray &files, int x, int y){
    std::cout << "DeckGUI::fileDropped"<<std::endl;
    //Load the file if only one file is dropped
    if(files.size() == 1)
    {
        //Load the audio file into the player
        player->loadURL(URL{File{files[0]}});
    }
}

//Function called periodically as part of a timer callback
void DeckGUI::timerCallback(){
    //Update the waveform position
    waveformDisplay.setPositionRelative(player->getPositionRelative());
    
    // Update the position slider to move with the track
    posSlider.setValue(player->getPositionRelative(), juce::dontSendNotification);
    
    //Apply jog wheel pitch bending if it's not at zero
    if (jogWheel.getValue() != 0) {
        player->pitchBend(jogWheel.getValue());
    }
}
 
//Function to load a new audio file into the player
void DeckGUI::loadFile(const juce::URL& audioURL) {
    //Load the audio  into the pkayer
    player->loadURL(audioURL);
    //Update waveform display
    waveformDisplay.loadURL(audioURL);
}

