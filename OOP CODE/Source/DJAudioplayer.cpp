#include "DJAudioplayer.h"

//Constructor: Initializes the audio player with the format manager
DJAudioplayer::DJAudioplayer(AudioFormatManager& _formatManager)
: formatManager(_formatManager)
{
   
}

//Destructor: Cleans up any allocated resources
DJAudioplayer::~DJAudioplayer() {
    
}

//Prepsared the audio player to start playing
void DJAudioplayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    lastSampleRate = sampleRate;
    
    setTrebleGain(trebleGain);
    setBass(bassValue);

    // Set up default reverb parameters
    juce::Reverb::Parameters params;
    params.roomSize   = 0.5f;
    params.damping    = 0.5f;
    params.wetLevel   = 1.0f;
    params.dryLevel   = 0.0f;
    params.width      = 1.0f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);   
}

//Function to retrieves the next block of audio data, applies reverb and EQ effects
void DJAudioplayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    //Ensure looping works correctly: if track is at the end, restart from the beginning
    if (looping)
    {
        double trackLength = transportSource.getLengthInSeconds();
        if (trackLength > 0 && transportSource.getCurrentPosition() >= trackLength - 0.1)
        {
            //Restart track
            transportSource.setPosition(0.0);
            //Start playing from the beginning
            transportSource.start();
        }
    }

    //Get the next block of audio from the resample source
    resampleSource.getNextAudioBlock(bufferToFill);

    //Apply reverb if wet/dry mix ratio is greater than 0 (reverb effect is active)
    if (wetDry > 0.0)
    {
        juce::AudioBuffer<float> wetBuffer;
        //Copy current audio buffer to apply reverb
        wetBuffer.makeCopyOf(*bufferToFill.buffer);

        //Process reverb
        if (wetBuffer.getNumChannels() >= 2)
            reverb.processStereo(wetBuffer.getWritePointer(0), wetBuffer.getWritePointer(1), bufferToFill.numSamples);
        else
            reverb.processMono(wetBuffer.getWritePointer(0), bufferToFill.numSamples);

        //Mix the wet and dry signals based on the wetDry ratio
        for (int channel = 0; channel < wetBuffer.getNumChannels(); ++channel)
        {
            float* dryData = bufferToFill.buffer->getWritePointer(channel);
            const float* wetData = wetBuffer.getReadPointer(channel);
            for (int i = 0; i < bufferToFill.numSamples; ++i)
                dryData[i] = dryData[i] * (1.0f - wetDry) + wetData[i] * wetDry;
        }
    }

    //Apply bass, mid, and treble EQ filters to each channel in the buffer
    int numChannels = bufferToFill.buffer->getNumChannels();
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = bufferToFill.buffer->getWritePointer(channel);

        //Apply bass filter
        if (channel == 0)
            bassFilterLeft.processSamples(channelData, bufferToFill.numSamples);
        else
            bassFilterRight.processSamples(channelData, bufferToFill.numSamples);

        //Apply mid EQ filter
        if (channel == 0)
            midFilterLeft.processSamples(channelData, bufferToFill.numSamples);
        else
            midFilterRight.processSamples(channelData, bufferToFill.numSamples);

        //Apply the manually adjusted mid EQ gain
        for (int i = 0; i < bufferToFill.numSamples; ++i)
            channelData[i] *= midGainFactor;

        //Apply treble filter
        if (channel == 0)
            highShelfFilterLeft.processSamples(channelData, bufferToFill.numSamples);
        else
            highShelfFilterRight.processSamples(channelData, bufferToFill.numSamples);
    }
}

//Function to release all audio resources when playback stops
void DJAudioplayer::releaseResources() {
    
    transportSource.releaseResources();
    resampleSource.releaseResources();
}
 
//Function to load an audio file from a URL into the player
void DJAudioplayer::loadURL(URL audioURL) {
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) {
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true));
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset (newSource.release());
    }
}

//Function to set the audio playback volume
void DJAudioplayer::setGain(double gain) {
    //Ensure the gain value is within the valid range
    if (gain < 0 || gain > 1.0)
    {
        //Print an error message if the gain is out of range
        std::cout << "DJAudioplayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else {
        //Set the gain for the audio transport source
        transportSource.setGain(gain);
    }
}

//Function to adjust playback speed (resampling)
void DJAudioplayer::setSpeed(double ratio) {
    //Ensure the speed ratio is within 0.5 to 2
    if (ratio < 0.5 || ratio > 2.0) {
        //Print an error message if the speed is out of range
        std::cout << "Speed must be between 0.5x and 2.0x" << std::endl;
    } else {
        //Set the resampling ratio to adjust playback speed
        resampleSource.setResamplingRatio(ratio);
        //Print the newly set speed ratio
        std::cout << "Speed set to: " << ratio << "x" << std::endl;
    }
}

