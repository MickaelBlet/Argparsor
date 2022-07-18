/**
 * strto.h
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

#ifndef _MBLET_STRTO_HPP_
#define _MBLET_STRTO_HPP_

#include <string>
#include <map>
#include <cstdlib>

namespace mblet {

/**
 * @brief exception of strto function
 */
class StrToException : public std::exception {
  public:
    /**
     * @brief Construct a new Str To Exception object
     *
     * @param value
     * @param type
     */
    StrToException(const std::string& value, const char* type) : std::exception(),
        _value(value), _type(type) {
        _what = "can't convert '" + value + "' with type '" + type + "'";
    }
    /**
     * @brief Destroy the Str To Exception object
     */
    virtual ~StrToException() throw() {}
    /**
     * @brief get message of exception
     *
     * @return const char*
     */
    const char* what() const throw() {
        return _what.c_str();
    }
    /**
     * @brief get value
     *
     * @return const char*
     */
    const char* value() const throw() {
        return _value.c_str();
    }
    /**
     * @brief get type
     *
     * @return const char*
     */
    const char* type() const throw() {
        return _type.c_str();
    }
  protected:
    std::string _what;
    const std::string _value;
    const std::string _type;
};

/**
 * @brief transform string to number strict
 *
 * @tparam T
 * @param str
 * @return T
 */
template<typename T>
inline T strto(const std::string& str) {
    T ret;
    std::stringstream stringStream("");
    std::size_t index = 0;

    if (str[index] == '-' || str[index] == '+') {
        ++index;
    }
    // is hex
    if (str[index] == '0' && (str[index + 1] == 'x' || str[index + 1] == 'X')) {
        ++index;
        ++index;
        while (str[index] != '\0') {
            if (str[index] >= '0' && str[index] <= '9') {
                ++index;
            }
            else if (str[index] >= 'a' && str[index] <= 'f') {
                ++index;
            }
            else if (str[index] >= 'A' && str[index] <= 'F') {
                ++index;
            }
            else {
                throw StrToException(str, "hex number");
            }
        }
        stringStream << strtoll(str.c_str(), NULL, 16);
    }
    // is binary
    else if (str[index] == '0' && (str[index + 1] == 'b' || str[index + 1] == 'B')) {
        ++index;
        ++index;
        std::size_t start = index;
        while (str[index] != '\0') {
            if (str[index] == '0' || str[index] == '1') {
                ++index;
            }
            else {
                throw StrToException(str, "binnary number");
            }
        }
        stringStream << strtoull(str.c_str() + start, NULL, 2);
    }
    // is octal
    else if (str[index] == '0' && str.find('.') == std::string::npos && str.find('e') == std::string::npos
             && str.find('E') == std::string::npos) {
        while (str[index] != '\0') {
            if (str[index] >= '0' && str[index] <= '9') {
                ++index;
            }
            else {
                throw StrToException(str, "octal number");
            }
        }
        stringStream << strtoll(str.c_str(), NULL, 8);
    }
    else {
        char* endPtr = NULL;
        stringStream << strtod(str.c_str(), &endPtr);
        if (endPtr != NULL && endPtr[0] != '\0') {
            throw StrToException(str, "number");
        }
    }
    if (stringStream >> ret) {
        return ret;
    }
    else {
        throw StrToException(str, "number");
    }
}

/**
 * @brief transform string to bool strict
 *
 * @tparam bool
 * @param str
 * @return true
 * @return false
 */
template<>
inline bool strto(const std::string& str) {
    static const std::pair<std::string, bool> pairStrToBool[] = {
        std::pair<std::string, bool>("TRUE", true),
        std::pair<std::string, bool>("True", true),
        std::pair<std::string, bool>("true", true),
        std::pair<std::string, bool>("ON", true),
        std::pair<std::string, bool>("On", true),
        std::pair<std::string, bool>("on", true),
        std::pair<std::string, bool>("YES", true),
        std::pair<std::string, bool>("Yes", true),
        std::pair<std::string, bool>("yes", true),
        std::pair<std::string, bool>("Y", true),
        std::pair<std::string, bool>("y", true),
        std::pair<std::string, bool>("1", true),
        std::pair<std::string, bool>("FALSE", false),
        std::pair<std::string, bool>("False", false),
        std::pair<std::string, bool>("false", false),
        std::pair<std::string, bool>("OFF", false),
        std::pair<std::string, bool>("Off", false),
        std::pair<std::string, bool>("off", false),
        std::pair<std::string, bool>("NO", false),
        std::pair<std::string, bool>("No", false),
        std::pair<std::string, bool>("no", false),
        std::pair<std::string, bool>("N", false),
        std::pair<std::string, bool>("n", false),
        std::pair<std::string, bool>("0", false)
    };
    static const std::map<std::string, bool> strToBool(pairStrToBool,
                                                       pairStrToBool + sizeof(pairStrToBool) / sizeof(*pairStrToBool));

    std::map<std::string, bool>::const_iterator cit = strToBool.find(str);
    if (cit != strToBool.end()) {
        return cit->second;
    }
    else {
        throw StrToException(str, "bool");
    }
}

} // namespace mblet

#endif // _MBLET_STRTO_HPP_