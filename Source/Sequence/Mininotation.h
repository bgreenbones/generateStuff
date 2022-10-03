/*
  ==============================================================================

    Mininotation.h
    Created: 3 Oct 2022 12:21:40am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once



namespace mininotation {
    char note = 'x';
    char accentNote = 'X';
    char rest = '~';
    char sustain = '.';

    char symbols[4] = {
        note, accentNote, rest, sustain
    };

    char noteSymbols[2] = {
        note, accentNote
    };

    bool isIn(char symbol, char array[]) {
        for (int i = 0; i < strlen(array); i++) {
            if (symbol == array[i]) {
                return true;
            }
        }
        return false;
    }

    bool isInNotation(char symbol) {
        return isIn(symbol, symbols);
    }

    bool isNote(char symbol) {
        return isIn(symbol, noteSymbols);
    }

    size_t getLength(std::string phraseString) {
        return phraseString.size();
    }
}
