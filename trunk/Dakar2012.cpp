// Dakar2012.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TheGame.h"

#ifndef __linux__
#include <Windows.h>
#endif

//int _tmain(int argc, _TCHAR* argv[])
int main()
{
//    try
//    {
        TheGame::initialize();

        TheGame::getInstance()->loop();

        TheGame::finalize();
/*    } catch (Ogre::Exception& e)
    {
        printf("FATAL EXCEPTION: %s\n", e.getFullDescription().c_str());
#ifndef __linux__
        MessageBox(NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#endif
    }
*/  
     return 0;
}

