/**
 * argparsor.cpp
 *
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * Copyright (c) 2022 BLET Mickaël.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <algorithm>

#include "argparsor.h"

#define PREFIX_SIZEOF_SHORT_OPTION (sizeof("-") - 1)
#define PREFIX_SIZEOF_LONG_OPTION (sizeof("--") - 1)

namespace mblet {

/**
 * @brief arg is "-[^-].*"
 *
 * @param arg
 * @return true
 * @return false
 */
static inline bool isShortOption(const char* arg) {
    if (arg != NULL &&
        arg[0] == '-' && arg[1] != '-' &&
        arg[1] != '\0') {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief arg is "--.*"
 *
 * @param arg
 * @return true
 * @return false
 */
static inline bool isLongOption(const char* arg) {
    if (arg != NULL &&
        arg[0] == '-' && arg[1] == '-' &&
        arg[2] != '\0') {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief arg is "--"
 *
 * @param arg
 * @return true
 * @return false
 */
static inline bool isEndOption(const char* arg) {
    if (arg != NULL &&
        arg[0] == '-' && arg[1] == '-' &&
        arg[2] == '\0') {
        return true;
    }
    else {
        return false;
    }
}

/**
 * @brief get index of en option index or argc
 *
 * @param argc
 * @param argv
 * @return int
 */
static inline int endOptionIndex(int argc, char** argv) {
    for (int i = 0 ; i < argc ; ++i) {
        if (isEndOption(argv[i])) {
            return i;
        }
    }
    return argc;
}

/**
 * @brief split arg if contain '=' character
 *
 * @param arg
 * @param retOptionName
 * @param retArgument
 * @return true
 * @return false
 */
static inline bool takeArg(const char* arg, std::string* retOptionName, std::string* retArgument) {
    char* equal = ::strchr(const_cast<char*>(arg), '=');
    if (equal) {
        std::string firstPart(arg, 0, equal - arg);
        *retOptionName = firstPart;
        std::string secondPart(arg, equal - arg + 1, std::string::npos);
        *retArgument = secondPart;
        return true;
    }
    else {
        *retOptionName = arg;
        return false;
    }
}

static inline bool isFlagCharacter(const char& c) {
    if (::isalnum(c) || c == '-' || c == '_') {
        return true;
    }
    else {
        return false;
    }
}

static inline std::vector<std::string> splitFlags(const std::string& str) {
    std::vector<std::string> ret;
    std::size_t start;
    std::size_t i = 0;
    while (str[i] != '\0') {
        while (!isFlagCharacter(str[i])) {
            ++i;
        }
        if (str[i] != '\0') {
            start = i;
            while (str[i] != '\0' && isFlagCharacter(str[i])) {
                ++i;
            }
            ret.push_back(str.substr(start, i - start));
        }
    }
    return ret;
}

Argparsor::Argparsor() {
    addArgument("-h|--help", "help", "show this help message and exit");
}

Argparsor::~Argparsor() {}

Argparsor::Argument::Argument() :
    std::vector<Argument>(),
    names(std::vector<std::string>()),
    _isExist(false),
    _isRequired(false),
    _count(0),
    _type(NONE),
    _help(std::string()),
    _argHelp(std::string()),
    _argument(std::string()),
    _defaultValue(std::string())
{}

static bool compareOption(const Argparsor::Argument& first, const Argparsor::Argument& second) {
    if (first.getType() == Argparsor::Argument::POSITIONAL_ARGUMENT && second.getType() == Argparsor::Argument::POSITIONAL_ARGUMENT) {
        return first.names[0] <= second.names[0];
    }
    else if (first.getType() == Argparsor::Argument::POSITIONAL_ARGUMENT) {
        return false;
    }
    else if (second.getType() == Argparsor::Argument::POSITIONAL_ARGUMENT) {
        return true;
    }
    if (isShortOption(first.names[0].c_str()) && isShortOption(second.names[0].c_str())) {
        return first.names[0] <= second.names[0];
    }
    else if (isShortOption(first.names[0].c_str()) && !isShortOption(second.names[0].c_str())) {
        return true;
    }
    else if (!isShortOption(first.names[0].c_str()) && isShortOption(second.names[0].c_str())) {
        return false;
    }
    else {
        return first.names[0] < second.names[0];
    }
}

static bool compareFlag(const std::string& first, const std::string& second) {
    if (isShortOption(first.c_str()) && isShortOption(second.c_str())) {
        return first <= second;
    }
    else if (isShortOption(first.c_str()) && !isShortOption(second.c_str())) {
        return true;
    }
    else if (!isShortOption(first.c_str()) && isShortOption(second.c_str())) {
        return false;
    }
    else {
        return first < second;
    }
}

std::ostream& Argparsor::getUsage(std::ostream& oss) {
    {
        std::list<Argument>::iterator it;
        for (it = _arguments.begin(); it != _arguments.end(); ++it) {
            std::sort(it->names.begin(), it->names.end(), &compareFlag);
        }
    }
    _arguments.sort(&compareOption);
    if (!_usage.empty()) {
        oss << _usage;
        return oss;
    }
    // usage line
    oss << "usage: " << _binaryName;
    std::list<Argument>::const_iterator it;
    for (it = _arguments.begin() ; it != _arguments.end() ; ++it) {
        if (it->getType() == Argument::POSITIONAL_ARGUMENT) {
            continue;
        }
        oss << ' ';
        if (!it->isRequired()) {
            oss << '[';
        }
        oss << it->names[0];
        switch (it->getType()) {
            case Argument::POSITIONAL_ARGUMENT:
            case Argument::SIMPLE_OPTION:
            case Argument::NUMBER_OPTION:
            case Argument::INFINITE_OPTION:
            case Argument::MULTI_OPTION:
            case Argument::MULTI_INFINITE_OPTION:
            case Argument::MULTI_NUMBER_OPTION:
                oss << ' ' << it->getArgHelp();
                break;
            default:
                break;
        }
        if (!it->isRequired()) {
            oss << ']';
        }
    }
    for (it = _arguments.begin() ; it != _arguments.end() ; ++it) {
        if (it->getType() != Argument::POSITIONAL_ARGUMENT) {
            continue;
        }
        oss << ' ';
        if (!it->isRequired()) {
            oss << '[';
        }
        oss << it->names[0];
        if (!it->isRequired()) {
            oss << ']';
        }
    }
    oss << "\n";
    // description
    if (!_description.empty()) {
        oss << "\n" << _description << "\n";
    }
    // optionnal
    if (!_arguments.empty()) {
        std::list<std::pair<std::string, std::string> > positionals;
        std::list<std::pair<std::string, std::string> > optionnals;
        for (it = _arguments.begin() ; it != _arguments.end() ; ++it) {
            std::list<std::pair<std::string, std::string> >* listOption = NULL;
            if (it->getType() == Argument::POSITIONAL_ARGUMENT) {
                positionals.push_back(std::pair<std::string, std::string>("", ""));
                listOption = &positionals;
            }
            else {
                optionnals.push_back(std::pair<std::string, std::string>("", ""));
                listOption = &optionnals;
            }
            std::string& optionStr = listOption->back().first;
            std::string& helpStr = listOption->back().second;
            optionStr += "  ";
            for (std::size_t i = 0; i < it->names.size(); ++i) {
                if (i > 0) {
                    optionStr += ", ";
                }
                optionStr += it->names[i];
            }
            switch (it->getType()) {
                case Argument::POSITIONAL_ARGUMENT:
                case Argument::SIMPLE_OPTION:
                case Argument::NUMBER_OPTION:
                case Argument::INFINITE_OPTION:
                case Argument::MULTI_OPTION:
                case Argument::MULTI_INFINITE_OPTION:
                case Argument::MULTI_NUMBER_OPTION:
                    optionStr += " " + it->getArgHelp();
                    break;
                default:
                    break;
            }
            helpStr += "  " + it->getHelp();
            if (it->isRequired()) {
                helpStr += " (required)";
            }
            else {
                switch (it->getType()) {
                    case Argument::POSITIONAL_ARGUMENT:
                    case Argument::SIMPLE_OPTION:
                    case Argument::NUMBER_OPTION:
                    case Argument::INFINITE_OPTION:
                    case Argument::MULTI_OPTION:
                    case Argument::MULTI_INFINITE_OPTION:
                    case Argument::MULTI_NUMBER_OPTION:
                        if (!it->getDefaultValue().empty()) {
                            helpStr += " (default: " + it->getDefaultValue() + ")";
                        }
                        break;
                    default:
                        break;
                }
            }
            helpStr += "\n";
        }
        // calculate width max
        std::size_t max = 0;
        std::list<std::pair<std::string, std::string> >::iterator optIt;
        for (optIt = positionals.begin() ; optIt != positionals.end() ; ++optIt) {
            if (max < optIt->first.size()) {
                max = optIt->first.size();
            }
        }
        for (optIt = optionnals.begin() ; optIt != optionnals.end() ; ++optIt) {
            if (max < optIt->first.size()) {
                max = optIt->first.size();
            }
        }
        if (!positionals.empty()) {
            oss << "\npositional arguments:\n";
            for (optIt = positionals.begin() ; optIt != positionals.end() ; ++optIt) {
                oss.width(max);
                oss.flags(std::ios::left);
                oss << optIt->first;
                oss.width(0);
                oss << optIt->second;
            }
        }
        if (!optionnals.empty()) {
            oss << "\noptional arguments:\n";
            for (optIt = optionnals.begin() ; optIt != optionnals.end() ; ++optIt) {
                oss.width(max);
                oss.flags(std::ios::left);
                oss << optIt->first;
                oss.width(0);
                oss << optIt->second;
            }
        }
    }
    // epilog
    if (!_epilog.empty()) {
        oss << "\n" << _epilog << "\n";
    }
    return oss;
}

void Argparsor::parseArguments(int argc, char* argv[], bool alternative, bool strict) {
    // sort all arguments

    _binaryName = argv[0];
    // save index of "--" if exist
    int endIndex = endOptionIndex(argc, argv);
    // create parse info
    int i = 1;
    // foreach argument
    for (i = 1 ; i < argc ; ++i) {
        if (isShortOption(argv[i])) {
            parseShortArgument(endIndex, argv, &i, alternative);
        }
        else if (isLongOption(argv[i])) {
            parseLongArgument(endIndex, argv, &i, alternative);
        }
        else if (isEndOption(argv[i])) {
            ++i;
            while (i < argc) {
                parsePositionnalArgument(argv, &i, strict);
                ++i;
            }
            break;
        }
        else {
            parsePositionnalArgument(argv, &i, strict);
        }
    }
    // check help option
    if (_helpOption != NULL && _helpOption->isExist()) {
        getUsage();
        exit(1);
    }
    // check require option
    std::list<Argument>::iterator it;
    for (it = _arguments.begin() ; it != _arguments.end() ; ++it) {
        if (it->isRequired() && it->isExist() == false) {
            if (it->getType() == Argument::POSITIONAL_ARGUMENT) {
                throw ParseArgumentRequiredException(it->names[0].c_str(), "argument is required");
            }
            else {
                throw ParseArgumentRequiredException(it->names[0].c_str(), "option is required");
            }
        }
    }
}

void Argparsor::addArgument(const std::vector<std::string>& nameOrFlags, const char* actionOrDefault, const char* help,
                            bool isRequired, const char* argsHelp, std::size_t nbArgs,
                            const std::vector<std::string>& defaultArgs) {
    std::vector<std::string> flags = nameOrFlags;
    std::sort(flags.begin(), flags.end(), &compareFlag);

    // check name or flags
    if (flags.size() == 1 && flags[0][0] != '-') {
        if (flags[0].empty()) {
            throw ArgumentException("", "bad name argument");
        }
        else if (_argumentFromName.find(flags[0]) != _argumentFromName.end()) {
            throw ArgumentException(flags[0].c_str(), "bad name argument already exist");
        }
        // create option
        _arguments.push_back(Argument());
        Argument& option = _arguments.back();
        _argumentFromName.insert(std::pair<std::string, Argument*>(flags[0], &option));

        option.setIsRequired(isRequired);
        option.names.push_back(nameOrFlags[0]);
        option.setType(Argument::POSITIONAL_ARGUMENT);
        if (help != NULL) {
            option.setHelp(help);
        }
        if (actionOrDefault != NULL && actionOrDefault[0] != '\0') {
            option.setArgument(actionOrDefault);
            option.setDefaultValue(actionOrDefault);
        }
        else if (defaultArgs.size() == 1) {
            option.setArgument(defaultArgs[0]);
            option.setDefaultValue(defaultArgs[0]);
        }
        return;
    }

    enum eAction {
        NONE = 0,
        STORE_TRUE,
        STORE_FALSE,
        APPEND,
        EXTEND,
        VERSION,
        HELP
    } action = NONE;
    if (actionOrDefault != NULL) {
        if (strncmp("store_true", actionOrDefault, sizeof("store_true")) == 0) {
            action = STORE_TRUE;
        }
        else if (strncmp("store_false", actionOrDefault, sizeof("store_false")) == 0) {
            action = STORE_FALSE;
        }
        else if (strncmp("append", actionOrDefault, sizeof("append")) == 0) {
            action = APPEND;
        }
        else if (strncmp("extend", actionOrDefault, sizeof("extend")) == 0) {
            action = EXTEND;
        }
        else if (strncmp("version", actionOrDefault, sizeof("version")) == 0) {
            action = VERSION;
        }
        else if (strncmp("help", actionOrDefault, sizeof("help")) == 0) {
            action = HELP;
        }
    }

    // remove last help option
    if (action == HELP && _helpOption != NULL) {
        for (std::list<Argument>::iterator itList = _arguments.begin() ; itList != _arguments.end() ; ++itList) {
            if (_helpOption == &(*itList)) {
                _arguments.erase(itList);
                break;
            }
        }
        std::map<std::string, Argument*>::iterator itMap = _argumentFromName.begin();
        while (itMap != _argumentFromName.end()) {
            if (_helpOption == itMap->second) {
                _argumentFromName.erase(itMap);
                itMap = _argumentFromName.begin();
            }
            else {
                ++itMap;
            }
        }
        _helpOption = NULL;
    }
    if (nameOrFlags.empty()) {
        if (action == HELP) {
            return;
        }
        else {
            throw ArgumentException("", "invalid empty flag");
        }
    }

    const char* shortFlag = NULL;
    const char* longFlag = NULL;
    // create argument
    _arguments.push_back(Argument());
    Argument& argument = _arguments.back();
    for (std::size_t i = 0 ; i < flags.size() ; ++i) {
        if (flags[i][0] != '-') {
            throw ArgumentException(flags[i].c_str(), "invalid flag not start by '-' character");
        }

        if (flags[i] == "-") {
            throw ArgumentException(flags[i].c_str(), "invalid flag not be only '-' character");
        }
        if (flags[i] == "--") {
            throw ArgumentException(flags[i].c_str(), "invalid flag not be only '--' characters");
        }

        if (flags[i][0] != '-' && flags[i][1] != '-' && flags[i][2] != '\0') {
            throw ArgumentException(flags[i].c_str(), "invalid short flag has not only one character");
        }

        if (_argumentFromName.find(flags[i]) != _argumentFromName.end()) {
            throw ArgumentException(flags[i].c_str(), "invalid flag already exist");
        }

        if (longFlag == NULL && flags[i][0] == '-' && flags[i][1] == '-') {
            longFlag = flags[i].c_str();
        }
        else if (shortFlag == NULL && flags[i][0] == '-' && flags[i][1] != '-') {
            shortFlag = flags[i].c_str();
        }
        _argumentFromName.insert(std::pair<std::string, Argument*>(flags[i], &argument));
        argument.names.push_back(flags[i]);
    }
    if (nbArgs > 0 && (argsHelp == NULL || argsHelp[0] == '\0')) {
        std::string defaultUsageName;
        // create a defaultUsageName from longName or shortName
        if (longFlag != NULL) {
            defaultUsageName = longFlag + PREFIX_SIZEOF_LONG_OPTION;
        }
        else {
            defaultUsageName = shortFlag + PREFIX_SIZEOF_SHORT_OPTION;
        }
        for (std::size_t i = 0 ; i < defaultUsageName.size() ; ++i) {
            defaultUsageName[i] = ::toupper(defaultUsageName[i]);
        }
        if (nbArgs == '+') {
            argument.setArgHelp(defaultUsageName + "...");
        }
        else {
            std::string numberDefaultUsageName;
            for (std::size_t i = 0 ; i < nbArgs ; ++i) {
                if (i > 0) {
                    numberDefaultUsageName += " ";
                }
                numberDefaultUsageName += defaultUsageName;
            }
            argument.setArgHelp(numberDefaultUsageName);
        }
    }
    argument.setIsRequired(isRequired);
    if (help != NULL && help[0] != '\0') {
        argument.setHelp(help);
    }
    if (argsHelp != NULL && argsHelp[0] != '\0') {
        argument.setArgHelp(argsHelp);
    }
    argument.setNbArgs(nbArgs);
    // is bool
    if (action == STORE_TRUE) {
        argument.setType(Argument::BOOLEAN_OPTION);
    }
    else if (action == STORE_FALSE) {
        argument.setType(Argument::REVERSE_BOOLEAN_OPTION);
    }
    // is simple
    else if (nbArgs == 1 && action == NONE) {
        argument.setType(Argument::SIMPLE_OPTION);
    }
    // is infinite
    else if (nbArgs == '+' && action == NONE) {
        argument.setType(Argument::INFINITE_OPTION);
    }
    // is number
    else if (nbArgs > 1 && action == NONE) {
        argument.setType(Argument::NUMBER_OPTION);
    }
    // is multi
    else if (nbArgs == 1 && action == APPEND) {
        argument.setType(Argument::MULTI_OPTION);
    }
    // is multi number
    else if (nbArgs > 0 && action == APPEND) {
        argument.setType(Argument::MULTI_NUMBER_OPTION);
    }
    // is multi
    else if (action == EXTEND) {
        argument.setType(Argument::MULTI_INFINITE_OPTION);
    }
    // default arguments
    if (isRequired == false && defaultArgs.size() > 0) {
        switch (argument.getType()) {
            case Argument::SIMPLE_OPTION:
            case Argument::NUMBER_OPTION:
                if (defaultArgs.size() != nbArgs) {
                    if (longFlag != NULL) {
                        throw ArgumentException(longFlag, "invalid number of argument with number of default argument");
                    }
                    else {
                        throw ArgumentException(shortFlag, "invalid number of argument with number of default argument");
                    }
                }
                for (std::size_t i = 0 ; i < defaultArgs.size() ; ++i) {
                    if (i > 0) {
                        argument.setDefaultValue(argument.getDefaultValue() + ", ");
                    }
                    argument.push_back(Argument());
                    argument.back().setArgument(defaultArgs[i]);
                    argument.back().setDefaultValue(defaultArgs[i]);
                    argument.setDefaultValue(argument.getDefaultValue() + defaultArgs[i]);
                }
                break;
            case Argument::MULTI_OPTION:
            case Argument::INFINITE_OPTION:
            case Argument::MULTI_INFINITE_OPTION:
                for (std::size_t i = 0 ; i < defaultArgs.size() ; ++i) {
                    if (i > 0) {
                        argument.setDefaultValue(argument.getDefaultValue() + ", ");
                    }
                    argument.push_back(Argument());
                    argument.back().setArgument(defaultArgs[i]);
                    argument.back().setDefaultValue(defaultArgs[i]);
                    argument.setDefaultValue(argument.getDefaultValue() + defaultArgs[i]);
                }
                break;
            case Argument::MULTI_NUMBER_OPTION:
                if (defaultArgs.size() % nbArgs != 0) {
                    if (longFlag != NULL) {
                        throw ArgumentException(longFlag, "invalid number of argument with number of default argument");
                    }
                    else {
                        throw ArgumentException(shortFlag, "invalid number of argument with number of default argument");
                    }
                }
                for (std::size_t i = 0 ; i < defaultArgs.size() / nbArgs; ++i) {
                    if (i > 0) {
                        argument.setDefaultValue(argument.getDefaultValue() + ", ");
                    }
                    argument.setDefaultValue(argument.getDefaultValue() + "(");
                    argument.push_back(Argument());
                    argument.back().setType(Argument::NUMBER_OPTION);
                    for (std::size_t j = 0 ; j < nbArgs ; ++j) {
                        if (j > 0) {
                            argument.setDefaultValue(argument.getDefaultValue() + ", ");
                            argument.back().setDefaultValue(argument.back().getDefaultValue() + ", ");
                        }
                        argument.back().push_back(Argument());
                        argument.back().back().setArgument(defaultArgs[i * nbArgs + j]);
                        argument.back().back().setDefaultValue(defaultArgs[i * nbArgs + j]);
                        argument.setDefaultValue(argument.getDefaultValue() + defaultArgs[i * nbArgs + j]);
                        argument.back().setDefaultValue(argument.back().getDefaultValue() + defaultArgs[i * nbArgs + j]);
                    }
                    argument.setDefaultValue(argument.getDefaultValue() + ")");
                }
                break;
            default:
                if (longFlag != NULL) {
                    throw ArgumentException(longFlag, "invalid number of argument with number of default argument");
                }
                else {
                    throw ArgumentException(shortFlag, "invalid number of argument with number of default argument");
                }
                break;
        }
    }
    if (action == HELP) {
        _helpOption = &argument;
    }
}

void Argparsor::addArgument(const char* nameOrFlags, const char* actionOrDefault, const char* help, bool isRequired,
                            const char* argsHelp, std::size_t nbArgs, std::size_t nbDefaultArgs, ...) {
    // transform nameOrFlags to vector
    std::vector<std::string> flags = splitFlags(nameOrFlags);
    // transform default arguments to vector
    std::vector<std::string> defaultArgs;
    va_list pa;
    va_start(pa, nbDefaultArgs);
    for (std::size_t i = 0 ; i < nbDefaultArgs ; ++i) {
        const char* defaultValue = (const char*)va_arg(pa, const char*);
        defaultArgs.push_back(defaultValue);
    }
    va_end(pa);
    addArgument(flags, actionOrDefault, help, isRequired, argsHelp, nbArgs, defaultArgs);
}

std::string Argparsor::dump() {
    {
        std::list<Argument>::iterator it;
        for (it = _arguments.begin(); it != _arguments.end(); ++it) {
            std::sort(it->names.begin(), it->names.end(), &compareFlag);
        }
    }
    _arguments.sort(&compareOption);
    std::ostringstream oss("");
    std::list<Argument>::const_iterator it;
    for (it = _arguments.begin(); it != _arguments.end(); ++it) {
        for (std::size_t i = 0; i < it->names.size(); ++i) {
            if (i > 0){
                oss << ", ";
            }
            oss << it->names[i];
        }
        oss << "  ";
        oss << "isExist: " << it->isExist() << ", ";
        oss << "type: " << it->getType() << ", ";
        oss << "values: " << it->str();
        oss << '\n';
    }
    return oss.str();
}

/*
** private
*/

void Argparsor::parseShortArgument(int maxIndex, char* argv[], int* index, bool alternative) {
    std::string options;
    std::string arg;
    std::map<std::string, Argument*>::iterator it;
    bool hasArg = takeArg(argv[*index], &options, &arg);
    if (alternative) {
        // try to find long option
        it = _argumentFromName.find("-" + options);
        if (it != _argumentFromName.end()) {
            parseArgument(maxIndex, argv, index, hasArg, options.c_str() + PREFIX_SIZEOF_SHORT_OPTION,
                          arg.c_str(), it->second, alternative);
            return ;
        }
    }
    // get firsts option
    for (std::size_t i = 1 ; i < options.size() - 1 ; ++i) {
        std::string charOption(options, i, 1);
        it = _argumentFromName.find("-" + charOption);
        if (it == _argumentFromName.end()) {
            throw ParseArgumentException(charOption.c_str(), "invalid option");
        }
        else if (it->second->getType() != Argument::BOOLEAN_OPTION &&
                 it->second->getType() != Argument::REVERSE_BOOLEAN_OPTION) {
            throw ParseArgumentException(charOption.c_str(), "only last option can be use a parameter");
        }
        it->second->setIsExist(true);
        it->second->addCount();
    }
    // get last option
    std::string charOption(options, options.size() - 1, 1);
    it = _argumentFromName.find("-" + charOption);
    if (it == _argumentFromName.end()) {
        throw ParseArgumentException(charOption.c_str(), "invalid option");
    }
    parseArgument(maxIndex, argv, index, hasArg, charOption.c_str(),
                  arg.c_str(), it->second, alternative);
}

void Argparsor::parseLongArgument(int maxIndex, char* argv[], int* index, bool alternative) {
    std::string option;
    std::string arg;
    std::map<std::string, Argument*>::iterator it;
    bool hasArg = takeArg(argv[*index], &option, &arg);
    it = _argumentFromName.find(option);
    if (it == _argumentFromName.end()) {
        throw ParseArgumentException(option.c_str() + PREFIX_SIZEOF_LONG_OPTION, "invalid option");
    }
    parseArgument(maxIndex, argv, index, hasArg, option.c_str() + PREFIX_SIZEOF_LONG_OPTION,
                  arg.c_str(), it->second, alternative);
}

void Argparsor::parseArgument(int maxIndex, char* argv[], int* index, bool hasArg, const char* option, const char* arg,
                              Argument* argument, bool alternative) {
    if (hasArg) {
        switch (argument->getType()) {
            case Argument::BOOLEAN_OPTION:
            case Argument::REVERSE_BOOLEAN_OPTION:
                throw ParseArgumentException(option, "option cannot use with argument");
                break;
            case Argument::SIMPLE_OPTION:
                argument->setArgument(arg);
                break;
            case Argument::NUMBER_OPTION:
                if (argument->nbArgs() != 1) {
                    throw ParseArgumentException(option, "option cannot use with only 1 argument");
                }
                argument->clear();
                argument->push_back(Argument());
                argument->back().setArgument(arg);
                break;
            case Argument::INFINITE_OPTION:
                argument->clear();
                argument->push_back(Argument());
                argument->back().setArgument(arg);
                break;
            case Argument::MULTI_OPTION:
                if (argument->isExist() == false) {
                    argument->clear();
                }
                argument->push_back(Argument());
                argument->back().setArgument(arg);
                break;
            case Argument::MULTI_INFINITE_OPTION:
                if (argument->isExist() == false) {
                    argument->clear();
                }
                argument->push_back(Argument());
                argument->back().setArgument(arg);
                break;
            case Argument::MULTI_NUMBER_OPTION:
                if (argument->nbArgs() != 1) {
                    throw ParseArgumentException(option, "option cannot use with only 1 argument");
                }
                if (argument->isExist() == false) {
                    argument->clear();
                }
                argument->push_back(Argument());
                argument->back().push_back(Argument());
                argument->back().back().setArgument(arg);
                break;
            default:
                break;
        }
    }
    else {
        switch (argument->getType()) {
            case Argument::BOOLEAN_OPTION:
            case Argument::REVERSE_BOOLEAN_OPTION:
                break;
            case Argument::SIMPLE_OPTION:
                if (*index + 1 >= maxIndex) {
                    throw ParseArgumentException(option, "bad number of argument");
                }
                ++(*index);
                argument->setArgument(argv[*index]);
                break;
            case Argument::NUMBER_OPTION:
                argument->clear();
                if (*index + argument->nbArgs() >= static_cast<unsigned int>(maxIndex)) {
                    throw ParseArgumentException(option, "bad number of argument");
                }
                for (unsigned int j = *index + 1 ; j <= (*index + argument->nbArgs()) ; ++j) {
                    argument->push_back(Argument());
                    argument->back().setArgument(argv[j]);
                }
                *index += argument->nbArgs();
                break;
            case Argument::INFINITE_OPTION: {
                argument->clear();
                std::size_t countArg = 0;
                for (int j = *index + 1 ; j < maxIndex ; ++j) {
                    if (endOfInfiniteArgument(argv[j], alternative)) {
                        break;
                    }
                    argument->push_back(Argument());
                    argument->back().setArgument(argv[j]);
                    ++countArg;
                }
                *index += countArg;
                break;
            }
            case Argument::MULTI_OPTION: {
                if (*index + 1 >= maxIndex) {
                    throw ParseArgumentException(option, "bad number of argument");
                }
                if (argument->isExist() == false) {
                    argument->clear();
                }
                ++(*index);
                argument->push_back(Argument());
                argument->back().setArgument(argv[*index]);
                break;
            }
            case Argument::MULTI_INFINITE_OPTION: {
                if (argument->isExist() == false) {
                    argument->clear();
                }
                std::size_t countArg = 0;
                for (int j = *index + 1 ; j < maxIndex ; ++j) {
                    if (endOfInfiniteArgument(argv[j], alternative)) {
                        break;
                    }
                    argument->push_back(Argument());
                    argument->back().setArgument(argv[j]);
                    ++countArg;
                }
                *index += countArg;
                break;
            }
            case Argument::MULTI_NUMBER_OPTION: {
                if (*index + argument->nbArgs() >= static_cast<unsigned int>(maxIndex)) {
                    throw ParseArgumentException(option, "bad number of argument");
                }
                if (argument->isExist() == false) {
                    argument->clear();
                }
                argument->push_back(Argument());
                argument->back().setType(Argument::NUMBER_OPTION);
                for (unsigned int i = *index + 1 ; i <= *index + argument->nbArgs() ; ++i) {
                    argument->back().push_back(Argument());
                    argument->back().back().setArgument(argv[i]);
                }
                *index += argument->nbArgs();
                break;
            }
            default:
                break;
        }
    }
    argument->setIsExist(true);
    argument->addCount();
}

bool Argparsor::endOfInfiniteArgument(const char* argument, bool alternative) {
    std::string option;
    std::string arg;
    std::map<std::string, Argument*>::iterator it;
    if (isShortOption(argument)) {
        takeArg(argument, &option, &arg);
        if (alternative) {
            it = _argumentFromName.find("-" + option);
            if (it != _argumentFromName.end()) {
                return true;
            }
        }
        // get firsts option
        for (std::size_t i = 1 ; i < option.size() - 1 ; ++i) {
            std::string charOption(option, i, 1);
            it = _argumentFromName.find("-" + charOption);
            if (it == _argumentFromName.end()) {
                return false;
            }
            else if (it->second->getType() != Argument::BOOLEAN_OPTION &&
                     it->second->getType() != Argument::REVERSE_BOOLEAN_OPTION) {
                return false;
            }
        }
        // get last option
        std::string charOption(option, option.size() - 1, 1);
        it = _argumentFromName.find("-" + charOption);
    }
    else if (isLongOption(argument)) {
        takeArg(argument, &option, &arg);
        it = _argumentFromName.find(option);
    }
    else {
        return false;
    }
    if (it == _argumentFromName.end()) {
        return false;
    }
    return true;
}

void Argparsor::parsePositionnalArgument(char* argv[], int* index, bool strict) {
    std::list<Argument>::iterator it;
    for (it = _arguments.begin() ; it != _arguments.end() ; ++it) {
        // assign to first not used positional argument
        if (it->getType() == Argument::POSITIONAL_ARGUMENT && it->isExist() == false) {
            it->setArgument(argv[*index]);
            it->setIsExist(true);
            break;
        }
    }
    if (it == _arguments.end()) {
        if (strict) {
            throw ParseArgumentException(argv[*index], "invalid additional argument");
        }
        else {
            _additionalArguments.push_back(argv[*index]);
        }
    }
}

} // namespace mblet