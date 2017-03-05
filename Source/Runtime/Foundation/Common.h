// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_COMMON_H__
#define __FOUNDATION_COMMON_H__

#include <math.h>
#include <limits>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <errno.h>

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <unordered_map>
#include <deque>

using std::pair;
using std::vector;
using std::map;
using std::string;
using std::stringstream;
using std::unordered_map;
using std::deque;

#include "Platform/Platform.h"
#include "NonCopyable.h"
#include "Util.h"
#include "Debug/Assert.h"
#include "Debug/Logging.h"
#include "Memory/Memory.h"
#include "Memory/Allocator.h"
#include "Memory/StlAllocator.h"
#include "Memory/LinearAllocator.h"
#include "Memory/LinearAllocatorWithBuffer.h"
#include "Memory/MemoryPool.h"
#include "Memory/SharedPtr.h"

#include "Container/StringId.h"
#include "Container/StringUtil.h"
#include "Container/Str.h"
#include "Macros.h"


#endif // __FOUNDATION_COMMON_H__
