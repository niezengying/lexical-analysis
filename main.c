#include "scan.h"

/* Analysize character;*/
int IndexOfInput(char ch)
{		
	//�ַ����࣬��������
	if(isspace(ch)) return WHITESPACE;
	else if(isdigit(ch)) return DIGIT;
	else if(isalpha(ch)) return LETTER;
	switch(ch){
	case '!':
	case '<':
	case '>':
		return SPECIALCHAR1;
		break;

	case '=':
		return EQUEL;
		break;

	case '/':
		return DIVIDE;
		break;

	case '*':
		return MULTI;
		break;

    case '+':
	case '-':
	case ';':
	case ',':
	case '(':
	case ')':
	case '[':
	case ']':
	case '{':
	case '}':
		return SPECIALCHAR2;
		break;  

	case EOF:
		return ENDOFFILE;
		break;

	default:
		return OTHERCH;
		break;
	}
}

int getNextChar(void){ 
	if (!(linepos < bufsize)){
		lineno++;
		if (fgets(lineBuf,BUFLEN-1,source)){
			if (EchoSource) fprintf(listing,"line %4d: %s\n",lineno,lineBuf);
				bufsize = strlen(lineBuf);
				linepos = 0;
				return lineBuf[linepos++];
			}
		else{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else return lineBuf[linepos++];
}

void ungetNextChar(void){
	if (!EOF_flag) linepos-- ;
}

TokenType FindResvd (char * s){
	int i;
	for (i=0;i<MAXRESERVED;i++)
		if (!strcmp(s,reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

 
void printToken( TokenType token, const char* tokenString ){
	switch (token){
	//Reserved words;
	case IF:
    case ELSE:
	case INT:
    case RETURN:
    case VOID:
    case WHILE:
		fprintf(listing,"Reserved words:\t%s\n",tokenString);
		break;

	case SPECIALCH:
		fprintf(listing,"Special symbol: \t%s\n",tokenString);
		break;

	case NUM:
		fprintf(listing,"Number: \t%s\n",tokenString);
		break;

    case ID:
		fprintf(listing,"ID: \t\t\t%s\n",tokenString);
		break;

	case ERRORID:
		fprintf(listing,"Error ID: \t\t%s\n",tokenString);
		break;

	case ENDFILE:
		fprintf(listing,"End of file: \tEOF\n");
		break;

   /*case ERROR:        //ֻҪ������ȷ����Զ����ﵽ��״̬
      fprintf(listing,"Unknown token: \t%s\n",tokenString);
      break;*/

	default:		//ERROR
      fprintf(listing,"Unknown token: \t%s\n",tokenString);
  }
}

TokenType getToken(void){
	int tokenStringIndex = 0;
	TokenType currentToken;
	StateType preState = START;
	StateType currentState = START;		//��ǰ״̬
	StateType newState;			//��״̬<--��ǰ״̬+��ǰ�����ַ�

	int nextChar = getNextChar();//��һ���ַ�
	int ch = IndexOfInput((char)nextChar);
	int save;

	while(!Accept[currentState-1]){
		save = TRUE;		//�������浽token
		newState =(StateType)TransitionTable[currentState-1][ch-1];
	
		if(!Accept[newState-1]){
			if(isspace((char)nextChar) || currentState == INBFCOMMENT || currentState == INCOMMENT || currentState == INAFCOMMENT )
			{
				if(currentState == INBFCOMMENT && preState == START) tokenStringIndex--; //��Ϊע��-- 
				save = FALSE;
			}
			if((save) && (tokenStringIndex <= MAXTOKENLEN)) 
				tokenString[tokenStringIndex++] = (char) nextChar;

			nextChar = getNextChar();
			ch = IndexOfInput((char)nextChar);
		}
		preState = currentState;//״̬ת��
		currentState = newState;
	}// end while
	
	tokenString[tokenStringIndex] = '\0';
	
	//����Ϊ����״̬���ж�token����
		switch(preState){
		case START:
			tokenString[tokenStringIndex]=(char)nextChar;
			tokenStringIndex++;
			tokenString[tokenStringIndex] = '\0';

			switch(ch ){
				//�������;
			//case SPECIALCHAR1:
			//case EQUEL:
			//case DIVIDE:
			case MULTI:
			case SPECIALCHAR2:
				currentToken = SPECIALCH;
				break;
			case ENDOFFILE:
				currentToken = ENDFILE;
				break;	
			case OTHERCH:	//����ʶ���ַ�
				currentToken = ERROR;
				break;
			}
			break;
		case INNUM:
			currentToken = NUM;
			ungetNextChar();
			break;
		case INID:
			currentToken = ID;
			ungetNextChar();//�Ż�buffer
			break;
		case INSSPECIAL:			
			currentToken = SPECIALCH;
			if(ch != EQUEL) ungetNextChar();
			else{
				tokenString[tokenStringIndex++]=(char)nextChar;
				tokenString[tokenStringIndex] = '\0';
			}
			break;
		case INBFCOMMENT:
			currentToken = SPECIALCH;	//������BFC,��Ϊ'/'
			ungetNextChar();
			break;
		case INCOMMENT:
		case INAFCOMMENT:
			currentToken = ENDFILE;
			break;
		case INERRORID:
			currentToken = ERRORID;
			ungetNextChar();
			break;
		}
	if(currentToken == ID)
		currentToken = FindResvd(tokenString);//���ұ�����

	fprintf(listing,"\tline %3d: ",lineno);
	printToken(currentToken,tokenString);

	return currentToken;
}


main( int argc, char * argv[] ){
	
	char filename[128];
	printf("�������ļ���:\n");
	scanf("%s",filename);
	if(!(source = fopen(filename,"r") ) ) {
		printf("�޷����ļ���%s\n",filename);
		return -1;
	}
	listing = stdout; /* send listing to screen */

	fprintf(listing,"\nC-Minus COMPILATION: %s\n",filename);
	while (getToken()!=ENDFILE);

	fclose(source);
	system("pause");
	return 0;
}

