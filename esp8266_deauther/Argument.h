#ifndef Argument_h
#define Argument_h

#include <stddef.h>
#include "Arduino.h"

class Argument {
  public:
    Argument* next = NULL;
    
    Argument(String argName, String altName, String defaultValue, bool required);
    ~Argument();

    String getArgName();
    String getAltName();
    String getValue();
    bool wasGiven();
    bool isRequired();
    
    void setValue(String value);
    
    void reset(); 
  private:
    String argName = "";
    String altName = "";
    String defaultValue = "";
    String value = "";
    bool given = false;
    bool required = false;
};

#endif
