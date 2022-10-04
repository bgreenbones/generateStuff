/*
  ==============================================================================

    Mininotation.h
    Created: 3 Oct 2022 12:21:40am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#ifndef mininotation_h
#define mininotation_H

class Mininotation {
public:
    static const char note = 'x';
    static const char accentNote = 'X';
    static const char rest = '~';
    static const char sustain = '.';

    static constexpr char symbols[4] = {
        note, accentNote, rest, sustain
    };

    static constexpr char noteSymbols[2] = {
        note, accentNote
    };

    static bool isIn(const char symbol, const char array[]) {
        for (int i = 0; i < strlen(array); i++) {
            if (symbol == array[i]) {
                return true;
            }
        }
        return false;
    }

    static bool isInNotation(const char symbol) {
        return isIn(symbol, &*symbols);
    }

    static bool isNote(const char symbol) {
        return isIn(symbol, &*noteSymbols);
    }

    static size_t getLength(std::string const phraseString) {
        return phraseString.size();
    }
};

# endif
