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

#include <vector>
#include "Duration.h"

using namespace std;

class Mininotation {
public:
    // TODO: implement values other than defaults
    static const char defaultValue = 'x';
    static const char modifiedDefault = 'X';
    static const char zero = '0';
    static const char one = '1';
    static const char two = '2';
    static const char three = '3';
    static const char four = '4';
    static const char five = '5';
    static const char six = '6';
    static const char seven = '7';
    static const char eight = '8';
    static const char nine = '9';
    static const char addTen = 't'; // haha cos we can't reuse '1'...
    static const char modifiedZero = ')';
    static const char modifiedOne = '!';
    static const char modifiedTwo = '@';
    static const char modifiedThree = '#';
    static const char modifiedFour = '$';
    static const char modifiedFive = '%';
    static const char modifiedSix = '^';
    static const char modifiedSeven = '&';
    static const char modifiedEight = '*';
    static const char modifiedNine = '(';
    
    static const char rest = '.';
    static const char sustain = '~';
    
    static const char openTuplet = '['; // TODO: implement
    static const char closeTuplet = ']';

    static constexpr char symbols[5] = {
        defaultValue, modifiedDefault, rest, sustain
    };

    static constexpr char defaultValueSymbols[3] = {
        defaultValue, modifiedDefault
    };
    static constexpr char valueSymbols[24] = {
        defaultValue, modifiedDefault,
        zero, one, two, three, four, five, six, seven, eight, nine,
        modifiedZero, modifiedOne, modifiedTwo, modifiedThree, modifiedFour, modifiedFive,
        modifiedSix, modifiedSeven, modifiedEight, modifiedNine, defaultValue,
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
        return isIn(symbol, &*defaultValueSymbols);
    }

    static size_t getLength(std::string const phraseString) {
        return phraseString.size();
    }
    
    template <class T>
    static vector<T> parse(std::string phraseString, Duration stepLength);
};

# endif
