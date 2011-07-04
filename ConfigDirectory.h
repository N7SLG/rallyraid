
#ifndef CONFIGDIRECTORY_H
#define CONFIGDIRECTORY_H

#include <irrlicht.h>
#include <list>
#include <string>

class ConfigDirectory
{
public:
    typedef std::list<irr::io::path> fileList_t;

    //ConfigFile();
    //virtual ~ConfigFile();
    /// load from a filename (not using resource group locations)
    static bool load(const irr::io::path& path, const irr::io::path& cfgFilename, fileList_t& fileList);
    static bool load(const irr::io::path& path, fileList_t& fileList);

    static bool mkdir(const std::string& directory);
};

#endif // CONFIGDIRECTORY_H
