#include "parser.h"
#include "error.h"

std::shared_ptr <Object> Read(TokenParser *tokenizer) {
    auto token = tokenizer->GetToken();
    tokenizer->Next();
    if (ConstantToken * number = std::get_if<ConstantToken>(&token)) {
        return std::make_shared<Number>(number->value);
    }
    if (SymbolToken * symbol = std::get_if<SymbolToken>(&token)) {
        return std::make_shared<Symbol>(symbol->name);
    }
    if (std::get_if<QuoteToken>(&token)) {
        auto next_object = Read(tokenizer);
        auto quote_tree = std::make_shared<Symbol>("quote");
        return std::make_shared<Cell>(quote_tree, std::make_shared<Cell>(next_object, nullptr));
    }
    BracketToken *bracket;
    if ((bracket = std::get_if<BracketToken>(&token)) == nullptr) {
        throw SyntaxError{"Описание ошибки"};
    } else {
        if (*bracket != BracketToken::OPEN) {
            throw SyntaxError{"Описание ошибки"};
        }
    }
    auto return_value = ReadList(tokenizer);
    if (tokenizer->IsEnd()) {
        throw SyntaxError{"gg"};
    }
    token = tokenizer->GetToken();
    if ((bracket = std::get_if<BracketToken>(&token)) == nullptr) {
        throw SyntaxError{"gg"};
    } else {
        if (*bracket != BracketToken::CLOSE) {
            throw SyntaxError{"gg"};
        }
    }
    tokenizer->Next();
    return return_value;
}

std::shared_ptr <Object> ReadList(TokenParser *tokenizer) {
    auto token = tokenizer->GetToken();
    BracketToken *bracket;
    if ((bracket = std::get_if<BracketToken>(&token)) != nullptr) {
        if (*bracket == BracketToken::CLOSE) {
            return nullptr;
        }
    }
    std::shared_ptr <Cell> head(new Cell());
    auto tail = head;
    std::shared_ptr <Object> current;
    bool is_first_read = false;
    while (true) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError{"gg"};
        }
        token = tokenizer->GetToken();
        if (std::holds_alternative<BracketToken>(token)) {
            auto value = std::get<BracketToken>(token);
            if (value == BracketToken::CLOSE) {
                return head;
            }
        }
        if (std::holds_alternative<DotToken>(token)) {
            if (!is_first_read) {
                throw SyntaxError{"gg"};
            }
            tokenizer->Next();
            ReadLast(tokenizer, tail);
            return head;
        }
        if (is_first_read) {
            tail->SetSecond(std::make_shared<Cell>(Cell()));
            tail = As<Cell>(tail->GetSecond());
        }
        tail->SetFirst(Read(tokenizer));
        is_first_read = true;
    }
}

void ReadLast(TokenParser *tokenizer, std::shared_ptr <Cell> &tail) {
    tail->SetSecond(Read(tokenizer));
}

std::pair<bool, std::vector<std::shared_ptr < Object>>>
GetObjectsList(std::shared_ptr<Cell>
cell) {
bool is_correct = true;
std::vector <std::shared_ptr<Object>> list_objects;
while (cell) {
list_objects.
push_back(cell
->

GetFirst()

);
auto next = cell->GetSecond();
if (
Is<Cell>(next)
) {
cell = As<Cell>(next);
} else {
if (next != nullptr) {
list_objects.
push_back(next);
is_correct = false;
}
break;
}
}
return
std::make_pair(is_correct, list_objects
);
}

bool IsObjectLambdaFunction(std::shared_ptr <Object> obj) {
    if (!Is<Cell>(obj)) {
        return false;
    }
    auto list = GetObjectsList(As<Cell>(obj)).second;
    if (list.empty()) {
        return false;
    }
    if (!Is<Symbol>(list[0]) || As<Symbol>(list.front())->GetName() != "lambda") {
        return false;
    }
    if (list.size() < 3) {
        throw SyntaxError("bad lambda!");
    }
    return true;
}