/**
 * argparsor.h
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

#ifndef _MBLET_ARGPARSOR_HPP_
#define _MBLET_ARGPARSOR_HPP_

#include <map>
#include <cstdarg>
#include <string>
#include <vector>
#include <list>
#include <sstream>
#include <iostream>

namespace mblet {

/**
 * @brief Object for parse the main arguments
 */
class Argparsor {

  public:

    /**
     * @brief Basic exception from std::exception
     */
    class Exception : public std::exception {
      public:
        Exception(const char* str) : _str(str) {}
        virtual ~Exception() throw() {}
        const char* what() const throw() {
            return _str.c_str();
        }
      protected:
        std::string _str;
    };

    /**
     * @brief Argument exception from Exception
     */
    class ArgumentException : public Exception {
      public:
        ArgumentException(const char* argument, const char* message) : Exception(message), _argument(argument) {}
        virtual ~ArgumentException() throw() {}
        const char* argument() const throw() {
            return _argument.c_str();
        }
      protected:
        std::string _argument;
    };

    /**
     * @brief Parse argument exception from ArgumentException
     */
    class ParseArgumentException : public ArgumentException {
      public:
        ParseArgumentException(const char* argument, const char* message) : ArgumentException(argument, message) {}
        virtual ~ParseArgumentException() throw() {}
    };

    /**
     * @brief Parse argument required exception from ParseArgumentException
     */
    class ParseArgumentRequiredException : public ParseArgumentException {
      public:
        ParseArgumentRequiredException(const char* argument, const char* message) : ParseArgumentException(argument, message) {}
        virtual ~ParseArgumentRequiredException() throw() {}
    };

    /**
     * @brief Get access denied exception from ArgumentException
     */
    class AccessDeniedException : public ArgumentException {
      public:
        AccessDeniedException(const char* argument, const char* message) : ArgumentException(argument, message) {}
        virtual ~AccessDeniedException() throw() {}
    };

    /**
     * @brief Argument object
     */
    class Argument : public std::vector<Argument> {

      public:

        enum Type {
            NONE = 0,
            BOOLEAN_OPTION,
            REVERSE_BOOLEAN_OPTION,
            SIMPLE_OPTION,
            NUMBER_OPTION,
            INFINITE_OPTION,
            MULTI_OPTION,
            MULTI_INFINITE_OPTION,
            MULTI_NUMBER_OPTION,
            POSITIONAL_ARGUMENT
        };

        /**
         * @brief Construct a new Argument object
         */
        Argument();

        inline bool isExist() const {
            return _isExist;
        }

        inline bool isRequired() const {
            return _isRequired;
        }

        inline std::size_t count() const {
            return _count;
        }

        inline std::size_t nbArgs() const {
            return _nbArgs;
        }

        inline Type getType() const {
            return _type;
        }

        inline const std::string& getHelp() const {
            return _help;
        }

        inline const std::string& getArgHelp() const {
            return _argHelp;
        }

        inline const std::string& getArgument() const {
            return _argument;
        }

        inline const std::string& getDefaultValue() const {
            return _defaultValue;
        }

        inline void setIsExist(bool isExist) {
            _isExist = isExist;
        }

        inline void setIsRequired(bool isRequired) {
            _isRequired = isRequired;
        }

        inline void addCount() {
            ++_count;
        }

        inline void setNbArgs(std::size_t nbArgs) {
            _nbArgs = nbArgs;
        }

        inline void setType(enum Type type) {
            _type = type;
        }

        inline void setHelp(const std::string& help) {
            _help = help;
        }

        inline void setArgHelp(const std::string& argHelp) {
            _argHelp = argHelp;
        }

        inline void setArgument(const std::string& argument) {
            _argument = argument;
        }

        inline void setDefaultValue(const std::string& defaultValue) {
            _defaultValue = defaultValue;
        }

        inline bool boolean() const {
            if (_type == BOOLEAN_OPTION) {
                return _isExist;
            }
            else if (_type == REVERSE_BOOLEAN_OPTION) {
                return !_isExist;
            }
            else {
                throw Exception("convertion to bool not authorized");
            }
        }

