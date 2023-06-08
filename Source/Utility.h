/*
  ==============================================================================

    Utility.h
    Created: 20 Mar 2023 7:48:00am
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once

#include <vector>
#include <set>

using std::vector, std::function, std::set;

template <typename T>
vector<T> filter(vector<T> toFilter, function<bool(T)> condition) {
    vector<T> result;
    copy_if(toFilter.begin(), toFilter.end(), back_inserter(result), condition);
    return result;
};

template <typename T, typename U>
vector<U> mapp(vector<T> const& toMap, function<U(T)> transformation) {
    vector<U> result;
    transform(toMap.begin(),
              toMap.end(),
              back_inserter(result),
              transformation);
    return result;
};

template <typename T>
vector<T> mapp(vector<T> toMap, function<T(T)> transformation) {
    return mapp<T, T>(toMap, transformation);
};

template <typename T>
vector<T*> toPointerVector(typename vector<T>::iterator begin,
                           typename vector<T>::iterator end) {
  vector<T*> result;
  while (begin < end) {
    result.push_back(&(*begin++));
  }
  return result;  
}

template <typename T>
vector<T*> toPointerVector(vector<T>& v) {
  return toPointerVector<T>(v.begin(), v.end());
}

template <typename T>
bool contains(vector<T> const& v, T const& e) {
  return std::find(v.begin(), v.end(), e) != v.end();
}
template <typename T>
bool contains(set<T> const& s, T const& e) {
  return std::find(s.begin(), s.end(), e) != s.end();
}
template <typename T>
void sort(vector<T>& v) {
  sort(v.begin(), v.end(),
    [](T const &a, T const &b) {
    return a < b;
  });
}
