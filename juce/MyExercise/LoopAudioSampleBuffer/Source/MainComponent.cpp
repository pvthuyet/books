#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] {openBtnClicked(); };

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] {closeBtnClicked(); };

    addAndMakeVisible(levelSlider);
    levelSlider.setRange(0.0, 1.0);
    levelSlider.onValueChange = [this] { currentLevel = levelSlider.getValue(); };

    formatManager.registerBasicFormats();

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (300, 200);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        //setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    //bufferToFill.clearActiveBufferRegion();
    auto level = currentLevel;
    auto startLevel = level == previousLevel ? level : previousLevel;

    auto numInputChannels = fileBuffer.getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;                                  // [8]
    auto outputSamplesOffset = bufferToFill.startSample;                                    // [9]

    while (outputSamplesRemaining > 0)
    {
        auto bufferSamplesRemaining = fileBuffer.getNumSamples() - position;                // [10]
        auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining); // [11]

        for (auto channel = 0; channel < numOutputChannels; ++channel)
        {
            bufferToFill.buffer->copyFrom(channel,                                         // [12]
                outputSamplesOffset,                             //  [12.1]
                fileBuffer,                                      //  [12.2]
                channel % numInputChannels,                      //  [12.3]
                position,                                        //  [12.4]
                samplesThisTime);                                //  [12.5]

            bufferToFill.buffer->applyGainRamp(channel,
                outputSamplesOffset,
                samplesThisTime,
                startLevel,
                level);
        }

        outputSamplesRemaining -= samplesThisTime;                                          // [13]
        outputSamplesOffset += samplesThisTime;                                             // [14]
        position += samplesThisTime;                                                        // [15]

        if (position == fileBuffer.getNumSamples())
            position = 0;                                                                   // [16]
    }

    previousLevel = level;
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    fileBuffer.setSize(0, 0);
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    openButton.setBounds(10, 10, getWidth() - 20, 20);
    clearButton.setBounds(10, 40, getWidth() - 20, 20);
    levelSlider.setBounds(10, 70, getWidth() - 20, 20);
}

void MainComponent::openBtnClicked()
{
    shutdownAudio();
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file shorter than 2 seconds to play...",
        juce::File{},
        "*.wav");

    auto chooserFlags = juce::FileBrowserComponent::openMode
        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc) 
        {
            auto file = fc.getResult();
            if (file == juce::File{})
                return;

            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
            if (reader)
            {
                auto duration = (float)(reader->lengthInSamples / reader->sampleRate);
                if (duration < 2)
                {
                    fileBuffer.setSize(reader->numChannels, reader->lengthInSamples);
                    reader->read(&fileBuffer,
                        0,
                        reader->lengthInSamples,
                        0,
                        true,
                        true);
                    position = 0;
                    setAudioChannels(0, reader->numChannels);
                }
            }
        });
}

void MainComponent::closeBtnClicked()
{
    shutdownAudio();
}