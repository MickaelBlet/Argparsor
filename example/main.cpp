#include "argparsor.h"

int main(int argc, char* argv[]) {
    mblet::Argparsor argparsor;
    argparsor.setDescription("custom description message");
    argparsor.setEpilog("custom epilog message");
    argparsor.addArgument("-h|--help", "help", "custom help option message");
    argparsor.addArgument("REQUIRED", NULL, "help of required positional argument", true);
    argparsor.addArgument("-b", "store_true");
    argparsor.addArgument("-c", "store_false", "help of count option", false);
    argparsor.addArgument("-s, --simple", NULL, "help of simple option", true, "argSimple", 1);
    argparsor.addArgument("-o", "", "help of number", false, "NONE", 0);
    argparsor.addArgument("-n --number", NULL, "help of number", false, "ARG1 ARG2", 2, 2, "foo", "bar");
    argparsor.addArgument("--infinite", NULL, "help of infinite", false, NULL, '+');
    argparsor.addArgument("--infinites", "extend", "help of infinites", false, NULL, '+');
    argparsor.addArgument("-m --multi", "append", "help of multi", false, "MULTI", 1, 3, "0", "1", "2");
    argparsor.addArgument("-N", "append", "help of multi number", false, "MULTI NUMBER", 2, 6, "0", "1", "2", "2", "1", "0");
    argparsor.addArgument("YYYYOOOOLLLLOOOO", "woot", "toto");
    argparsor.addArgument("-l, --op, --ls", "", "toto", false, "", 2, 0);
    argparsor.addArgument("-f", "append", "toto", false, "", 2, 4, "0", "1", "2", "3");
    try {
        argparsor.parseArguments(argc, argv, true);
        std::cout << argparsor.dump() << std::endl;
        std::cout << "-b: " << argparsor["-b"] << std::endl;
        std::cout << "-c: " << argparsor["-c"].count() << std::endl;
        std::string required = argparsor["REQUIRED"];
        std::cout << "REQUIRED: " << argparsor["REQUIRED"] << " (" << required << ")" << std::endl;
        if (argparsor["-s"]) {
            std::cout << "-s: " << argparsor["-s"] << std::endl;
        }
        std::cout << "-n: [0]: " << argparsor["-n"][0] << ", [1]: " << argparsor["-n"][1] << " (" << argparsor["-n"] << ")" <<
                  std::endl;
        if (argparsor["--infinite"]) {
            std::vector<std::string> rrr = argparsor["--infinite"];
            for (std::size_t i = 0; i < rrr.size(); ++i) {
                std::cout << rrr[i] << std::endl;
            }
            std::cout << "--infinite: " << argparsor["--infinite"] << std::endl;
        }
        std::cout << "-m: " << argparsor["-m"] << std::endl;
        const mblet::Argparsor::Argument& sss = argparsor["-N"];
        std::cout << argparsor["-N"] << std::endl;
        for (std::size_t i = 0; i < sss.size(); ++i) {
            std::cout << sss[i] << std::endl;
        }
        std::cout << "multiNumberSize: " << argparsor["-N"].size() <<  std::endl;
        for (std::size_t i = 0; i < argparsor["-N"].size(); ++i) {
            std::cout << argparsor["-N"][i][0] << std::endl;
        }
        std::cout << argparsor["-o"].isExist() << std::endl;
    }
    catch (const mblet::Argparsor::ParseArgumentException& e) {
        std::cerr << argparsor.getBynaryName() << ": " << e.what() << " -- '" << e.argument() << "'" << std::endl;
        return 1;
    }
    return 0;
}