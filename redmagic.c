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

// initialize position of token as zero
int pos = 0;

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

		if (*p == '+' || *p == '-' || *p == '*') {
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

enum {
	ND_NUM = 256,
};

typedef struct Node {
	int ty;
	struct Node *lhs;
	struct Node *rhs;
	int val;
} Node;

Node *new_node(int ty, Node *lhs, Node *rhs) {
	Node *node = malloc(sizeof(Node));
	node->ty = ty;
	node->lhs = lhs;
	node->rhs = rhs;
	return node;
}

Node *new_node_num(int val) {
	Node *node = malloc(sizeof(Node));
	node->ty = ND_NUM;
	node->val = val;
	return node;
}

int consume(int ty) {
	if (tokens[pos].ty != ty)
		return 0;
	pos++;
	return 1;
}

// declare expr(), mul() and term().
Node *expr();
Node *mul();
Node *term();

Node *term() {
	// if next token is '(', must be "(" expr ")"
	if (consume('(')) {
		Node *node = expr();
		if (!consume(')'))
			error_at(tokens[pos].input, "no closing brace");
		return node;
	}

	// otherwise must be numbers
	if (tokens[pos].ty = TK_NUM)
		return new_node_num(tokens[pos++].val);

	error_at(tokens[pos].input, "unexpected token");
}

Node *mul() {
	Node *node = term();

	for (;;) {
		if (consume('*'))
			node = new_node('*', node, term());
		else if (consume('/'))
			node = new_node('/', node, term());
		else
			return node;
	}
}

Node *expr() {
	Node *node = mul();

	for (;;) {
		if (consume('+'))
			node = new_node('+', node, mul());
		else if (consume('-'))
			node = new_node('-', node, mul());
		else
			return node;
	}
}

void gen(Node *node) {
	if (node->ty == ND_NUM) {
		printf("  push %d\n", node->val);
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->ty) {
	case '+':
		printf("  add rax, rdi\n");
		break;
	case '-':
		printf("  sub rax, rdi\n");
		break;
	case '*':
		printf("  imul rdi\n");
		break;
	case '/':
		printf("  cqo\n");
		printf("  idiv rdi\n");
	}

	printf("  push rax\n");
}



int main(int argc, char **argv) {
	if (argc != 2){
		fprintf(stderr, "invalid number of arguments.\n");
		return 1;
	}

	// tokenize and parse
	user_input = argv[1];
	tokenize();
	Node *node = expr();

	// put former half of asccenbly
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

	// from top to bottom abstract syntax tree
	gen(node);

	printf("  pop rax\n");
	printf("  ret\n");
	return 0;
}

