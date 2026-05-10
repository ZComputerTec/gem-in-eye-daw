#include "MainComponent.h"
#include "BinaryData.h"

MainComponent::MainComponent()
{
    // Sliders
    addAndMakeVisible(frequencySlider);
    frequencySlider.setRange(50.0, 5000.0);
    frequencySlider.setValue(440.0);

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0);
    volumeSlider.setValue(0.125);

    // Wave Toggle (Smooth/Gritty)
    addAndMakeVisible(waveToggle);
    waveToggle.onClick = [this] {
        isSquareWave = !isSquareWave;
        waveToggle.setButtonText(isSquareWave ? "GRITTY" : "SMOOTH");
        };

    // Visualizer Toggle
    addAndMakeVisible(vizToggle);
    vizToggle.setToggleState(true, juce::dontSendNotification);
    vizToggle.onClick = [this] { showVisualizer = vizToggle.getToggleState(); };

    // Initially hide everything for the splash
    frequencySlider.setVisible(false);
    volumeSlider.setVisible(false);
    waveToggle.setVisible(false);
    vizToggle.setVisible(false);

    setSize(800, 600);
    setAudioChannels(0, 2);
    startTimer(3000);
}

MainComponent::~MainComponent() { shutdownAudio(); }

void MainComponent::timerCallback()
{
    isSplashScreen = false;
    frequencySlider.setVisible(true);
    volumeSlider.setVisible(true);
    waveToggle.setVisible(true);
    vizToggle.setVisible(true);
    stopTimer();
    repaint();
}

void MainComponent::prepareToPlay(int expected, double sampleRate) { currentSampleRate = sampleRate; }

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto level = (float)volumeSlider.getValue();
    auto freq = frequencySlider.getValue();

    if (currentSampleRate > 0.0)
    {
        phaseDelta = (freq / currentSampleRate) * 2.0 * juce::MathConstants<double>::pi;
        auto* left = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* right = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (auto s = 0; s < bufferToFill.numSamples; ++s)
        {
            float val = std::sin(currentPhase);
            if (isSquareWave) val = (val >= 0) ? 1.0f : -1.0f;

            currentPhase += phaseDelta;
            lastSample = val; // Feed the visualizer

            left[s] = val * level;
            right[s] = val * level;
        }
    }
}

void MainComponent::releaseResources() {}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    if (isSplashScreen)
    {
        auto logo = juce::ImageCache::getFromMemory(BinaryData::gem_in_eye_png, BinaryData::gem_in_eye_pngSize);
        if (logo.isValid())
            g.drawImageWithin(logo, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::centred);
    }
    else
    {
        // Neon UI
        g.setColour(juce::Colours::cyan);
        g.drawRect(frequencySlider.getBounds().expanded(2), 2);
        g.setColour(juce::Colours::magenta);
        g.drawRect(volumeSlider.getBounds().expanded(2), 2);

        if (showVisualizer)
        {
            g.setColour(juce::Colours::yellow);
            float cy = getHeight() / 2.0f;
            g.drawLine(0, cy + (lastSample * 100), getWidth(), cy - (lastSample * 100), 2.0f);
        }

        g.setColour(juce::Colours::white);
        g.drawText("GEM IN EYE | ACTIVE", 10, 10, 200, 20, juce::Justification::left);
    }
}

void MainComponent::resized()
{
    frequencySlider.setBounds(20, getHeight() - 110, getWidth() - 40, 30);
    volumeSlider.setBounds(20, getHeight() - 60, getWidth() - 40, 30);
    waveToggle.setBounds(getWidth() - 120, 20, 100, 30);
    vizToggle.setBounds(getWidth() - 120, 60, 100, 30);
}