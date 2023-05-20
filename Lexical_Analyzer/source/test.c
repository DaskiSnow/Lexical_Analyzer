// hello.c
static void run(const char* source) {
    initScanner(source);
    int line = -1;
    // 打印Token, 遇到TOKEN_EOF为止
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
    // 与用户交互，用户每输入一行代码，分析一行代码，并将结果输出
    // repl是"read evaluate print loop"的缩写

    while (1) {
        char str[1000];
        if (fgets(str, 1000, stdin) == NULL) {
            fprintf(stderr, "Error: fgets() return NULL.\n");
        }
        run(str); // 每输入一次，调用一次run函数
    }
}

static char* readFile(const char* path) {
    // 用户输入文件名，将整个文件的内容读入内存，并在末尾添加'\0'
    // 注意: 这里应该使用动态内存分配，因此应该事先确定文件的大小。

    // 输入文件名，输出一个动态字符串数组

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
    // 处理'.c'文件:用户输入文件名，分析整个文件，并将结果输出

    // 调用readFile获取动态字符数组，然后调用一次run函数，然后free。

    char* file_contents = readFile(path);
    run(file_contents);
    free(file_contents);
}

int main(int argc, const char* argv[]) {
    if (argc == 1) {
        // ./scanner 没有参数,则进入交互式界面
        repl();
    }
    else if (argc == 2) {
        // ./scanner file 后面跟一个参数,则分析整个文件
        runFile(argv[1]);
    }
    else {
        fprintf(stderr, "Usage: scanner [path]\n");
        exit(1);
    }

    return 0;
}