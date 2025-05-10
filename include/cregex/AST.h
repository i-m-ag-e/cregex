//
// Created by akulg on 10-05-2025.
//

#ifndef AST_H
#define AST_H
#include <stddef.h>

enum NodeType {
    Wildcard,
    String,
    Concat,
    Alteration,
    Star,
    Plus,
    Quest,
    Group
};

struct Node {
    enum NodeType type;
    union {
        struct {
            char* str;
            size_t len;
        } string;
        struct {
            struct Node* left;
            struct Node* right;
        } concat;
    };
};

#endif //AST_H
