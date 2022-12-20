#include "main.h"
#include <string>
#include <iostream>
using std::ostream;
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric> 
#include <stdexcept>
#include <math.h>

Measurement::Measurement (double value, double uncertainty, std::string unit)
{
    value_ = value;
    uncertainty_ = uncertainty;
    unit_ = unit;
}
Measurement Measurement::operator+(Measurement &m)
{
    
    if (this->unit_ != m.unit_)
    {
        throw std::invalid_argument("Units don't match");
    }
    else
    {
        Measurement measurement;
        measurement.value_ = this->value_+m.value_;
        measurement.unit_=m.unit_;
        measurement.uncertainty_ = sqrt (this->uncertainty_*this->uncertainty_ 
            + m.uncertainty_*m.uncertainty_);
        return measurement;
    }
    
}
Measurement Measurement::operator-(Measurement &m)
{
    
    if (this->unit_ != m.unit_)
    {
        throw std::invalid_argument("Units don't match");
    }
    else
    {
        Measurement measurement;
        measurement.value_ = this->value_ - m.value_;
        measurement.uncertainty_ = sqrt (this->uncertainty_ * this->uncertainty_ 
            + m.uncertainty_ * m.uncertainty_);
        measurement.unit_ = m.unit_;
        return measurement;
    }
    
}
Measurement Measurement::operator/(Measurement &m)
{
    Measurement measurement;
    measurement.value_ = this->value_ / m.value_;
    measurement.uncertainty_ = sqrt ( pow(this->uncertainty_ / this->value_ * 100, 2) 
        + pow(m.uncertainty_ / m.value_ * 100, 2)) * (measurement.value_/100.0);
    measurement.unit_ = trim(DivideUnits(this->unit_, m.unit_));
    CheckFirstPower(measurement.unit_);
    return measurement;
}
Measurement Measurement::operator*(Measurement &m)
{

    Measurement measurement;
    measurement.value_ = this->value_ * m.value_;
    measurement.uncertainty_ = sqrt ( pow(this->uncertainty_ / this->value_ * 100, 2) 
        + pow(m.uncertainty_ / m.value_ * 100, 2)) * (measurement.value_/100.0);
    measurement.unit_ = trim(MultiplyUnits(this->unit_, m.unit_));
    CheckFirstPower(measurement.unit_);
    return measurement;
}
std::vector<std::string> Measurement::split(const std::string &s, char sep)
{
    std::stringstream ss(s);
    std::string str;
    std::vector <std::string> v{};
    while (getline(ss, str, sep))
    {
        v.push_back(str);
    }
    return v;
}

std::string Measurement::MultiplyUnits (std::string const &unit1, std::string const & unit2)
{
    //unit 2 not in unit1
    std::string result ="";
    if (unit1.find(unit2)==std::string::npos)
    {
        return MultDivOfDiffUnits(unit1, unit2);
    }
    else //when u2 in u1
    {
        //a whitespace found meaning there's at least 2 units in a str
        if (unit1.find(" ")!=std::string::npos)
        {
            //vector of unit1 split by whitespace 
            std::vector<std::string> vect_unit = split(unit1, ' ');
            for (auto &s: vect_unit)
            {
                if (s.find (unit2) != std::string::npos )
                {
                    if (s.find("^")== std::string::npos )
                    {
                        s+="^2";
                    }
                    else
                    {
                        if (s.find("-")!=std::string::npos)
                        {
                            int pw = std::stoi(s.substr(s.find("-")+1))*(-1)+1;
                            s= s.substr(0, s.find("^"))+"^"+std::to_string(pw);
                        }
                        else
                        {
                            int pw = std::stoi(s.substr(s.find("^")+1))+1;
                            s= s.substr(0, s.find("^"))+"^"+std::to_string(pw);
                        }
                    }
                }
            }
            std::sort(vect_unit.begin(), vect_unit.end());
            return std::accumulate (vect_unit.begin(), vect_unit.end(), result, 
                [] (std::string s1, std::string s2) { return s1+" "+s2;} );
        }
        else //no whitespace => only one unit
        {
            //could have power or no power
            //no power
            if (unit1.find("^")== std::string::npos )
            {
                return unit1+"^2";
            }
            // raised to some power already
            if (unit1.find("-")!=std::string::npos)
            {
                int pw = std::stoi(unit1.substr(unit1.find("-")+1))*(-1)+1;
                return unit1.substr(0, unit1.find("^"))+"^"+std::to_string(pw);
            }
            int pw = std::stoi(unit1.substr(unit1.find("^")+1))+1;
            return unit1.substr(0, unit1.find("^"))+"^"+std::to_string(pw);
        }
    }
}

