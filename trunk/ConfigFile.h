
#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <map>
#include <iostream>
#include "IteratorWrapper.h"

class ConfigFile
{
public:

    ConfigFile();
    virtual ~ConfigFile();
    /// load from a filename (not using resource group locations)
    void load(const std::string& filename, const std::string& separators = "\t:=", bool trimWhitespace = true);
    /// load from a data stream
    void load(std::ifstream& stream, const std::string& separators = "\t:=", bool trimWhitespace = true);
	/// load from a filename (not using resource group locations)
	void loadDirect(const std::string& filename, const std::string& separators = "\t:=", bool trimWhitespace = true);

    /** Gets the first setting from the file with the named key. 
    @param key The name of the setting
    @param section The name of the section it must be in (if any)
	@param defaultValue The value to return if the setting is not found
    */
    std::string getSetting(const std::string& key, const std::string& section = "", const std::string& defaultValue = "") const;

    typedef std::multimap<std::string, std::string> SettingsMultiMap;
    typedef MapIterator<SettingsMultiMap> SettingsIterator;
    /** Gets an iterator for stepping through all the keys / values in the file. */
    typedef std::map<std::string, SettingsMultiMap*> SettingsBySection;
    typedef MapIterator<SettingsBySection> SectionIterator;
    /** Get an iterator over all the available sections in the config file */
    SectionIterator getSectionIterator(void);
    /** Get an iterator over all the available settings in a section */
    SettingsIterator getSettingsIterator(const std::string& section = "");

    size_t getSectionCount() const; // inline

    /** Clear the settings */
    void clear(void);
protected:
    void trim(std::string& str, bool left = true, bool right = true);
    
    SettingsBySection mSettings;
};

inline size_t ConfigFile::getSectionCount() const
{
    return mSettings.size();
}

#endif // CONFIGFILE_H
