#include "MainComponent.h"
#include <cmath>

MainComponent::MainComponent() : deckGUI1(&player1, formatManager, thumbCache, true), //Initialise deck 1
deckGUI2(&player2, formatManager, thumbCache, false), //Initialise deck 2
playlistComponent(&deckGUI1, &deckGUI2, &player1) //Initialise playlist component
{
    //Canvas size
    setSize (1000, 800);

    //Request audio input permissions if needed
    if (RuntimePermissions::isRequired (RuntimePermissions::recordAudio)
        && ! RuntimePermissions::isGranted (RuntimePermissions::recordAudio))
    {
        RuntimePermissions::request (RuntimePermissions::recordAudio,
                                     [&] (bool granted) { if (granted)  setAudioChannels (2, 2); });
    }  
    else
    {
        //Open 2 output channels
        setAudioChannels (0, 2);
    }
    
    //Add components to the UI
    addAndMakeVisible(deckGUI1);
    addAndMakeVisible(deckGUI2);
    addAndMakeVisible(playlistComponent);
    //Register audio formats
    formatManager.registerBasicFormats();
    
    //Configure crossfader slider
    crossFaderSlider.setLookAndFeel(&crossFaderLookAndFeel);
    crossFaderSlider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    crossFaderSlider.setRange(0.0, 1.0); //Range from 0 to 1
    crossFaderSlider.setValue(0.5);
    crossFaderSlider.addListener(this);
    //Crossfader label
    addAndMakeVisible(crossFaderSlider);crossFaderLabel.attachToComponent(&crossFaderSlider, false);
    crossFaderLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(crossFaderLabel);

    //Set different colors for each waveform
    deckGUI1.setWaveformColour(juce::Colour(97, 132, 216));
    deckGUI2.setWaveformColour(juce::Colour(80, 162, 167));
}

MainComponent::~MainComponent()
{
    //This shuts down the audio device and clears the audio source
    shutdownAudio();
    crossFaderSlider.setLookAndFeel(nullptr);//Reset LookAndFeel
}

//Prepares the audio systm to play with given sample rate and block size
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    //Adds both player to mixer
    mixerSource.addInputSource(&player1, false);
    mixerSource.addInputSource(&player2, false);
}

//Gets the next block of audio and mixes it for playback
void MainComponent::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    mixerSource.getNextAudioBlock(bufferToFill);
}

//This will be called when the audio device stops, or when it is being
void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
    mixerSource.releaseResources();
}

//Draws the background
void MainComponent::paint (Graphics& g)
{
    //Background gradient effect
    Colour startColour = Colour(50, 50, 50);
    Colour endColour   = Colour(31, 31, 31);
    ColourGradient gradient(startColour, 0, 0, endColour, 0, getHeight(), false);
    g.setGradientFill(gradient);
    g.fillRect(getLocalBounds());
}

//Positions the layout of all the elements
void MainComponent::resized()
{
    //Variables for the height
    int totalHeight = getHeight();
    int deckHeight = totalHeight * 0.8;
    int playlistHeight = 210;
    int playlistOffset = -35;
    int width = getWidth();

    //Positions for the deck
    deckGUI1.setBounds(0, 0, getWidth() / 2, deckHeight);
    deckGUI2.setBounds(getWidth() / 2, 0, getWidth() / 2, deckHeight);

    //Set PlaylistComponent below the decks
    playlistComponent.setBounds(0, deckHeight + playlistOffset, getWidth(), playlistHeight);
    
    //Varianles for crossfader positon
    int sliderWidth = width / 3;
    int sliderHeight = 50;
    crossFaderSlider.setBounds(480, 670, sliderWidth, sliderHeight);
    crossFaderSlider.setBounds(480, 640, sliderWidth, sliderHeight);
    crossFaderLabel.setBounds(480, 620, sliderWidth, 20);
}

//Function to impleament equal-power crossfading
void MainComponent::sliderValueChanged (Slider* slider)
{
    if (slider == &crossFaderSlider)
    {
        //Get the current crossfade value (0: full player1, 1: full player2)
        double crossValue = crossFaderSlider.getValue();
        //Compute gains using equal-power crossfade:
    
        double leftGain  = std::sin(crossValue * (juce::MathConstants<double>::pi / 2));
        double rightGain = std::cos(crossValue * (juce::MathConstants<double>::pi / 2));

        //Set the gains for each player
        player1.setGain(leftGain);
        player2.setGain(rightGain);
        
        std::cout << "Crossfader updated: Left gain = " << leftGain
                  << ", Right gain = " << rightGain << std::endl;
    }
}
