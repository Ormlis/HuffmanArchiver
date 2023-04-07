#pragma once

#include <string>
#include <vector>
#include <sstream>

class CLAParser {
public:
    CLAParser();

    template <typename T>
    void AddArgument(char short_name, const std::string& long_name, const std::string& type, const std::string& help,
                     bool required);

    template <typename T>
    void AddMultipleArguments(const std::string& type, const std::string& help, size_t required);

    void AddFlag(char short_name, const std::string& long_name, const std::string& help);

    bool Parse(int argc, char** argv);

    template <typename T>
    const T* GetArgumentValue(const std::string& long_name) const;

    template <typename T>
    size_t GetMultiplyArgumentsNumber() const;

    template <typename T>
    const T* GetMultiplyArgumentValue(size_t index) const;

    std::string GetHelp() const;

private:
    class ArgumentHolderBase {
    public:
        virtual ~ArgumentHolderBase() {
        }

        virtual bool IsEmpty() const = 0;
        virtual bool CanSet() const = 0;
        virtual bool Set(const std::string& value) = 0;
        virtual bool IsRequired() const = 0;

        virtual const std::string& GetShortName() const = 0;
        virtual const std::string& GetLongName() const = 0;
        virtual const std::string& GetType() const = 0;
        virtual const std::string& GetHelp() const = 0;
    };

    template <typename T>
    class ArgumentHolder : public ArgumentHolderBase {
    public:
        ArgumentHolder(char short_name, const std::string& long_name, const std::string& type, const std::string& help,
                       bool required);

        ArgumentHolder(const std::string& short_name, const std::string& long_name, const std::string& type,
                       const std::string& help, bool required);

        bool IsEmpty() const override;
        bool CanSet() const override;
        bool Set(const std::string& value) override;
        bool IsRequired() const override;

        const std::string& GetShortName() const override;
        const std::string& GetLongName() const override;
        const std::string& GetType() const override;
        const std::string& GetHelp() const override;

        const T* GetValue() const;

    protected:
        std::string short_name_;
        std::string long_name_;
        std::string type_;
        std::string help_;
        bool required_;

        std::unique_ptr<T> value_;
    };

    class FlagHolder : public ArgumentHolder<bool> {
    public:
        FlagHolder(char short_name, const std::string& long_name, const std::string& help);

        bool CanSet() const override;
        bool Set(const std::string& value) override;
    };

    template <typename T>
    class MultiplyArgumentsHolder : public ArgumentHolder<std::vector<T>> {
    public:
        MultiplyArgumentsHolder(const std::string& type, const std::string& help, size_t required);

        bool IsEmpty() const override;
        bool CanSet() const override;
        bool Set(const std::string& value) override;

        const size_t GetNumberArgs() const;
        const T* GetValue(size_t index) const;

    private:
        size_t required_cnt_;
    };

    class InvalidArgumentCastException : std::exception {};
    class InvalidArgumentException : std::exception {};

    template <typename T>
    static T GetFromString(const std::string& value);

    std::string program_name_;
    std::vector<std::unique_ptr<ArgumentHolderBase>> args_;
    std::unique_ptr<ArgumentHolderBase> multiply_arguments_;
};

template <typename T>
T CLAParser::GetFromString(const std::string& value) {
    throw InvalidArgumentCastException();
}

template <>
int CLAParser::GetFromString<int>(const std::string& value) {
    return std::stoi(value);
}

template <>
std::string CLAParser::GetFromString<std::string>(const std::string& value) {
    return value;
}

template <>
bool CLAParser::GetFromString<bool>(const std::string& value) {
    if (value == "1" || value == "true") {
        return true;
    } else if (value == "0" || value == "false") {
        return false;
    } else {
        throw InvalidArgumentCastException();
    }
    return true;
}

template <typename T>
CLAParser::ArgumentHolder<T>::ArgumentHolder(char short_name, const std::string& long_name, const std::string& type,
                                             const std::string& help, bool required)
    : short_name_(1, short_name), long_name_(long_name), type_(type), help_(help), required_(required) {
}

