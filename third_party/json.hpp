// Minimal JSON shim compatible with a tiny subset of nlohmann::json API used in this project.
// This is NOT a full JSON library. Only features used here are implemented:
// - object and array construction
// - operator[](string) for objects
// - push_back for arrays
// - assignment from string, const char*, integer (uint64_t/int), and bool
// - dump(int indent = -1) serialization (basic, escapes quotes and backslashes)
// For production, replace with the real nlohmann/json single-header (MIT).

#pragma once
#include <cstdint>
#include <map>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

namespace nlohmann
{

class json
{
  public:
    enum class kind
    {
        null_t,
        boolean,
        number,
        string,
        object,
        array
    };
    using object_t = std::map<std::string, json>;
    using array_t = std::vector<json>;

    json() : data_(object_t{}) {}  // default to object
    static json array()
    {
        json j;
        j.data_ = array_t{};
        return j;
    }

    // element access for objects
    json& operator[](const std::string& key)
    {
        ensure_object();
        return std::get<object_t>(data_)[key];
    }

    // push for arrays
    void push_back(const json& v)
    {
        ensure_array();
        std::get<array_t>(data_).push_back(v);
    }

    // assignments
    json& operator=(const std::string& s)
    {
        data_ = s;
        return *this;
    }
    json& operator=(const char* s)
    {
        data_ = std::string(s);
        return *this;
    }
    json& operator=(uint64_t n)
    {
        data_ = static_cast<double>(n);
        return *this;
    }
    json& operator=(int n)
    {
        data_ = static_cast<double>(n);
        return *this;
    }
    json& operator=(bool b)
    {
        data_ = b;
        return *this;
    }

    // helpers for value() used in our code (not generic template)
    template <typename T>
    T value(const std::string& key, const T& def) const
    {
        if (!is_object())
            return def;
        const auto& obj = std::get<object_t>(data_);
        auto it = obj.find(key);
        if (it == obj.end())
            return def;
        return it->second.get<T>(def);
    }

    // dump
    std::string dump(int indent = -1) const
    {
        std::ostringstream os;
        serialize(os, *this, indent, 0);
        return os.str();
    }

  private:
    std::variant<std::nullptr_t, bool, double, std::string, object_t, array_t> data_{};

    bool is_object() const { return std::holds_alternative<object_t>(data_); }
    bool is_array() const { return std::holds_alternative<array_t>(data_); }
    void ensure_object()
    {
        if (!is_object())
            data_ = object_t{};
    }
    void ensure_array()
    {
        if (!is_array())
            data_ = array_t{};
    }

    template <typename T>
    T get(const T& def) const
    {
        return def;
    }

    bool get_bool(const bool& def) const
    {
        if (std::holds_alternative<bool>(data_))
            return std::get<bool>(data_);
        return def;
    }

    uint64_t get_uint64(const uint64_t& def) const
    {
        if (std::holds_alternative<double>(data_))
            return static_cast<uint64_t>(std::get<double>(data_));
        return def;
    }

    std::string get_string(const std::string& def) const
    {
        if (std::holds_alternative<std::string>(data_))
            return std::get<std::string>(data_);
        return def;
    }

    static void indent_os(std::ostringstream& os, int count)
    {
        for (int i = 0; i < count; i++)
            os << ' ';
    }
    static std::string escape(const std::string& s)
    {
        std::string out;
        out.reserve(s.size() + 4);
        for (char c : s)
        {
            switch (c)
            {
                case '"':
                    out += "\\\"";
                    break;
                case '\\':
                    out += "\\\\";
                    break;
                case '\n':
                    out += "\\n";
                    break;
                case '\r':
                    out += "\\r";
                    break;
                case '\t':
                    out += "\\t";
                    break;
                default:
                    out += c;
                    break;
            }
        }
        return out;
    }
    static void serialize(std::ostringstream& os, const json& j, int indent, int level)
    {
        if (std::holds_alternative<std::nullptr_t>(j.data_))
        {
            os << "null";
            return;
        }
        if (std::holds_alternative<bool>(j.data_))
        {
            os << (std::get<bool>(j.data_) ? "true" : "false");
            return;
        }
        if (std::holds_alternative<double>(j.data_))
        {
            os << (uint64_t)std::get<double>(j.data_);
            return;
        }
        if (std::holds_alternative<std::string>(j.data_))
        {
            os << '"' << escape(std::get<std::string>(j.data_)) << '"';
            return;
        }
        if (std::holds_alternative<object_t>(j.data_))
        {
            const auto& obj = std::get<object_t>(j.data_);
            os << '{';
            bool first = true;
            for (auto& kv : obj)
            {
                if (!first)
                    os << ',';
                if (indent >= 0)
                {
                    os << '\n';
                    indent_os(os, (level + 1) * indent);
                }
                os << '"' << escape(kv.first) << '"' << ':';
                if (indent >= 0)
                    os << ' ';
                serialize(os, kv.second, indent, level + 1);
                first = false;
            }
            if (!obj.empty() && indent >= 0)
            {
                os << '\n';
                indent_os(os, level * indent);
            }
            os << '}';
            return;
        }
        if (std::holds_alternative<array_t>(j.data_))
        {
            const auto& arr = std::get<array_t>(j.data_);
            os << '[';
            bool first = true;
            for (auto& el : arr)
            {
                if (!first)
                    os << ',';
                if (indent >= 0)
                {
                    os << '\n';
                    indent_os(os, (level + 1) * indent);
                }
                serialize(os, el, indent, level + 1);
                first = false;
            }
            if (!arr.empty() && indent >= 0)
            {
                os << '\n';
                indent_os(os, level * indent);
            }
            os << ']';
            return;
        }
    }
};

}  // namespace nlohmann

using nlohmann::json;
