#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdbool.h>

char buf[256];
char temp[256];
const int NOT_EXPR = -1;
const int WRONG_FORMAT = -2;
const int DELIMITER = -3;
const int NUMBER = -4;
const int NOT_FOUND = -5;
const int ERROR = -6;
const char* RESERVED[] = { "and", "not", "xor", "or", "True", "False" };
const char* RESERVED_C[] = { "&", "!", "+", "|", "1", "0" };

typedef struct Node {
    struct Node* Next;
    char* Name;
    bool Data;
} Node_t;

typedef struct list {
    size_t size;
    Node_t *head;
    Node_t *tail;
} list_t;

typedef struct stack {
    char c;
    struct stack *next;
} stack;

/* Пpототипы функций */
stack *push(struct stack *, char);
char DEL(struct stack **);
int PRIOR(char);

/* работа со строками*/
/* замена подстроки строкой*/
char *str_replace(char *, int, const char *,
    const char *, const char *);
/* удаление выбранного символа*/
size_t remove_ch(char *, char);

/* работа с листом*/
list_t *create_list();
void list_push(list_t *, char *, bool);
int get_value(list_t *, char *);
void list_pop(list_t *);

/* проверка на правильный формат*/
bool is_right_format(const char *);
bool is_reserved(char);

/* парсинг <name>=True|False */
int analyse_and_add(char *, list_t *);
/* перевод выражения к виду (!1&0)|1+1 */
int convert(char *, list_t *);
/* перевод в обратную польскую нотацию*/
char *to_rpn(const char *, list_t *);
/* вычисление */
int calculate(char *, list_t *);

int main()
{
    list_t *list = create_list();
    do
    {
        fgets(buf, 256, stdin);

        int status = analyse_and_add(buf, list);
        if (status == WRONG_FORMAT)
        {
            printf("%s", "[error]");
            break;
        }
        else if (status == NOT_EXPR)
        {
            int result = calculate(buf, list);
            if (result == 1) {
                printf("%s", "True");
            } else if (result == 0) {
                printf("%s", "False");
            } else {
                printf("%s", "[error]");
            }
            break;
        }
    } while (1);

    while (list->size != 0) {
        list_pop(list);
    }
    free(list);

    system("pause");
    return 0;
}

/* Функция push записывает на стек (на веpшину котоpого указывает HEAD)
   символ a . Возвpащает указатель на новую веpшину стека */
stack *push(struct stack *HEAD, char a)
{
    stack *PTR = (stack*)malloc(sizeof(struct stack));
    if (PTR == NULL)
    {
        /* Если её нет - выход */
        puts("[error]");
        exit(0);
    }
    /* Инициализация созданной веpшины */
    PTR->c = a;
    /* и подключение её к стеку */
    PTR->next = HEAD;
    /* PTR -новая веpшина стека */
    return PTR;
}

/* Функция DEL удаляет символ с веpшины стека.
   Возвpащает удаляемый символ.
   Изменяет указатель на веpшину стека */
char DEL(stack **HEAD)
{
    stack *PTR;
    char a;
    /* Если стек пуст,  возвpащается '\0' */
    if (*HEAD == NULL)
        return '\0';
    /* в PTR - адpес веpшины стека */
    PTR = *HEAD;
    a = PTR->c;
    /* Изменяем адpес веpшины стека */
    *HEAD = PTR->next;
    /* Освобождение памяти */
    free(PTR);
    /* Возвpат символа с веpшины стека */
    return a;
}

/* Функция PRIOR возвpащает пpиоpитет аpифм. опеpации */
int PRIOR(char a)
{
    switch (a)
    {
    case '!':
        return 5;
    case '&':
        return 4;
    case '|':
        return 3;
    case '+':
        return 2;
    case '(':
        return 1;
    default:
        return 0;
    }
}

char *str_replace(char *dst, int num, const char *str,
    const char *orig, const char *rep) {
    if (!str || !orig || !rep) {
        return NULL;
    }
    const char* ptr;
    size_t len1 = strlen(orig);
    size_t len2 = strlen(rep);
    char*  tmp = dst;

    num -= 1;
    while ((ptr = strstr(str, orig)) != NULL) {
        num -= (ptr - str) + len2;
        if (num < 1)
            break;

        strncpy(dst, str, (size_t)(ptr - str));
        dst += ptr - str;
        strncpy(dst, rep, len2);
        dst += len2;
        str = ptr + len1;
    }

    for (; (*dst = *str) && (num > 0); --num) {
        ++dst;
        ++str;
    }
    return tmp;
}

