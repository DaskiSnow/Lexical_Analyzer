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
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
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

// scanner.currentǰ��n����λ
static char span(int n) {
    for (int i = 0; i < n; i++) {
        advance();
    }
    return peek();
}

// scanner.current����n����λ
static char revert(int n) {
    for (int i = 0; i < n; i++) {
        scanner.current--;
    }
    return peek();
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

    while (1) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        }
        else if (c == '\n') {
            advance();
            scanner.line++;
        }
        else if (c == '/') {
            advance();
            if (match('/')) {
                while (peek() != '\n') {
                    advance();
                }
            }
            advance();
        }
        else {
            break;
        }
    }

}
//static TokenType identifierType() {
//    // ȷ��identifier������Ҫ�����ַ�ʽ��
//    // 1. �����еĹؼ��ַ����ϣ���У�Ȼ����ȷ��
//    // 2. �����еĹؼ��ַ���Trie���У�Ȼ����ȷ��
//    // Trie���ķ�ʽ�����ǿռ��ϻ���ʱ���϶����ڹ�ϣ��ķ�ʽ
//}

static Token number() {
    // ����������ǽ�NUMBER�Ĺ���������:
    // 1. NUMBER���԰������ֺ����һ��'.'��
    // 2. '.'��ǰ��Ҫ������
    // 3. '.'�ź���ҲҪ������
    // ��Щ���ǺϷ���NUMBER: 123, 3.14
    // ��Щ���ǲ��Ϸ���NUMBER: 123., .14
    char c;
    bool next_to_dot = false;
    while (1) {
        c = peek();
        if (isDigit(c)) {
            advance();
            if (next_to_dot) {
                next_to_dot = false;
            }
        }
        else if(c=='.' && !next_to_dot) {
            advance();
            next_to_dot = true;
        }
        else if(!isDigit(c) && next_to_dot){
            return errorToken("Illegal number.");
        }
        else {
            return makeToken(TOKEN_NUMBER);
        }
    }
}

static Token string() {
    // �ַ�����"��ͷ����"��β�����Ҳ��ܿ���
    char c;
    while (1) {
        c = peek();
        if (c == '"') {
            advance();
            return makeToken(TOKEN_STRING);
        }
        else if (c=='\n') {
            scanner.line++;
            advance();
            return errorToken("Illegal string.");
        }
        else {
            advance();
        }
    }
}

static Token character() {
    // �ַ�'��ͷ����'��β�����Ҳ��ܿ���
    char c;
    bool has_char = false;
    while (1) {
        c = peek();
        if (c == '\'') {
            if (!has_char) {
                advance();
                return errorToken("Empty character.");
            }
            advance();
            return makeToken(TOKEN_CHARACTER);
        }
        else if (!has_char) {
            has_char = true;
            if (c == '\\' ) {
                advance();
                if (peek() == '\'') {
                    errorToken("Not a character.");
                }
            }
            advance();
        }
        else {
            return errorToken("Illegal character.");
        }
    }
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
    case 'b':
        return checkKeyword(1, 4, "reak", TOKEN_BREAK);
    case 'c':
        c = scanner.start[1];
        if (c == 'a') {
            return checkKeyword(2, 2, "se", TOKEN_CASE);
        }
        else if (c == 'h') {
            return checkKeyword(2, 2, "ar", TOKEN_CHAR);
        }
        else if (c == 'o') {
            c = scanner.start[2];
            if (c == 'n') {
                c = scanner.start[3];
                if (c == 't') {
                    return checkKeyword(4, 4, "inue", TOKEN_CONTINUE);
                }
                else if (c == 's') {
                    return checkKeyword(4, 1, "t", TOKEN_CONST);
                }
                else {
                    return TOKEN_IDENTIFIER;
                }
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'd':
        c = scanner.start[1];
        if (c == 'e') {
            return checkKeyword(2, 5, "fault", TOKEN_DEFAULT);
        }
        else if (c == 'o') {
            return checkKeyword(2, 4, "uble", TOKEN_DOUBLE);
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'e':
        c = scanner.start[1];
        if (c == 'n') {
            return checkKeyword(2, 2, "um", TOKEN_ENUM);
        }
        else if (c == 'l') {
            return checkKeyword(2, 2, "se", TOKEN_ELSE);
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'f':
        c = scanner.start[1];
        if (c == 'l') {
            return checkKeyword(2, 3, "oat", TOKEN_FLOAT);
        }
        else if (c == 'o') {
            return checkKeyword(2, 1, "r", TOKEN_FOR);
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'g': return checkKeyword(1, 3, "oto", TOKEN_GOTO);
    case 'i':
        c = scanner.start[1];
        if (c == 'f') {
            return TOKEN_IF;
        }
        else if (c == 'n') {
            return checkKeyword(2, 1, "t", TOKEN_INT);
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'l':
        return checkKeyword(1, 3, "ong", TOKEN_LONG);
    case 'r':
        return checkKeyword(1, 5, "eturn", TOKEN_RETURN);
    case 's':
        c = scanner.start[1];
        if (c == 'h') {
            return checkKeyword(2, 3, "ort", TOKEN_SHORT);
        }
        else if (c == 'i') {
            c = scanner.start[2];
            if (c == 'g') {
                return checkKeyword(3, 3, "ned", TOKEN_SIGNED);
            }
            else if (c == 'z') {
                return checkKeyword(3, 3, "eof", TOKEN_SIZEOF);
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else if (c == 't') {
            return checkKeyword(2, 4, "ruct", TOKEN_STRUCT);
        }
        else if (c == 'w') {
            return checkKeyword(2, 4, "itch", TOKEN_SWITCH);
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 't': return checkKeyword(1, 6, "ypedef", TOKEN_TYPEDEF);
    case 'u':
        c = scanner.start[1];
        if (c == 'n') {
            c = scanner.start[2];
            if (c == 's') {
                return checkKeyword(3, 5, "igned", TOKEN_UNSIGNED);
            }
            else if (c=='i') {
                return checkKeyword(3, 2, "on", TOKEN_UNION);
            }
            else {
                return TOKEN_IDENTIFIER;
            }
        }
        else {
            return TOKEN_IDENTIFIER;
        }
    case 'v':
        return checkKeyword(1, 3, "oid", TOKEN_VOID);
    case 'w':
        return checkKeyword(1, 4, "hile", TOKEN_WHILE);
    }
    // identifier
    return TOKEN_IDENTIFIER;
}

static Token identifier() {
    // IDENTIFIER����: ��ĸ�����ֺ��»���  a
    while (isAlpha(peek()) || isDigit(peek())) {
        advance();
    }
    // ������Token�����Ǳ�ʶ��, Ҳ�����ǹؼ���, identifierType()������ȷ��Token���͵�
    return makeToken(identifierType());
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
        else if (match('>')) return makeToken(TOKEN_GREATER_GREATER);
        else return makeToken(TOKEN_GREATER);

        // various-character tokens
    case '"': return string();  // �ַ��� ʶ��״̬�����ó��ַ���TOKEN
    case '\'': return character(); // �ַ� ʶ��״̬�����ó��ַ�TOKEN
    }

    return errorToken("Unexpected character."); // ����ֱ���������TOKEN
}
