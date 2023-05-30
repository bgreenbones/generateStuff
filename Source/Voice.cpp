/*
  ==============================================================================

    Voice.cpp
    Created: 26 May 2023 12:17:47pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Voice.h"
#include "Ensemble.h"


Phrase Voice::atOrEmpty(Position startTime) const {
  Phrase const *phrase = schedule.at(startTime);
  if (phrase == nullptr) {
    Phrase result = ensemble.emptyPhrase(name);
    result.setStartTime(startTime);
    return result;
  }
  return *phrase;
}    

Phrase Voice::connecting(Probability connectingProb, // TODO: just get all this stuff from editor state instead of passing it in
                    Probability associationProb,
                    Probability connectingLengthProb) const {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    if (!schedule.isScheduledAt(startTime)) return ensemble.emptyPhrase(name);
    Phrase const* phrase = schedule.at(startTime);
    if (phrase == nullptr) {
        return ensemble.emptyPhrase(name);
    }
    Phrase phrasePhrase = (*phrase).loop(phraseLength);
    Phrase connectingPhrase = phrasePhrase.fillWithRolls(connectingProb, associationProb, connectingLengthProb);
// Todo: we actually just have to add notes to the phrase 
// reference we don't have to make a whole new phrase and schedule it
    return connectingPhrase;

}

vector<OrnamentSimple> getOrnamentVector(bool flams, bool drags, bool ruffs) {
    vector<OrnamentSimple> result;
    if (flams) { result.push_back(flam); }
    if (drags) { result.push_back(drag); }
    if (ruffs) { result.push_back(ruff); }
    return result;
}

Phrase Voice::ornament(Probability prob,
                             double breadth,
                             bool flams,
                             bool drags,
                             bool ruffs) const {
    Position startTime = ensemble.editorState.getStartTime();
    Duration phraseLength = ensemble.editorState.getPhraseLength();
    if (!schedule.isScheduledAt(startTime)) return ensemble.emptyPhrase(name);
    Voice &voice = ensemble.getVoice(name);
    Phrase const* phrase = voice.schedule.at(ensemble.editorState.phraseStartTime);
    if (phrase == nullptr) { return ensemble.emptyPhrase(name); }
    Phrase phrasePhrase = *phrase;
    auto possibleOrnaments = getOrnamentVector(flams, drags, ruffs);
    if (possibleOrnaments.empty()) { return phrasePhrase; }
    Phrase ornamentsPhrase = phrasePhrase.addOrnaments(possibleOrnaments, prob, breadth);
    return ornamentsPhrase;
}