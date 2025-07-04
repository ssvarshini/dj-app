#pragma once

#include <JuceHeader.h>
#include "DJAudioplayer.h"
#include "WaveformDisplay.h"

//==============================================================================
/*
*/
#pragma once
#include <JuceHeader.h>



class EQKnobLookAndFeel : public juce::LookAndFeel_V4 {
public:
    //Function to draw the rotary slider (knob)
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider&) override {
        
        //Dynamically calculate the knob's radius based on the actual size of the slider
            int radius = juce::jmin(width, height) / 2; 
            int centerX = x + width / 2;
            int centerY = y + height / 2;
        //Calculate the rotation angle based on the slider position
        const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        
        //Draw the outer dark grey circle
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(centerX - radius, centerY - radius, radius * 2.0, radius * 2.0);
        
        //Draw the inner lighter grey circle
        g.setColour(juce::Colour(148, 148, 148));
        g.fillEllipse(centerX - radius * 0.9, centerY - radius * 0.9, radius * 1.8, radius * 1.8);

        //Create the rectangle pointer
        juce::Path pointer;
        pointer.addRectangle(-1.0, -radius, 2.0, radius * 0.9);
        //Set the pointer color (cyan/turquoise)
        g.setColour(Colour(0, 240, 255));
        //Rotate the pointer based on the calculated angle and move it to the center of the knob
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    }
};

class ButtonLookAndFeel : public LookAndFeel_V4
{
public:
    //Function that draws all the buttons
    void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
                              bool isMouseOverButton, bool isButtonDown) override
    {
        //Get the button bounds as a floating rectangle
        auto bounds = button.getLocalBounds().toFloat();
        //Define border thickness and button radius
        int borderThickness = 3;
        float radius = 60.0f;
        //Get the center position of the button
        auto centerX = bounds.getCentreX();
        auto centerY = bounds.getCentreY();

        //Default fill color (dark grey)
        Colour fillColour = Colour(31, 31, 31);
        //Change brightness based on user interaction
        if (isMouseOverButton) fillColour = fillColour.brighter();
        if (isButtonDown) fillColour = fillColour.darker();
        g.setColour(fillColour);
        g.fillEllipse(centerX - radius / 2, centerY - radius / 2, radius, radius);

        //Draw the border outline in cyan
        g.setColour(Colour(0, 240, 255));
        g.drawEllipse(centerX - radius / 2, centerY - radius / 2, radius, radius, borderThickness);

        //Create a path for drawing the icon
        Path icon;
        g.setColour(Colour(0, 240, 255));

        //Drawing different icons based on button text
        if (button.getButtonText() == "PLAY")
        {
            //Draws a play icon
            icon.addTriangle(centerX - 5, centerY - 10,
                             centerX - 5, centerY + 10,
                             centerX + 10, centerY);
        }
        else if (button.getButtonText() == "STOP")
        {
            //Draws a stop icon
            icon.addRectangle(centerX - 8, centerY - 8, 16, 16);
        }
        else if (button.getButtonText() == "LOOP")
        {
            //Draws the loop symbol
            icon.addRectangle(centerX - 13, centerY - 14, 20, 8);

           
            icon.addTriangle(centerX + 7, centerY - 20,
                             centerX + 7, centerY,
                             centerX + 17, centerY - 10);
            icon.addRectangle(centerX - 13, centerY - 6, 8, 4);

            icon.addRectangle(centerX - 6, centerY+6, 20, 8);
            icon.addTriangle(centerX - 16, centerY + 10,
                             centerX - 6, centerY,
                             centerX - 6, centerY + 20);
            icon.addRectangle(centerX + 6, centerY + 2, 8, 4);
        }
        else if (button.getButtonText() == "FORWARD") {
            //Draws the forward button
            icon.addTriangle(centerX, centerY - 10,
                             centerX, centerY + 10,
                             centerX + 15, centerY);
            icon.addTriangle(centerX - 13, centerY - 10,
                             centerX - 13, centerY + 10,
                             centerX + 2, centerY);
        }
        else if (button.getButtonText() == "BACKWARD") {
            //Draws the backwardbutton
            icon.addTriangle(centerX + 11, centerY - 10,
                             centerX + 11, centerY + 10,
                             centerX - 4, centerY);
            icon.addTriangle(centerX - 3, centerY - 10,
                             centerX - 3, centerY + 10,
                             centerX - 18, centerY);
        }
        else if (button.getButtonText() == "LOAD")
        {
            //Draws th load button
            g.setColour(Colour(0, 240, 255));
            g.setFont(16.0f);
            g.drawFittedText("LOAD", bounds.toNearestInt(), juce::Justification::centred, 1);
        }
        //Fills the icon with colour
        g.fillPath(icon);
    }

    void drawButtonText(Graphics& /*g*/, TextButton& /*button*/, bool /*isMouseOverButton*/, bool /*isButtonDown*/) override
    {
       
    }
};

class JogWheelLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //Function that draws the rotary-style jogwheel
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override
    {
        //Calculate the rotation angle based on the slider's position
        double angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        //Set the overall scale of the jog wheel
        double dialScale = 0.75;
        //Set the diameter and radius of the jog wheel
        double diameter = juce::jmin(width, height) * dialScale;
        double radius = diameter / 2.0;
        //Set the center position of the jog wheel
        double centerX = x + width / 2.0;
        double centerY = y + height / 2.0;
        
        //Applying translation transformation to align drawing
        g.addTransform(juce::AffineTransform::translation((float)x, (float)y));
        g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
        
        //Draws the outer glowing cyan border of the jog wheel
        g.setColour(juce::Colour(0, 240, 255));
        double borderThickness = 4.0;
        g.drawEllipse(centerX - radius - 3, centerY - radius - 3, diameter + 6, diameter + 6, (float)borderThickness);
        
        //Fill the main body of the jog wheel with black
        g.setColour(juce::Colour(0, 0, 0));
        g.fillEllipse(centerX - radius, centerY - radius, diameter, diameter);
        
        //Draws inner grooves to give a textured
        g.setColour(juce::Colour(0, 150, 170));
        for (int i = 0; i < 15; ++i)
        {
            double grooveRadius = radius - (i * 4.0);
            if (grooveRadius > 0)
                g.drawEllipse(centerX - grooveRadius, centerY - grooveRadius, grooveRadius * 2, grooveRadius * 2, 0.4f);
        }
        
        //Draws the center label ring (inner cyan ring)
        double labelDiameter = 75 * dialScale;
        double labelRadius = labelDiameter / 2.0;
        g.setColour(juce::Colour(0, 240, 255));
        g.drawEllipse(centerX - labelRadius, centerY - labelRadius, labelDiameter, labelDiameter, 2.0f);
        
        //Create a rectangle pointer and position the rotating pointer
        juce::Path pointer;
        pointer.addRoundedRectangle(-2, -radius + 12, 4, radius / 3.0, 1.0f);
        //Rotate the pointer based on the calculated angle and move it to the center of the knob
        pointer.applyTransform(juce::AffineTransform::rotation((float)angle)
                               .translated((float)centerX, (float)centerY));
        //Set light blue color
        g.setColour(juce::Colour(0, 150, 170));
        g.fillPath(pointer);
    }
};

class SlidersLookAndFeel : public juce::LookAndFeel_V4
{
public:
    //Constructor to set default colors for the slider
    SlidersLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::lightgrey);
        setColour(juce::Slider::trackColourId, juce::Colours::black);
    }
    //Function to draw the sliders
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        //Define track and tick parameters
        const int trackWidth = 4;
        const int tickLength = 10;
        const int numTicks = 10;
        int centerX = x + width / 2;
        int intSliderPos = static_cast<int>(sliderPos);

        //Retrieves colors set in the constructor
        auto trackColour = slider.findColour(juce::Slider::trackColourId);
        auto thumbColour = slider.findColour(juce::Slider::thumbColourId);

        //Checks if the lsider is in vertical mode
        if (style == juce::Slider::LinearVertical)
        {
            //Calculate track position
            int trackX = centerX - trackWidth / 2;

            //Draws the background of the track
            g.setColour(trackColour);
            g.fillRect(trackX, y, trackWidth, height);

            //Draws the filled portion of the track
            int fillHeight = (y + height) - intSliderPos;
            g.setColour(thumbColour);
            g.fillRect(trackX, intSliderPos, trackWidth, fillHeight);

            //Draws the tick marks along the slider
            g.setColour(trackColour.brighter());
            for (int i = 0; i <= numTicks; ++i)
            {
                int tickY = y + (i * height) / numTicks;
                g.drawLine(centerX - trackWidth - tickLength, tickY, centerX - trackWidth - 4, tickY, 1);
                g.drawLine(centerX + trackWidth + 4, tickY, centerX + trackWidth + tickLength, tickY, 1);
            }

            //Draws the slider handle
            const int thumbWidth = width * 4 / 5;
            const int thumbHeight = 14;
            int thumbX = x + (width - thumbWidth) / 2;
            int thumbY = intSliderPos - thumbHeight / 2;

            //Draws the slider handle body
            g.setColour(thumbColour);
            g.fillRect(thumbX, thumbY, thumbWidth, thumbHeight);
            g.setColour(Colour(31, 31, 31));
            g.fillRect(thumbX, thumbY + 4, thumbWidth, thumbHeight / 2 - 2);
        }
        //Checks if the slider is horizontal
        else if (style == juce::Slider::LinearHorizontal)
        {
            //Define track height and position it in the middle
            const int trackHeight = 4;
            int trackY = y + (height - trackHeight) / 2;

            //Draws the background of the track
            g.setColour(trackColour);
            g.fillRect(x, trackY, width, trackHeight);

            //Draws the filled portion of the track
            g.setColour(thumbColour);
            g.fillRect(x, trackY, intSliderPos - x, trackHeight);

            //Draws the slider handle
            const int thumbWidth = 14;
            const int thumbHeight = height * 4 / 5;
            int thumbX = intSliderPos - thumbWidth / 2;
            int thumbY = y + (height - thumbHeight) / 2;

            //Draws the body of the handle
            g.setColour(thumbColour);
            g.fillRect(thumbX, thumbY, thumbWidth, thumbHeight);
            g.setColour(Colour(31, 31, 31));
            g.fillRect(thumbX + 4.9, thumbY, thumbWidth / 2 - 2, thumbHeight);
            
        }
    }

};

