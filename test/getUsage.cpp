#include <gtest/gtest.h>

#include "argparsor.h"

GTEST_TEST(getUsage, custom) {
    mblet::Argparsor argparsor;
    argparsor.setUsage("foo");
    std::ostringstream oss("");
    argparsor.getUsage(oss);
    EXPECT_EQ(oss.str(), "foo");
}

GTEST_TEST(getUsage, allTypeArgument) {
    mblet::Argparsor argparsor;
    argparsor.setDescription("custom description message");
    argparsor.setEpilog("custom epilog message");
    argparsor.setHelpArgument("-h", "--help", "custom help option message");
    argparsor.addPositionalArgument("REQUIRED", "help of required positional argument", true);
    argparsor.addPositionalArgument("SLOW", "help of required positional argument", true);
    argparsor.addPositionalArgument("ALOW", "help of required positional argument", true);
    argparsor.addBooleanArgument("-b", NULL, "help of boolean option", false);
    argparsor.addBooleanArgument(NULL, "--count2", "help of count option", false);
    argparsor.addBooleanArgument(NULL, "--count", "help of count option", false);
    argparsor.addSimpleArgument("", "--simple", "help of simple option", false, "ARG", NULL);
    argparsor.addNumberArgument("-n", "--number", "help of number", false, "ARG1 ARG2", 2, 2, "foo", "bar");
    argparsor.addInfiniteArgument("-i", "", "help of infinite", false);
    argparsor.addMultiArgument("-m", "--multi", "help of multi", false, "MULTI", 3, "0", "1", "2");
    argparsor.addMultiNumberArgument("-N", "--multiN", "help of multi", false, "MULTI", 3, 6, "0", "1", "2", "0", "1", "2");
    std::ostringstream oss("");
    argparsor.getUsage(oss);
    std::ostringstream usage("");
    usage << "usage:  [-N MULTI] [-b] [-h] [-i I...] [-m MULTI] [-n ARG1 ARG2] [--count] [--count2] [--simple ARG] REQUIRED SLOW ALOW\n";
    usage << "\n";
    usage << "custom description message\n";
    usage << "\n";
    usage << "positional arguments:\n";
    usage << "  REQUIRED                help of required positional argument (required)\n";
    usage << "  SLOW                    help of required positional argument (required)\n";
    usage << "  ALOW                    help of required positional argument (required)\n";
    usage << "\n";
    usage << "optional arguments:\n";
    usage << "  -N, --multiN MULTI      help of multi (default: (0, 1, 2), (0, 1, 2))\n";
    usage << "  -b                      help of boolean option\n";
    usage << "  -h, --help              custom help option message\n";
    usage << "  -i I...                 help of infinite\n";
    usage << "  -m, --multi MULTI       help of multi (default: 0, 1, 2)\n";
    usage << "  -n, --number ARG1 ARG2  help of number (default: foo, bar)\n";
    usage << "  --count                 help of count option\n";
    usage << "  --count2                help of count option\n";
    usage << "  --simple ARG            help of simple option\n";
    usage << "\n";
    usage << "custom epilog message\n";
    EXPECT_EQ(oss.str(), usage.str());
}