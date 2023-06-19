/*
  ==============================================================================

    Random.h
    Created: 11 Oct 2022 10:15:25pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <random>
#include <iostream>


std::mt19937 &getGen();
int uniformInt(int min, int max);
double uniformDouble(double min, double max);
int rollDie(int sides);
bool flipCoin();
bool flipWeightedCoin(double pTrue);
double boundedNormal(double min, double max, double thickness = 0);
double boundedNormal(double min, double max, double thickness, double skew);

template <typename T>
T draw(std::vector<T> hat, std::vector<double> distribution = {}) {
    if (hat.size() == 0) {
         std::cout << "real problem here";
         return T(); // TODO: use optional
    }
    if (distribution.size() != hat.size()) {
        return hat[rollDie((int) hat.size()) - 1];
    }
    std::discrete_distribution<int> dist (distribution.begin(), distribution.end());
    return hat[dist(getGen())];
}
