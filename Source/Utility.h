/*
  ==============================================================================

    Utility.h
    Created: 20 Mar 2023 7:48:00am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>

using std::vector, std::function;

template <typename T>
vector<T> filter(vector<T> toFilter, function<bool(T)> condition) {
    vector<T> result;
    copy_if(toFilter.begin(), toFilter.end(), back_inserter(result), condition);
    return result;
};

