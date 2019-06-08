#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

// values represents type of tokens
enum {
	TK_NUM = 256,	// integer token
	TK_EOF,			// token that represents end of input
};

// type of tokens
typedef struct {
	int ty;			// type of token
	int val;		// if ty == TK_NUM, the actual value
	char *input;	// Token string (for error message)
} Token;

// input program
char *user_input;

// store token list to this array
// can not accept over 100 tokens
Token tokens[100];

// report errors
// same arguments as printf
void error(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	exit(1);
}

// report the location of error occures
void error_at(char *loc, char *msg) {
	int pos = loc - user_input;
	fprintf(stderr, "%s\n", user_input);
	fprintf(stderr, "%*s", pos, "");	// puts white spaces
	fprintf(stderr, "^ %s\n", msg);
	exit(1);
}

// split user input string to tokens
// and store them
void tokenize() {
	char *p = user_input;

	int i = 0;
	while (*p) {
		// skip white spaces
		if (isspace(*p)) {
			p++;
			continue;
		}

		if (*p == '+' || *p == '-') {
			tokens[i].ty = *p;
			tokens[i].input = p;
			i++;
			p++;
			continue;
		}

		if (isdigit(*p)) {
			tokens[i].ty = TK_NUM;
			tokens[i].input = p;
			tokens[i].val = strtol(p, &p, 10);
			i++;
			continue;
		}

		error_at(p, "can not tokenize");
	}

	tokens[i].ty = TK_EOF;
	tokens[i].input = p;
}


int main(int argc, char **argv) {
	if (argc != 2){
		fprintf(stderr, "invalid number of arguments.\n");
		return 1;
	}

	// tokenize
	user_input = argv[1];
	tokenize();

	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	if (tokens[0].ty != TK_NUM)
		error_at(tokens[0].input, "not a number");
	printf("  mov rax, %d\n", tokens[0].val);

	int i = 1;
	while (tokens[i].ty != TK_EOF) {
		if (tokens[i].ty == '+') {
			i++;
			if (tokens[i].ty != TK_NUM)
				error_at(tokens[i].input, "not a number");
			printf("  add rax, %d\n", tokens[i].val);
			i++;
			continue;
		}

		if (tokens[i].ty == '-') {
			i++;
			if (tokens[i].ty != TK_NUM)
				error_at(tokens[i].input, "not a number");
			printf("  sub rax, %d\n", tokens[i].val);
			i++;
			continue;
		}

		error_at(tokens[i].input, "Unexpected token");
	}

	printf("  ret\n");
	return 0;
}

