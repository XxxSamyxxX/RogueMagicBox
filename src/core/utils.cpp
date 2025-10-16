#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

// minimal nlohmann json
#include "../../third_party/json.hpp"

namespace fs = std::filesystem;

namespace rogue
{
    namespace utils
    {

        std::string iso_timestamp()
        {
            using namespace std::chrono;
            auto now = system_clock::now();
            std::time_t t = system_clock::to_time_t(now);
            std::tm tm{};
#ifdef _WIN32
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
            return std::string(buf);
        }

        std::string file_mtime_iso(const fs::path &p)
        {
            try
            {
                auto ftime = fs::last_write_time(p);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now());
                std::time_t t = std::chrono::system_clock::to_time_t(sctp);
                char buf[32];
                std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&t));
                return buf;
            }
            catch (...)
            {
                return "";
            }
        }

        std::vector<std::string> load_ignore_patterns(const fs::path &file)
        {
            std::vector<std::string> out;
            std::ifstream f(file);
            if (!f)
                return out;
            std::string line;
            while (std::getline(f, line))
            {
                if (line.empty() || line[0] == '#')
                    continue;
                out.push_back(line);
            }
            return out;
        }

        static bool glob_match_simple(const std::string &pattern, const std::string &text)
        {
            // supports '*' wildcard only for simplicity
            size_t p = 0, t = 0, star = std::string::npos, match = 0;
            while (t < text.size())
            {
                if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == text[t]))
                {
                    p++;
                    t++;
                }
                else if (p < pattern.size() && pattern[p] == '*')
                {
                    star = p++;
                    match = t;
                }
                else if (star != std::string::npos)
                {
                    p = star + 1;
                    t = ++match;
                }
                else
                    return false;
            }
            while (p < pattern.size() && pattern[p] == '*')
                p++;
            return p == pattern.size();
        }

        bool is_ignored(const std::string &relPath, const std::vector<std::string> &patterns)
        {
            for (auto &p : patterns)
            {
                if (glob_match_simple(p, relPath) || glob_match_simple(p, std::filesystem::path(relPath).filename().string()))
                    return true;
            }
            return false;
        }

        size_t ifind(const std::string &hay, const std::string &needle)
        {
            auto it = std::search(hay.begin(), hay.end(), needle.begin(), needle.end(), [](char a, char b)
                                  { return std::tolower(a) == std::tolower(b); });
            if (it == hay.end())
                return std::string::npos;
            return static_cast<size_t>(it - hay.begin());
        }

        // Very small SHA256 implementation placeholder using std::hash as fallback (not cryptographic)
        // For workshop demo, acceptable; in production use a proper SHA256.
        std::string sha256_file(const std::string &filepath)
        {
            std::ifstream f(filepath, std::ios::binary);
            if (!f)
                return "";
            std::ostringstream ss;
            ss << f.rdbuf();
            std::string content = ss.str();
            std::hash<std::string> h;
            auto v = h(content);
            std::ostringstream hs;
            hs << std::hex << v;
            return hs.str();
        }

        int system_in_dir(const std::string &dir, const std::string &cmd, bool hide_output)
        {
#ifdef _WIN32
            // Do not wrap the whole command in quotes; quote only the path
            std::string full = "cmd /c cd /d \"" + dir + "\" && " + cmd;
            return system(full.c_str());
#else
            std::string full = "bash -lc 'cd " + dir + " && " + cmd + "'";
            if (hide_output)
                full += " > /dev/null 2>&1";
            return system(full.c_str());
#endif
        }

        std::string read_github_token()
        {
            const char *env = std::getenv("GITHUB_TOKEN");
            if (env && *env)
                return std::string(env);
            // Simple gh config check omitted for brevity
            return "";
        }

        std::string getenv(const std::string &key, const std::string &def)
        {
            const char *v = std::getenv(key.c_str());
            if (v && *v)
                return std::string(v);
            return def;
        }

        void ensure_file_with_content(const fs::path &p, const std::string &content)
        {
            if (fs::exists(p))
                return;
            fs::create_directories(p.parent_path());
            std::ofstream f(p);
            if (f)
                f << content;
        }

    }
}
