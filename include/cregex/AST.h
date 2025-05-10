//
// Created by akulg on 10-05-2025.
//

#ifndef AST_H
#define AST_H
#include <stddef.h>
#include <stdbool.h>

enum NodeType {
    WILDCARD,
    STRING,
    CONCAT,
    ALTERNATION,
    STAR,
    PLUS,
    QUEST,
    GROUP
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
        } compound;

        struct {
            struct Node* child;
            bool greedy;
        } unary;

        struct {
            struct Node* child;
            int group_id;
        } group;
    };
};

#ifndef NDEBUG
void print_ast(const struct Node* node, int indent);
#endif

#endif //AST_H
