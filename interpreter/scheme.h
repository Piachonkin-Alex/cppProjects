#pragma once
#include <string>
#include <map>
#include <memory>
#include "object.h"
class Interpreter {
public:
    Interpreter();
    Interpreter(const std::map<std::string, std::shared_ptr<Object>>& map);
    std::string Interpret(const std::string& code);

private:
    std::shared_ptr<Object> Evaluate(const std::shared_ptr<Object>& obj);
    std::shared_ptr<Object> EvaluateNumber(const std::shared_ptr<Number>& obj);
    std::shared_ptr<Object> EvaluateSymbol(const std::shared_ptr<Symbol>& obj);
    std::shared_ptr<Object> EvaluateCell(const std::shared_ptr<Cell>& obj);
    std::vector<std::shared_ptr<Object>> EvaluateObjectList(
        const std::vector<std::shared_ptr<Object>>& list);
    std::shared_ptr<Object> EvaluateLambda(std::shared_ptr<LambdaFunction> obj,
                                           const std::vector<std::shared_ptr<Object>>& args);

    std::shared_ptr<Object> DoSomeOperation(const std::string& operation,
                                            const std::vector<std::shared_ptr<Object>>& args);

    std::shared_ptr<Object> IsNumber(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> IsBoolean(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> IsSymbol(const std::shared_ptr<Object>& arg);

    template <typename T>
    std::shared_ptr<Object> CompareOperations(const std::vector<std::shared_ptr<Object>>& args,
                                              const T& predicate);
    std::shared_ptr<Object> PlusOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> MinusOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> MultiplyOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> DivideOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> MaxOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> MinOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> AbsOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> DefineOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> SetOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> NotOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> AndOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> OrOperation(const std::vector<std::shared_ptr<Object>>& args);
    std::shared_ptr<Object> PairOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> IsListOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> CarOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> CdrOperation(const std::shared_ptr<Object>& arg);
    std::shared_ptr<Object> SetCarOperation(const std::shared_ptr<Object>& arg1,
                                            const std::shared_ptr<Object>& arg2);
    std::shared_ptr<Object> SetCdrOperation(const std::shared_ptr<Object>& arg1,
                                            const std::shared_ptr<Object>& arg2);
    std::shared_ptr<Object> ListOperation(std::vector<std::shared_ptr<Object>> reverse_args);
    std::shared_ptr<Object> RefOperation(const std::shared_ptr<Object>& arg1,
                                         const std::shared_ptr<Object>& arg2);
    std::shared_ptr<Object> TailOperation(const std::shared_ptr<Object>& arg1,
                                          const std::shared_ptr<Object>& arg2);
    std::shared_ptr<Object> If2Operation(const std::shared_ptr<Object>& arg1,
                                         const std::shared_ptr<Object>& arg2);
    std::shared_ptr<Object> If3Operation(const std::shared_ptr<Object>& arg1,
                                         const std::shared_ptr<Object>& arg2,
                                         const std::shared_ptr<Object>& arg3);
    std::map<std::string, std::shared_ptr<Object>> variables_map_;
};