#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
    public juce::ChangeListener,
    public juce::Timer
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

    //==============================================================================
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //==============================================================================
    void timerCallback() override;

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping
    };

    void openButtonClicked();
    void playButtonClicked();
    void stopButtonClicked();
    void loopButtonChanged();
    void updateLoopState(bool shouldLoop);
    void changeState(TransportState newState);

private:
    //==============================================================================

    juce::TextButton openButton;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::ToggleButton loopingToggle;
    juce::Label currentPositionLabel;

    std::unique_ptr<juce::FileChooser> chooser;
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    TransportState state;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
