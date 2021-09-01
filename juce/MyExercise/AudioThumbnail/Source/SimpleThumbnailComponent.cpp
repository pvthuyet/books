#include "SimpleThumbnailComponent.h"

SimpleThumbnailComponent::SimpleThumbnailComponent(int srcSample,
	juce::AudioFormatManager& mgr,
	juce::AudioThumbnailCache& cache) :
	thumb(srcSample, mgr, cache)
{
	thumb.addChangeListener(this);
}

void SimpleThumbnailComponent::setFile(const juce::File& file)
{
	thumb.setSource(new juce::FileInputSource(file));
}

void SimpleThumbnailComponent::paintIfNoFileLoaded(juce::Graphics& g)
{
	g.fillAll(juce::Colours::white);
	g.setColour(juce::Colours::darkgrey);
	g.drawFittedText("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
}

void SimpleThumbnailComponent::paintIfFileLoaded(juce::Graphics& g)
{
    g.fillAll(juce::Colours::white);
    g.setColour(juce::Colours::red);
    thumb.drawChannels(g,
        getLocalBounds(),
        0.0,
        thumb.getTotalLength(),
        1.0f);
}

void SimpleThumbnailComponent::paint(juce::Graphics& g)
{
    if (thumb.getNumChannels() == 0)
        paintIfNoFileLoaded(g);
    else
        paintIfFileLoaded(g);
}

void SimpleThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (source == &thumb)
        thumnailChanged();
}

void SimpleThumbnailComponent::thumnailChanged()
{
    repaint();
}
