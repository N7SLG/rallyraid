#include "ConfigFile.h"
#include "stdafx.h"

#include <fstream>

//-----------------------------------------------------------------------
ConfigFile::ConfigFile()
{
}
//-----------------------------------------------------------------------
ConfigFile::~ConfigFile()
{
    SettingsBySection::iterator seci, secend;
    secend = mSettings.end();
    for (seci = mSettings.begin(); seci != secend; ++seci)
    {
        delete seci->second;
    }
}
//-----------------------------------------------------------------------
void ConfigFile::clear(void)
{
    for (SettingsBySection::iterator seci = mSettings.begin(); 
        seci != mSettings.end(); ++seci)
    {
            delete seci->second;
    }
    mSettings.clear();
}
//-----------------------------------------------------------------------
void ConfigFile::load(const std::string& filename, const std::string& separators, bool trimWhitespace)
{
    loadDirect(filename, separators, trimWhitespace);
}
//-----------------------------------------------------------------------
void ConfigFile::loadDirect(const std::string& filename, const std::string& separators, 
    bool trimWhitespace)
{
    /* Open the configuration file */
    std::ifstream fp;
    // Always open in binary mode
    fp.open(filename.c_str(), std::ios::in | std::ios::binary);
    if(!fp)
    {
        dprintf(MY_DEBUG_ERROR, "ConfigFile::loadDirect(): error loading file: %s\n", filename.c_str());
        return;
    }

    load(fp, separators, trimWhitespace);

    fp.close();
}
//-----------------------------------------------------------------------
void ConfigFile::load(std::ifstream& stream, const std::string& separators, bool trimWhitespace)
{
    /* Clear current settings map */
    clear();

    std::string currentSection = "";
    SettingsMultiMap* currentSettings = new SettingsMultiMap();
    mSettings[currentSection] = currentSettings;

    /* Process the file line for line */
    std::string line, optName, optVal;
    char buffer[2048];

    while (!stream.eof())
    {
        stream.getline(buffer, 2048);
        line = buffer;
        trim(line);
        /* Ignore comments & blanks */
        if (line.length() > 0 && line.at(0) != '#' && line.at(0) != '@')
        {
            if (line.at(0) == '[' && line.at(line.length()-1) == ']')
            {
                // Section
                currentSection = line.substr(1, line.length() - 2);
                SettingsBySection::const_iterator seci = mSettings.find(currentSection);
                if (seci == mSettings.end())
                {
                    currentSettings = new SettingsMultiMap();
                    mSettings[currentSection] = currentSettings;
                }
                else
                {
                    currentSettings = seci->second;
                } 
            }
            else
            {
                /* Find the first seperator character and split the string there */
                std::string::size_type separator_pos = line.find_first_of(separators, 0);
                if (separator_pos != std::string::npos)
                {
                    optName = line.substr(0, separator_pos);
                    /* Find the first non-seperator character following the name */
                    std::string::size_type nonseparator_pos = line.find_first_not_of(separators, separator_pos);
                    /* ... and extract the value */
                    /* Make sure we don't crash on an empty setting (it might be a valid value) */
                    optVal = (nonseparator_pos == std::string::npos) ? "" : line.substr(nonseparator_pos);
                    if (trimWhitespace)
                    {
                        trim(optVal);
                        trim(optName);
                    }
                    currentSettings->insert(SettingsMultiMap::value_type(optName, optVal));
                }
            }
        }
    }
}
//-----------------------------------------------------------------------
std::string ConfigFile::getSetting(const std::string& key, const std::string& section, const std::string& defaultValue) const
{
    SettingsBySection::const_iterator seci = mSettings.find(section);
    if (seci == mSettings.end())
    {
        return defaultValue;
    }
    else
    {
        SettingsMultiMap::const_iterator i = seci->second->find(key);
        if (i == seci->second->end())
        {
            return defaultValue;
        }
        else
        {
            return i->second;
        }
    }
}
//-----------------------------------------------------------------------
ConfigFile::SettingsIterator ConfigFile::getSettingsIterator(const std::string& section)
{
    SettingsBySection::const_iterator seci = mSettings.find(section);
    if (seci == mSettings.end())
    {
        assert(0 && "Cannot find section");
        return SettingsIterator(seci->second->begin(), seci->second->end());
    }
    else
    {
        return SettingsIterator(seci->second->begin(), seci->second->end());
    }
}
//-----------------------------------------------------------------------
ConfigFile::SectionIterator ConfigFile::getSectionIterator(void)
{
    return SectionIterator(mSettings.begin(), mSettings.end());
}

void ConfigFile::trim(std::string& str, bool left, bool right)
{
    /*
    size_t lspaces, rspaces, len = length(), i;

    lspaces = rspaces = 0;

    if( left )
    {
        // Find spaces / tabs on the left
        for( i = 0;
            i < len && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
            ++lspaces, ++i );
    }
        
    if( right && lspaces < len )
    {
        // Find spaces / tabs on the right
        for( i = len - 1;
            i >= 0 && ( at(i) == ' ' || at(i) == '\t' || at(i) == '\r');
            rspaces++, i-- );
    }

    *this = substr(lspaces, len-lspaces-rspaces);
    */
    static const std::string delims = " \t\r";
    if(right)
        str.erase(str.find_last_not_of(delims)+1); // trim right
    if(left)
        str.erase(0, str.find_first_not_of(delims)); // trim left
}
