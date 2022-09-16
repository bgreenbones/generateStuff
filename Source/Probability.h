/*
  ==============================================================================

    Probability.h
    Created: 27 Aug 2022 8:12:06pm
    Author:  Benjamin Greenwood

  ==============================================================================
*/

#pragma once



class Probability {
public:
    Probability();
    Probability(double value);
    Probability(Probability const&);
    Probability operator=(Probability other);
    Probability operator=(double other);
    bool operator>(Probability other);
    bool operator>(double other);
    bool operator>=(Probability other);
    bool operator>=(double other);
    bool operator==(Probability other);
    bool operator==(double other);
    bool operator<=(Probability other);
    bool operator<=(double other);
    bool operator<(Probability other);
    bool operator<(double other);
    Probability operator+(Probability other);
    Probability operator+(double other);
    Probability operator-(Probability other);
    Probability operator-(double other);
    Probability operator*(Probability other);
    Probability operator*(double other);
    Probability operator/(Probability other);
    Probability operator/(double other);
    explicit operator bool() const;
    operator double() const;
private:
    double guard(double value);
    double value;
};
