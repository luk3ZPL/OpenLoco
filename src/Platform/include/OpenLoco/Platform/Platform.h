#pragma once

#include <cstdint>
#include <vector>

#include <OpenLoco/Core/FileSystem.hpp>

namespace OpenLoco::Platform
{
    uint32_t getTime();
    fs::path getUserDirectory();
    fs::path promptDirectory(const std::string& title, void* hwnd);
    fs::path getCurrentExecutablePath();
    std::vector<fs::path> getDrives();
    bool isRunningInWine();
#if defined(__APPLE__) && defined(__MACH__)
    fs::path GetBundlePath();
#endif
}
