//
// Created by akulg on 10-05-2025.
//
#include <stdbool.h>
#include <stdio.h>
#include <cregex/AST.h>

#ifndef NDEBUG

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

const char* INDENT[] = {
    "",
    "    ",
    "        ",
    "            ",
    "                ",
    "                    ",
    "                        ",
    "                            ",
};

void print_ast(const struct Node* node, const int indent) {
    bool concat = false;
    switch (node->type) {
        case WILDCARD:
            printf(BOLD CYAN "WILDCARD" RESET);
            break;
        case STRING:
            printf(BOLD CYAN "STRING" RESET "(" MAGENTA "\"%.*s\"" RESET ")", (int)node->string.len, node->string.str);
            break;
        case CONCAT:
            printf(BOLD CYAN "CONCAT" RESET "(\n%s", INDENT[indent + 1]);
            concat = true;
        case ALTERNATION:
            if (!concat)
                printf(BOLD CYAN "ALTERNATION" RESET "(\n%s", INDENT[indent + 1]);
            print_ast(node->compound.left, indent + 1);
            printf(",\n%s", INDENT[indent + 1]);
            print_ast(node->compound.right, indent + 1);
            printf("\n%s)", INDENT[indent]);
            break;
        case STAR:
        case PLUS:
        case QUEST:
            printf(BOLD CYAN "QUANTIFIER" RESET "(" YELLOW "<" GREEN "%s%s" YELLOW ">" RESET ", ",
                   node->type == STAR ? "*" : node->type == PLUS ? "+" : "?",
                   node->unary.greedy ? "" : "?");
            print_ast(node->unary.child, indent + 1);
            printf("\n%s)", INDENT[indent]);
            break;
        case GROUP:
            printf(BOLD CYAN "GROUP" RESET "(" YELLOW "<" GREEN "%d" YELLOW ">" RESET ", ", node->group.group_id);
            print_ast(node->group.child, indent + 1);
            printf("\n%s)", INDENT[indent]);
            break;
    }
}

#endif