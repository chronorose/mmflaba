#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

typedef struct _Node {
    int value;
    struct _Node* left;
    struct _Node* right;
} Node;

typedef struct {
    char* str;
    size_t size;
    size_t cap;
} String;

String* createString() {
    String* str = malloc(sizeof(String));
    str->size = 0;
    str->cap = 8;
    str->str = malloc(sizeof(char) * str->cap);
    return str;
}

void freeStr(String* str) {
    free(str->str);
    free(str);
}

void push_str(String* str, char ch) {
    if (str->size >= str->cap) {
        str->cap *= 2;
        str->str = realloc(str->str, str->cap);
    }
    str->str[str->size++] = ch;
}

Node* nNode(int value) {
    Node* newNode = malloc(sizeof(Node));
    newNode->value = value;
    newNode->right = newNode->left = NULL;
    return newNode;
}

void freeTree(Node* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

int validation_helper(Node* node, int low, int high) {
    if (node == NULL) return 1;
    if (node->value <= low || node->value >= high) return 0;
    return validation_helper(node->left, low, node->value) && validation_helper(node->right, node->value, high);
}

int validation(Node* root) {
    if (root == NULL) return 1;
    return validation_helper(root, INT_MIN, INT_MAX);
}

int deserialize_number(char* str, size_t* counter) {
    int num = 0;
    while (str[*counter] != '\n' && str[*counter] != '(' && str[*counter] != 'x') {
        num *= 10;
        num += str[*counter] - 48;
        ++(*counter);
    }
    return num;
}

Node* deserialize_helper(char* str, size_t* counter) {
    if (str[*counter] == '\n') {
        printf("incorrect input\n");
        exit(1);
    }
    if(str[*counter] == 'x') {
        return NULL;
    }
    ++(*counter);
    int num = deserialize_number(str, counter);
    Node* node = nNode(num);
    node->left = deserialize_helper(str, counter);
    ++(*counter);
    node->right = deserialize_helper(str, counter);
    ++(*counter);
    return node;
}

char* readToBuf(FILE* input) {
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char* buf = malloc(sizeof(char) * size + 1);
    fread(buf, sizeof(char), size, input);
    buf[size] = '\0';
    return buf;
}

void printTree(Node* node) {
    if (node == NULL) return;
    printTree(node->left);
    printf("%d ", node->value);
    printTree(node->right);
}

int checkWord(char* str, size_t* i, char* to_check) {
    size_t j = 0;
    while (1) {
        if (to_check[j] == '\0') return 1;
        if (str[*i] == '\0' || str[*i] != to_check[j]) {
            return 0;
        }
        ++(*i);
        ++j;
    }
}

Node* addNode(Node* node, int value) {
    if (node == NULL) {
        return nNode(value);
    } else if (node->value > value) {
        node->left = addNode(node->left, value);
    } else if (node->value < value) {
        node->right = addNode(node->right, value);
    }
    return node;
}

void serialize_number(int number, String* str) {
    String* tmp = createString();
    do {
        int x = number % 10;
        push_str(tmp, x + 48);
        number /= 10;
    } while (number != 0);
    for (size_t i = tmp->size; i > 0; i--) {
        push_str(str, tmp->str[i - 1]);
    }
    freeStr(tmp);
}

void serialize_helper(Node* node, String* str) {
    if (node) {
        push_str(str, '(');
        serialize_number(node->value, str);
        serialize_helper(node->left, str);
        serialize_helper(node->right, str);
        push_str(str, ')');
    } else {
        push_str(str, 'x');
    }
}

int countBorder(Node* node, int k) {
    if (node == NULL) return 0;
    if (node->value < k) return countBorder(node->right, k);
    else return countBorder(node->left, k) + countBorder(node->right, k) + 1;
}

String* serialize(Node* root) {
    String* str = createString();
    serialize_helper(root, str);
    return str;
}

int main() {
    char* str = readToBuf(stdin);
    size_t i = 0;
    Node* root = deserialize_helper(str, &i);
    i += 2;
    if (!validation(root)) {
        printf("INCORRECT"); 
    } else {
        while (str[i] >= 48 && str[i] <= 57) {
            int to_add = deserialize_number(str, &i);
            addNode(root, to_add);
            i++;
        }
        if (!checkWord(str, &i, "Border: ")) {
            printf("incorrect input");
        }
        int border = deserialize_number(str, &i);
        String* string = serialize(root);
        fwrite(string->str, sizeof(char), string->size, stdout);
        fprintf(stdout, "\n%d", countBorder(root, border));
        freeStr(string);
    }
    freeTree(root);
    free(str);
}
