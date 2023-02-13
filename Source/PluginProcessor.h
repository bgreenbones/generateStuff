/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Generator.hpp"
#include "Duration.h"
#include "LoopTasks.h"


//==============================================================================
/**
*/
class GenerateStuffAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    GenerateStuffAudioProcessor();
    ~GenerateStuffAudioProcessor() override;

    //==============================================================================
    double mSampleRate;
    int mSamplesPerBlock;
    double samplesPerBeat;
    double samplesPerMinute;
    float improviseRolls;
    float improviseOrnaments;
    Generator generator;
    shared_ptr<map<string, Playable>> playQueue;
    vector<juce::MidiMessage> allNotesOff;
    unsigned long currentNoteOff;
    bool noteOffIssued;
    LoopTasks loopTasks;
    
    void setDisplacement(Beats displacement);
    void setStartBar(bars startingBar);
    void setStopBar(bars stoppingBar);
    Beats displacement;
    bars startingBar;
    bars stoppingBar;
    //============================================================================
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
//    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GenerateStuffAudioProcessor)
    //==============================================================================
    void updateTimeSignature(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo);
    void updateBpm(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo);
    void playPlayables(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo,
                       juce::MidiBuffer& midiMessages);
};
