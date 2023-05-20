// scanner.c
#include "scanner.h"
#include <stdbool.h>

typedef struct {
    const char* start;  // ָ��Token����ʼλ��
    const char* current;
    int line;
} Scanner;

// ȫ�ֱ���
Scanner scanner;

void initScanner(const char* source) {
    // ��ʼ��scanner
    scanner.current = source;
    scanner.start = source;
}

/***************************************************************************************
 *                                   ��������											*
 ***************************************************************************************/
static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '_';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char advance() {
    return *scanner.current++;
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd()) return '\0';
    return *(scanner.current + 1);
}

static bool match(char expected) {
    if (isAtEnd()) return false;
    if (peek() != expected) return false;
    scanner.current++;
    return true;
}

// ����TokenType, ������Ӧ���͵�Token�������ء�
static Token makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.start = scanner.start;
    token.length = (int)(scanner.current - scanner.start);
    token.line = scanner.line;
    return token;
}

// �������ܽ��������ʱ�����Ǵ���һ��ERROR Token. ���磺����@��$�ȷ���ʱ�������ַ������ַ�û�ж�Ӧ��������ʱ��
static Token errorToken(const char* message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = scanner.line;
    return token;
}

static void skipWhitespace() {
    // �����հ��ַ�: ' ', '\r', '\t', '\n'��ע��
    // ע����'//'��ͷ, һֱ����β
    // ע�����scanner.line��
}
static TokenType identifierType() {
    // ȷ��identifier������Ҫ�����ַ�ʽ��
    // 1. �����еĹؼ��ַ����ϣ���У�Ȼ����ȷ��
    // 2. �����еĹؼ��ַ���Trie���У�Ȼ����ȷ��
    // Trie���ķ�ʽ�����ǿռ��ϻ���ʱ���϶����ڹ�ϣ��ķ�ʽ
}

static Token identifier() {
    // IDENTIFIER����: ��ĸ�����ֺ��»���  a
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    // ������Token�����Ǳ�ʶ��, Ҳ�����ǹؼ���, identifierType()������ȷ��Token���͵�
    return makeToken(identifierType());
}

static Token number() {
    // ����������ǽ�NUMBER�Ĺ���������:
    // 1. NUMBER���԰������ֺ����һ��'.'��
    // 2. '.'��ǰ��Ҫ������
    // 3. '.'�ź���ҲҪ������
    // ��Щ���ǺϷ���NUMBER: 123, 3.14
    // ��Щ���ǲ��Ϸ���NUMBER: 123., .14
}

static Token string() {
    // �ַ�����"��ͷ����"��β�����Ҳ��ܿ���
}

static Token character() {
    // �ַ�'��ͷ����'��β�����Ҳ��ܿ���
}


// ����˵����
// start: ���ĸ�����λ�ÿ�ʼ�Ƚ�
// length: Ҫ�Ƚ��ַ��ĳ���
// rest: Ҫ�Ƚϵ�����
// type: �����ȫƥ�䣬��˵����type���͵Ĺؼ���
static TokenType checkKeyword(int start, int length, const char* rest, TokenType type) {
    int len = (int)(scanner.current - scanner.start);
    if (start + length == len && memcmp(scanner.start + start, rest, length) == 0) {
        return type;
    }
    return TOKEN_IDENTIFIER;
}

// �жϵ�ǰToken�����Ǳ�ʶ�����ǹؼ���
static TokenType identifierType() {
    char c = scanner.start[0];
    // ��switch���ʵ��Trie��
    switch (c) {  // ���ݷ�֧����Ҫ���ϵ���chackKeyword()
    case 'b': return checkKeyword(1, 4, "reak", TOKEN_BREAK);
        ... // TODO
    }

    // identifier
    return TOKEN_IDENTIFIER;
}
/***************************************************************************************
 *                                   	�ִ�											  *
 ***************************************************************************************/
