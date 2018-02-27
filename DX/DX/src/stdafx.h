#pragma once

/**********************************************************************
    class: None (stdafx.h)

    author: S. Hau
    date: October 3, 2017

    Use of precompiled header files to speed up compilation.
    Include here all header files which rarely change AND are often 
    used to gain a huge performance boost in compilation.
**********************************************************************/

#include <unordered_map>
#include <assert.h>
#include <cstddef>
#include <vector>
#include <memory>
#include <mutex>
#include <queue>
#include <stack>
#include <map>

#ifdef _WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef ERROR
    #include <DirectXMath.h>
#endif

#include "common.hpp"
#include "logging.h"
