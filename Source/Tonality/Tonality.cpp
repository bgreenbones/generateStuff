/*
  ==============================================================================

    Tonality.cpp
    Created: 21 Mar 2023 5:01:59pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Tonality.h"
#include "Utility.h"


double voicing::crunch(vector<Pitch> const& voicing) {
  if (voicing.size() < 2) {
    return 0;
  }
  double totalCrunch = 0;
  for (int i = 0; i < voicing.size() - 1; i++) {
    for (int j = i + 1 ; j < voicing.size(); j++) {
      Pitch pitch_i = voicing[i];
      Pitch pitch_j = voicing[j];
      Interval interval = pitch_i - pitch_j;
      totalCrunch += interval == m2 ? 2 : 0;
      totalCrunch += interval == M2 ? 1 : 0;
    }
  }
  return totalCrunch;
}
void voicing::decreaseCrunch(vector<Pitch> & voicing) {
  if (voicing.size() < 2) {
    return;
  }
  double totalCrunch = crunch(voicing);
  shuffle(voicing.begin(), voicing.end(), getGen());
  for (int i = 0; i < voicing.size() - 1; i++) {
    for (int j = i + 1 ; j < voicing.size(); j++) {
      Pitch &pitch_i = voicing[i];
      Pitch &pitch_j = voicing[j];
      Interval interval = pitch_i - pitch_j;
      if(interval == M2) {
        Pitch & pitchToMove = flipCoin() ? pitch_i : pitch_j;
        if (pitchToMove.getOctave() <= 3) {
            pitchToMove += octave;
        } else if (pitchToMove.getOctave() >= 5) {
            pitchToMove -= octave;
        } else {
          if (flipCoin()) {
            pitchToMove += octave;
          } else {
            pitchToMove -= octave;
          }
        }
        break;
      }
      if(interval == m2) {
        Pitch &highPitch = pitch_i > pitch_j ? pitch_i : pitch_j;
        Pitch &lowPitch = pitch_i > pitch_j ? pitch_j : pitch_i;

        if (lowPitch.getOctave() <= 3) {
            lowPitch += octave;
        } else if (highPitch.getOctave() >= 5) {
            highPitch -= octave;
        } else {
          if (flipCoin()) {
            lowPitch += octave;
          } else {
            highPitch -= octave;
          }
        }
        break;
      }
    }
    if (crunch(voicing) < totalCrunch) {
      break;
    }
  }
}

void voicing::decreaseSpread(vector<Pitch> & voicing) {
  if (voicing.size() < 2) {
    return;
  }
  for (Pitch & pitch_i : voicing) {
    bool allGreaterThanOctave = true;
    int downs = 0;
    int ups = 0;
    for (Pitch & pitch_j : voicing) {
      if (pitch_i == pitch_j) {
        continue;
      }
      Interval interval = pitch_i - pitch_j;
      if (interval > octave) {
        ups += pitch_i > pitch_j ? 1 : 0;
        downs += pitch_i > pitch_j ? 0 : 1;
      }  else {
        allGreaterThanOctave = false;
      }
    }
    if (allGreaterThanOctave) {
      if (ups > downs) {
        pitch_i -= octave;
      } else {
        pitch_i += octave;
      }
    }
  }
}

void voicing::preventRumble(vector<Pitch> & voicing) {
  if (voicing.size() < 2) {
    return;
  }
  for (Pitch & pitch_i : voicing) {
    if (pitch_i.getOctave() > 3) {
      continue;
    }
    for (Pitch & pitch_j : voicing) {
      if (pitch_i == pitch_j || pitch_j.getOctave() > 3) {
        continue;
      }
      Interval interval = pitch_i - pitch_j;
      if (interval < P5) {
        if (pitch_i > pitch_j || flipWeightedCoin(0.8)) {
            pitch_j += octave;
        } else {
            pitch_i += octave;
        }
      }
    }
  }
}

Tonality::Tonality(PitchClass root, vector<Interval> intervals): root(root), intervalsUpFromRoot(intervals) {
        std::sort(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end());
    };
Tonality::Tonality(): Tonality(C, ionian) {};

int Tonality::stepHelper(Interval first, Direction direction) const {
    auto tonalityMemberInterval = std::find(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), first);
    bool notInTonality = tonalityMemberInterval == intervalsUpFromRoot.end();
    if (notInTonality) {
        auto lessIt = intervalsUpFromRoot.begin();
        while (lessIt + 1 != intervalsUpFromRoot.end() && *(lessIt + 1) < first) { lessIt++; }
        int less = *lessIt < first ? *lessIt : *(intervalsUpFromRoot.end()) - 12;
        int more = lessIt + 1 != intervalsUpFromRoot.end() ? *(lessIt + 1) : *intervalsUpFromRoot.begin() + 12;
        return direction == Direction::up ? more : less;
    }
    
    Interval beginInterval = *intervalsUpFromRoot.begin();
    Interval endInterval = *(intervalsUpFromRoot.end() - 1);

    bool wrapDownward = direction == Direction::down && *tonalityMemberInterval == beginInterval;
    if (wrapDownward) { return endInterval - 12; }
    bool wrapUpward = direction == Direction::up && *tonalityMemberInterval == endInterval;
    if (wrapUpward) { return beginInterval + 12; }
    
    return *(tonalityMemberInterval + direction * 1);
}


Pitch Tonality::step(Pitch first, Direction direction) const {
    Interval firstInterval = root > first.getPitchClass()
        ? (Interval) (12 + first.getPitchClass() - root)
        : (Interval) (first.getPitchClass() - root);

    return Pitch(first.pitchValue + stepHelper(firstInterval, direction) - firstInterval);
}

Pitch Tonality::multiStep(Pitch first, int steps, Direction direction) const {
    Pitch newPitch(first);
    while (steps-- > 0) {
        newPitch = step(newPitch, direction);
    }
    return newPitch;
}

vector<Pitch> Tonality::getPitches(int octave) const {
    octave = (octave < -2 || octave > 8) ? 3 : octave;
    Pitch rootPitch = Pitch(root, octave);
    vector<Pitch> pitches;
    transform(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), back_inserter(pitches),
        [&](Interval const& interval) { return rootPitch.pitchFromInterval(interval, up); });
    return pitches;
}

vector<Pitch> Tonality::randomVoicing() const {
    vector<Pitch> pitches = getPitches();
    vector<Pitch> voicing;
    transform(pitches.begin(), pitches.end(), back_inserter(voicing), [](Pitch pitch) {
        int octave = uniformInt(3, 6);
        return Pitch(pitch.getPitchClass(), octave);
    });
    return voicing;
}

vector<Pitch> Tonality::smoothVoicing(vector<Pitch> previousVoicing) const {
    vector<int> octaves = {3,4,5,6};
    vector<Pitch> voicing = mapp<Pitch>(getPitches(), [&](Pitch pitch) {
        vector<Pitch> options = mapp<int, Pitch>(octaves, [&](int octave) {
            return Pitch(pitch.getPitchClass(), octave);
        });
        Pitch closest;
        Interval closestDistance = IntervalCount;
        for (Pitch precedingPitch : previousVoicing) {
            for (Pitch option : options) {
                if (option - precedingPitch < closestDistance) {
                    closest = option;
                    closestDistance = option - precedingPitch;
                }
            }
        }
        return closest;
    });
    return voicing;
}

Tonality Tonality::getMode(int n) const {
    return Tonality(root, nthMode(intervalsUpFromRoot, n));
}

bool Tonality::operator==(Tonality other) const {
    if (other.intervalsUpFromRoot.size() != intervalsUpFromRoot.size()) { return false; }
    bool equals = root == other.root;

    std::sort(other.intervalsUpFromRoot.begin(), other.intervalsUpFromRoot.end());
    for (int i = 0; i < intervalsUpFromRoot.size(); i++) {
        equals = equals && intervalsUpFromRoot.at(i) == other.intervalsUpFromRoot.at(i);
    }

    return equals;
}

Tonality Tonality::harmonyToScale() const {
    return Tonality(root, {});
}

Tonality Tonality::scaleToHarmony() const {
    vector<Interval> const& scale = intervalsUpFromRoot;
    vector<Interval> harmony;
    harmony.push_back(unison);
    if (scale.size() < 2) { return Tonality(root, harmony); }
    if (scale.size() < 3) {
        harmony.push_back(scale.at(2));
        return Tonality(root, harmony);;
    }
    
    auto isThird = [](Interval interval) { return interval == m3 || interval == M3; };
    vector<Interval> thirds = filter<Interval>(scale, isThird);
    auto isSus = [](Interval interval) { return interval == M2 || interval == P4; };
    vector<Interval> suspensions = filter<Interval>(scale, isSus);
    
    Interval third = thirds.size() > 0 ? draw<Interval>(thirds) :
        suspensions.size() > 0 ? draw<Interval>(suspensions) :
        scale.at(2);

    
    harmony.push_back(third);
    
    auto isFifth = [](Interval interval) { return interval == P5 || interval == tritone; };
    vector<Interval> fifths = filter<Interval>(scale, isFifth);
    
    if (fifths.empty()) { return Tonality(root, harmony); }
    harmony.push_back(draw<Interval>(fifths));
    
    return Tonality(root, harmony);
}

vector<PitchClass> Tonality::getPitchClasses() const {
    vector<Pitch> pitches = getPitches();
    vector<PitchClass> pitchClasses;
    
    transform(pitches.begin(), pitches.end(), back_inserter(pitchClasses),
        [](Pitch pitch) { return pitch.getPitchClass(); });
    
    sort(pitchClasses.begin(), pitchClasses.end());
    return pitchClasses;
}

double Tonality::similarity(Tonality other) const {
    double containedInOther = 0; // will be 1. if all pitch classes in this are also in other
    double containedInThis = 0; // will be 1. if all pitch classes in other are also in this
    // will be < 1. (but >= 0) if some are missing
    
    for(PitchClass inThis : this->getPitchClasses()) {
        bool isInOther = false;
        for(PitchClass inOther : other.getPitchClasses()) {
            isInOther = isInOther || inThis == inOther;
        }
        containedInOther += isInOther ? 1. : 0.;
    }
    containedInOther /= this->getPitchClasses().size();
    
    for(PitchClass inOther : other.getPitchClasses()) {
        bool isInThis = false;
        for(PitchClass inThis : this->getPitchClasses()) {
            isInThis = isInThis || inThis == inOther;
        }
        containedInThis += isInThis ? 1. : 0.;
    }
    containedInThis /= other.getPitchClasses().size();

    return (containedInThis + containedInOther) / 2.; // return average of two contain measures. 
}

bool Tonality::includes(Tonality other) const {
    bool isInThis = true;
    vector<PitchClass> pitchClasses = this->getPitchClasses();
    for(PitchClass inOther : other.getPitchClasses()) {
        isInThis = isInThis && contains<PitchClass>(pitchClasses, inOther);
    }
    return isInThis;
}

Tonality Tonality::smoothModulation(int n, Direction direction) const {
    if (n <= 0) {
        return *this;
    }
    vector<Interval> options;
    
    for (auto interval_it = intervalsUpFromRoot.begin(); interval_it < intervalsUpFromRoot.end(); interval_it++) {
        Interval interval = *interval_it;
        int previous = interval_it == intervalsUpFromRoot.begin() ? intervalsUpFromRoot.back() - octave : *(interval_it - 1);
        int next = interval_it == intervalsUpFromRoot.end() - 1 ? intervalsUpFromRoot.front() + octave : *(interval_it + 1);
        
        Interval between_previous = (Interval) (interval - previous);
        Interval between_next = (Interval) (next - interval);
        
        if ((direction == Direction::up && (between_previous < M2 && between_next >= M2))
            || (direction == Direction::down && (between_next < M2 && between_previous >= M2))) {
            options.push_back(interval);
        }
    }
    
    if (options.empty()) {
        return *this; // nothing to raise...
    }
    
    Interval toModulate = draw<Interval>(options);
    Interval modulated = (Interval) (toModulate + direction);
    
    vector<Interval> newIntervalsUpFromRoot;
    transform(intervalsUpFromRoot.begin(), intervalsUpFromRoot.end(), back_inserter(newIntervalsUpFromRoot), [toModulate, modulated](Interval interval) {
        return interval == toModulate ? modulated : interval;
    });
    
    return Tonality(root, newIntervalsUpFromRoot).smoothModulation(n - 1, direction);
}


Tonality Tonality::raise(int n) const {
    return smoothModulation(n, Direction::up);
}

Tonality Tonality::lower(int n) const {
    return smoothModulation(n, Direction::down);
}