Token scanToken() {  // ״̬�����
    // ����ǰ�ÿհ��ַ���ע��
    skipWhitespace();
    // ��¼��һ��Token����ʼλ��
    scanner.start = scanner.current;

    if (isAtEnd()) return makeToken(TOKEN_EOF);

    char c = advance();
    if (isAlpha(c)) return identifier();  // ��ʶ�����߹ؼ��� ʶ��״̬�����ó���ӦTOKEN
    if (isDigit(c)) return number();      // ���� ʶ��״̬�����ó�����TOKEN

    switch (c) {
        // single-character tokens  һ��������ֱ�Ӵ�����ret��ӦTOKEN�Ϳ�����
    case '(': return makeToken(TOKEN_LEFT_PAREN);
    case ')': return makeToken(TOKEN_RIGHT_PAREN);
    case '[': return makeToken(TOKEN_LEFT_BRACKET);
    case ']': return makeToken(TOKEN_RIGHT_BRACKET);
    case '{': return makeToken(TOKEN_LEFT_BRACE);
    case '}': return makeToken(TOKEN_RIGHT_BRACE);
    case ',': return makeToken(TOKEN_COMMA);
    case '.': return makeToken(TOKEN_DOT);
    case ';': return makeToken(TOKEN_SEMICOLON);
    case '~': return makeToken(TOKEN_TILDE);

        // one or two characters tokens  �����󣬻�Ҫ����һ���ַ�������ret��ӦTOKEN
    case '+':
        if (match('+')) return makeToken(TOKEN_PLUS_PLUS);
        else if (match('=')) return makeToken(TOKEN_PLUS_EQUAL);
        else return makeToken(TOKEN_PLUS);
    case '-':
        if (match('-')) return makeToken(TOKEN_MINUS_MINUS);
        else if (match('=')) return makeToken(TOKEN_MINUS_EQUAL);
        else if (match('>')) return makeToken(TOKEN_MINUS_GREATER);
        else return makeToken(TOKEN_MINUS);
    case '*':
        if (match('=')) return makeToken(TOKEN_STAR_EQUAL);
        else return makeToken(TOKEN_STAR);
    case '/':
        if (match('=')) return makeToken(TOKEN_SLASH_EQUAL);
        else return makeToken(TOKEN_SLASH);
    case '%':
        if (match('=')) return makeToken(TOKEN_PERCENT_EQUAL);
        else return makeToken(TOKEN_PERCENT);
    case '&':
        if (match('=')) return makeToken(TOKEN_AMPER_EQUAL);
        else if (match('&')) return makeToken(TOKEN_AMPER_AMPER);
        else return makeToken(TOKEN_AMPER);
    case '|':
        if (match('=')) return makeToken(TOKEN_PIPE_EQUAL);
        else if (match('|')) return makeToken(TOKEN_PIPE_PIPE);
        else return makeToken(TOKEN_PIPE);
    case '^':
        if (match('=')) return makeToken(TOKEN_HAT_EQUAL);
        else return makeToken(TOKEN_HAT);
    case '=':
        if (match('=')) return makeToken(TOKEN_EQUAL_EQUAL);
        else return makeToken(TOKEN_EQUAL);
    case '!':
        if (match('=')) return makeToken(TOKEN_BANG_EQUAL);
        else return makeToken(TOKEN_BANG);
    case '<':
        if (match('=')) return makeToken(TOKEN_LESS_EQUAL);
        else if (match('<')) return makeToken(TOKEN_LESS_LESS);
        else return makeToken(TOKEN_LESS);
    case '>':
        if (match('=')) return makeToken(TOKEN_GREATER_EQUAL);
        else if (match('<')) return makeToken(TOKEN_GREATER_GREATER);
        else return makeToken(TOKEN_GREATER);

        // various-character tokens
    case '"': return string();  // �ַ��� ʶ��״̬�����ó��ַ���TOKEN
    case '\'': return character(); // �ַ� ʶ��״̬�����ó��ַ�TOKEN
    }

    return errorToken("Unexpected character."); // ����ֱ���������TOKEN
}