size_t remove_ch(char *s, char ch)
{
    size_t i = 0, j = 0;
    for (i = 0; s[i]; ++i)
        if (s[i] != ch)
            s[j++] = s[i];

    s[j] = '\0';
    return j;
}

list_t* create_list()
{
    list_t *lt = (list_t*)malloc(sizeof(list_t));
    if (!lt)
        return NULL;

    lt->size = 0;
    lt->head = NULL;
    lt->tail = lt->head;

    return lt;
}

void list_push(list_t *lt, char *name, bool value)
{
    Node_t* node = (Node_t*)malloc(sizeof(Node_t));
    if (!node)
        return;

    node->Name = name;
    node->Data = value;
    node->Next = lt->head;

    lt->head = node;
    lt->size += 1;
}

int get_value(list_t *lt, char* name)
{
    Node_t *curr = lt->head;
    while (curr != NULL) {
        if (strcmp(name, curr->Name) == 0)
            return curr->Data;
        curr = curr->Next;
    }
    return NOT_FOUND;
}

void list_pop(list_t *lt)
{
    if (lt->size == 0) {
        return;
    }

    Node_t *node = lt->head;
    free(lt->head->Name);
    lt->size -= 1;
    lt->head = node->Next;
    free(node);

    if (lt->size == 0) {
        lt->head = NULL;
        lt->tail = NULL;
    }
}

bool is_right_format(const char *str)
{
    bool only_small = false;
    // проверка на маленькие латинские буквы
    for (size_t i = 0; i < strlen(str); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            only_small = true;
        } else {
            only_small = false;
            break;
        }
    }

    // проверка на запрещенные имена and or not xor;
    if (only_small) {
        for (size_t k = 0; RESERVED[k]; ++k) {
            if (strcmp(str, RESERVED[k]) == 0)
                return false;
        }
    }

    return only_small;
}

// Запись в лист пары <name>,  <True|False>
int analyse_and_add(char *expr, list_t *lt)
{
    if (!expr)
        return ERROR;

    // если не <name>=True|False
    char* pos = strchr(expr, '=');
    char* end = strchr(expr, ';');
    if (pos == NULL || end == NULL)
        return NOT_EXPR;

    remove_ch(expr, ' ');
    pos = strchr(expr, '=');
    end = strchr(expr, ';');

    /* Записываем имя*/
    size_t len_n = pos - expr;
    char* name = (char*)calloc(len_n + 1, sizeof(char));
    if (!name) {
        return ERROR;
    }
    memmove(name, expr, sizeof(char) * len_n);

    if (!is_right_format(name)) {
        free(name);
        return WRONG_FORMAT;
    }

    /* Получаем значение*/
    size_t len_v = end - pos;
    char* value = (char*) calloc(len_v, sizeof(char));
    if (!value) {
        free(name);
        return ERROR;
    }
    memmove(value, pos + 1, sizeof(char) * (len_v-1));

    /* Определяем True|False */
    bool v;
    if (strcmp(value, "True") == 0)
        v = true;
    else if (strcmp(value, "False") == 0)
        v = false;
    else
    {
        free(name);
        free(value);
        return WRONG_FORMAT;
    }

    /* Записываем в лист ключ-значение*/
    list_push(lt, name, v);
    free(value);
    return v;
}

bool is_reserved(char c)
{
    return c == ' ' || c == '(' || c == ')' || c == '!' || c == '&' || c == '+' || c == '|' || c == '\n' || c == '\t';
}

int convert(char *expr, list_t *lt)
{
    if (!expr || lt == NULL)
        return ERROR;

    /* Замена ключевых слов на соответсвующие символы */
    size_t i = 0;
    for (i = 0; RESERVED[i] && RESERVED_C[i]; ++i)
    {
        str_replace(temp, sizeof(temp) - 1, expr, RESERVED[i], RESERVED_C[i]);
        strcpy(expr, temp);
    }
    
    /* Замена переменных на их значение */
    for (i = 0; expr[i]; ++i)
    {
        if (strchr("&|!+() 01", expr[i]) == NULL)
        {
            size_t j = i;
            for (j = i; !is_reserved(expr[j]) && expr[j]; ++j);

            char* varible = (char*)calloc(j - i + 1, sizeof(char));
            if (!varible) {
                return ERROR;
            }
            memmove(varible, expr + i, j - i);

            if (strlen(varible) == 0)
                break;

            int status = get_value(lt, varible);
            if (status == NOT_FOUND)
            {
                free(varible);
                return NOT_FOUND;
            }

            if (status == 0)
                str_replace(temp, sizeof(temp) - 1, expr, varible, "0");
            if (status == 1)
                str_replace(temp, sizeof(temp) - 1, expr, varible, "1");

            strcpy(expr, temp);
            free(varible);
        }
    }

    /* удаление пробелов*/
    remove_ch(expr, ' ');

    /* Успех*/
    return 0;
}

