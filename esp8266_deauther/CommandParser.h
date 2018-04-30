#ifndef CommandParser_h
#define CommandParser_h

#include <stddef.h>
#include "Arduino.h"
#include "Command.h"

class CommandParser {
  public:
    CommandParser();
    ~CommandParser();
    
    void parse(String input);
    void parseLines(String str);
    
    void addCommand(Command* newCommand);

    std::function<void(String cmdName)> onNotFound = NULL;
    std::function<void(String invalidArgument)> onParseError = NULL;
  private:
    Command* firstCommand = NULL;
};

#endif
