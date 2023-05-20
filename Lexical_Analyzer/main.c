#define _CRT_SECURE_NO_WARNINGS 1

// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"

static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // ��ӡToken, ����TOKEN_EOFΪֹ
    for (;;) {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        }
        else {
            printf("   | ");
        }
        printf("%2d '%.*s'\n", token.type, token.length, token.start);


        if (token.type == TOKEN_EOF) break;
    }
}

static void repl() {
    // ���û��������û�ÿ����һ�д��룬����һ�д��룬����������
    // repl��"read evaluate print loop"����д

    while (1) {
        char str[1000];
        if (fgets(str, 1000, stdin) == NULL) {
            fprintf(stderr, "Error: fgets() return NULL.\n");
        }
        run(str); // ÿ����һ�Σ�����һ��run����
    }
}

static char* readFile(const char* path) {
    // �û������ļ������������ļ������ݶ����ڴ棬����ĩβ���'\0'
    // ע��: ����Ӧ��ʹ�ö�̬�ڴ���䣬���Ӧ������ȷ���ļ��Ĵ�С��

    // �����ļ��������һ����̬�ַ�������

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "Open %s failed.\n", path);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    char* contents = (char*)malloc(file_size + 1);
    if (!contents) {
        fprintf(stderr, "malloc failed in readFile.\n");
        exit(1);
    }

    int n = fread(contents, 1, file_size, fp);
    contents[n] = '\0';

    fclose(fp);

    return contents;
}

static void runFile(const char* path) {
    // ����'.c'�ļ�:�û������ļ��������������ļ�������������

    // ����readFile��ȡ��̬�ַ����飬Ȼ�����һ��run������Ȼ��free��

    char* file_contents = readFile(path);
    run(file_contents);
    free(file_contents);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        // ./scanner û�в���,����뽻��ʽ����
        repl();
    }
    else if (argc == 2) {
        // ./scanner file �����һ������,����������ļ�
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: scanner [path]\n");
        exit(1);
    }

    return 0;
}