template <typename T>
CLAParser::ArgumentHolder<T>::ArgumentHolder(const std::string& short_name, const std::string& long_name,
                                             const std::string& type, const std::string& help, bool required)
    : short_name_(short_name), long_name_(long_name), type_(type), help_(help), required_(required) {
}

template <typename T>
bool CLAParser::ArgumentHolder<T>::CanSet() const {
    return !value_;
}

template <typename T>
bool CLAParser::ArgumentHolder<T>::Set(const std::string& value) {
    if (!CanSet()) {
        return false;
    }
    try {
        value_ = std::make_unique<T>(GetFromString<T>(value));
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    } catch (const InvalidArgumentCastException& e) {
        return false;
    }
    return false;
}

template <typename T>
bool CLAParser::ArgumentHolder<T>::IsRequired() const {
    return required_;
}

template <typename T>
const std::string& CLAParser::ArgumentHolder<T>::GetShortName() const {
    return short_name_;
}

template <typename T>
const std::string& CLAParser::ArgumentHolder<T>::GetLongName() const {
    return long_name_;
}

template <typename T>
const std::string& CLAParser::ArgumentHolder<T>::GetType() const {
    return type_;
}

template <typename T>
const std::string& CLAParser::ArgumentHolder<T>::GetHelp() const {
    return help_;
}

template <typename T>
const T* CLAParser::ArgumentHolder<T>::GetValue() const {
    return value_.get();
}

template <typename T>
bool CLAParser::ArgumentHolder<T>::IsEmpty() const {
    return !value_;
}

CLAParser::FlagHolder::FlagHolder(char short_name, const std::string& long_name, const std::string& help)
    : ArgumentHolder<bool>(short_name, long_name, "[FLAG]", help, false) {
    this->value_ = std::make_unique<bool>(false);
}

bool CLAParser::FlagHolder::CanSet() const {
    return !(*value_);
}

bool CLAParser::FlagHolder::Set(const std::string& value) {
    if (!CanSet() || !value.empty()) {
        return false;
    }
    value_ = std::make_unique<bool>(true);
    return true;
}

template <typename T>
CLAParser::MultiplyArgumentsHolder<T>::MultiplyArgumentsHolder(const std::string& type, const std::string& help,
                                                               size_t required)
    : ArgumentHolder<std::vector<T>>(std::string(""), std::string(""), type, help, required >= 1),
      required_cnt_(required) {
    this->value_ = std::make_unique<std::vector<T>>();
}

template <typename T>
bool CLAParser::MultiplyArgumentsHolder<T>::Set(const std::string& value) {
    if (!CanSet()) {
        return false;
    }
    try {
        this->value_->push_back(GetFromString<T>(value));
        return true;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    } catch (const InvalidArgumentCastException& e) {
        return false;
    }
    return false;
}

template <typename T>
bool CLAParser::MultiplyArgumentsHolder<T>::CanSet() const {
    return true;
}

template <typename T>
bool CLAParser::MultiplyArgumentsHolder<T>::IsEmpty() const {
    return this->value_->size() < required_cnt_;
}

template <typename T>
const size_t CLAParser::MultiplyArgumentsHolder<T>::GetNumberArgs() const {
    return this->value_->size();
}

template <typename T>
const T* CLAParser::MultiplyArgumentsHolder<T>::GetValue(size_t index) const {
    try {
        return &(this->value_->at(index));
    } catch (std::out_of_range& e) {
        throw InvalidArgumentException();
    }
}

CLAParser::CLAParser() : program_name_("program_name"), args_(), multiply_arguments_() {
}

template <typename T>
void CLAParser::AddArgument(char short_name, const std::string& long_name, const std::string& type,
                            const std::string& help, bool required) {
    args_.emplace_back(std::make_unique<ArgumentHolder<T>>(short_name, long_name, type, help, required));
}