//Function to set playback position in seconds
void DJAudioplayer::setPosition(double posInsecs){
    transportSource.setPosition(posInsecs);
}

//Function to set playback position relative to track length
void DJAudioplayer::setPositionRelative(double pos){
    //Ensure the position is within 0 to 1
    if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioplayer::setGain pos should be between 0 and 1" << std::endl;
    }
    else {
        std::cout << "Setting Position: " << pos << " (Seconds: " << transportSource.getLengthInSeconds() * pos << ")" << std::endl;
        //Calculate the actual position in seconds based on track length
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        //Set the playback position using the time
        setPosition(posInSecs);
    }
}

//Function to start audio playback
void DJAudioplayer::start() {
    transportSource.start();
}

//Function to stop audio playback
void DJAudioplayer::stop() {
    transportSource.stop();
}

//Function to get the current playback position relative to track length
double DJAudioplayer::getPositionRelative() {
    //Get the total length of the track in seconds
    double length = transportSource.getLengthInSeconds();
    //Calculate the relative position as a fraction of the totaol length
    if (length > 0) {
        double pos = transportSource.getCurrentPosition() / length;
        //Ensure the returned value is clamped between 0 and 1
        return jlimit(0.0, 1.0, pos);
    }
    //Return 0 if no track is loaded
    return 0.0;
}

//Function to apply pitch bending using the jog wheel
void DJAudioplayer::pitchBend(double jogAmount)
{
    //Adjusting speed dynamically based on jog wheel movement
    double newSpeed = 1.0 + (jogAmount * 0.5);
    //Ensure speed stays within the allowed range
    resampleSource.setResamplingRatio(jlimit(0.5, 2.0, newSpeed));
}

//Function for the wet/dry mix ratio for the reverb effect
void DJAudioplayer::setWetDry(double ratio)
{
    //If ratio is out of valid range or hasn't changed, return early to avoid redundant updates
    if (ratio < 0.0 || ratio > 1.0 || ratio == wetDry)
        return;

    //Update the wet/dry mix ratio for reverb effect
    wetDry = ratio;
}

//Function to adjusts the treble EQ gain and updates the high shelf filters
void DJAudioplayer::setTrebleGain(double newTrebleGain)
{
    //If the value hasn't changed, return to avoid unnecessary recalculations
    if (newTrebleGain == trebleGain) return;

    //Convert slider value to decibels and then to a gain factor
    trebleGain = newTrebleGain;
    double dB = trebleGain * 12.0;
    double gainFactor = std::pow(10.0, dB / 20.0);

    //Update high-shelf filter coefficients for treble adjustment
    auto coeffs = juce::IIRCoefficients::makeHighShelf(lastSampleRate, 3000.0, 0.707, gainFactor);
    highShelfFilterLeft.setCoefficients(coeffs);
    highShelfFilterRight.setCoefficients(coeffs);
}

//Function to adjust the bass effect and updates the low pass filters
void DJAudioplayer::setBass(double newBass)
{
    //If the bass value hasn't changed, avoid recalculating
    if (newBass == bassValue) return;

    //Map slider value to cutoff frequency for bass filter
    bassValue = newBass;
    double cutoffFrequency = 200.0 + ((bassValue + 1.0) / 2.0) * (19800.0);

    //Set the low-pass filter coefficients based on the cutoff frequency
    auto coeffs = juce::IIRCoefficients::makeLowPass(lastSampleRate, cutoffFrequency);
    bassFilterLeft.setCoefficients(coeffs);
    bassFilterRight.setCoefficients(coeffs);
}

//Function to adjusts the mid EQ gain updates the band-pass filters for left and right channels.
void DJAudioplayer::setMid(double midGain)
{
    //If the mid EQ gain hasn't changed, return early
    if (midGain == midGainFactor) return;

    //Convert mid gain from slider value to dB and then to linear gain factor
    midGainFactor = static_cast<float>(std::pow(10.0, (midGain * 12.0) / 20.0));

    // Set bandpass filter for mid frequencies (~1000 Hz) and apply the gain
    auto coeffs = juce::IIRCoefficients::makeBandPass(lastSampleRate, 1000.0, 0.707);
    midFilterLeft.setCoefficients(coeffs);
    midFilterRight.setCoefficients(coeffs);
}

//Function to get the total length of the track in seconds
double DJAudioplayer::getLengthInSeconds() const {
    //Return the total duration of the track
    return transportSource.getLengthInSeconds();
}

//Function to enable and diaable looping
void DJAudioplayer::setLooping(bool shouldLoop) {
    //Set the looping state based on the boolean
    looping = shouldLoop;
    std::cout << "Looping is now: " << (looping ? "Enabled" : "Disabled") << std::endl;
}

//Function to check if looping is enabled
bool DJAudioplayer::isLooping() const {
    //Return the current looping state
    return looping;
}
