#pragma once

/**********************************************************************
    class: None (macros.hpp)

    author: S. Hau
    date: October 3, 2017

    Common macros used throughout whole program.
**********************************************************************/

#define TS(s)                   std::to_string(s)
#define ASSERT(exp)             assert(exp)

#ifdef _DEBUG
    #define NEW                 new(__FILE__, __LINE__)
#else
    #define NEW                 new
#endif