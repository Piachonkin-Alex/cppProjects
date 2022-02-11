#pragma once
#include <map>

class Test {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~Test() {
    }
};

class IHolder {
public:
    virtual std::unique_ptr<Test> Create() = 0;
};

template <typename Test>
class Holder : public IHolder {
public:
    Holder() = default;
    virtual ~Holder() = default;

    virtual std::unique_ptr<Test> Create() override {
        return std::make_unique<Test>();
    }
};

class TestFramework {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        tests_[class_name] = std::make_unique<Holder<TestClass>>();
    }

    std::unique_ptr<Test> CreateTest(const std::string& class_name) {
        auto iter = tests_.find(class_name);
        if (iter == tests_.end()) {
            throw std::out_of_range("range");
        }
        return iter->second->Create();
    }

    void RunTest(const std::string& test_name) {
        auto test = CreateTest(test_name);
        test->SetUp();
        std::exception_ptr ptr;
        try {
            test->Run();
        } catch (...) {
            ptr = std::current_exception();
        }
        test->TearDown();
        if (ptr) {
            std::rethrow_exception(ptr);
        }
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> result;
        for (auto& elem : tests_) {
            if (callback(elem.first)) {
                result.push_back(elem.first);
            }
        }
        return result;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> result;
        std::transform(tests_.begin(), tests_.end(), std::back_inserter(result),
                       [](const auto& pair) { return pair.first; });
        return result;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        auto need_tests = ShowTests(std::move(callback));
        for (auto& elem : need_tests) {
            RunTest(elem);
        }
    }

    static TestFramework& Instance() {
        if (!object) {
            object = std::unique_ptr<TestFramework>(new TestFramework);
        }
        return *object;
    }

    void Clear() {
        tests_.clear();
    }

    TestFramework(const TestFramework& other) = delete;
    TestFramework(TestFramework&& other) = delete;
    TestFramework& operator=(const TestFramework& other) = delete;
    TestFramework& operator=(TestFramework&& other) = delete;

protected:
    TestFramework() {
    }

private:
    std::map<std::string, std::unique_ptr<IHolder>> tests_;
    static std::unique_ptr<TestFramework> object;
};
std::unique_ptr<TestFramework> TestFramework::object = nullptr;

auto FindSubStr(const std::string& str) {
    return [&str](const std::string& string) { return string.find(str) != std::string::npos; };
}

auto FindFullMatch(const std::string& str) {
    return [&str](const std::string& string) { return string == str; };
}