std::string Measurement::MultDivOfDiffUnits (std::string const &unit1, std::string const &unit2)
{
    std::string result ="";
    //a whitespace found meaning there's at least 2 units in a str
    if (unit1.find(" ")!=std::string::npos)
    {
        //vector of unit1 split by whitespace 
        std::vector<std::string> vect_unit = split(unit1, ' ');
        vect_unit.push_back(unit2);
        std::sort(vect_unit.begin(), vect_unit.end());
        return std::accumulate (vect_unit.begin(), vect_unit.end(), result, 
            [] (auto s1, auto s2) { return s1+" "+s2;} );
    }
    //no whitespace => only one unit 
    if (unit1<unit2)
    {
        return unit1+" "+unit2;
    }
    return unit2+" "+unit1;
}
std::string Measurement::DivideUnits (std::string const &unit1, std::string const &unit2)
{
    std::string result ="";
    //unit2 not in unit1
    if (unit1.find(unit2)==std::string::npos)
    {
        return MultDivOfDiffUnits(unit1, unit2+"^-1");
    }

    else //when u2 in u1
    {
        //a whitespace found meaning there's at least 2 units in a str
        if (unit1.find(" ")!=std::string::npos)
        {
            std::vector<std::string> vect_unit = split(unit1, ' ');
            for (auto &s: vect_unit)
            {
                if (s.find (unit2) != std::string::npos )
                {
                    if (s.find("^")== std::string::npos )
                    {
                        s="";
                    }
                    else //check for positive /negative power
                    {
                        if (s.find("-")!=std::string::npos)
                        {
                            int pw = std::stoi(s.substr(s.find("-")+1))*(-1)-1;
                            s= s.substr(0, s.find("^"))+"^"+std::to_string(pw);
                        }
                        else
                        {
                            int pw = std::stoi(s.substr(s.find("^")+1))-1;
                            s= s.substr(0, s.find("^"))+"^"+std::to_string(pw);
                        }
                    }
                }
            }
            std::sort(vect_unit.begin(), vect_unit.end());
            return std::accumulate (vect_unit.begin(), vect_unit.end(), result, 
                [] (auto s1, auto s2) { return s1+" "+s2;} );
        }
        else //no whitespace => only one unit
        {
            //could have power or no power (power 1), this handles power 1
            if (unit1.find("^")== std::string::npos )
            {
                return ""; //division of the same unit gets rid of the unit
            }
            // raised to some power already
            if (unit1.find("-")!=std::string::npos) //check for negative power
            {
                int pw = std::stoi(unit1.substr(unit1.find("-")+1))*(-1)-1;
                return unit1.substr(0, unit1.find("^"))+"^"+std::to_string(pw);
            }
            int pw = std::stoi(unit1.substr(unit1.find("^")+1))-1;
            return unit1.substr(0, unit1.find("^"))+"^"+std::to_string(pw);
        }
    }
}

std::string Measurement::RaiseOneUnitToPower(std::string const & unit, int raise)
{
    if (unit.find("^")==std::string::npos) //if current power is 1 
        {
            return unit+"^"+std::to_string(raise);
        }
        else //if some power is already present 
        {
            if (unit.find("-")!=std::string::npos) // handle negative pow
            {
                int power = std::stoi(unit.substr(unit.find("-")+1))*(-1)*raise;
                return unit.substr(0, unit.find("^"))+"^"+std::to_string(power);
            }
            else //handle positive power
            {
                int power = std::stoi(unit.substr(unit.find("^")+1))*raise;
                return unit.substr(0, unit.find("^"))+"^"+std::to_string(power);
            }
        }

}

