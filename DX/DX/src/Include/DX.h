#pragma once

/**********************************************************************
    class: None (DX.h)

    author: S. Hau
    date: October 27, 2017

    Main include file when using the engine.
**********************************************************************/

#include "../stdafx.h"
#include "Core/locator.h"
#include "GameplayLayer/i_game.hpp"
#include "OS/FileSystem/virtual_file_system.h"
#include "Math/math_utils.h"
#include "Math/random.h"
#include "GameplayLayer/layers.hpp"
#include "Core/mesh_generator.h"
#include "Assets/environment_map.h"
#include "Assets/brdf_lut.h"
#include "Events/event_dispatcher.h"
#include "Common/string_utils.h"
#include "Graphics/VR/vr.h"
#include "Math/splines.h"