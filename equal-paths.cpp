#include "equal-paths.h"
using namespace std;

// -1 is used to denote mismatching paths
int path_len(Node* root) {
    if (root == nullptr) {
        return 0;
    }
    int left_len = path_len(root->left);
    int right_len = path_len(root->right);
    if (left_len == -1 || right_len == -1) {
        return -1;
    }
    if (left_len == 0) {
        return right_len + 1;
    }
    if (right_len == 0) {
        return left_len + 1;
    }
    if (left_len != right_len) {
        return -1;
    }
    return left_len + 1;
}

bool equalPaths(Node* root) { return path_len(root) != -1; }
