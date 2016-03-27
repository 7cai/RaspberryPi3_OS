
#include <stdarg.h>

#define PTF_MAX_BUF 100

/* macros */
#define		panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

extern void uart_send(unsigned int c);

static int make_char(char *, char, int, int, int);
static int make_string(char *, char *, int, int, int);
static int make_num(char *, unsigned long, int, int, int, int, char, int, int);
static void print_str(char *s, int l);
static void panic_buffer_overflow();
static void lp_print(void (*output)(char *, int),
                     char *fmt,
                     va_list ap);

/* private variable */
static const char theFatalMsg[] = "fatal error in lp_print!";

void putchar(unsigned int c)
{
    switch (c)
    {
    case '\n':
    {
        uart_send(0x0D);
        uart_send(0x0A);
        break;
    }
    default:
        uart_send(c);
        break;
    }
}


void printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    lp_print(print_str, fmt, ap);
    va_end(ap);
}

void _panic(const char *file, int line, const char *fmt, ...)
{
    va_list ap;


    va_start(ap, fmt);
    printf("panic at %s:%d: ", file, line);
    lp_print(print_str, (char *)fmt, ap);
    printf("\n");
    va_end(ap);

    for (;;);
}

/* --------------- local help functions --------------------- */

static void print_str(char *s, int l)
{
    // special termination call
    if ((l == 1) && (s[0] == '\0'))
        return;

    for (int i = 0; i < l; i++)
    {
        putchar(s[i]);
    }
}

/* -*-
 * A low level printf() function.
 */
