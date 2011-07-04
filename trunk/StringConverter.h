#ifndef STRINGCONVERTER_H
#define STRINGCONVERTER_H

#include <string>
#include <sstream>

class StringConverter
{
public:
    static float StringConverter::parseFloat(const std::string& val, float defaultValue)
    {
        std::istringstream str(val);
        float ret = defaultValue;
        str >> ret;

        return ret;
    }

    static void StringConverter::parseFloat3(const std::string& val, float& x, float& y, float& z)
    {
        std::istringstream str(val);
        str >> x;
        str >> y;
        str >> z;
    }

    static int parseInt(const std::string& val, int defaultValue)
    {
        std::istringstream str(val);
        int ret = defaultValue;
        str >> ret;

        return ret;
    }

    static unsigned int parseUnsignedInt(const std::string& val, unsigned int defaultValue)
    {
        std::istringstream str(val);
        unsigned int ret = defaultValue;
        str >> ret;

        return ret;
    }

    static bool parseBool(const std::string& val, bool defaultValue)
    {
        if (val == "true" || val == "yes" || val == "1")
        {
            return true;
        } else if (val == "false" || val == "no" || val == "0")
        {
            return false;
        } else
        {
            return defaultValue;
        }
    }
};

#endif // STRINGCONVERTER_H
