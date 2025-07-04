#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//DJAudioplayer class that handles audio playback, effects, and control
class DJAudioplayer : public AudioSource{
    
public:
    //Constructor: Takes an AudioFormatManager reference for handling file formats
    DJAudioplayer(AudioFormatManager& _formatManager);
    //Destructor: Cleans up resources
    ~DJAudioplayer();
    
    //Prepares the audio player for playback
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    //Fills the buffer with audio data
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    //Releases resources when playback stops
    void releaseResources() override;
    
    //Load an audio file from a given URL
    void loadURL(URL audioURL);
    
    //Audio control functions
    //Set the volume level
    void setGain(double gain);
    //Adjust playback speed
    void setSpeed(double ratio);
    //Set playback position in seconds
    void setPosition(double posInsecs);
    //Set playback position relative to track length
    void setPositionRelative(double pos);
    
    //Start playback
    void start();
    //Stop playback
    void stop();
    
    //Get the current position relative to track length
    double getPositionRelative();
    
    //void jogScrub(double jogAmount);
    //Temporarily change pitch for pitch bending effect
    void pitchBend(double jogAmount);
    
    
    //Sets the wet/dry mix ratio for the reverb effect
    void setWetDry(double ratio);
    //Adjusts the treble EQ gain
    void setTrebleGain(double newTrebleGain);
    //setBass: Adjusts the bass effect
    void setBass(double newBass);
    //Adjust the mid effect
    void setMid(double midGain);

    //Get the total length of the track in seconds
    double getLengthInSeconds() const;

    //Enable or disable track looping
    void setLooping(bool shouldLoop);
    bool isLooping() const;            

private:
    AudioFormatManager& formatManager;
    //Pointer to an audio file reader source
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    //Handles audio playback transport
    AudioTransportSource transportSource;
    //Handles speed adjustments
    ResamplingAudioSource resampleSource{&transportSource, false, 2};
    
    //Stores the last used sample rate
    double lastSampleRate;
    
    //Indicates whether looping is enabled
    bool looping = false;
    
    //Wet/dry mix ratio for reverb
    double wetDry { 0.0 };
    // Reverb processor
    juce::Reverb reverb;
    //Treble gain value
    double trebleGain { 0.0 };
    //Bass slider value
    double bassValue { 0.0 };
    float midGainFactor = 1.0f;
    
    // EQ filter components for left and right channels
    juce::IIRFilter highShelfFilterLeft;
    juce::IIRFilter highShelfFilterRight;
    juce::IIRFilter bassFilterLeft;
    juce::IIRFilter bassFilterRight;
    juce::IIRFilter midFilterLeft;
    juce::IIRFilter midFilterRight;
};
