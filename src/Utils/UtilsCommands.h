#ifndef UTILS_COMMANDS_H
#define UTILS_COMMANDS_H

#include <Arduino.h>

struct ParsedCommand
{
    String command;     // Comando en mayúsculas
    String params[6];   // Max 6 parámetros
    int paramCount = 0; // Cantidad de parámetros
};

class UtilsCommands
{
public:
    static ParsedCommand parse(const String &input);
    static bool validate(const ParsedCommand &cmd, String &error);
};

#endif
