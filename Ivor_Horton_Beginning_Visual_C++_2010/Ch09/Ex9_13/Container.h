// Container.h for Ex9_13


#pragma once
#include <iostream>
using std::cout;
using std::endl;

class CContainer        // Generic base class for specific containers
{
  public:
    // Destructor
    virtual ~CContainer()
    { cout << "CContainer destructor called" << endl; }

    // Function for calculating a volume - no content
    // This is defined as a 'pure' virtual function, signified by '= 0'
    virtual double Volume() const = 0;

    // Function to display a volume
    virtual void ShowVolume() const
    {
      cout << endl
           << "Volume is " << Volume();
    }
};
