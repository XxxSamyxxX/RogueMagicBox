#include "github_api.hpp"
#include "logger.hpp"
#include "utils.hpp"
#ifdef HAVE_LIBCURL
#include <curl/curl.h>
#endif
#include <sstream>

namespace rogue
{

    bool GitHubApi::create_repo_via_api(const std::string &repoName, const std::optional<std::string> &org, bool isPrivate, const std::string &token)
    {
#ifndef HAVE_LIBCURL
        (void)repoName;
        (void)org;
        (void)isPrivate;
        (void)token;
        logger_.warn("github", "libcurl not available; cannot use API");
        return false;
#else
        std::string url = org ? ("https://api.github.com/orgs/" + *org + "/repos") : "https://api.github.com/user/repos";
        std::string payload = std::string("{\"name\":\"") + repoName + "\",\"private\":" + (isPrivate ? "true" : "false") + "}";

        CURL *curl = curl_easy_init();
        if (!curl)
        {
            logger_.error("github", "curl init failed");
            return false;
        }
        struct curl_slist *headers = nullptr;
        std::string auth = std::string("Authorization: token ") + token;
        headers = curl_slist_append(headers, auth.c_str());
        headers = curl_slist_append(headers, "User-Agent: RogueMagicBox");
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
                         {
        (void)userdata; return size*nmemb; });

        auto res = curl_easy_perform(curl);
        long code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        if (res != CURLE_OK || code >= 300)
        {
            logger_.error("github", "API create repo failed", {{"http", std::to_string(code)}});
            return false;
        }
        logger_.info("github", "Repo created via API");
        return true;
#endif
    }

    bool GitHubApi::create_repo_via_gh(const std::filesystem::path &workdir, const std::string &repoName, const std::string &org, bool isPrivate)
    {
        // If repo already exists, skip creation
        {
            std::ostringstream check;
#ifdef _WIN32
            check << "cmd /c gh repo view ";
#else
            check << "gh repo view ";
#endif
            if (!org.empty())
                check << org << "/";
            check << repoName;
            int exists = utils::system_in_dir(workdir.string(), check.str(), true);
            if (exists == 0)
            {
                logger_.info("github", "Repo already exists on GitHub; skipping creation");
                return true;
            }
        }

        std::ostringstream cmd;
#ifdef _WIN32
        cmd << "cmd /c gh repo create ";
#else
        cmd << "gh repo create ";
#endif
        if (!org.empty())
            cmd << org << "/";
        cmd << repoName << " --confirm ";
        cmd << (isPrivate ? "--private" : "--public");
        int code = utils::system_in_dir(workdir.string(), cmd.str(), true);
        if (code != 0)
        {
            logger_.error("github", "gh repo create failed", {{"code", std::to_string(code)}});
            return false;
        }
        logger_.info("github", "Repo created via gh CLI");
        return true;
    }

}
