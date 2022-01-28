#pragma once
#include <error.h>
#include <memory>
#include <vector>
#include <sstream>
#include <map>

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual ~Object() = default;
    virtual std::string StringRepresentation() {
        throw RuntimeError("Unknown object type");
    }
};

class Number : public Object {
public:
    Number() = default;
    Number(int value) : value_(value) {
    }
    int GetValue() const {
        return value_;
    }
    virtual std::string StringRepresentation() override {
        return std::to_string(value_);
    }

private:
    int value_;
};

class Symbol : public Object {
public:
    Symbol() = default;
    Symbol(const std::string& name) : name_(name) {
    }
    const std::string& GetName() const {
        return name_;
    }

    virtual std::string StringRepresentation() override {
        return name_;
    }

private:
    std::string name_;
};

template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    auto pointer = std::dynamic_pointer_cast<T>(obj);
    return pointer;
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    auto pointer = std::dynamic_pointer_cast<T>(obj);
    if (pointer != nullptr) {
        return true;
    } else {
        return false;
    }
}

class Cell;

std::pair<bool, std::vector<std::shared_ptr<Object>>> GetObjectsList(std::shared_ptr<Cell> cell);

class Cell : public Object {
public:
    Cell() = default;
    Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_(std::move(first)), second_(std::move(second)) {
    }

    void SetFirst(std::shared_ptr<Object> ptr) {
        first_ = ptr;
    }

    void SetSecond(std::shared_ptr<Object> ptr) {
        second_ = ptr;
    }
    std::shared_ptr<Object> GetFirst() const {
        return first_;
    }
    std::shared_ptr<Object> GetSecond() const {
        return second_;
    }

    virtual std::string StringRepresentation() override {
        auto [is_correct, list_objects] = GetObjectsList(As<Cell>(shared_from_this()));
        std::stringstream return_value;
        return_value << "(";
        for (size_t i = 0; i < list_objects.size(); ++i) {
            return_value << (list_objects[i]->StringRepresentation());
            if (list_objects.size() != i + 1) {
                return_value << " ";
            }
            if (!is_correct && i + 2 == list_objects.size()) {
                return_value << ". ";
            }
        }
        return_value << ")";
        return return_value.str();
    }

private:
    std::shared_ptr<Object> first_, second_;
};

class LambdaFunction : public Object {
public:
    LambdaFunction(std::shared_ptr<Cell> cell,
                   const std::map<std::string, std::shared_ptr<Object>>& variables_map)
        : variables_map_(variables_map) {
        auto objects_list = GetObjectsList(std::move(cell)).second;
        DoParse(objects_list);
    }

    LambdaFunction(const std::vector<std::shared_ptr<Object>>& objects_list,
                   const std::map<std::string, std::shared_ptr<Object>>& variables_map)
        : variables_map_(variables_map) {
        DoParse(objects_list);
    }

    virtual std::string StringRepresentation() override {
        return "";
    }
    auto GetArgNames() {
        return arg_names_;
    }

    auto GetFunctions() {
        return function_;
    }

    auto GetVariablesMap() {
        return variables_map_;
    }

    void UpdateVariablesMap(const std::map<std::string, std::shared_ptr<Object>>& map) {
        variables_map_ = map;
    }

private:
    void DoParse(const std::vector<std::shared_ptr<Object>>& objects_list) {
        if (objects_list.size() < 2 ||
            (objects_list[1] && !Is<Cell>(objects_list[1]))) {  // must be a cell after lambda
            throw SyntaxError("Lambda syntax fails");
        }
        auto lambda_args = GetObjectsList(As<Cell>(objects_list[1])).second;
        for (auto& arg : lambda_args) {
            if (!Is<Symbol>(arg)) {
                throw SyntaxError("Lambda syntax fails");
            }
            arg_names_.push_back(As<Symbol>(arg)->GetName());
        }
        std::copy(objects_list.begin() + 2, objects_list.end(), std::back_inserter(function_));
    }
    std::map<std::string, std::shared_ptr<Object>> variables_map_;
    std::vector<std::string> arg_names_;
    std::vector<std::shared_ptr<Object>> function_;
};

bool IsObjectLambdaFunction(std::shared_ptr<Object> obj);