//
// Created by akulg on 10-05-2025.
//
#include <stdio.h>
#include <cregex/CRegex.h>
#include <cregex/AST.h>

int main() {
    struct Node left = {
        .type = STRING,
        .string.str = "abcd",
        .string.len = 4
    };
    struct Node right = {
        .type = STRING,
        .string.str = "efgh",
        .string.len = 4
    };
    struct Node rleft = {
        .type = STRING,
        .string.str = "ijkl",
        .string.len = 4,
    };
    struct Node alt = {
        .type = ALTERNATION,
        .compound.left = &rleft,
        .compound.right = &right
    };
    struct Node group = {
        .type = STAR,
        .unary.child = &alt,
        .unary.greedy = true
    };
    struct Node root = {
        .type = CONCAT,
        .compound.left = &left,
        .compound.right = &group
    };

    print_ast(&root, 0);
}