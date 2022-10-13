/*
  ==============================================================================

    Random.h
    Created: 11 Oct 2022 10:15:25pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <random>


std::mt19937 &getGen();
int uniformInt(int min, int max);
int rollDie(int sides);
bool flipCoin();
bool flipWeightedCoin(double pTrue);
double boundedNormal(double min, double max, double thickness = 0);

template <class T>
T draw(std::vector<T> hat);
