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

#define MAXRESERVED 6    //�����ֳ���
#define MAXTOKENLEN 40
#define BUFLEN 256	/* BUFLEN = length of the input buffer for source code lines */
#define numInput 10
#define numStateType  9

FILE * source;
FILE * listing;

int lineno = 0;
int EchoSource = TRUE;
//int TraceScan = TRUE;		//ѡ���Ƿ��ӡtoken
char tokenString[MAXTOKENLEN+1];/* lexeme of identifier or reserved word */
static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0; /* current position in LineBuf */
static int bufsize = 0; /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

typedef enum       //token����
   {ENDFILE,ERROR,ERRORID,		/* book-keeping tokens */
    ELSE,IF,INT,RETURN,VOID,WHILE,	/* reserved words */
    ID,NUM,		 /* multicharacter tokens */  
	SPECIALCH		/* special symbols */
}TokenType;

typedef enum{			//DFA״̬�� 9��
	START=1,INNUM,INID,INSSPECIAL,INBFCOMMENT,INCOMMENT,INAFCOMMENT,INERRORID,DONE
}StateType;

typedef enum{        //�����
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

//״̬ת����
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
int IndexOfInput(char ch);		//������±�
int getNextChar(void);			//��ȡ��һ���ַ�
void ungetNextChar(void);       //�����ַ������ı�����

TokenType FindResvd (char * s);	//ƥ��ؼ���
TokenType getToken(void);
void printToken( TokenType, const char* );