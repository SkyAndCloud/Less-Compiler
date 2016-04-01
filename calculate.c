//
// Created by yongshan on 16-3-31.
//

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "util.h"
#include "calculate.h"

int findLast(const char* s, char a)
{
    int pos = strlen(s);
    // 从字符串末尾位置开始查找
    const char* p = s + pos;

    // 如果没有到达字符串开始的前一个位置（s-1）
    while ((s - 1) != p)
    {
        // 如果当前位置的字符就是要查找的字符
        if (*p == a)
            break; // 结束查找哦啊
        p--; // 变换到下一个位置
        pos--;
    }
    if ((s - 1) != p) // 找到字符
        return pos;
    else
        // 未找到
        return -1;
}
// 取得字符串的左半部分
char* leftStr(char* s, int pos)
{
    s[pos] = '\0';
    return s;
}
// 取得字符串的右半部分
char* rightStr(char* s, int pos)
{
    return s + pos + 1;
}
// 计算字符串计算式s的值
int calc(char* s)
{
    int n = 0;

    n = findLast(s, '*');
    if (-1 != n)
        return calc(leftStr(s, n)) * calc(rightStr(s, n));
    n = findLast(s, '/');
    if (-1 != n)
        return calc(leftStr(s, n)) / calc(rightStr(s, n));

    n = findLast(s, ')');
    int m = findLast(s, '(');
    if (-1 != n && -1 != m)
    {
        int i;
        char temp[strlen(s) + 1];
        int j = 0;
        for (i = m + 1; i < n; i++)
            temp[j++] = s[i];
        return calc(temp);
    }
    // 找到最后一个加号
    n = findLast(s, '+');
    if (-1 != n)
        // 以加号所在的位置，将字符串分为左右两部分分别计算
        // 然后将两部分的值加起来
        return calc(leftStr(s, n)) + calc(rightStr(s, n));
    n = findLast(s, '-');
    if (-1 != n)
        return calc(leftStr(s, n)) - calc(rightStr(s, n));

    // 当字符串中不包含运算符时，返回这个数字本身
    return atoi(s);
}
char *calStr(char *str, char type)
{
    if (type == 's')
    {
        if (str[0] == '"')
        {
            char *newStr;
            myMalloc((void **)(&newStr), strlen(str) - 1);
            str[strlen(str) - 1] = '\0';
            strcpy(newStr, str + 1);
            free(str);
            return newStr;
        }
        return str;
    }
    else if (type == 'p')
    {
        char *calInt, *p, *q;
        int result = 0;
        myMalloc((void **) (&calInt), strlen(str) + 1);
        for (p = calInt, q = str; *q != '\0'; q++)
            if (*q != 'p' && *q != 'x')
            {
                *p = *q;
                p++;
            }

        *p = '\0';
        free(str);
        //纯整数计算
        result = calc(calInt);
        free(calInt);
        //返回%d+"px"
        int i = 0, tmp = result;
        while (tmp)
        {
            i++;
            tmp /= 10;
        }
        myMalloc((void **) (&calInt), (i + 3) * sizeof(char));
        sprintf(calInt, "%d", result);
        strcat(calInt, "px");
        return calInt;
    }
    else if (type == 'c')
    {
        //计算颜色
        char *calColor, *p, *q;
        char tmp[7], *result, *color;
        tmp[6] = '\0';
        myMalloc((void **) (&calColor), strlen(str) + 1);
        memset(calColor, '\0', strlen(str) + 1);
        for (p = calColor, q = str; *q != '\0'; p++, q++)
        {
            if (*q == '#')
            {
                if (isdigit(q[4]))
                {
                    strncpy(tmp, q + 1, 6);
                    q += 6;
                }
                else
                {
                    memset(tmp, q[1], 6);
                    q += 3;
                }
                char *ch = hex2int(tmp);
                strcat(calColor, ch);
                p += strlen(ch) - 1;
                free(ch);
            }
            else
                *p = *q;
        }
        color = int2hex(calc(calColor));
        myMalloc((void **) (&result), strlen(color) + 2);
        result[0] = '#';
        result[1] = '\0';
        strcat(result, color);
        free(color);
        free(calColor);
        return result;
    }
    else
        return str;
}

char *int2hex(int num)
{
    char *hex;
    myMalloc((void **) (&hex), 7);
    memset(hex, '0', 6);
    hex[6] = '\0';
    int i = 5;
    while (num && i >= 0)
    {
        hex[i] = "0123456789abcdef"[num % 16];
        num /= 16;
        i--;
    }
    if (num)
    {
        hex[0] = 'f';
        hex[1] = 'f';
    }
    return hex;
}
char *hex2int(char *s)
{
    char *num;
    int i, j, n = 0;
    for (i = 0; i < 6; i++)
    {
        n *= 16;
        if (isdigit(s[i]))
            n += s[i] - '0';
        else
            n += tolower(s[i]) - 'a' + 10;
    }
    j = n;
    while (j)
    {
        i++;
        j /= 10;
    }
    myMalloc((void **) (&num), i + 1);
    sprintf(num, "%d", n);
    return num;
}

