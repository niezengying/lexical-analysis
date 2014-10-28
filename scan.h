#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAXRESERVED 6    //保留字长度
#define MAXTOKENLEN 40
#define BUFLEN 256	/* BUFLEN = length of the input buffer for source code lines */
#define numInput 10
#define numStateType  9

FILE * source;
FILE * listing;

int lineno = 0;
int EchoSource = TRUE;
//int TraceScan = TRUE;		//选择是否打印token
char tokenString[MAXTOKENLEN+1];/* lexeme of identifier or reserved word */
static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

typedef enum       //token类型
   {ENDFILE,ERROR,ERRORID,		/* book-keeping tokens */
    ELSE,IF,INT,RETURN,VOID,WHILE,	/* reserved words */
    ID,NUM,		 /* multicharacter tokens */  
	SPECIALCH		/* special symbols */
}TokenType;

typedef enum{			//DFA状态集 9个
	START=1,INNUM,INID,INSSPECIAL,INBFCOMMENT,INCOMMENT,INAFCOMMENT,INERRORID,DONE
}StateType;

typedef enum{        //输入表
	WHITESPACE=1,DIGIT,LETTER,
	SPECIALCHAR1,EQUEL,DIVIDE,MULTI,SPECIALCHAR2,
	ENDOFFILE,OTHERCH
}Input;

static struct{
	char* str;
	TokenType tok;
}reservedWords[MAXRESERVED]
	 =	{{"if",IF},{"else",ELSE},
		{"int",INT},{"return",RETURN},
		{"void",VOID},{"while",WHILE}};


static const int Accept[numStateType]={0,0,0,0,0,0,0,0,1};

//状态转换表
static const int TransitionTable[numStateType-1][numInput] = {
	{1,2,3,4,4,5,9,9,9,9},
	{9,2,8,9,9,9,9,9,9,9},
	{9,8,3,9,9,9,9,9,9,9},
	{9,9,9,9,9,9,9,9,9,9},
	{9,9,9,9,9,9,6,9,9,9},
	{6,6,6,6,6,6,7,6,9,6},
	{6,6,6,6,6,1,6,6,9,6},
	{9,8,8,9,9,9,9,9,9,9}
};

//static int Advance[numStateType-1][numInput];

/* Analysize character;*/
int IndexOfInput(char ch);		//输入的下标
int getNextChar(void);			//获取下一个字符
void ungetNextChar(void);       //回退字符，即改变索引

TokenType FindResvd (char * s);	//匹配关键字
TokenType getToken(void);
void printToken( TokenType, const char* );