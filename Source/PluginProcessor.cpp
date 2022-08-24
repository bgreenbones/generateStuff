/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GenerateStuffAudioProcessor::GenerateStuffAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    this->generator = Generator();
    this->playQueue = vector<Playable>();
}

GenerateStuffAudioProcessor::~GenerateStuffAudioProcessor()
{
}

//==============================================================================
const juce::String GenerateStuffAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GenerateStuffAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GenerateStuffAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GenerateStuffAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GenerateStuffAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GenerateStuffAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GenerateStuffAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GenerateStuffAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GenerateStuffAudioProcessor::getProgramName (int index)
{
    return {};
}

void GenerateStuffAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void GenerateStuffAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mSampleRate = sampleRate;
    mSamplesPerBlock = samplesPerBlock;
    
    auto playhead = getPlayHead();
    auto positionInfo = playhead->getPosition();
    bpm = (positionInfo->getBpm()).orFallback(120);
    
    mSamplesPerMinute = mSampleRate * 60;
    samplesPerBeat = mSamplesPerMinute / bpm;
}

void GenerateStuffAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GenerateStuffAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


void GenerateStuffAudioProcessor::queuePlayable(Playable playable) {
    for (auto it = playQueue.begin(); it < playQueue.end();) {
        if ((*it).midiChannel == playable.midiChannel) {
            playQueue.erase(it);
        } else {
            ++it;
        }
    }
    playQueue.push_back(playable);
    return;
}

void GenerateStuffAudioProcessor::cascara() {
    queuePlayable(generator.cascara());
    return;
}

void GenerateStuffAudioProcessor::clave() {
    queuePlayable(generator.claveFromCascara());
    return;
}


void GenerateStuffAudioProcessor::updateTimeSignature(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newTimeSignature = (positionInfo->getTimeSignature())
                             .orFallback(juce::AudioPlayHead::TimeSignature());
    
    if (newTimeSignature.numerator != timeSignature.numerator || // todo: shouldn't we be able to just check equality of the TimeSignature objects?
        newTimeSignature.denominator != timeSignature.denominator) {
        timeSignature = newTimeSignature;
    }

    juce::AudioPlayHead::TimeSignature generatorTimeSignature = generator.getTimeSignature();
    if (timeSignature.numerator != generatorTimeSignature.numerator ||
        timeSignature.denominator != generatorTimeSignature.denominator) {
        generator.setTimeSignature(timeSignature.numerator, timeSignature.denominator);
    }
    
    return;
}
void GenerateStuffAudioProcessor::updateBpm(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newBpm = (positionInfo->getBpm()).orFallback(120);
    
    if (newBpm != this->bpm) {
        bpm = newBpm;
        samplesPerBeat = mSamplesPerMinute / bpm;
    }
    
    return;
}

void GenerateStuffAudioProcessor::playPlayables(
        juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo,
        juce::MidiBuffer& midiMessages)
{
    
//    double ppqLastBarStart = (positionInfo->getPpqPositionOfLastBarStart()).orFallback(0);
    const double ppqPosition = (positionInfo->getPpqPosition()).orFallback(0);
//    float barPosition = ppqPosition - ppqLastBarStart;
//    float ppqFraction = ppqPosition - floor(ppqPosition);
//    double noteTime = (1.0 - ppqFraction) * mSamplesPerBeat;
    for (auto playableIt = playQueue.begin(); playableIt < playQueue.end(); ++playableIt) {
        Playable playable = *playableIt;
        Sequence sequence = playable.sequence;
        Phrase phrase = playable.phrase;
        int midiChannel = playable.midiChannel;
        
        for (auto noteIt = sequence.notes.begin(); noteIt != sequence.notes.end(); ++noteIt) {
            Note note = *noteIt;
            
            float ppqBarInQuarters = timeSignature.numerator * (4.0 / (float) timeSignature.denominator);
            
            double noteOnTimeInQuarters = phrase.bar * ppqBarInQuarters + phrase.offset + note.startTime; // todo: this doesn't work right if we have time signature changes
            double noteOffTimeInQuarters = noteOnTimeInQuarters + note.duration;
            
            function <float(float)> bufferTimeFromPpqTime = [&](float ppqTime) -> float {
                
                if (ppqTime < ppqPosition) {
                    // check the loop markers to make sure a previous phrase isn't coming up ahead
                    bool isLooping = positionInfo->getIsLooping();
                    if (isLooping) {
                        juce::AudioPlayHead::LoopPoints no_loop = juce::AudioPlayHead::LoopPoints();
                        no_loop.ppqStart = -1;
                        no_loop.ppqEnd = -1;
                        
                        juce::AudioPlayHead::LoopPoints loop = (positionInfo->getLoopPoints()).orFallback(no_loop);
                        
                        if (loop != no_loop) {
                            if (loop.ppqStart <= ppqTime) {
                                return (loop.ppqEnd - ppqPosition + ppqTime - loop.ppqStart) * samplesPerBeat + 1;
                            }
                        }
                    }
                }
                
                return (ppqTime - ppqPosition) * samplesPerBeat + 1; // + 1 seems to line things up better...
            };
            function<bool(float)> isPpqTimeInBuffer = [&](float ppqTime) -> bool {
                float bufferTime = bufferTimeFromPpqTime(ppqTime);
    //            return 0 <= bufferTime && bufferTime < mSamplesPerBlock;
                return 0 < bufferTime && bufferTime <= mSamplesPerBlock; // todo: get actual lowest and highest indexes for the buffer instead of these guesses
            };
            
            if (isPpqTimeInBuffer(noteOnTimeInQuarters)) {
                auto noteOn = juce::MidiMessage::noteOn (midiChannel,
                                                        note.pitch,
                                                        (juce::uint8) note.velocity);
                bool success = midiMessages.addEvent (noteOn,
                                            bufferTimeFromPpqTime(noteOnTimeInQuarters));
                if (!success) {
                    throw exception(); // idfk
                }
                
            }
            
            if (isPpqTimeInBuffer(noteOffTimeInQuarters)) {
                auto noteOff = juce::MidiMessage::noteOff (midiChannel,
                                                          note.pitch,
                                                          (juce::uint8) note.velocity);
                bool success = midiMessages.addEvent (noteOff,
                                       bufferTimeFromPpqTime(noteOffTimeInQuarters));
                if (!success) {
                    throw exception(); // idfk
                }
            }
        }
    }
}


void GenerateStuffAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    auto playhead = getPlayHead();
    auto positionInfo = playhead->getPosition();
    
    updateTimeSignature(positionInfo);
    updateBpm(positionInfo);

    playPlayables(positionInfo, midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
     // In case we have more outputs than inputs, this code clears any output
     // channels that didn't contain input data, (because these aren't
     // guaranteed to be empty - they may contain garbage).
     // This is here to avoid people getting screaming feedback
     // when they first compile a plugin, but obviously you don't need to keep
     // this code if your algorithm always overwrites all the output channels.
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
         buffer.clear (i, 0, buffer.getNumSamples());
}

//==============================================================================
bool GenerateStuffAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GenerateStuffAudioProcessor::createEditor()
{
    return new GenerateStuffAudioProcessorEditor (*this);
}

//==============================================================================
void GenerateStuffAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GenerateStuffAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GenerateStuffAudioProcessor();
}
