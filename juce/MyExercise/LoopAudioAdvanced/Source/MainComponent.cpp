#include "MainComponent.h"

MainComponent::ReferenceCountedBuffer::ReferenceCountedBuffer(const juce::String& nameToUse,
    int numChannels,
    int numSamples) :
    name(nameToUse),
    buffer(numChannels, numSamples)
{
    DBG(juce::String("Buffer named '") + name + "' constructed. numChannels = " + juce::String(numChannels) + ", numSamples = " + juce::String(numSamples));
}

MainComponent::ReferenceCountedBuffer::~ReferenceCountedBuffer()
{
    DBG(juce::String("Buffer named '") + name + "' destroyed");
}

juce::AudioSampleBuffer* MainComponent::ReferenceCountedBuffer::getAudioSampleBuffer()
{
    return &buffer;
}

//==============================================================================
MainComponent::MainComponent() :
    Thread("Background Thread")
{
    addAndMakeVisible(openButton);
    openButton.setButtonText("Open...");
    openButton.onClick = [this] { openButtonClicked(); };

    addAndMakeVisible(clearButton);
    clearButton.setButtonText("Clear");
    clearButton.onClick = [this] { clearButtonClicked(); };

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
        setAudioChannels(0, 2);
    }
    
    startThread();
}

MainComponent::~MainComponent()
{
    stopThread(4000);
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
    auto retainedCurBuf = currentBuffer;
    if (!retainedCurBuf)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto* curSample = retainedCurBuf->getAudioSampleBuffer();
    auto pos = retainedCurBuf->position;

    auto numInputChannels = curSample->getNumChannels();
    auto numOutputChannels = bufferToFill.buffer->getNumChannels();

    auto outputSamplesRemaining = bufferToFill.numSamples;
    auto outputSampleOffset{0};

    while (outputSamplesRemaining > 0)
    {
        auto bufferSamplesRemaining = curSample->getNumSamples() - pos;
        auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining);

        for (auto c = 0; c < numOutputChannels; ++c)
        {
            bufferToFill.buffer->copyFrom(c,
                bufferToFill.startSample + outputSampleOffset,
                *curSample,
                c % numInputChannels,
                pos,
                samplesThisTime);
        }

        outputSamplesRemaining -= samplesThisTime;
        outputSampleOffset += samplesThisTime;
        pos += samplesThisTime;

        if (pos == curSample->getNumSamples())
            pos = 0;
    }
    retainedCurBuf->position = pos;
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
    currentBuffer = nullptr;
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
}

void MainComponent::run()
{
    while (!threadShouldExit())
    {
        checkForPathToOpenFile();
        checkForBuffersToFree();
        wait(500);
    }
}

void MainComponent::checkForBuffersToFree()
{
    for (auto i = buffers.size() - 1; i >= 0; --i)
    {
        auto buf = buffers.getUnchecked(i);
        if (2 == buf->getReferenceCount())
            buffers.remove(i);
    }
}

void MainComponent::checkForPathToOpenFile()
{
    juce::String path;
    path.swapWith(chooserPath);

    if (path.isNotEmpty())
    {
        juce::File file(path);
        std::unique_ptr<juce::AudioFormatReader> reader(
            formatManager.createReaderFor(file));

        if (reader)
        {
            auto duration = (float)reader->lengthInSamples / reader->sampleRate;
            if (duration < 2)
            {
                ReferenceCountedBuffer::Ptr newBuf = new ReferenceCountedBuffer(
                    file.getFileName(),
                    reader->numChannels,
                    reader->lengthInSamples
                );
                reader->read(newBuf->getAudioSampleBuffer(),
                    0,
                    reader->lengthInSamples,
                    0,
                    true,
                    true);
                currentBuffer = newBuf;
                buffers.add(newBuf);
            }
            else
            {

            }
        }
    }
}

void MainComponent::clearButtonClicked()
{
    currentBuffer = nullptr;
}

void MainComponent::openButtonClicked()
{
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
            auto path = file.getFullPathName();
            chooserPath.swapWith(path);
            notify();
        });
}