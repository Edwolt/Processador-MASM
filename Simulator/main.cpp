#include <iostream>
#include <string>

#include "processor.hpp"
#include "util.hpp"

using namespace std;

int main(int argc, char const* argv[]) {
    if (argc < 2) {
        cout << "Missing source" << endl;
        return EXIT_FAILURE;
    }

    string path(argv[1]);
    Processor processor(path);

    while (processor.hasNext()) {
        processor.next();
    }

    cout << "Number of instruction read: " << processor.numInstructions << endl
         << "Number of instructions that isn't noop: " << processor.numExecuted << endl
         << "Number of jumps read: " << processor.numJumps << endl
         << "Number of jumps executed: " << processor.numJumpsExecuted << endl;

    return EXIT_SUCCESS;
}