#include "UtilsCommands.h"

ParsedCommand UtilsCommands::parse(const String &input)
{
    ParsedCommand out;
    String buff = input;
    buff.trim();

    if (buff.length() == 0)
        return out;

    int spaceIndex = buff.indexOf(' ');

    // Solo comando sin parámetros
    if (spaceIndex == -1)
    {
        out.command = buff.substring(0, min(8, buff.length()));
        out.command.toUpperCase();
        return out;
    }

    // Comando
    out.command = buff.substring(0, spaceIndex);
    out.command = out.command.substring(0, min(8, out.command.length()));
    out.command.toUpperCase();

    // Parametros
    String paramsPart = buff.substring(spaceIndex + 1);
    paramsPart.trim();

    if (paramsPart.length() == 0)
        return out;

    int start = 0;
    int idx;
    int count = 0;

    while ((idx = paramsPart.indexOf(',', start)) != -1 && count < 6)
    {
        out.params[count++] = paramsPart.substring(start, idx);
        start = idx + 1;
    }

    if (count < 6 && start < paramsPart.length())
        out.params[count++] = paramsPart.substring(start);

    out.paramCount = count;
    return out;
}

bool UtilsCommands::validate(const ParsedCommand &cmd, String &error)
{

    if (cmd.command.length() == 0)
    {
        error = "Comando vacío";
        return false;
    }

    if (cmd.command.length() > 8)
    {
        error = "Comando demasiado largo";
        return false;
    }

    for (int i = 0; i < cmd.command.length(); i++)
    {
        if (!isAlphaNumeric(cmd.command[i]))
        {
            error = "Comando contiene caracteres inválidos";
            return false;
        }
    }

    if (cmd.paramCount > 6)
    {
        error = "Demasiados parámetros";
        return false;
    }

    return true;
}