std::vector<std::string> Measurement::RaiseTwoOrMoreUnitsToPower(std::string const & unit, int raise)
{
    std::vector<std::string> vect_unit = split(unit, ' ');
    for (auto &s: vect_unit)
    {
        if (s.find("^")==std::string::npos) //if current power is 1 
        {
            s= s+"^"+std::to_string(raise);
        }
        else //if some power is already present 
        {
            if (s.find("-")!=std::string::npos) // handle negative pow
            {
                int power = std::stoi(s.substr(s.find("-")+1))*(-1)*raise;
                s=s.substr(0, s.find("^"))+"^"+std::to_string(power);
            }
            else //handle positive power
            {
                int power = std::stoi(s.substr(s.find("^")+1))*raise;
                s=s.substr(0, s.find("^"))+"^"+std::to_string(power);
            }
        }
    }
    return vect_unit;
}

std::string Measurement::RaiseToPower (std::string const & unit, int raise)
{
    std::string result ="";
    if (unit.find(" ")==std::string::npos) //only one unit in a str
    {
        return RaiseOneUnitToPower(unit, raise);   
    }
    else //two or more units are present in a unit string
    {
        std::vector<std::string> vect_unit = RaiseTwoOrMoreUnitsToPower(unit, raise);

        std::sort(vect_unit.begin(), vect_unit.end());
        return std::accumulate (vect_unit.begin(), vect_unit.end(), result, 
            [] (std::string s1, std::string s2) { return s1+" "+s2;} );
    }
}

//took the series of trim funcs from here
// https://www.techiedelight.com/trim-string-cpp-remove-leading-trailing-spaces/
std::string Measurement::ltrim(const std::string &s)
{
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == std::string::npos) ? "" : s.substr(start);
}
std::string Measurement::rtrim(const std::string &s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
std::string Measurement::trim(const std::string &s) {
    return rtrim(ltrim(s));
}

Measurement Measurement::operator^(int power)
{
    Measurement measurement;

    measurement.value_ = pow(this->value_, power);
    measurement.uncertainty_ = abs(power)*(this->uncertainty_ 
                    / this->value_*measurement.value_);
    measurement.unit_ = trim(RaiseToPower(this->unit_, power));
    CheckFirstPower(measurement.unit_);
    return measurement;
}

void Measurement::CheckFirstPower (std::string & unit)
{
    if (unit.find("^1")!=std::string::npos)
    {
        unit= unit.substr(0, 
            unit.find("^1"));
    }
}

// << operator with correct formatting 
ostream &operator<<(ostream & oss, const Measurement &c)
{

    oss.precision(1);
    oss << std::scientific;
    oss.precision(2);
    oss << c.value_ << " +- "<<c.uncertainty_<< " "<<c.unit_<< " ";
    oss.unsetf(std::ios::fixed | std::ios::scientific);
    oss.precision(5);
    return oss;
}

 
int main()
{
    std::ostringstream oss;

    Measurement m_1(0.20, 0.01, "s");
    Measurement m_2 = m_1 ^ 2;
    oss << m_2 << std::endl;


    Measurement m_3(34, 1, "m");
    Measurement m_4(16, 0.9, "m");
    Measurement m_x(13.5, 2.1, "m");
    Measurement m_5 = (m_3 + m_4 - m_x) * m_2;
    oss << m_5 << std::endl;

    oss << (m_5 ^ -1) << std::endl;
    oss << (m_5 ^ 3) << std::endl;

    Measurement m_a(34, 1, "a");
    Measurement m_b(34, 1, "bb");
    Measurement m_c(34, 1, "c");
    Measurement m_d(34, 1, "d");
    oss << (m_5 / m_3 * m_a * m_a / m_b / m_c * m_d) << std::endl;


    Measurement m_6(13.4, 1.2, "eggs");
    oss << (m_6 * m_6 / m_6) << std::endl;
    std::string k = oss.str();
    std::cout << k << '\n';

    
}
