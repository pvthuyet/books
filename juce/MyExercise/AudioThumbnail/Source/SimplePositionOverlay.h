#pragma once
#include <JuceHeader.h>

class SimplePositionOverlay : public juce::Component,
	private juce::Timer
{
private:
	juce::AudioTransportSource& transportSource;

public:
	SimplePositionOverlay(juce::AudioTransportSource& src);

	void paint(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& e) override;

private:
	void timerCallback() override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SimplePositionOverlay)
};

