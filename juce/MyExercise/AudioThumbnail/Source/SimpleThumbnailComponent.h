#pragma once

#include <JuceHeader.h>

class SimpleThumbnailComponent : public juce::Component,
	private juce::ChangeListener
{
private:
	juce::AudioThumbnail thumb;

public:
	SimpleThumbnailComponent(int srcSample,
		juce::AudioFormatManager& mgr,
		juce::AudioThumbnailCache& cache);

	void setFile(const juce::File& file);

	void paintIfNoFileLoaded(juce::Graphics& g);
	void paintIfFileLoaded(juce::Graphics& g);

	void paint(juce::Graphics& g) override;
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	void thumnailChanged();
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimpleThumbnailComponent)
};

