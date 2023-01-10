/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <algorithm>
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "HostSettings.h"


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
                       ),
        playQueue(make_unique<map<string, Playable>>(map<string, Playable>()))
#endif
{
    this->generator = Generator(playQueue);
    this->displacement = 0;
    this->startingBar = 1;
    this->stoppingBar = (Bars) (this->startingBar + generator.phraseLength());
    this->noteOffIssued = true;
    
    this->allNotesOff = vector<juce::MidiMessage>();
    for (int pitch = 0; pitch <= 127; pitch ++) { // yikes. for now this is the only thing that turns off note on messages when we stop playing
        for (int midiChannel = 1; midiChannel <= 16; midiChannel++) {
            auto noteOff = juce::MidiMessage::noteOff (midiChannel, pitch, (juce::uint8) 0);
            this->allNotesOff.push_back(noteOff);
        }
    }
    
    srand((int) time(0)); // init random number generator stuff
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
    this->mSampleRate = sampleRate;
    samplesPerMinute = sampleRate * 60;
    this->mSamplesPerBlock = samplesPerBlock;
    
    auto playhead = getPlayHead();
    if (playhead != nullptr) {
        auto positionInfo = playhead->getPosition();
        HostSettings::instance().setTempo((positionInfo->getBpm()).orFallback(120));
        samplesPerBeat = samplesPerMinute / HostSettings::instance().getTempo();
    }
    
    this->noteOffIssued = true;
    this->currentNoteOff = 0;
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

//void GenerateStuffAudioProcessor::removePlayable(string id) {
//    playQueue->erase(id);
//}
//
//void GenerateStuffAudioProcessor::toggleMutePlayable(string id) {
//    if (playQueue->find(id) == playQueue->end()) {
//        return;
//    };
//    Playable &playable = playQueue->at(id);
//    playable.mute = !(playable.mute);
//}
//
//
//void GenerateStuffAudioProcessor::queuePlayable(string id, Playable playable) {
//    //    for (auto it = playQueue.begin(); it < playQueue.end();) {
//    //        if ((*it).midiChannel == playable.midiChannel) {
//    //            playQueue.erase(it);
//    //        } else {
//    //            ++it;
//    //        }
//    //    }
//    auto result = playQueue->emplace(id, playable);
//    if (result.second) return;
//    playQueue->at(id) = playable;
//}

void GenerateStuffAudioProcessor::setDisplacement(Beats displacement){
    this->displacement = displacement;
    return;
}

void GenerateStuffAudioProcessor::setStartBar(Bars startingBar){
    if (startingBar >= this->stoppingBar) return;
    if (startingBar < Bars(1)) return;
    this->startingBar = startingBar;
    return;
}

void GenerateStuffAudioProcessor::setStopBar(Bars stoppingBar){
    if (this->startingBar >= stoppingBar) return;
    this->stoppingBar = stoppingBar;
    return;
}

void GenerateStuffAudioProcessor::updateTimeSignature(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newTimeSignatureJuce = (positionInfo->getTimeSignature())
                             .orFallback(juce::AudioPlayHead::TimeSignature());

    TimeSignature newTimeSignature = TimeSignature(newTimeSignatureJuce.numerator, newTimeSignatureJuce.denominator);
    if (HostSettings::instance().getTimeSignature() != newTimeSignature) {
        HostSettings::instance().setTimeSignature(newTimeSignature);
//        generator.updateTimeSignature();
    }
    
    return;
}

void GenerateStuffAudioProcessor::updateBpm(juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo)
{
    auto newBpm = (positionInfo->getBpm()).orFallback(120);
    
    if (newBpm != HostSettings::instance().getTempo()) {
        HostSettings::instance().setTempo(newBpm);
        samplesPerBeat = samplesPerMinute / newBpm;
    }
    
    return;
}

void GenerateStuffAudioProcessor::playPlayables(
        juce::Optional<juce::AudioPlayHead::PositionInfo> positionInfo,
        juce::MidiBuffer& midiMessages)
{
    this->generator.phraseLength();
    
    
    const double ppqPosition = (positionInfo->getPpqPosition()).orFallback(0);
    for (auto playableIt = playQueue->begin(); playableIt != playQueue->end(); ++playableIt) {
        Playable playable = playableIt->second;
        Phrase phrase = playable.phrase;
        int midiChannel = playable.midiChannel;
        
        if (playable.mute) {
            continue;
        }
        
        for (auto noteIt = phrase.notes.begin(); noteIt != phrase.notes.end(); ++noteIt) {
            Note note = *noteIt;
            
//            float ppqBarInQuarters = HostSettings::instance().getTimeSignature().barLengthInQuarters();
//            double noteOnTimeInQuarters = phrase.bar * ppqBarInQuarters + phrase.offset + note.startTime; // todo: this doesn't work right if we have time signature changes
            Bars playPeriod = this->stoppingBar - this->startingBar;
            double loopStart = this->startingBar - Bars(1);
            double loopEnd = this->stoppingBar -Bars(1);
            double noteOnTimeInQuarters = loopStart + ((this->displacement + phrase.startTime + note.startTime) % playPeriod);
            while (ppqPosition > noteOnTimeInQuarters) { // might as well set it to be in the future
                noteOnTimeInQuarters += phrase.duration;
            }
            if (noteOnTimeInQuarters > loopEnd) { // but don't go too far in the future, we've set an end bar
                noteOnTimeInQuarters = (Bars(noteOnTimeInQuarters - loopStart) % playPeriod) + loopStart;
            }
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
                                return (loop.ppqEnd - ppqPosition + ppqTime - loop.ppqStart) * samplesPerBeat + 2; // + 2 ? I guess we're adding two + 1s demonstrated below?
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
    if (!midiMessages.isEmpty()) {
        midiMessages.clear();
    }

    auto playhead = getPlayHead();
    if (playhead != nullptr) {
        auto positionInfo = playhead->getPosition();
        updateTimeSignature(positionInfo);
        updateBpm(positionInfo);
        auto isPlaying = positionInfo->getIsPlaying();
        if (isPlaying) {
            noteOffIssued = false;
            currentNoteOff = 0;
            playPlayables(positionInfo, midiMessages);
        } else {
            if (!noteOffIssued) {
                if (currentNoteOff < allNotesOff.size()) {
                    size_t upperBound = std::min(size_t(currentNoteOff + mSamplesPerBlock), allNotesOff.size());
                    for (auto i = currentNoteOff; i < upperBound; i++) {
                        bool success = midiMessages.addEvent(allNotesOff[i], 0);
                        if (!success) {
                            throw exception();
                        }
                    }
                    currentNoteOff = upperBound;
                    if (currentNoteOff >= allNotesOff.size()) {
                        noteOffIssued = true;
                    }
                }
            }
        }
    }
    
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
//    return new juce::GenericAudioProcessorEditor (*this);
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

juce::AudioProcessorValueTreeState::ParameterLayout GenerateStuffAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(make_unique<juce::AudioParameterFloat>(
                                                      "Probability",
                                                      "Probability",
                                                      juce::NormalisableRange<float>(0.f, 1.f, 0.01f, 1.f),
                                                      0.2f
                                                      ));
    
    juce::StringArray subdivisionStrings;
    for (int i = 1; i <= 9; i++) {
        juce::String subdivisionString;
        subdivisionString << "1/";
        subdivisionString << i;
        subdivisionStrings.add(subdivisionString);
    }
    layout.add(make_unique<juce::AudioParameterChoice>(
                                                      "Subdivision",
                                                      "Subdivision",
                                                      subdivisionStrings,
                                                      0
                                                      ));
    
    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GenerateStuffAudioProcessor();
}