void CLAParser::AddFlag(char short_name, const std::string& long_name, const std::string& help) {
    args_.emplace_back(std::make_unique<FlagHolder>(short_name, long_name, help));
}

template <typename T>
void CLAParser::AddMultipleArguments(const std::string& type, const std::string& help, size_t required) {
    multiply_arguments_ = std::make_unique<MultiplyArgumentsHolder<T>>(type, help, required);
}

bool CLAParser::Parse(int argc, char** argv) {
    if (argc < 1) {
        return false;
    }
    program_name_ = static_cast<std::string>(argv[0]);
    for (size_t i = 1; i < static_cast<size_t>(argc); ++i) {
        std::string_view current(argv[i]);
        bool is_exist = false;
        if (current.size() >= 2 && current.substr(0, 2) == "--") {
            std::string_view long_name;
            std::string value;
            for (size_t j = 2; j <= current.size(); ++j) {
                if (j == current.size() || current[j] == '=') {
                    long_name = current.substr(2, j - 2);
                    value = static_cast<std::string>(current.substr(j));
                    break;
                }
            }
            for (auto& arg : args_) {
                if (arg->GetLongName() == long_name) {
                    if (!arg->Set(value)) {
                        return false;
                    }
                    is_exist = true;
                    break;
                }
            }
        } else if (!current.empty() && current.substr(0, 1) == "-") {
            if (current.size() < 2) {
                return false;
            }
            std::string short_name(1, current[1]);
            std::string value = static_cast<std::string>(current.substr(2));
            for (auto& arg : args_) {
                if (arg->GetShortName() == short_name) {
                    if (!arg->Set(value)) {
                        return false;
                    }
                    is_exist = true;
                    break;
                }
            }
        } else if (multiply_arguments_ != nullptr) {
            if (!multiply_arguments_->Set(std::string(current))) {
                return false;
            }
            is_exist = true;
        }
        if (!is_exist) {
            return false;
        }
    }
    for (auto& arg : args_) {
        if (arg->IsRequired() && arg->IsEmpty()) {
            return false;
        }
    }
    if (multiply_arguments_ != nullptr && multiply_arguments_->IsRequired() && multiply_arguments_->IsEmpty()) {
        return false;
    }
    return true;
}
template <typename T>
const T* CLAParser::GetArgumentValue(const std::string& long_name) const {
    for (auto& arg : args_) {
        if (arg->GetLongName() == long_name) {
            const ArgumentHolder<T>* arg_holder = dynamic_cast<const ArgumentHolder<T>*>(arg.get());
            return arg_holder->GetValue();
        }
    }
    throw InvalidArgumentException();
    return nullptr;
}

template <typename T>
size_t CLAParser::GetMultiplyArgumentsNumber() const {
    if (!multiply_arguments_) {
        throw InvalidArgumentCastException();
    }
    const MultiplyArgumentsHolder<T>* arg_holder =
        dynamic_cast<const MultiplyArgumentsHolder<T>*>(multiply_arguments_.get());
    return arg_holder->GetNumberArgs();
}

template <typename T>
const T* CLAParser::GetMultiplyArgumentValue(size_t index) const {
    if (!multiply_arguments_) {
        throw InvalidArgumentCastException();
    }
    const MultiplyArgumentsHolder<T>* arg_holder =
        dynamic_cast<const MultiplyArgumentsHolder<T>*>(multiply_arguments_.get());
    return arg_holder->GetValue(index);
}

std::string CLAParser::GetHelp() const {
    std::ostringstream stream;
    stream << "        Help information:\n";
    for (auto& arg : args_) {
        stream << "    -" << arg->GetShortName() << "  --" << arg->GetLongName() << "  " << arg->GetType();
        if (arg->IsRequired()) {
            stream << "   [REQUIRED]";
        }
        stream << "\n" << arg->GetHelp() << "\n\n";
    }

    return stream.str();
}