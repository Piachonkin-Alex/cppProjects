#pragma once

#pragma once

#include <variant>
#include <optional>
#include <istream>

struct SymbolToken {
    SymbolToken() = default;
    SymbolToken(const std::string& value) : name(value) {
    }
    std::string name;

    bool operator==(const SymbolToken& other) const {
        return name == other.name;
    }
};

struct QuoteToken {
    bool operator==(const QuoteToken&) const {
        return true;
    }
};

struct DotToken {
    bool operator==(const DotToken&) const {
        return true;
    }
};

enum class BracketToken { OPEN, CLOSE };

struct ConstantToken {
    ConstantToken() = default;
    ConstantToken(int value) : value(value) {
    }
    int value = 0;

    bool operator==(const ConstantToken& other) const {
        return value == other.value;
    }
};

typedef std::variant<ConstantToken, BracketToken, SymbolToken, QuoteToken, DotToken> Token;

// Интерфейс позволяющий читать токены по одному из потока.
class TokenParser {
public:
    TokenParser(std::istream* in) : in_(in) {
        ReadNext();
    }

    bool IsEnd() {
        return is_end_;
    }

    void Next() {
        ReadNext();
    }

    Token GetToken() {
        return next_;
    }

private:
    std::istream* in_;
    Token next_;
    bool is_end_ = false;

    void ReadNext() {
        char next = in_->peek();
        while (next == ' ' || next == '\n' || next == '\t') {
            char empty;
            in_->read(&empty, 1);
            next = in_->peek();
        }
        if (next == EOF) {
            is_end_ = true;
            return;
        }
        if (next == '(') {
            *in_ >> next;
            next_ = BracketToken::OPEN;
        } else if (next == ')') {
            *in_ >> next;
            next_ = BracketToken::CLOSE;
        } else if (next == '\'') {
            *in_ >> next;
            next_ = QuoteToken{};
        } else if (next == '.') {
            *in_ >> next;
            next_ = DotToken{};
        } else if (std::isdigit(next)) {
            int value;
            *in_ >> value;
            next_ = ConstantToken(value);
        } else if (next == '+' || next == '-') {
            *in_ >> next;
            char next_next = in_->peek();
            if (std::isdigit(next_next)) {
                int value;
                *in_ >> value;
                (next == '+') ? (next_ = ConstantToken(value))
                              : (next_ = ConstantToken(-1 * value));
            } else {
                next_ = SymbolToken(std::string(1, next));
            }
        } else {
            std::string symbol_name;
            if (!(next >= 'a' && next <= 'z') && !(next >= 'A' && next <= 'Z') && next != '*' &&
                next != '/' && next != '#' && !(next >= '<' && next <= '>')) {
                exit(1);
            }
            *in_ >> next;
            symbol_name.push_back(next);
            while (true) {
                next = in_->peek();
                if (!(next >= 'a' && next <= 'z') && !(next >= 'A' && next <= 'Z') && next != '*' &&
                    next != '#' && !(next >= '<' && next <= '>') && !(next >= '0' && next <= '9') &&
                    next != '?' && next != '-' && next != '!') {
                    break;
                }
                in_->read(&next, 1);
                symbol_name.push_back(next);
            }
            next_ = SymbolToken(symbol_name);
        }
    }
};