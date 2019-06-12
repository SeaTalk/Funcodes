#pragma once

#include <string>
#include <exception>
#include <boost/filesystem.hpp>

struct path_incorrect_exception : std::exception {
    const char* what() const throw(){
        return "path incorrect";
    }
};

void list_path(const char* n);

const std::string flat_path(const char* n);
