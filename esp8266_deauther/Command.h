#ifndef Command_h
#define Command_h

#include <stddef.h>
#include <functional>
#include <stdarg.h>
#include "Arduino.h"
#include "Argument.h"

class Command {
  public:
    Command* next = NULL;
    
    Command(String keyword, std::function<void(Command* command)> runFnct, std::function<void()> onError);
    ~Command();

    String getName();
    
    void exec(Command* command);
    bool equals(String keyword, int argNum, Argument* firstArg);
    
    void addOptArg(String argName, String altName, String defaultValue);
    void addReqArg(String argName, String altName, String defaultValue);
    void addArg(String argName, String altName, String defaultValue, bool required);
    void addArg(Argument* newArg);
    
    bool hasArg(String argName);
    Argument* getArg(String argName);
    
    bool has(String argName);
    String value(String argName);
    
    void resetArguments();
    void error();
  private:
    String keyword = "";
    int argNum = 0;
    Argument* firstArg = NULL;
    std::function<void(Command* command)> runFnct = NULL;
    std::function<void()> onError = NULL;
};

#endif
