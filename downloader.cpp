#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include <thread>


using namespace std::literals;

namespace {
    void Download(const std::string& url, const std::string& filename) {
        std::ofstream of(filename);

        cURLpp::Easy req;
        req.setOpt(cURLpp::Options::Url(url));
        req.setOpt(cURLpp::Options::NoProgress(false));
        req.setOpt(cURLpp::Options::FollowLocation(true));
        req.setOpt(cURLpp::Options::ProgressFunction([&](std::size_t total, std::size_t done, auto...) {
            std::cout << "\r" << done << " of " << total << " bytes downloaded (" << int(total ? done*100./total : 0) << "%)" << std::flush;
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
    Download("http://i.imgur.com/kcM5x79.jpg"s, "two.jpg"s);
}
