

#include "VoiceParameters.h"

HarmonyApproach getHarmonyApproach(juce::String approach) {
    return (HarmonyApproach) harmonyApproaches.indexOf(approach);
}

string voiceParameterKey(string voiceKey, string paramKey) {
    return voiceKey + paramKey;
}
