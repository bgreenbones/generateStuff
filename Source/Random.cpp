/*
  ==============================================================================

    Random.cpp
    Created: 11 Oct 2022 10:15:25pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#include "Random.h"
#include "Note.hpp"
#include <random>
#include <iostream>
#include "Syncopation.h"
#include "Pitch.h"
#include <algorithm>
#include "ChordScale.h"
#include "Phrase.hpp"


static std::random_device rd;
static std::mt19937 gen(rd());

std::mt19937 &getGen() {
    return gen;
}

int uniformInt(int min, int max) {
    if (min > max) {
        std::cout << "nonsensical random call";
        int temp = min;
        min = max;
        max = min;
    }
    int possibleNumbers = max - min + 1;
    int choice = ((rand() % possibleNumbers) + min);
    return choice;
}

double uniformDouble(double min, double max) {
    std::uniform_real_distribution<double> distribution(min, max);
    return distribution(gen);
}

int rollDie(int sides) {
    if (sides < 1) {
        std::cout << "nonsensical random call";
        return 0;
    }
    return uniformInt(1, sides);
};

bool flipCoin() {
    return uniformInt(0, 1);
}

bool flipWeightedCoin(double pTrue) {
    if (pTrue < 0 || pTrue > 1.0) {
        std::cout << "nonsensical probability";
    }
    return uniformInt(0, 9999) < (pTrue * 10000);
}

double boundedNormal(double min, double max, double thickness) { // thickness = 0-1
    if (min >= max || thickness < 0 || thickness > 1) {
        std::cout << "nonsensical random call";
    }
    double mean = (min + max) / 2.;
//    double denominator = (1 - thickness) * 5 + 1; // max is 6 ... 3 std devs to either side gives 99% of results
//    double stdDev = (max - min) / denominator;
    double stdDev = ((max - min) - 1) * thickness + 1;
    double choice = normal_distribution<double>(mean, stdDev)(gen);
    return std::max(std::min(choice, max), min);
}

double boundedNormal(double min, double max, double thickness, double skew) { // skew: 0.5 - 1 skews greater and 0 - 0.5 skews less than
    if (skew < 0 || skew > 1) {
        std::cout << "nonsensical skew value in random call";
    }
    double unskewed = boundedNormal(min, max, thickness);
    double skewed = (unskewed - min) * skew * 2. + min;
    return std::max(std::min(skewed, max), min);
}