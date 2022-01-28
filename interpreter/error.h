#pragma once

#include <stdexcept>

struct NameError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct SyntaxError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
