//
// Created by akulg on 10-05-2025.
//
#include <stdio.h>
#include <cregex/CRegex.h>
#include <cregex/AST.h>
#include <cregex/Parser.h>

struct Parser;

int main() {
    struct Node* root = parse("ab\bcd\\|+\\(nef(gh)|(\\\\t)+");
    print_ast(root, 0);
    free_ast(root);
}