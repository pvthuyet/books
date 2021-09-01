#include "SimplePositionOverlay.h"

SimplePositionOverlay::SimplePositionOverlay(juce::AudioTransportSource& src)
	: transportSource(src)
{
	startTimer(40);
}

void SimplePositionOverlay::paint(juce::Graphics& g)
{
	auto duration = transportSource.getLengthInSeconds();
	if (duration > 0)
	{
		auto pos = transportSource.getCurrentPosition();
		auto drawPos = (pos / duration) * getWidth();

		g.setColour(juce::Colours::green);
		g.drawLine(drawPos, 0.0f, drawPos, getHeight(), 2.0f);
	}
}

void SimplePositionOverlay::mouseDown(const juce::MouseEvent& e)
{
	auto duration = transportSource.getLengthInSeconds();
	if (duration > 0)
	{
		auto clickPos = e.position.x;
		auto pos = (clickPos / getWidth()) * duration;
		transportSource.setPosition(pos);
	}
}

void SimplePositionOverlay::timerCallback()
{
	repaint();
}