char* to_rpn(const char *expr, list_t *lt)
{
    struct stack *OPERS = NULL;
    char* outstring = (char*)malloc(256*sizeof(char));
    if (outstring == NULL)
        return NULL;
    int k, point;

    k = point = 0;
    /* Повтоpяем , пока не дойдем до '=' */
    while (expr[k])
    {
        /* Если очеpедной символ - ')' */
        if (expr[k] == ')')
            /* то выталкиваем из стека в выходную стpоку */
        {
            /* все знаки опеpаций до ближайшей */
            while ((OPERS->c) != '(')
                /* откpывающей скобки */
                outstring[point++] = DEL(&OPERS);
            /* Удаляем из стека саму откpывающую скобку */
            DEL(&OPERS);
        }
        /* Если очеpедной символ - значение , то */
        if (expr[k] == '0' || expr[k] == '1')
            /* пеpеписываем её в выходную стpоку */
            outstring[point++] = expr[k];
        /* Если очеpедной символ - '(' , то */
        if (expr[k] == '(')
            /* заталкиваем её в стек */
            OPERS = push(OPERS, '(');
        if (expr[k] == '!' || expr[k] == '&' || expr[k] == '|' || expr[k] == '+')
            /* Если следующий символ - знак опеpации , то: */
        {
            /* если стек пуст */
            if (OPERS == NULL)
                /* записываем в него опеpацию */
                OPERS = push(OPERS, expr[k]);
            /* если не пуст */
            else
                /* если пpиоpитет поступившей опеpации больше
                                пpиоpитета опеpации на веpшине стека */
                if (PRIOR(OPERS->c) < PRIOR(expr[k]))
                    /* заталкиваем поступившую опеpацию на стек */
                    OPERS = push(OPERS, expr[k]);
            /* если пpиоpитет меньше */
                else
                {
                    while ((OPERS != NULL) && (PRIOR(OPERS->c) >= PRIOR(expr[k])))
                        /* пеpеписываем в выходную стpоку все опеpации
                                            с большим или pавным пpиоpитетом */
                        outstring[point++] = DEL(&OPERS);
                    /* записываем в стек поступившую  опеpацию */
                    OPERS = push(OPERS, expr[k]);
                }
        }
        /* Пеpеход к следующему символу входной стpоки */
        k++;
    }
    /* после pассмотpения всего выpажения */
    while (OPERS != NULL)
        /* Пеpеписываем все опеpации из */
        outstring[point++] = DEL(&OPERS);
    /* стека в выходную стpоку */
    outstring[point] = '\0';

    return outstring;
}

int calculate(char *expr, list_t *lt)
{
    if (convert(expr, lt) != 0)
        return ERROR;

    char* outstring = to_rpn(expr, lt);
    if (outstring == NULL)
        return ERROR;

    // Если нет операторов и больше 1 переменной
    if (strlen(outstring) > 1)
    {
        bool no_operators = strchr(outstring, '&') == NULL && 
            strchr(outstring, '+') == NULL && 
            strchr(outstring, '|') == NULL && 
            strchr(outstring, '+') == NULL && 
            strchr(outstring, '!') == NULL;
        if (no_operators)
        {
            free(outstring);
            return ERROR;
        }
    }
    
    /* Выделене стэка для операций вычисления */
    int *stack = (int*)malloc(strlen(expr)*sizeof(int));
    if (stack == NULL)
    {
        free(outstring);
        return ERROR;
    }
    // sp = индекс ячейки, куда будет push-иться очередное число
    int sp = 0;      // (sp-1) = индекс ячейки, являющейся вершиной стека
    for (size_t k = 0; outstring[k]; ++k) {
        int c = outstring[k];
        switch (c) {
        case  ' ':
        case '\n':
            break;
        case '+':
            stack[sp - 2] ^= stack[sp - 1];  sp--;
            break;
        case '|':
            stack[sp - 2] |= stack[sp - 1];  sp--;
            break;
        case '&':
            stack[sp - 2] = stack[sp - 1] & stack[sp - 2];  sp--;
            break;
        case '!':
            stack[sp - 1] = !stack[sp - 1];
            break;
        case '0':
        {
            stack[sp++] = 0;
            break;
        }
        case '1':
        {
            stack[sp++] = 1;
            break;
        }
        default:
            printf("%s", "[error]");
            free(stack);
            free(outstring);
            return -1;
        }
    }

    int result = stack[sp - 1];
    free(outstring);
    free(stack);
    if (result == 0 || result == 1)
        return result;

    return -1;
}