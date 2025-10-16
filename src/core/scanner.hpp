#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace rogue
{

struct ScanOptions
{
    std::string root;
    std::vector<std::string> includes;
    std::vector<std::string> excludes;
    int maxSizeMb{50};
    bool includeSecrets{false};
};

struct FileEntry
{
    std::string path;
    std::uintmax_t size{};
};

struct ScanResult
{
    bool ok{true};
    std::string inventoryJson;
    std::string errorMessage;
    std::vector<FileEntry> files;  // structured result
    std::uintmax_t totalSize{0};
};

class Logger;

ScanResult scan_workspace(const ScanOptions& options, Logger& logger);

}  // namespace rogue
