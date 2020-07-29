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
    Processor* processor = createProcessor(path);

    while (hasNext(processor)) {
        next(processor);
    }

    cout << "Number of instruction read: " << numInstructions(processor) << endl
         << "Number of instructions that isn't noop: " << numExecuted(processor) << endl
         << "Number of jumps read: " << numJumps(processor) << endl
         << "Number of jumps executed: " << numJumpsExecuted(processor) << endl;

    return EXIT_SUCCESS;
}