static void lp_print(void (*output)(char *, int),
                     char *fmt,
                     va_list ap)
{

#define 	OUTPUT(s, l)  \
  { if (((l) < 0) || ((l) > PTF_MAX_BUF)) { \
       (*output)((char*)theFatalMsg, sizeof(theFatalMsg)-1); for(;;); \
    } else { \
      (*output)(s, l); \
    } \
  }

    char buf[PTF_MAX_BUF];

    char c;
    char *s;
    long int num;

    int longFlag;
    int negFlag;
    int width;
    int ladjust;
    char padc;

    int length;

    for (;;)
    {
        /* scan for the next '%' */
        /* flush the string found so far */
        while (*fmt != '%' && *fmt != '\0')
        {
            OUTPUT(fmt++, 1);
        }

        /* are we hitting the end? */
        if (*fmt == '\0')
        {
            break;
        }

        /* we found a '%' */
        fmt++;

        /* check flags */
        ladjust = 0;
        padc = ' ';
        for (;;)
        {
            switch (*fmt)
            {
            case '-':
                ladjust = 1;
                fmt++;
                break;
            case '0':
                padc = '0';
                fmt++;
                break;
            default:
                goto check_width;
                break;
            }
        }

check_width:
        /* check for other prefixes */
        width = 0;
        while (*fmt >= '0' && *fmt <= '9')
        {
            width = width * 10 + *fmt - '0';
            ++fmt;
        }

        /* check format flag */

        /* check for long */
        longFlag = 0;
        if (*fmt == 'l' || *fmt == 'L')
        {
            longFlag = 1;
            ++fmt;
        }

        negFlag = 0;
        switch (*fmt)
        {
        case 'b':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            length = make_num(buf, num, 2, 0, width, ladjust, padc, 0, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'd':
        case 'D':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            if (num < 0)
            {
                num = - num;
                negFlag = 1;
            }
            length = make_num(buf, num, 10, negFlag, width, ladjust, padc, 0, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'o':
        case 'O':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            length = make_num(buf, num, 8, 0, width, ladjust, padc, 0, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'u':
        case 'U':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            length = make_num(buf, num, 10, 0, width, ladjust, padc, 0, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'x':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            length = make_num(buf, num, 16, 0, width, ladjust, padc, 0, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'X':
            if (longFlag)
            {
                num = va_arg(ap, long int);
            }
            else
            {
                num = va_arg(ap, int);
            }
            length = make_num(buf, num, 16, 0, width, ladjust, padc, 1, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 'c':
            c = (char)va_arg(ap, int);
            length = make_char(buf, c, width, ladjust, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case 's':
            s = (char*)va_arg(ap, char *);
            length = make_string(buf, s, width, ladjust, PTF_MAX_BUF);
            OUTPUT(buf, length);
            break;

        case '\0':
            fmt --;
            break;

        default:
            /* output this char as it is */
            OUTPUT(fmt, 1);
        }	/* switch (*fmt) */

        fmt ++;
    }		/* for(;;) */

    /* special termination call */
    OUTPUT("\0", 1);
}

static int make_char(char *to, char from, int length, int ladjust, int max_buf)
{
    int i;

    if (length > max_buf)
    {
        panic_buffer_overflow();
    }

    if (length < 1)
        length = 1;

    if (ladjust)
    {
        *to = from;
        for (i = 1; i < length; i++)
            to[i] = ' ';
    }
    else
    {
        for (i = 0; i < length - 1; i++)
            to[i] = ' ';
        to[length - 1] = from;
    }
    return length;
}


static int make_string(char *to, char *from, int length, int ladjust, int max_buf)
{
    int len = 0;
    char* t = from;
    while (*t++)
        len++;
    if (length < len)
        length = len;

    if (length > max_buf)
    {
        panic_buffer_overflow();
    }

    if (ladjust)
    {
        int i;
        for (i = 0; i < len; i++)
            to[i] = from[i];
        for (; i < length; i++)
            to[i] = ' ';
    }
    else
    {
        int i;
        for (i = 0; i < length - len; i++)
            to[i] = ' ';
        for (i = length - len; i < length; i++)
            to[i] = from[i - length + len];
    }
    return length;
}


static int make_num(char *to, unsigned long u, int base, int negFlag,
                    int length, int ladjust, char padc, int upcase, int max_buf)
{
    /* algorithm :
     *  1. prints the number from left to right in reverse form.
     *  2. fill the remaining spaces with padc if length is longer than
     *     the actual length
     *     TRICKY : if left adjusted, no "0" padding.
     *		    if negtive, insert  "0" padding between "0" and number.
     *  3. if (!ladjust) we reverse the whole string including paddings
     *  4. otherwise we only reverse the actual string representing the num.
     */

    int actualLength = 0;
    char *p = to;
    int i;

    do
    {
        int tmp = u % base;
        if (tmp <= 9)
        {
            *p++ = '0' + tmp;
        }
        else if (upcase)
        {
            *p++ = 'A' + tmp - 10;
        }
        else
        {
            *p++ = 'a' + tmp - 10;
        }
        u /= base;
    }
    while (u != 0);

    if (negFlag)
    {
        *p++ = '-';
    }

    /* figure out actual length and adjust the maximum length */
    actualLength = p - to;
    if (length < actualLength)
        length = actualLength;
    if (length > max_buf)
    {
        panic_buffer_overflow();
    }

    /* add padding */
    if (ladjust)
    {
        padc = ' ';
    }
    if (negFlag && !ladjust && (padc == '0'))
    {
        for (i = actualLength - 1; i < length - 1; i++)
            to[i] = padc;
        to[length - 1] = '-';
    }
    else
    {
        for (i = actualLength; i < length; i++)
            to[i] = padc;
    }


    /* prepare to reverse the string */
    {
        int begin = 0;
        int end;
        if (ladjust)
        {
            end = actualLength - 1;
        }
        else
        {
            end = length - 1;
        }

        while (end > begin)
        {
            char tmp = to[begin];
            to[begin] = to[end];
            to[end] = tmp;
            begin ++;
            end --;
        }
    }

    /* adjust the string pointer */
    return length;
}

static void panic_buffer_overflow()
{
    panic("Buffer overflow");
}