        inline std::string str() const {
            if (_type == BOOLEAN_OPTION) {
                return ((_isExist) ? "true" : "false");
            }
            else if (_type == REVERSE_BOOLEAN_OPTION) {
                return ((_isExist) ? "false" : "true");
            }
            else {
                std::ostringstream oss("");
                if (!empty()) {
                    for (std::size_t i = 0 ; i < size() ; ++i) {
                        if (i > 0) {
                            oss << ", ";
                        }
                        if (!at(i).empty()) {
                            oss << "(";
                            for (std::size_t j = 0 ; j < at(i).size() ; ++j) {
                                if (j > 0) {
                                    oss << ", ";
                                }
                                oss << at(i).at(j).getArgument();
                            }
                            oss << ")";
                        }
                        else {
                            oss << at(i).getArgument();
                        }
                    }
                }
                else {
                    oss << _argument;
                }
                return oss.str();
            }
        }

        /**
         * @brief Override bool operator
         *
         * @return true if exist or false if not exist
         */
        inline operator bool() const {
            return isExist();
        }

        /**
         * @brief tranform to string
         *
         * @return std::string
         */
        inline operator std::string() const {
            return str();
        }

        /**
         * @brief tranform to vector of string
         *
         * @return std::vector<std::string>
         */
        inline operator std::vector<std::string>() const {
            if (_type == NUMBER_OPTION || _type == MULTI_OPTION ||
                _type == INFINITE_OPTION || _type == MULTI_INFINITE_OPTION) {
                std::vector<std::string> ret;
                for (std::size_t i = 0 ; i < size() ; ++i) {
                    ret.push_back(at(i).getArgument());
                }
                return ret;
            }
            else {
                throw Exception("convertion to vector of string not authorized");
            }
        }

        /**
         * @brief tranform to vector of vector of string
         *
         * @return std::vector<std::vector<std::string> >
         */
        inline operator std::vector<std::vector<std::string> >() const {
            if (_type == MULTI_NUMBER_OPTION) {
                std::vector<std::vector<std::string> > ret;
                for (std::size_t i = 0 ; i < size() ; ++i) {
                    ret.push_back(std::vector<std::string>());
                    for (std::size_t j = 0 ; j < at(i).size() ; ++j) {
                        ret[i].push_back(at(i).at(j).getArgument());
                    }
                }
                return ret;
            }
            else {
                throw Exception("convertion to vector of vector of string not authorized");
            }
        }

        /**
         * @brief overide brakcet operator
         *
         * @param index
         * @return const Argument&
         */
        inline const Argument& operator[](unsigned long index) const {
            return at(index);
        }

        /**
         * @brief Friend function for convert Argument object to ostream
         *
         * @param os
         * @param map
         * @return std::ostream&
         */
        inline friend std::ostream& operator<<(std::ostream& os, const Argument& map) {
            os << map.str();
            return os;
        }

        std::vector<std::string> names;
        bool _isExist;
        bool _isRequired;
        std::size_t _count;
        std::size_t _nbArgs;
        enum Type _type;
        std::string _help;
        std::string _argHelp;
        std::string _argument;
        std::string _defaultValue;

      private:


    };

    /**
     * @brief Construct a new Argparsor object
     */
    Argparsor();

    /**
     * @brief Destroy the Argparsor object
     */
    ~Argparsor();

    /**
     * @brief Get the bynary name
     *
     * @return const std::string&
     */
    inline const std::string& getBynaryName() const {
        return _binaryName;
    }

    /**
     * @brief Get the usage object
     *
     * @param oss
     * @return std::ostream&
     */
    std::ostream& getUsage(std::ostream& oss = std::cout);

    /**
     * @brief Get the argument object
     *
     * @param str
     * @return const Argument&
     */
    inline const Argument& getOption(const char* str) const {
        std::map<std::string, Argument*>::const_iterator cit = _argumentFromName.find(str);
        if (cit == _argumentFromName.end()) {
            throw AccessDeniedException(str, "option not found");
        }
        return *(cit->second);
    }

