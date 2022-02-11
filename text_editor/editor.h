#pragma once

#include <list>
#include <vector>

class Command {
public:
    virtual std::shared_ptr <Command> GetOpposite() = 0;

    virtual ~Command() = default;
};

using CommandPtr = std::shared_ptr<Command>;

class TypeCommand : public Command {
public:
    TypeCommand(char c) : c(c) {};
    char c;

    virtual CommandPtr GetOpposite() override;
};

class BackSpaceCommand : public Command {
public:
    BackSpaceCommand(char c) : c(c) {};

    BackSpaceCommand() = default;

    char c;

    virtual CommandPtr GetOpposite() override;
};

class Shift : public Command {
public:
    Shift(int val) : delta(val) {};
    int delta;

    virtual CommandPtr GetOpposite() override;
};

CommandPtr TypeCommand::GetOpposite() {
    return std::make_shared<BackSpaceCommand>(c);
}

CommandPtr BackSpaceCommand::GetOpposite() {
    return std::make_shared<TypeCommand>(c);
}

CommandPtr Shift::GetOpposite() {
    return std::make_shared<Shift>(-delta);
}

template<class T>
std::shared_ptr <T> As(const std::shared_ptr <Command> &obj) {
    auto pointer = std::dynamic_pointer_cast<T>(obj);
    return pointer;
}

template<class T>
bool Is(const std::shared_ptr <Command> &obj) {
    auto pointer = std::dynamic_pointer_cast<T>(obj);
    if (pointer != nullptr) {
        return true;
    } else {
        return false;
    }
}

class EditorImpl {
public:
    EditorImpl() {
        current_position_ = history_.end();
        cursor_ = text_.begin();
    }

    void Commit(const CommandPtr &command) {
        while (current_position_ != history_.end()) {
            if (current_position_ == --history_.end()) {
                history_.pop_back();
                break;
            }
            history_.pop_back();
        }
        current_position_ = history_.end();
        if (ApplyCommand(command)) {
            history_.push_back(command);
        }
    }

    const std::string &GetText() const {
        return text_;
    }

    void Undo() {
        if (current_position_ != history_.begin()) {
            --current_position_;
            ApplyCommand((*current_position_)->GetOpposite());
        }
    }

    void Redo() {
        if (current_position_ != history_.end()) {
            ApplyCommand((*current_position_));
            ++current_position_;
        }
    }

private:
    bool ApplyCommand(const std::shared_ptr <Shift> &ptr) {
        if (ptr->delta <= 0 && cursor_ - text_.begin() >= -ptr->delta) {
            cursor_ += ptr->delta;
            return true;
        }
        if (ptr->delta > 0 && text_.end() - cursor_ >= ptr->delta) {
            cursor_ += ptr->delta;
            return true;
        }
        return false;
    }

    bool ApplyCommand(const std::shared_ptr <BackSpaceCommand> &ptr) {
        if (cursor_ != text_.begin()) {
            --cursor_;
            ptr->c = *cursor_;
            cursor_ = text_.erase(cursor_);
            return true;
        }
        return false;
    }

    bool ApplyCommand(const std::shared_ptr <TypeCommand> &ptr) {
        cursor_ = text_.insert(cursor_, ptr->c);
        ++cursor_;
        return true;
    }

    bool ApplyCommand(const std::shared_ptr <Command> &ptr) {
        if (Is<Shift>(ptr)) {
            return ApplyCommand(As<Shift>(ptr));
        } else if (Is<BackSpaceCommand>(ptr)) {
            return ApplyCommand(As<BackSpaceCommand>(ptr));
        } else if (Is<TypeCommand>(ptr)) {
            return ApplyCommand(As<TypeCommand>(ptr));
        }
        return false;
    }

    std::list <CommandPtr> history_;
    std::list<CommandPtr>::iterator current_position_;
    std::string text_;
    std::string::iterator cursor_;
};

class Editor {
public:
    const std::string &GetText() const {
        return impl_.GetText();
    }

    void Type(char c) {
        impl_.Commit(std::make_shared<TypeCommand>(c));
    }

    void ShiftLeft() {
        impl_.Commit(std::make_shared<Shift>(-1));
    }

    void ShiftRight() {
        impl_.Commit(std::make_shared<Shift>(1));
    }

    void Backspace() {
        impl_.Commit(std::make_shared<BackSpaceCommand>());
    }

    void Undo() {
        impl_.Undo();
    }

    void Redo() {
        impl_.Redo();
    }

private:
    EditorImpl impl_;
};
