#pragma once
#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent,
    public juce::Timer
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;

private:
    juce::Slider frequencySlider;
    juce::Slider volumeSlider;
    juce::TextButton waveToggle{ "SMOOTH" };
    juce::ToggleButton vizToggle{ "VISUALIZER" };

    bool isSplashScreen = true;
    bool isSquareWave = false;
    bool showVisualizer = true;

    double currentSampleRate = 0.0;
    double currentPhase = 0.0;
    double phaseDelta = 0.0;
    float lastSample = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};