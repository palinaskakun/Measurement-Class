#pragma once

#include <string>
#include <iostream>
using std::ostream;
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>

/*
a class that keeps track of values, error, and units
for a measurement or calculation involving the same
*/
class Measurement {
    private:
    double value_;
    double uncertainty_;
    std::string unit_;
    //private member functions
    std::string MultiplyUnits (std::string const &, std::string const &);
    std::string DivideUnits (std::string const &, std::string const &);
    std::string RaiseToPower (std::string const &, int);

    std::string RaiseOneUnitToPower(std::string const &, int);
    std::vector<std::string> RaiseTwoOrMoreUnitsToPower(std::string const &, int);
    std::string MultDivOfDiffUnits (std::string const &, std::string const &);

    std::vector<std::string> split(const std::string &, char);
    std::string ltrim(const std::string &);
    std::string rtrim(const std::string &);
    std::string trim(const std::string &);
    void CheckFirstPower (std::string &);
    

    public:
    

    //constructors
    Measurement() = default;
    Measurement (double, double, std::string);

    //overloaded operators
    Measurement operator+(Measurement &);
    Measurement operator-(Measurement &);
    Measurement operator/(Measurement &);
    Measurement operator*(Measurement &);
    Measurement operator^(int);
    

    //friends 
    friend ostream &operator<<(ostream &, const Measurement &);

};
