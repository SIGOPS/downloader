#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <thread>


using namespace std::literals;

namespace {
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
            std::cout << Line(line) << Color(getURLFile(url)) << done << " of " << total << " bytes downloaded (" << int(total ? done*100./total : 0) << "%)" << std::flush;
            return 0;
        }));
        req.setOpt(cURLpp::Options::WriteFunction([&](const char* p, std::size_t size, std::size_t nmemb) {
            of.write(p, size*nmemb);
            return size*nmemb;
        }));
        req.perform();
    }
}


int main() {
    cURLpp::initialize();
    unsigned line = 1;

    for(const auto& p: {"http://i.imgur.com/Wt6xNSA.jpg"s,
                        "http://i.imgur.com/RxfpuNO.jpg"s,
                        "http://i.imgur.com/TTGRX5D.png"s,
                        "http://i.imgur.com/LCdmRya.png"s}) {
        Download(p, line++);
    }
}
