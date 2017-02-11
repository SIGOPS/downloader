#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <thread>
#include <vector>
#include <mutex>


using namespace std::literals;

namespace {
    std::mutex stdOutLock;
    unsigned ln = 1;
    auto Color(const std::string& s) {
        return "\x1B[32m" + s + ": " + "\x1B[0m";
    }

    auto Line(int l) {
        int m = l - ln;
        ln = l;
        return "\r"+(m<0?"\33["+std::to_string(-m)+'A':std::string(m,'\n'));
    }

    std::string getURLFile(const std::string& url) {
        const auto pos = url.find_last_of("/");
        return pos == std::string::npos ? url : url.substr(pos+1);
    }

    void Download(const std::string& url, unsigned line) {
        std::ofstream of(getURLFile(url));

        cURLpp::Easy req;
        req.setOpt(cURLpp::Options::Url(url));
        req.setOpt(cURLpp::Options::NoProgress(false));
        req.setOpt(cURLpp::Options::FollowLocation(true));
        req.setOpt(cURLpp::Options::ProgressFunction([&](std::size_t total, std::size_t done, auto...) {
            stdOutLock.lock();
            std::cout << Line(line) << Color(getURLFile(url)) << done << " of " << total << " bytes downloaded (" << int(total ? done*100./total : 0) << "%)" << std::flush;
            stdOutLock.unlock();
            return 0;
        }));
        req.setOpt(cURLpp::Options::WriteFunction([&](const char* p, std::size_t size, std::size_t nmemb) {
            of.write(p, size*nmemb);
            return size*nmemb;
        }));
        req.perform();
    }
}


int main(int argc, char* argv[]) {
    cURLpp::initialize();
    unsigned line = 1;

    if (argc < 2) {
        return 1;
    }
    std::vector<std::string> file;
    std::string line_f;
    file.clear();
    std::ifstream infile (argv[1], std::ios_base::in);
    while (getline(infile, line_f, '\n'))
    {
        file.push_back (line_f);
    }

    std::vector<std::thread> currDownloading;

    std::cout << "Read " << file.size() << " lines.\n";
    for(const auto& p: file) {
        currDownloading.emplace_back([p, l=line++] {
            Download(p, l);
        });
    }
    for(auto& p: currDownloading){
        p.join();
    }
}
