#include "code.hpp"

static bool cmp(string a, string b) {
    if (a.size() != b.size()) return false;
    for (unsigned i = 0; i < a.size(); i++) {
        if (tolower(a[0]) != tolower(b[0])) return false;
    }
    return true;
}

pair<u16, CodeType> createCode(string str) {
    return {0, NOOP};
}

u16 createRegister(int line, string str) {
    // TODO Make this code more eficient

    if (cmp(str, "r0")) {
        return 0x0000;
    } else if (cmp(str, "r1")) {
        return 0x0001;
    } else if (cmp(str, "r2")) {
        return 0x0002;
    } else if (cmp(str, "r3")) {
        return 0x0003;
    } else if (cmp(str, "r4")) {
        return 0x0004;
    } else if (cmp(str, "r5")) {
        return 0x0005;
    } else if (cmp(str, "r6")) {
        return 0x0006;
    } else if (cmp(str, "r7")) {
        return 0x0007;
    } else if (cmp(str, "r8")) {
        return 0x0008;
    } else if (cmp(str, "r9")) {
        return 0x0009;
    } else if (cmp(str, "r10") || cmp(str, "ra")) {
        return 0x000A;
    } else if (cmp(str, "r11") || cmp(str, "rb")) {
        return 0x000B;
    } else if (cmp(str, "r12") || cmp(str, "rc")) {
        return 0x000C;
    } else if (cmp(str, "r13") || cmp(str, "re")) {
        return 0x000D;
    } else if (cmp(str, "aux")) {
        return 0x000E;
    } else if (cmp(str, "sp")) {
        return 0x000F;
    } else {
        lerror(line) << "Invalid register `" << str << "` (using aux)" << endl;
        return 0x000E;
    }
}