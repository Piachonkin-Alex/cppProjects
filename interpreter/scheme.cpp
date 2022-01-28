#include "scheme.h"
#include "tokenizer.h"
#include "parser.h"
#include <functional>
#include <cmath>

Interpreter::Interpreter() {
    variables_map_["#t"] = std::make_shared<Symbol>("#t");
    variables_map_["#f"] = std::make_shared<Symbol>("#f");
}

Interpreter::Interpreter(const std::map<std::string, std::shared_ptr<Object>> &map) : variables_map_(map) {
    variables_map_["#t"] = std::make_shared<Symbol>("#t");
    variables_map_["#f"] = std::make_shared<Symbol>("#f");
}

std::string Interpreter::Interpret(const std::string &code) {
    std::stringstream flow(code);
    TokenParser tokenizer(&flow);
    auto obj = Read(&tokenizer);
    if (!tokenizer.IsEnd()) {
        throw SyntaxError("Syntax problem");
    }
    auto gg = Evaluate(obj);
    if (!gg) {
        return "()";
    }
    return (gg)->StringRepresentation();
}

std::shared_ptr<Object> Interpreter::Evaluate(const std::shared_ptr<Object> &obj) {
    if (!obj) {
        throw RuntimeError("Empty");
    } else if (Is<Number>(obj)) {
        return EvaluateNumber(As<Number>(obj));
    } else if (Is<Symbol>(obj)) {
        return EvaluateSymbol(As<Symbol>(obj));
    } else if (Is<Cell>(obj)) {
        return EvaluateCell(As<Cell>(obj));
    }
    throw RuntimeError("Unknown object type");
}

std::shared_ptr<Object> Interpreter::EvaluateNumber(const std::shared_ptr<Number> &obj) {
    return obj;
}

std::shared_ptr<Object> Interpreter::EvaluateSymbol(const std::shared_ptr<Symbol> &obj) {
    for (auto &[key, value] : variables_map_) {
        if (key == obj->GetName()) {
            return value;
        }
    }
    throw NameError("No name");
}
std::shared_ptr<Object> Interpreter::EvaluateCell(const std::shared_ptr<Cell> &obj) {
    if (IsObjectLambdaFunction(obj)) {
        return std::make_shared<LambdaFunction>(obj, variables_map_);
    }
    auto obj_list = GetObjectsList(obj).second;
    if (!Is<Symbol>(*obj_list.begin())) {  // so it should be list
        if (!Is<Cell>(*obj_list.begin())) {
            throw RuntimeError("Invalid!");
        }
        auto lambda = EvaluateCell(As<Cell>(*obj_list.begin()));
        if (!Is<LambdaFunction>(lambda)) {
            throw RuntimeError("Invalid!");
        }
        obj_list.erase(obj_list.begin());
        return EvaluateLambda(As<LambdaFunction>(lambda), EvaluateObjectList(obj_list));
    } else {
        auto operation = As<Symbol>(*obj_list.begin())->GetName();
        obj_list.erase(obj_list.begin());
        return Interpreter::DoSomeOperation(operation, obj_list);
    }
}

std::vector<std::shared_ptr<Object>> Interpreter::EvaluateObjectList(
    const std::vector<std::shared_ptr<Object>> &list) {
    std::vector<std::shared_ptr<Object>> result;
    for (auto &element : list) {
        result.push_back(Evaluate(element));
    }
    return result;
}

std::shared_ptr<Object> Interpreter::EvaluateLambda(std::shared_ptr<LambdaFunction> obj,
                                               const std::vector<std::shared_ptr<Object>> &args) {
    auto arg_names = obj->GetArgNames();
    if (arg_names.size() != args.size()) {
        throw RuntimeError("arg number!");
    }
    auto var_map = variables_map_;
    for (auto &[key, value] : obj->GetVariablesMap()) {
        var_map[key] = value;
    }
    for (size_t i = 0; i < args.size(); ++i) {
        var_map[arg_names[i]] = Evaluate(args[i]);
    }
    Interpreter helper(var_map);
    auto functions = obj->GetFunctions();
    std::shared_ptr<Object> res;
    for (auto &func : functions) {
        res = helper.Evaluate(func);
        obj->UpdateVariablesMap(helper.variables_map_);
    }
    return res;
}

