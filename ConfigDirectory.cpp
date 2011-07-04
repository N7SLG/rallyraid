
#include "ConfigDirectory.h"
#include "TheGame.h"
#include "stdafx.h"

#include <direct.h>
#include <errno.h>

bool ConfigDirectory::load(const irr::io::path& path, const irr::io::path& cfgFilename, ConfigDirectory::fileList_t& fileList)
{
    irr::io::IFileSystem* fs = TheGame::getInstance()->getDevice()->getFileSystem();
    
    irr::io::path baseDir = fs->getWorkingDirectory();
    
    bool ret = fs->changeWorkingDirectoryTo(path);
    if (ret == false)
    {
        dprintf(MY_DEBUG_WARNING, "Unable to open directory: %s\n", path.c_str());
        return false;
    }
    
    irr::io::IFileList* fl = fs->createFileList();
    
    dprintf(MY_DEBUG_NOTE, "ConfigDirectory::list(): [%s] contains %u files\n", path.c_str(), fl->getFileCount());
    
    for (unsigned int i = 0; i < fl->getFileCount(); i++)
    {
        irr::io::path dir = fl->getFileName(i);
        if (fl->isDirectory(i) && dir[0] != '.')
        {
            if ((cfgFilename.size() > 0 && fs->existFile(dir+"/"+cfgFilename)) ||
                (cfgFilename.size() == 0 && fs->existFile(dir+"/"+dir+".cfg")))
            {
                dprintf(MY_DEBUG_NOTE, "\t+ [%s]\n", dir.c_str());
                fileList.push_back(dir);
            }
            else
            {
                dprintf(MY_DEBUG_NOTE, "\t- [%s]\n", dir.c_str());
            }
        }
        else
        {
            dprintf(MY_DEBUG_NOTE, "\t- [%s]\n", dir.c_str());
        }
    }    
    
    fs->changeWorkingDirectoryTo(baseDir);
    return true;
}

bool ConfigDirectory::load(const irr::io::path& path, ConfigDirectory::fileList_t& fileList)
{
    irr::io::IFileSystem* fs = TheGame::getInstance()->getDevice()->getFileSystem();
    
    irr::io::path baseDir = fs->getWorkingDirectory();
    
    bool ret = fs->changeWorkingDirectoryTo(path);
    if (ret == false)
    {
        dprintf(MY_DEBUG_WARNING, "Unable to open directory: %s\n", path.c_str());
        return false;
    }
    
    irr::io::IFileList* fl = fs->createFileList();
    
    dprintf(MY_DEBUG_NOTE, "ConfigDirectory::list(): [%s] contains %u files\n", path.c_str(), fl->getFileCount());
    
    for (unsigned int i = 0; i < fl->getFileCount(); i++)
    {
        irr::io::path file = fl->getFileName(i);
        if (!fl->isDirectory(i) && file[0] != '.')
        {
            dprintf(MY_DEBUG_NOTE, "\t+ [%s]\n", file.c_str());
            fileList.push_back(file);
        }
        else
        {
            dprintf(MY_DEBUG_NOTE, "\t- [%s]\n", file.c_str());
        }
    }    
    
    fs->changeWorkingDirectoryTo(baseDir);
    return true;
}

bool ConfigDirectory::mkdir(const std::string& directory)
{
    std::string dir = directory;
    int rc = _mkdir(dir.c_str());
    if (rc < 0)
    {
        int err = errno;
        if (err != EEXIST)
        {
            printf("unable to create directory: %s, errno: %d\n", dir.c_str(), err);
            return false;
        }
        else
        {
            dprintf(MY_DEBUG_NOTE, "directory already exist: %s\n", dir.c_str());
        }
    }
    return true;
}
