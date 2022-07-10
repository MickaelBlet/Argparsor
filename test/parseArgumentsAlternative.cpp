#include <gtest/gtest.h>

#include "argparsor.h"

GTEST_TEST(parseArgumentsAlternative, multi_short_invalid_option) {
    const char* argv[] = {
        "binaryname",
        "-ab"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "a");
            EXPECT_STREQ(e.what(), "invalid option");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, multi_short_not_one_argument_option) {
    const char* argv[] = {
        "binaryname",
        "-ab"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    argparsor.addSimpleArgument("-a", NULL);
    EXPECT_THROW({
        try {
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "a");
            EXPECT_STREQ(e.what(), "only last option can be use a parameter");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, short_invalid_option) {
    const char* argv[] = {
        "binaryname",
        "-a"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "a");
            EXPECT_STREQ(e.what(), "invalid option");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, short_number_option_with_arg) {
    const char* argv[] = {
        "binaryname",
        "-a=foo"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addNumberArgument("-a", NULL, NULL, true, NULL, 0);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "a");
            EXPECT_STREQ(e.what(), "option cannot use with only 1 argument");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, short_option_bad_narg) {
    const char* argv[] = {
        "binaryname",
        "-a",
        "--"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addSimpleArgument("-a", NULL);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "a");
            EXPECT_STREQ(e.what(), "bad number of argument");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, long_invalid_option) {
    const char* argv[] = {
        "binaryname",
        "--invalid"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "invalid");
            EXPECT_STREQ(e.what(), "invalid option");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, long_number_option_with_arg) {
    const char* argv[] = {
        "binaryname",
        "-foo=bar"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addNumberArgument(NULL, "--foo", NULL, true, NULL, 0);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "foo");
            EXPECT_STREQ(e.what(), "option cannot use with only 1 argument");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, long_option_bad_narg) {
    const char* argv[] = {
        "binaryname",
        "-foo",
        "--"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addSimpleArgument(NULL, "--foo");
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "foo");
            EXPECT_STREQ(e.what(), "bad number of argument");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, positionnal_invalid_additional_option) {
    const char* argv[] = {
        "binaryname",
        "arg"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.parseArguments(argc, const_cast<char**>(argv), true, true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "arg");
            EXPECT_STREQ(e.what(), "invalid additional argument");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, required_argument) {
    const char* argv[] = {
        "binaryname"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addPositionalArgument("foo", NULL, true, NULL);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "foo");
            EXPECT_STREQ(e.what(), "argument is required");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, required_short_option) {
    const char* argv[] = {
        "binaryname"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addNumberArgument("-f", NULL, NULL, true, NULL, 0);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "f");
            EXPECT_STREQ(e.what(), "option is required");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, required_long_option) {
    const char* argv[] = {
        "binaryname"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addNumberArgument(NULL, "--foo", NULL, true, NULL, 0);
            argparsor.parseArguments(argc, const_cast<char**>(argv), true);
        }
        catch (const mblet::Argparsor::ParseArgumentException& e) {
            EXPECT_STREQ(e.argument(), "foo");
            EXPECT_STREQ(e.what(), "option is required");
            throw;
        }
    }, mblet::Argparsor::ParseArgumentException);
}

GTEST_TEST(parseArgumentsAlternative, positional_argument) {
    const char* argv[] = {
        "binaryname",
        "--",
        "woot",
        "woot"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    argparsor.addPositionalArgument("arg");
    argparsor.parseArguments(argc, const_cast<char**>(argv), true);
    EXPECT_EQ(argparsor["arg"].str(), "woot");
}

GTEST_TEST(parseArgumentsAlternative, count_argument) {
    const char* argv[] = {
        "binaryname",
        "-vvv"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    argparsor.addBooleanArgument("-v");
    argparsor.parseArguments(argc, const_cast<char**>(argv), true);
    EXPECT_EQ(argparsor["-v"].count, 3);
}

GTEST_TEST(parseArgumentsAlternative, multi_argument) {
    const char* argv[] = {
        "binaryname",
        "-m=0",
        "-m", "0"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    argparsor.addMultiArgument("-m");
    argparsor.parseArguments(argc, const_cast<char**>(argv), true);
    EXPECT_EQ(argparsor["-m"].count, 2);
    EXPECT_EQ(argparsor["-m"].size(), 2);
    EXPECT_EQ(argparsor["-m"][0], "0");
    EXPECT_EQ(argparsor["-m"][1], "0");
}

GTEST_TEST(parseArgumentsAlternative, infinite_argument) {
    const char* argv[] = {
        "binaryname",
        "-i", "0", "1", "-1", "-10", "-s1", "-bbb",
        "-i", "0", "1", "--boolean",
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    argparsor.addBooleanArgument("-b", "--boolean");
    argparsor.addSimpleArgument("-s");
    argparsor.addInfiniteArgument("-i");
    argparsor.parseArguments(argc, const_cast<char**>(argv), true);
    EXPECT_EQ(argparsor["-i"].count, 2);
    EXPECT_EQ(argparsor["-i"].size(), 2);
    EXPECT_EQ(argparsor["-i"][0], "0");
    EXPECT_EQ(argparsor["-i"][1], "1");
}

GTEST_TEST(parseArgumentsAlternative, help) {
    const char* argv[] = {
        "binaryname",
        "-h"
    };
    const int argc = sizeof(argv) / sizeof(*argv);
    mblet::Argparsor argparsor;
    EXPECT_DEATH(argparsor.parseArguments(argc, const_cast<char**>(argv), true), "");
}