    /**
     * @brief Get the argument object
     *
     * @param str
     * @return const Argument&
     */
    inline const Argument& getOption(const std::string& str) const {
        return getOption(str.c_str());
    }

    /**
     * @brief Override bracket operator with getOption
     *
     * @param str
     * @return const Argument&
     */
    inline const Argument& operator[](const char* str) const {
        return getOption(str);
    }

    /**
     * @brief Override bracket operator with getOption
     *
     * @param str
     * @return const Argument&
     */
    inline const Argument& operator[](const std::string& str) const {
        return getOption(str);
    }

    /**
     * @brief Get the vector of additional argument
     *
     * @return const std::vector<std::string>&
     */
    inline const std::vector<std::string>& getAdditionalArguments() const {
        return _additionalArguments;
    }

    /**
     * @brief Set the usage message
     *
     * @param usage
     */
    inline void setUsage(const char* usage) {
        _usage = usage;
    }

    /**
     * @brief Set the description in usage message
     *
     * @param description
     */
    inline void setDescription(const char* description) {
        _description = description;
    }

    /**
     * @brief Set the epilog in usage message
     *
     * @param epilog
     */
    inline void setEpilog(const char* epilog) {
        _epilog = epilog;
    }

    /**
     * @brief Parse arguments
     *
     * @param argc
     * @param argv
     * @param alternative
     * @param strict
     */
    void parseArguments(int argc, char* argv[], bool alternative = false, bool strict = false);

    void addArgument(const std::vector<std::string>& nameOrFlags, const char* actionOrDefault = NULL,
                     const char* help = NULL, bool isRequired = false, const char* argsHelp = NULL,
                     std::size_t nbArgs = 0, const std::vector<std::string>& defaultArgs = std::vector<std::string>());

    /**
     * @brief add argument
     *
     * @param nameOrFlags separate by space or comma
     * @param actionOrDefault action list [store_true, store_false, append, extend, version]
     * @param help
     * @param isRequired
     * @param argsHelp
     * @param nbArgs
     * @param nbDefaultArgs
     * @param ...
     */
    void addArgument(const char* nameOrFlags, const char* actionOrDefault = NULL, const char* help = NULL,
                     bool isRequired = false, const char* argsHelp = NULL, std::size_t nbArgs = 0,
                     std::size_t nbDefaultArgs = 0, ...);

    std::string dump();

  private:

    /**
     * @brief Get the short argument decompose multi short argument
     *
     * @param maxIndex
     * @param argv
     * @param index
     * @param alternative
     */
    void parseShortArgument(int maxIndex, char* argv[], int* index, bool alternative);

    /**
     * @brief Get the long argument
     *
     * @param maxIndex
     * @param argv
     * @param index
     * @param alternative
     */
    void parseLongArgument(int maxIndex, char* argv[], int* index, bool alternative);

    /**
     * @brief Get the argument
     *
     * @param maxIndex
     * @param argv
     * @param index
     * @param hasArg
     * @param option
     * @param arg
     * @param argument
     * @param alternative
     */
    void parseArgument(int maxIndex, char* argv[], int* index, bool hasArg, const char* option, const char* arg,
                       Argument* argument, bool alternative);

    /**
     * @brief Get the positionnal argument
     *
     * @param argv
     * @param index
     * @param strict
     */
    void parsePositionnalArgument(char* argv[], int* index, bool strict);

    /**
     * @brief Check end of infinite parsing
     *
     * @param argument
     * @param alternative
     * @return true
     * @return false
     */
    bool endOfInfiniteArgument(const char* argument, bool alternative);

    std::string _binaryName;

    std::list<Argument> _arguments;
    std::map<std::string, Argument*> _argumentFromName;

    Argument* _helpOption;

    std::string _usage;
    std::string _description;
    std::string _epilog;

    std::vector<std::string> _additionalArguments;
};

} // namespace mblet

#endif // _MBLET_ARGPARSOR_HPP_