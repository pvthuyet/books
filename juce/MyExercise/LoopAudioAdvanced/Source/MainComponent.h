#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
    private juce::Thread
{
public:
    class ReferenceCountedBuffer :  public juce::ReferenceCountedObject
    {
    public:
        using Ptr = juce::ReferenceCountedObjectPtr<ReferenceCountedBuffer>;
        ReferenceCountedBuffer(const juce::String& nameToUse,
            int numChannels,
            int numSamples);

        ~ReferenceCountedBuffer() override;

        juce::AudioSampleBuffer* getAudioSampleBuffer();

        int position{};

    private:
        juce::String name;
        juce::AudioSampleBuffer buffer;
    };

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
    //==============================================================================
    void run() override;

    void checkForBuffersToFree();
    void checkForPathToOpenFile();

    void clearButtonClicked();
    void openButtonClicked();

private:
    //==============================================================================
    // Your private member variables go here...
    juce::TextButton openButton;
    juce::TextButton clearButton;

    std::unique_ptr<juce::FileChooser> chooser;

    juce::AudioFormatManager formatManager;

    juce::ReferenceCountedArray<ReferenceCountedBuffer> buffers;
    ReferenceCountedBuffer::Ptr currentBuffer{nullptr};

    juce::String chooserPath;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
