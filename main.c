//
// Created by yongshan on 16-3-31.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "calculate.h"
#include "util.h"

#define CAL_SIZE 30

void init(struct item **itemPtr);
void rule_1(char *buffer);
void scan(char *buffer);
char *subWord(char **p, char end);
char *getValue(char *word, struct item *cur);
char getTypes(char *value);
void printTree(struct item *q, int depth);
void printSpaces(int n);

struct item *root;
struct item *function;

int main(int argc, char *argv[])
{
    FILE *src;
    FILE *des;
    char *buffer;
    int size = 0;

    if (argc != 3)
    {
        fprintf(stderr, "\nUsage: %s src_file des_file\n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    init(&root);
    init(&function);

    if ((src = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "\nOpen buffer %s failed!\n\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    rewind(src);

    int c;
    while ((c = fgetc(src)) != EOF)
        if (c != ' ' && c != '\t')
            size++;
    myMalloc((void **) (&buffer), (size + 1) * sizeof(char));

    char *s = buffer;
    rewind(src);
    while ((c = fgetc(src)) != EOF)
        if (c != ' ' && c != '\t')
        {
            *s = c;
            s++;
        }

    buffer[size] = '\0';

    rule_1(buffer);
    scan(buffer);
    printTree(root, 0);
    getchar();

    //创建输出文件，对照变量树将编译结果输出
    if ((des = fopen(argv[2], "w")) == NULL)
    {
        fprintf(stderr, "\nOpen buffer %s failed!\n\n", argv[2]);
        exit(EXIT_FAILURE);
    }
    fprintf(des, "%s", buffer);
    fclose(src);
    fflush(des);
    fclose(des);

    return 0;
}

void init(struct item **itemPtr)
{
    myMalloc((void **)itemPtr, sizeof(struct item));
    (*itemPtr)->name = "";
    (*itemPtr)->isVar = false;
    (*itemPtr)->next = NULL;
    (*itemPtr)->parent = NULL;
    (*itemPtr)->mem.sonMemory = NULL;
}

void rule_1(char *buffer)
{
    char *p;
    char *q;
    char *s;
    for (p = buffer; *p != '\0'; p++)
    {
        if (*p == '/' && *(p + 1) == '/')
        {
            for (q = p + 1; *q != '\0'; q++)
                if (*q == '\n')
                    break;
            for (s = p; s < q; s++)
                *s = ' ';
            p = q;
        }
    }
}

void scan(char *buffer)
{
    char *p;
    char *word;
    struct item *head = root;
    struct item *cur;
    for (p = buffer; *p != '\0'; p++)
    {
        if ((*p == '@' && isalpha(*(p + 1))) || isalpha(*p))
        {
            char *curName;
            struct item *cursor = head->mem.sonMemory;
            if (isalpha(*p))
            {
                char *tailName, c;
                c = *p;
                tailName = subWord(&p, ':');
                myMalloc((void **) (&curName), strlen(tailName) + 2);
                curName[0] = c;
                curName[1] = '\0';
                strcat(curName, tailName);
                free(tailName);
            }
            else
                curName = subWord(&p, ':');

            //在当前作用域寻找同名变量
            while (cursor != NULL)
            {
                if (cursor->isVar == true)
                if (!strcmp(cursor->name, curName))
                    break;
                cursor = cursor->next;
            }
            //在当前作用域找到同名变量
            if (cursor != NULL)
            {
                char *oldValue = cursor->mem.varMemory.value;
                cursor->mem.varMemory.value = getValue(subWord(&p, ';'), cursor);
                free(oldValue);
            }
            else
            {
                myMalloc((void **) (&cur), sizeof(struct item));
                cur->name = curName;
                cur->isVar = true;
                cur->next = head->mem.sonMemory;
                head->mem.sonMemory = cur;
                cur->parent = head;
                cur->mem.varMemory.type = '0';
                cur->mem.varMemory.value = getValue(subWord(&p, ';'), cur);
            }
        }
        else if ((*p == '#' || *p == '.') && isalpha(*(p + 1)))
        {
            char *subName;
            char suffix[2] = { *p, '\0' };
            myMalloc((void **) (&cur), sizeof(struct item));
            cur->isVar = false;
            cur->parent = head;
            subName = subWord(&p, '{');
            myMalloc((void **) (&word), strlen(cur->parent->name) + strlen(subName) + 2);
            strcpy(word, cur->parent->name);
            strcat(word, suffix);
            strcat(word, subName);
            cur->name = word;
            cur->next = head->mem.sonMemory;
            head->mem.sonMemory = cur;
            cur->mem.sonMemory = NULL;
            head = cur;
        }
        else if (*p == '}')
        {
            cur = head;
            head = cur->parent;
        }
        else
            ;
    }
}

char *getValue(char *word, struct item *cur)
{
    //先改变类型，再计算
    char *p, *s, *name, *cal, tmp[2];
    struct item *cursor;
    int ctr = 0;

    tmp[1] = '\0';
    myMalloc((void **) (&cal), CAL_SIZE * sizeof(char));
    memset(cal, '\0', CAL_SIZE);
    for (p = word; *p != '\0'; p++)
    {
        if (*p == '@' && (isalpha(*(p + 1)) || *(p + 1) == '{'))
        {
            char *q;
            if (isalpha(*(p + 1)))
            {
                q = p;
                while (*q != '\0')
                {
                    if (*q == '+' || *q == '*' || *q == '/' || *q == ')' || (*q == '-' && isdigit(*(q + 1))))
                        break;
                    q++;
                }
                myMalloc((void **) (&name), (q - p) * sizeof(char));
                for (s = name, p++; p != q; s++, p++)
                    *s = *p;
                *s = '\0';
                p--;
            }
                //字符串插值
            else
            {
                q = ++p;
                while (*q != '\0')
                {
                    if (*q == '}')
                        break;
                    q++;
                }
                myMalloc((void **) (&name), (q - p) * sizeof(char));
                for (s = name, p++; p != q; s++, p++)
                    *s = *p;
                *s = '\0';
            }

            cursor = cur->parent->mem.sonMemory;
            while (cursor != root && cursor != NULL)
            {
                if (cursor->isVar == true)
                    if (!strcmp(cursor->name, name) && cursor->mem.varMemory.type != '0')
                        break;

                if (cursor->next == NULL)
                    cursor = cursor->parent;
                else
                    cursor = cursor->next;
            }
            free(name);
            strcat(cal, cursor->mem.varMemory.value);

            //TO DO 在第二次扫描输出css时忽略读入的最外层变量
            //有限状态自动机
            //不断读取字符，若读取的是变量或子层就搜索并拼接value等，否则直接写入字符
            //注意字符指针同步，不要重复写一个变量！
            //删除最外层的无用变量和被引用的变量

            if (ctr == 0)
            {
                cur->mem.varMemory.type = cursor->mem.varMemory.type;
                ctr++;
            }
        }
        else
        {
            tmp[0] = *p;
            strcat(cal, tmp);
        }
    }
    //计算的具体步骤

    free(word);
    //类似于black的颜色当作字符串处理
    if (ctr == 1)
        return calStr(cal, cur->mem.varMemory.type);
    else
    {
        cur->mem.varMemory.type = getTypes(cal);
        return calStr(cal, cur->mem.varMemory.type);
    }
}

char getTypes(char *value)
{
    if (value[0] == '"' || isalpha(value[0]))
        return 's';
    if (value[0] == '#')
        return 'c';
    return 'p';
}

char *subWord(char **p, char end)
{
    char *q = *p, *s, *word;
    while (*q != '\0')
    {
        if (*q == end)
            break;
        q++;
    }

    myMalloc((void **) (&word), (q - *p) * sizeof(char));
    for (s = word, (*p)++; (*p) != q; (*p)++, s++)
        *s = **p;
    *s = '\0';
    return word;
}

void printTree(struct item *q, int depth)
{
    struct item *p = q;
    if (p == NULL)
        return;
    printSpaces(depth);
    if (p->isVar == false)
    {
        printf("%s %d\n", p->name, p->isVar);
        printTree(p->mem.sonMemory, depth + 1);
    }
    else
        printf("%s %d %s\n", p->name, p->isVar, p->mem.varMemory.value);
    printTree(p->next, depth);
}

void printSpaces(int n)
{
    int i;
    for (i = 0; i < n; i++)
        printf("   ");
}

