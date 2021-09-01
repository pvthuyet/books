#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    void openBtnClicked();
    void closeBtnClicked();

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton openButton;
    juce::TextButton clearButton;
    juce::Slider levelSlider;
    
    std::unique_ptr<juce::FileChooser> chooser;
    
    juce::AudioFormatManager formatManager;
    juce::AudioSampleBuffer fileBuffer;
    int position{};

    float currentLevel{};
    float previousLevel{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
