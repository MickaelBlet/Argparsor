#include <gtest/gtest.h>

#include "argparsor.h"

GTEST_TEST(addMultiNumberArgument, invalid_shortName_and_longName) {
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName and longName arguments");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("", NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName and longName arguments");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, "");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName and longName arguments");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("", "");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName and longName arguments");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
}

GTEST_TEST(addMultiNumberArgument, invalid_shortName) {
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("not -", NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName argument not start by '-' character");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("-", NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName argument is '-'");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("-ab", NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName argument not only one character");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    argparsor.addMultiNumberArgument("-a", NULL);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("-a", NULL);
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid shortName argument already exist");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
}

GTEST_TEST(addMultiNumberArgument, invalid_longName) {
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, "not --");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid longName argument not start by '--' characters");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, "--");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid longName argument is '--'");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    argparsor.addMultiNumberArgument(NULL, "--abc");
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, "--abc");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid longName argument already exist");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
}

GTEST_TEST(addMultiNumberArgument, invalid_number_args) {
    mblet::Argparsor argparsor;
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument(NULL, "--abc", NULL, false, NULL, 2, 1, "");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid number of argument with number of default argument");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
    EXPECT_THROW({
        try {
            argparsor.addMultiNumberArgument("-a", NULL, NULL, false, NULL, 2, 1, "");
        }
        catch (const mblet::Argparsor::ArgumentException& e) {
            EXPECT_STREQ(e.what(), "invalid number of argument with number of default argument");
            throw;
        }
    }, mblet::Argparsor::ArgumentException);
}

GTEST_TEST(addMultiNumberArgument, valid_type) {
    mblet::Argparsor argparsor;
    argparsor.addMultiNumberArgument("-a", "--abc");
    EXPECT_EQ(argparsor["-a"].getType(), mblet::Argparsor::Argument::MULTI_NUMBER_OPTION);
    EXPECT_EQ(argparsor["--abc"].getType(), mblet::Argparsor::Argument::MULTI_NUMBER_OPTION);
}

GTEST_TEST(addMultiNumberArgument, valid_size) {
    mblet::Argparsor argparsor;
    argparsor.addMultiNumberArgument("-a", "--abc");
    EXPECT_EQ(argparsor["-a"].size(), 0);
    EXPECT_EQ(argparsor["--abc"].size(), 0);
    // with default value
    argparsor.addMultiNumberArgument("-b", "--bcd", "help", false, "NUMBER", 2, 6, "foo", "bar", "too", "tar", "coo",
                                     "car");
    EXPECT_EQ(argparsor["-b"].size(), 3);
    EXPECT_EQ(argparsor["--bcd"].size(), 3);
}