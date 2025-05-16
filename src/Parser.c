//
// Created by akulg on 10-05-2025.
//
#include <assert.h>
#include <stdio.h>
#include <cregex/Parser.h>
#include <cregex/AST.h>

#include <stdlib.h>
#include <string.h>

struct Parser {
    const char* regex;
    size_t len;

    int group_id;
    bool group_open;
    size_t curr;
};

static const char* METACHARS = "()[]{}|^$\\.+*?";
static struct Parser parser;

static void init_parser(const char* regex, size_t len);
static bool is_at_end();
static char peek();
static char peek_next();
static char advance();

static char escape(char c);

static struct Node* parse_regex();
static struct Node* parse_concat();
static struct Node* parse_term();
static struct Node* parse_quantifier(struct Node* node);
static struct Node* parse_string();
static struct Node* parse_group();

struct Node* parse(const char* regex) {
    return parse_n(regex, strlen(regex));
}

struct Node* parse_n(const char* regex, const size_t len) {
    init_parser(regex, len);
    return parse_regex();
}

static void init_parser(const char* regex, const size_t len) {
    parser.regex = regex;
    parser.len = len;
    parser.group_id = 0;
    parser.group_open = false;
    parser.curr = 0;
}

static bool is_at_end() {
    return parser.curr >= parser.len;
}

static char peek() {
    if (parser.curr < parser.len)
        return parser.regex[parser.curr];
    return '\0';
}

static char peek_next() {
    if (parser.curr + 1 < parser.len) {
        return parser.regex[parser.curr + 1];
    }
    return '\0';
}

static char advance() {
    char c = peek();
    parser.curr++;
    return c;
}

static char escape(char c) {
    switch (c) {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'b':
            return '\b';
        case 'a':
            return '\a';
        case 'r':
            return '\r';
        case 'f':
            return '\f';
        case 'v':
            return '\v';
        case '\0':
            fprintf(stderr, "Unexpected end of escape sequence\n");
            exit(EXIT_FAILURE);
        default:
            if (strchr(METACHARS, c) != NULL)
                return c;
            fprintf(stderr, "bad escape sequence '\\%c'\n", c);
            exit(EXIT_FAILURE);
    }
}

static struct Node* parse_regex() {
    struct Node* left = parse_concat();

    while (!is_at_end() && peek() == '|') {
        advance();
        struct Node* right = parse_concat();
        struct Node* alt = new_node(ALTERNATION);
        alt->compound.left = left;
        alt->compound.right = right;
        left = alt;
    }

    return left;
}

static struct Node* parse_concat() {
    struct Node* left = parse_term();

    while (!is_at_end() && peek() != ')' && peek() != '|') {
        struct Node* right = parse_term();
        struct Node* concat = new_node(CONCAT);
        concat->compound.left = left;
        concat->compound.right = right;
        left = concat;
    }

    if (peek() == ')' && !parser.group_open) {
        fprintf(stderr, "unmatched ')'\n");
        exit(EXIT_FAILURE);
    }

    return left;
}

static struct Node* parse_term() {
    struct Node* node;
    switch (peek()) {
        case '.': {
            advance();
            node = new_node(WILDCARD);
            break;
        }
        case '(':
            node = parse_group();
            break;
        case '\0':
            fprintf(stderr, "Unexpected end of string\n");
            exit(EXIT_FAILURE);
        case '+':
        case '*':
        case '?':
            fprintf(stderr, "use of quantifier without expression\n");
            exit(EXIT_FAILURE);

        case '\\': {
            advance();
            node = new_node(CHAR);
            node->ch = escape(peek());
            advance();
            break;
        }

        default:
            node = parse_string();
            break;
    }

    if (peek() == '+' || peek() == '*' || peek() == '?') {
        return parse_quantifier(node);
    }
    return node;
}

struct Node* parse_quantifier(struct Node* node) {
    const char quant = advance();
    const enum NodeType type = quant == '*'
        ? STAR
        : quant == '+' ? PLUS : QUEST;
    struct Node* qnode = new_node(type);
    qnode->unary.child = node;

    if (peek() == '?') {
        qnode->unary.greedy = false;
        advance();
    } else {
        qnode->unary.greedy = true;
    }
    return qnode;
}

struct Node* parse_string() {
    const size_t start_pos = parser.curr;
    while (!is_at_end() && strchr(METACHARS, peek()) == NULL) {
        if (parser.curr - start_pos > 0 && peek_next() != '\0' && strchr("+*?", peek_next()) != NULL)
            break;
        advance();
    }
    const size_t len = parser.curr - start_pos;
    assert(len != 0);

    struct Node* node = new_node(STRING);
    if (len == 1) {
        *node = (struct Node){ .type = CHAR, .ch = parser.regex[start_pos] };
        return node;
    }
    node->string.str = &parser.regex[start_pos];
    node->string.len = len;
    return node;
}
static struct Node* parse_group() {
    advance(); // consume '('
    parser.group_open = true;

    bool capturing = true;

    if (peek() == '?' && peek_next() == ':') {
        advance();
        advance();
        capturing = false;
    }

    struct Node* node = parse_regex();
    if (peek() != ')') {
        fprintf(stderr, "expected closing parenthesis\n");
        exit(EXIT_FAILURE);
    }
    advance(); // consume ')'
    struct Node* group = new_node(GROUP);
    group->group.child = node;
    group->group.group_id = capturing ? parser.group_id++ : -1;

    parser.group_open = false;
    return group;
}
