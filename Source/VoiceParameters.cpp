

#include "VoiceParameters.h"

HarmonyApproach getHarmonyApproach(juce::String approach) {
    return (HarmonyApproach) harmonyApproaches.indexOf(approach);
}
