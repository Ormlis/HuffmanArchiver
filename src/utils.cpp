#include "utils.h"

bool IncreasePath(std::vector<bool> &path) {
    for (auto it = path.rbegin(); it != path.rend(); ++it) {
        if (!(*it)) {
            (*it) = true;
            return true;
        }
        (*it) = false;
    }
    return false;  // Path overflow
}