std::shared_ptr<Object> Interpreter::DoSomeOperation(const std::string &operation,
                                                const std::vector<std::shared_ptr<Object>> &args) {
    auto it = variables_map_.find(operation);
    if (it != variables_map_.end()) {
        if (Is<LambdaFunction>(it->second)) {
            return EvaluateLambda(As<LambdaFunction>(it->second), args);
        }
    }
    if (operation == "number?") {
        if (args.size() != 1) {
            throw RuntimeError("number? args!");
        }
        return IsNumber(EvaluateObjectList(args).front());
    } else if (operation == "symbol?") {
        if (args.size() != 1) {
            throw RuntimeError("symbol? args!");
        }
        return IsSymbol(EvaluateObjectList(args).front());
    } else if (operation == "boolean?") {
        if (args.size() != 1) {
            throw RuntimeError("boolean? args!");
        }
        return IsBoolean(EvaluateObjectList(args).front());
    } else if (operation == "not") {
        if (args.size() != 1) {
            throw RuntimeError("not args!");
        }
        return NotOperation(EvaluateObjectList(args).front());
    } else if (operation == "and") {
        return AndOperation(args);
    } else if (operation == "or") {
        return OrOperation(args);
    } else if (operation == "define") {
        return DefineOperation(args);
    } else if (operation == "set!") {
        return SetOperation(args);
    } else if (operation == "if") {
        if (args.size() == 2) {
            return If2Operation(Evaluate(args.front()), args.back());
        }
        if (args.size() == 3) {
            return If3Operation(Evaluate(args.front()), args[1], args.back());
        }
        throw SyntaxError("Inccorect num of args for if");
    } else if (operation == "abs") {
        if (args.size() != 1) {
            throw RuntimeError("abs args!");
        }
        return AbsOperation(EvaluateObjectList(args).front());
    } else if (operation == "null?") {
        if (args.size() != 1) {
            throw RuntimeError("null args!");
        }
        if (!Evaluate(args.front())) {
            return std::make_shared<Symbol>("#t");
        }
        return std::make_shared<Symbol>("#f");
    } else if (operation == "pair?") {
        if (args.size() != 1) {
            throw RuntimeError("pair args!");
        }
        return PairOperation(Evaluate(args.front()));
    } else if (operation == "list?") {
        if (args.size() != 1) {
            throw RuntimeError("list? args!");
        }
        return IsListOperation(Evaluate(args.front()));
    } else if (operation == "quote") {
        if (args.size() != 1) {
            throw RuntimeError("Bad quote!");
        }
        return args.front();
    } else if (operation == "cons") {
        if (args.size() != 2) {
            throw RuntimeError("Bad cons!");
        }
        return std::make_shared<Cell>(Evaluate(args.front()), Evaluate(args.back()));
    } else if (operation == "list-ref") {
        if (args.size() != 2) {
            throw RuntimeError("Bad list-ref!");
        }
        return RefOperation(Evaluate(args.front()), Evaluate(args.back()));
    } else if (operation == "list-tail") {
        if (args.size() != 2) {
            throw RuntimeError("Bad list-tail!");
        }
        return TailOperation(Evaluate(args.front()), Evaluate(args.back()));
    } else if (operation == "list") {
        auto reverse_args = args;
        std::reverse(reverse_args.begin(), reverse_args.end());
        return ListOperation(EvaluateObjectList(reverse_args));
    } else if (operation == "operation") {

    } else if (operation == "car") {
        if (args.size() != 1) {
            throw RuntimeError("Bad car!");
        }
        return CarOperation(Evaluate(args.front()));
    } else if (operation == "cdr") {
        if (args.size() != 1) {
            throw RuntimeError("Bad cdr!");
        }
        return CdrOperation(Evaluate(args.front()));
    } else if (operation == "set-car!") {
        if (args.size() != 2) {
            throw RuntimeError("Bad set-car!");
        }
        return SetCarOperation(args.front(), Evaluate(args.back()));
    } else if (operation == "set-cdr!") {
        if (args.size() != 2) {
            throw RuntimeError("Bad set-cdr!");
        }
        return SetCdrOperation(args.front(), Evaluate(args.back()));
    } else if (operation == "+") {
        return PlusOperation(EvaluateObjectList(args));
    } else if (operation == "-") {
        if (args.empty()) {
            throw RuntimeError("No args to subtract!");
        }
        return MinusOperation(EvaluateObjectList(args));
    } else if (operation == "*") {
        return MultiplyOperation(EvaluateObjectList(args));
    } else if (operation == "/") {
        if (args.empty()) {
            throw RuntimeError("No args to divide");
        }
        return DivideOperation(EvaluateObjectList(args));
    } else if (operation == "min") {
        if (args.empty()) {
            throw RuntimeError("No args to min");
        }
        return MinOperation(EvaluateObjectList(args));
    } else if (operation == "max") {
        if (args.empty()) {
            throw RuntimeError("No args to max");
        }
        return MaxOperation(EvaluateObjectList(args));
    } else if (operation == ">=" || operation == "<=" || operation == "=" || operation == ">" ||
               operation == "<" || operation == "!=") {
        for (auto arg : args) {
            if (!Is<Number>(Evaluate(arg))) {
                throw RuntimeError("Compare with not a number");
            }
        }
        std::function predicate = [](int64_t, int64_t) { return rand() % 2 == 0; };
        if (operation == ">=") {
            predicate = [](int64_t x, int64_t y) { return x >= y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        } else if (operation == "<=") {
            predicate = [](int64_t x, int64_t y) { return x <= y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        } else if (operation == "<") {
            predicate = [](int64_t x, int64_t y) { return x < y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        } else if (operation == ">") {
            predicate = [](int64_t x, int64_t y) { return x > y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        } else if (operation == "=") {
            predicate = [](int64_t x, int64_t y) { return x == y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        } else if (operation == "!=") {
            predicate = [](int64_t x, int64_t y) { return x != y; };
            return Interpreter::CompareOperations(EvaluateObjectList(args), predicate);
        }
        throw RuntimeError("No such operation!");
    }
    throw RuntimeError("No such operation!");
}

std::shared_ptr<Object> Interpreter::IsNumber(const std::shared_ptr<Object> &arg) {
    if (!Is<Number>(arg)) {
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}
std::shared_ptr<Object> Interpreter::IsSymbol(const std::shared_ptr<Object> &arg) {
    if (!Is<Symbol>(arg)) {
        return std::make_shared<Symbol>("#f");
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Interpreter::IsBoolean(const std::shared_ptr<Object> &arg) {
    if (!Is<Symbol>(arg)) {
        return std::make_shared<Symbol>("#f");
    }
    if (As<Symbol>(arg)->GetName() == "#f" || As<Symbol>(arg)->GetName() == "#t") {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

template <typename T>
std::shared_ptr<Object> Interpreter::CompareOperations(const std::vector<std::shared_ptr<Object>> &args,
                                                  const T &predicate) {
    for (int i = 0; i < static_cast<int>(args.size()) - 1; ++i) {
        if (!predicate(As<Number>(args[i])->GetValue(), As<Number>(args[i + 1])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Interpreter::PlusOperation(const std::vector<std::shared_ptr<Object>> &args) {
    int64_t sum = 0;
    for (auto &arg : args) {
        if (!Is<Number>(arg)) {
            throw RuntimeError("Sum not only Numbers");
        }
        sum += As<Number>(arg)->GetValue();
    }
    return std::make_shared<Number>(sum);
}

std::shared_ptr<Object> Interpreter::MultiplyOperation(
    const std::vector<std::shared_ptr<Object>> &args) {
    int64_t prod = 1;
    for (auto &arg : args) {
        if (!Is<Number>(arg)) {
            throw RuntimeError("Sum not only Numbers");
        }
        prod *= As<Number>(arg)->GetValue();
    }
    return std::make_shared<Number>(prod);
}

std::shared_ptr<Object> Interpreter::MinusOperation(const std::vector<std::shared_ptr<Object>> &args) {
    int64_t sum = 0;
    for (size_t i = 0; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Sum not only Numbers");
        }
        if (i == 0) {
            sum += As<Number>(args[i])->GetValue();
        } else {
            sum -= As<Number>(args[i])->GetValue();
        }
    }
    return std::make_shared<Number>(sum);
}

std::shared_ptr<Object> Interpreter::DivideOperation(const std::vector<std::shared_ptr<Object>> &args) {
    int64_t prod = 1;
    for (size_t i = 0; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Sum not only Numbers");
        }
        if (i == 0) {
            prod *= As<Number>(args[i])->GetValue();
        } else {
            prod /= As<Number>(args[i])->GetValue();
        }
    }
    return std::make_shared<Number>(prod);
}

std::shared_ptr<Object> Interpreter::MaxOperation(const std::vector<std::shared_ptr<Object>> &args) {
    int64_t current_max = -1e18;
    for (auto &elem : args) {
        if (!Is<Number>(elem)) {
            throw RuntimeError("Max not only Numbers");
        }
        if (As<Number>(elem)->GetValue() > current_max) {
            current_max = As<Number>(elem)->GetValue();
        }
    }
    return std::make_shared<Number>(current_max);
}

std::shared_ptr<Object> Interpreter::MinOperation(const std::vector<std::shared_ptr<Object>> &args) {
    int64_t current_min = 1e18;
    for (auto &elem : args) {
        if (!Is<Number>(elem)) {
            throw RuntimeError("Max not only Numbers");
        }
        if (As<Number>(elem)->GetValue() < current_min) {
            current_min = As<Number>(elem)->GetValue();
        }
    }
    return std::make_shared<Number>(current_min);
}

std::shared_ptr<Object> Interpreter::AbsOperation(const std::shared_ptr<Object> &arg) {
    if (!Is<Number>(arg)) {
        throw RuntimeError("Abs not only numbers!");
    }
    return std::make_shared<Number>(std::fabs(As<Number>(arg)->GetValue()));
}

std::shared_ptr<Object> Interpreter::DefineOperation(const std::vector<std::shared_ptr<Object>> &args) {
    if (args.size() != 2) {
        throw SyntaxError("Bad define syntax");
    }
    if (Is<Cell>(args.front())) {
        auto name_and_args = GetObjectsList(As<Cell>(args.front())).second;
        if (!Is<Symbol>(name_and_args.front())) {
            throw SyntaxError("Bad define");
        }
        std::vector<std::shared_ptr<Object>> lam_args;
        std::copy(name_and_args.begin() + 1, name_and_args.end(), std::back_inserter(lam_args));
        auto lam_args_list = ListOperation(lam_args);
        auto lam_obj_list = std::vector<std::shared_ptr<Object>>{std::make_shared<Symbol>("lambda"),
                                                                 lam_args_list, args.back()};
        auto lam = std::make_shared<LambdaFunction>(lam_obj_list, variables_map_);
        return variables_map_[As<Symbol>(name_and_args.front())->GetName()] = lam;
    }
    if (Is<Symbol>(args.front())) {
        variables_map_[As<Symbol>(args.front())->GetName()] = Evaluate(args.back());
        return variables_map_[As<Symbol>(args.front())->GetName()];
    }
    throw SyntaxError("Bad define");
}

std::shared_ptr<Object> Interpreter::SetOperation(const std::vector<std::shared_ptr<Object>> &args) {
    if (args.size() != 2) {
        throw SyntaxError("Set bad args");
    }
    if (!Is<Symbol>(args.front())) {
        throw SyntaxError("Set bad args");
    }
    auto var_name = As<Symbol>(args.front());
    if (variables_map_.find(var_name->GetName()) == variables_map_.end()) {
        throw NameError("No such variable!");
    }
    return variables_map_[var_name->GetName()] = Evaluate(args.back());
}

std::shared_ptr<Object> Interpreter::NotOperation(const std::shared_ptr<Object> &arg) {
    if (Is<Symbol>(arg) && As<Symbol>(arg)->GetName() == "#f") {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Interpreter::AndOperation(const std::vector<std::shared_ptr<Object>> &args) {
    for (size_t i = 0; i < args.size(); ++i) {
        auto result = Evaluate(args[i]);
        if ((Is<Symbol>(result) && As<Symbol>(result)->GetName() == "#f") || i + 1 == args.size()) {
            return result;
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Interpreter::OrOperation(const std::vector<std::shared_ptr<Object>> &args) {
    for (size_t i = 0; i < args.size(); ++i) {
        auto result = Evaluate(args[i]);
        if ((Is<Symbol>(result) && As<Symbol>(result)->GetName() == "#t") || i + 1 == args.size()) {
            return result;
        }
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Interpreter::PairOperation(const std::shared_ptr<Object> &arg) {
    if (!arg) {
        return std::make_shared<Symbol>("#f");
    }
    if (!Is<Cell>(arg)) {
        throw RuntimeError("Bad Pair predicate");
    }
    if (GetObjectsList(As<Cell>(arg)).second.size() == 2) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Interpreter::IsListOperation(const std::shared_ptr<Object> &arg) {
    if (!arg) {
        return std::make_shared<Symbol>("#t");
    }
    if (!Is<Cell>(arg)) {
        return std::make_shared<Symbol>("#f");
    }
    bool is_correct_list = GetObjectsList(As<Cell>(arg)).first;
    return is_correct_list ? std::make_shared<Symbol>("#t") : std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> Interpreter::CarOperation(const std::shared_ptr<Object> &arg) {
    if (!Is<Cell>(arg)) {
        throw RuntimeError("Car invalid argument");
    }
    return As<Cell>(arg)->GetFirst();
}

std::shared_ptr<Object> Interpreter::CdrOperation(const std::shared_ptr<Object> &arg) {
    if (!Is<Cell>(arg)) {
        throw RuntimeError("Cdr invalid argument");
    }
    return As<Cell>(arg)->GetSecond();
}

std::shared_ptr<Object> Interpreter::SetCarOperation(const std::shared_ptr<Object> &arg1,
                                                const std::shared_ptr<Object> &arg2) {
    if (!Is<Symbol>(arg1)) {
        throw RuntimeError("Set-car bad syntax");
    }
    auto find_variable = variables_map_.find(As<Symbol>(arg1)->GetName());
    if (find_variable == variables_map_.end()) {
        throw RuntimeError("No such variable in Set-car");
    }
    if (Is<Cell>(find_variable->second)) {
        find_variable->second =
            std::make_shared<Cell>(arg2, As<Cell>(find_variable->second)->GetSecond());
        return find_variable->second;
    }
    throw RuntimeError("No pair in variable!");
}

std::shared_ptr<Object> Interpreter::SetCdrOperation(const std::shared_ptr<Object> &arg1,
                                                const std::shared_ptr<Object> &arg2) {
    if (!Is<Symbol>(arg1)) {
        throw RuntimeError("Set-car bad syntax");
    }
    auto find_variable = variables_map_.find(As<Symbol>(arg1)->GetName());
    if (find_variable == variables_map_.end()) {
        throw RuntimeError("No such variable in Set-car");
    }
    if (Is<Cell>(find_variable->second)) {
        find_variable->second =
            std::make_shared<Cell>(As<Cell>(find_variable->second)->GetFirst(), arg2);
        return find_variable->second;
    }
    throw RuntimeError("No pair in variable!");
}

std::shared_ptr<Object> Interpreter::ListOperation(std::vector<std::shared_ptr<Object>> reverse_args) {
    if (reverse_args.empty()) {
        return nullptr;
    }
    auto head = reverse_args.back();
    reverse_args.pop_back();
    return std::make_shared<Cell>(head, ListOperation(reverse_args));
}

std::shared_ptr<Object> Interpreter::RefOperation(const std::shared_ptr<Object> &arg1,
                                             const std::shared_ptr<Object> &arg2) {
    if (!Is<Number>(arg2) || !Is<Cell>(arg1)) {
        throw RuntimeError("Bad ref args!");
    }
    auto object_list = GetObjectsList(As<Cell>(arg1)).second;
    if (As<Number>(arg2)->GetValue() < 0 ||
        As<Number>(arg2)->GetValue() >= static_cast<int64_t>(object_list.size())) {
        throw RuntimeError("No index in ref operation!");
    }
    return object_list[As<Number>(arg2)->GetValue()];
}

std::shared_ptr<Object> Interpreter::TailOperation(const std::shared_ptr<Object> &arg1,
                                              const std::shared_ptr<Object> &arg2) {
    if (!Is<Number>(arg2) || !Is<Cell>(arg1)) {
        throw RuntimeError("Bad tail args!");
    }
    auto object_list = GetObjectsList(As<Cell>(arg1)).second;
    if (As<Number>(arg2)->GetValue() < 0 ||
        As<Number>(arg2)->GetValue() > static_cast<int64_t>(object_list.size())) {
        throw RuntimeError("No index in tail operation!");
    }
    std::vector<std::shared_ptr<Object>> args;
    std::copy(object_list.begin() + As<Number>(arg2)->GetValue(), object_list.end(),
              std::back_inserter(args));
    std::reverse(args.begin(), args.end());
    return ListOperation(args);
}

std::shared_ptr<Object> Interpreter::If2Operation(const std::shared_ptr<Object> &arg1,
                                             const std::shared_ptr<Object> &arg2) {
    if (!Is<Symbol>(arg1) ||
        !(As<Symbol>(arg1)->GetName() == "#f" || As<Symbol>(arg1)->GetName() == "#t")) {
        throw RuntimeError("Bad if2 syntax!");
    }
    if (As<Symbol>(arg1)->GetName() == "#t") {
        return Evaluate(arg2);
    }
    return nullptr;
}

std::shared_ptr<Object> Interpreter::If3Operation(const std::shared_ptr<Object> &arg1,
                                             const std::shared_ptr<Object> &arg2,
                                             const std::shared_ptr<Object> &arg3) {
    if (!Is<Symbol>(arg1) ||
        !(As<Symbol>(arg1)->GetName() == "#f" || As<Symbol>(arg1)->GetName() == "#t")) {
        throw RuntimeError("Bad if3 syntax!");
    }
    if (As<Symbol>(arg1)->GetName() == "#t") {
        return Evaluate(arg2);
    }
    return Evaluate(arg3);
}