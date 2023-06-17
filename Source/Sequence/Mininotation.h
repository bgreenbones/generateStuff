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

template <typename T>
class Timed;

class Mininotation {
public:
    // TODO: implement values other than defaults
    static const char defaultValue = 'x';
    static const char alternateDefault = 'X';
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
    static const char alternateZero = ')';
    static const char alternateOne = '!';
    static const char alternateTwo = '@';
    static const char alternateThree = '#';
    static const char alternateFour = '$';
    static const char alternateFive = '%';
    static const char alternateSix = '^';
    static const char alternateSeven = '&';
    static const char alternateEight = '*';
    static const char alternateNine = '(';
    
    static const char rest = '.';
    static const char sustain = '~';
    
    static const char openTuplet = '['; // TODO: implement
    static const char closeTuplet = ']';

    static constexpr char symbols[5] = {
        defaultValue, alternateDefault, rest, sustain
    };

    static constexpr char defaultValueSymbols[3] = {
        defaultValue, alternateDefault
    };
    
    static constexpr char alternateValueSymbols[13] = {
        alternateDefault, alternateZero, alternateOne, alternateTwo, alternateThree, alternateFour,
        alternateFive, alternateSix, alternateSeven, alternateEight, alternateNine,
    };
    
    static constexpr char valueSymbols[24] = {
        defaultValue, alternateDefault,
        zero, one, two, three, four, five, six, seven, eight, nine,
        alternateZero, alternateOne, alternateTwo, alternateThree, alternateFour, alternateFive,
        alternateSix, alternateSeven, alternateEight, alternateNine,
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

    static bool isValue(const char symbol) {
        return isIn(symbol, &*valueSymbols);
    }
    
    static bool isAlternate(const char symbol) {
        return isIn(symbol, &*alternateValueSymbols);
    }

    static size_t getLength(std::string const phraseString) {
        return phraseString.size();
    }
    
    template <class T>
    static vector<Timed<T>> parse(std::string phraseString, Duration stepLength);
};

# endif
