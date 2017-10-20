#pragma once

/**********************************************************************
    class: FileSystem (file_system.h)

    author: S. Hau
    date: October 18, 2017

    Pure static class. Work with the file-system of the underlying
    OS. Some calls are OS dependant and have to be implemented for
    each OS. For working with a single file use the "File"-Class.
    Does not works in conjunction with the virtual file system.
**********************************************************************/

namespace Core { namespace OS {


    class FileSystem
    {
    public:
        //----------------------------------------------------------------------
        // @Params:
        //  "physicalPath": Physical path on disk.
        // @Return:
        //  True if the file exists on disk, otherwise false.
        //----------------------------------------------------------------------
        static bool exists(const char* physicalPath);

        //----------------------------------------------------------------------
        // @Params:
        //  "directory": Directory path on disk.
        // @Return:
        //  True if the directory exists on disk, otherwise false.
        //----------------------------------------------------------------------
        static bool dirExists(const char* directory);

        //----------------------------------------------------------------------
        // Create a new directory on disk.
        // @Params:
        //  "outputFolder": Folder on disk where to create the new directory in.
        //----------------------------------------------------------------------
        static void createDirectory(const char* outputFolder);
    };


} } // end namespaces