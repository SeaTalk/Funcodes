#include <iostream>
#include <string>
#include <utility>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>

#include "filesys.h"
using namespace boost::filesystem;

void list_path(const char* n) {
    path p(n);
    try {
        if (exists(p)) {
            if (is_regular_file(p)) {
                std::cout << "file: " << p << std::endl;
            } else if (is_directory(p)) {
                std::cout << "directory: " << p << std::endl;
                for (directory_entry& x : directory_iterator(p)) {
                    std::cout << "sub_path: " << x.path() << " is '" << (is_regular_file(x.path()) ? "file" : "dir") << "'" << std::endl;
                }
            } else {
                std::cout << p << " is unknown existance..." << std::endl;
            }
        }
    } catch (const filesystem_error& e) {
        std::cout << e.what() << std::endl;
    }
}

void print_error_type(const boost::regex_constants::error_type type) {
    switch (type) {
        case boost::regex_constants::error_collate:
            std::cout << "error type: collate\n"; break;
        case boost::regex_constants::error_ctype:
            std::cout << "error type: ctype\n"; break;
        case boost::regex_constants::error_escape:
            std::cout << "error type: escape\n"; break;
        case boost::regex_constants::error_backref:
            std::cout << "error type: backref\n"; break;
        case boost::regex_constants::error_brack:
            std::cout << "error type: brack\n"; break;
        case boost::regex_constants::error_paren:
            std::cout << "error type: paren\n"; break;
        case boost::regex_constants::error_brace:
            std::cout << "error type: brace\n"; break;
        case boost::regex_constants::error_badbrace:
            std::cout << "error type: badbrace\n"; break;
        case boost::regex_constants::error_range:
            std::cout << "error type: range\n"; break;
        case boost::regex_constants::error_space:
            std::cout << "error type: space\n"; break;
        case boost::regex_constants::error_badrepeat:
            std::cout << "error type: badrepeat\n"; break;
        case boost::regex_constants::error_complexity:
            std::cout << "error type: complexity\n"; break;
        case boost::regex_constants::error_stack:
            std::cout << "error type: stack\n"; break;
        default:
            std::cout << "don't know error type\n"; break;
    }
}

const std::string flat_path(const char* n) {
    std::string path(n);
    try {
        boost::regex res("\\/+");
        std::string rep("/");
        std::string result(256, 0);
    if (path[0] == '/') {
        *boost::regex_replace(result.begin(), path.begin() + 1, path.end(), res, rep) = '\0';
        path.replace(path.begin() + 1, path.end(), result.c_str());
    } else {
        *boost::regex_replace(result.begin(), path.begin(), path.end(), res, rep) = '\0';
        path.replace(path.begin(), path.end(), result.c_str());
    }
    std::cout << "format path: " << path << std::endl;
    boost::regex re("\\/[^/\\s]+\\/\\.\\.");
    boost::regex re2("[^/\\s]+\\/\\.\\.\\/");
    rep.assign("");
    while (path.find("..") != std::string::npos) {
        if (boost::regex_search(path, re2)) {
            path = boost::regex_replace(path, re2, rep, boost::regex_constants::format_first_only);
        } else if (boost::regex_search(path, re)) {
            path = boost::regex_replace(path, re, rep, boost::regex_constants::format_first_only);
        } else {
            throw path_incorrect_exception();
        }
    }
    boost::regex re3("\\/\\.");
    boost::regex re4("\\.\\/");
    while (path.find(".") != std::string::npos) {
        if (boost::regex_search(path, re3)) {
            path = boost::regex_replace(path, re3, rep);
        } else if (boost::regex_search(path, re4)) {
            path = boost::regex_replace(path, re4, rep);
        } else {
            throw path_incorrect_exception();
        }
    }
std::cout << "cutted path:" << path << std::endl;
    } catch (const boost::regex_error& e) {
        print_error_type(e.code());
    }
    return std::move(path);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " ${path}\n";
        return -1;
    }
    list_path(argv[1]);
    return 0;
}
