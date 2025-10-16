#pragma once
#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

namespace rogue
{
    namespace logfmt
    {
        inline std::string to_json_line(const std::string &level, const std::string &ctx, const std::string &msg, const std::map<std::string, std::string> &kv)
        {
            nlohmann::json j;
            j["level"] = level;
            j["ctx"] = ctx;
            j["msg"] = msg;
            for (auto &p : kv)
                j[p.first] = p.second;
            return j.dump();
        }
    }
}
