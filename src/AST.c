//
// Created by akulg on 10-05-2025.
//
#include <stdbool.h>
#include <stdio.h>
#include <cregex/AST.h>
#include <stdlib.h>

struct Node* new_node(enum NodeType type) {
    struct Node* node = malloc(sizeof(struct Node));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for allocation of AST Node\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    return node;
}

void free_ast(const struct Node* node) {
    switch (node->type) {
        case CONCAT:
        case ALTERNATION:
            free(node->compound.left);
            free(node->compound.right);
            break;
        case STRING:
            // free(node->string.str);
            break;
        case STAR:
        case PLUS:
        case QUEST:
            free(node->unary.child);
            break;
        case GROUP:
            free(node->group.child);
            break;
        case WILDCARD:
        case CHAR:
            break;
    }
}

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

static void print_char(char c) {
    switch (c) {
        case '\n':
            printf(BOLD YELLOW "\\n" RESET MAGENTA);
            break;
        case '\t':
            printf(BOLD YELLOW "\\t" RESET MAGENTA);
            break;
        case '\r':
            printf(BOLD YELLOW "\\r" RESET MAGENTA);
            break;
        case '\f':
            printf(BOLD YELLOW "\\f" RESET MAGENTA);
            break;
        case '\b':
            printf(BOLD YELLOW "\\b" RESET MAGENTA);
            break;
        case '\a':
            printf(BOLD YELLOW "\\a" RESET MAGENTA);
            break;
        case '\v':
            printf(BOLD YELLOW "\\v" RESET MAGENTA);
            break;
        case '\\':
            printf(BOLD YELLOW "\\\\" RESET MAGENTA);
            break;
        case '\'':
            printf(BOLD YELLOW "\\\'" RESET MAGENTA);
            break;
        case '\"':
            printf(BOLD YELLOW "\\\"" RESET MAGENTA);
            break;
        default:
            printf("%c", c);
            break;
    }
}

void print_ast(const struct Node* node, const int indent) {
    bool concat = false;
    switch (node->type) {
        case WILDCARD:
            printf(BOLD CYAN "WILDCARD" RESET);
            break;
        case CHAR:
            printf(BOLD CYAN "CHAR" RESET "(" MAGENTA "'");
            print_char(node->ch);
            printf("'" RESET ")");
            break;
        case STRING:
            printf(BOLD CYAN "STRING" RESET "(" MAGENTA "\"");
            for (size_t i = 0; i < node->string.len; ++i) {
                print_char(node->string.str[i]);
            }
            printf("\"" RESET ")");
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
            printf(")");
            break;
        case GROUP:
            printf(BOLD CYAN "GROUP" RESET "(" YELLOW "<" GREEN "%d" YELLOW ">" RESET ", ", node->group.group_id);
            print_ast(node->group.child, indent + 1);
            printf(")");
            break;
    }
}

#endif