class DeckGUI  : public juce::Component,
                 public Button::Listener,
                 public Slider::Listener,
                 public FileDragAndDropTarget,
                 public Timer
{
public:
    //Constructor and destructor
    DeckGUI(DJAudioplayer* player,
            AudioFormatManager & formatManagerToUse,
            AudioThumbnailCache & cacheToUse, bool isLeftDeck);
    ~DeckGUI() override;

    //Draws the component
    void paint (juce::Graphics&) override;
    //Handles component resizing
    void resized() override;
    
    //Event handlers for button and slider interactions
    void buttonClicked (Button *) override;
    void sliderValueChanged (Slider *slider) override;
    
    //Drag and drop file handling
    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override;
    
    //Timer callback function
    void timerCallback() override;
    
    //Loading audio file into the deck
    void loadFile(const juce::URL& audioURL);
    
    //Sets the waveform color deck
    void setWaveformColour(juce::Colour newColour)
        {
            waveformDisplay.setWaveformColour(newColour);
        }
    
   
private:
    //Determines if it is the left or right deck
    bool isLeftDeck;
    
    //Custom LookAndFeel objects for UI elements
    EQKnobLookAndFeel eqKnobLookAndFeel;
    ButtonLookAndFeel buttonLookAndFeel;
    JogWheelLookAndFeel jogLookAndFeel;
    SlidersLookAndFeel slidersLookAndFeel;
    
    //Sliders for jogwheel, volume, speed and position
    Slider jogWheel;
    juce::Slider volSlider;
    Slider speedSlider;
    Slider posSlider;
    
    //ALL THE BUTTONS HERE
    TextButton playButton{"PLAY"};
    TextButton stopButton{"STOP"};
    TextButton loadButton{"LOAD"};
    TextButton forwardButton{"FORWARD"};
    TextButton backwardButton{"BACKWARD"};
    TextButton loopButton{"LOOP"};

    //Pointer to the audio player object
    DJAudioplayer* player;
    
    //Displays the waveform of the track
    WaveformDisplay waveformDisplay;
    
    //Arrays to manage multiple sliders and labels
    juce::Array<juce::Slider*> sliders;
    juce::Array<juce::Label*> labels;
    
    //Labels for UI elements
    juce::Label volLabel{"volLabel", "V"};
    juce::Label speedLabel{"speedLabel", "S"};
    juce::Label posLabel{"posLabel", "P"};
    juce::Label forwardLabel{"forwardLabel", "F"};
    juce::Label backwardLabel{"backwardLabel", "B"};
    juce::Label MidLabel{"MidLabel", "Mid"};
    juce::Label wetdryLabel{"wetdryLabel", "Wet/Dry"};
    juce::Label bassLabel{"bassLabel", "Bass"};
    juce::Label trebleLabel{"trebleLabel", "Treble"};
    
    //To enable toggling
    bool isLoopEnabled = false;
    
    //Sliders for sound effect
    juce::Slider bassSlider;
    juce::Slider highSlider;
    juce::Slider wetDrySlider;
    juce::Slider midSlider;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};


