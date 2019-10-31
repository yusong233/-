#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

//整型常量
int testINTCON(char* str, int i) {
	int j;
	int judge = 1;
	for (j = 0; j < i; j++) {
		if (str[j] < 48 || str[j] > 57) {
			judge = 0;
			break;
		}

	}
	return judge;

}

void testKeyWords(char* str, FILE* fp2, int i) {
	if (str[0] == 'c' && str[1] == 'o' && str[2] == 'n' && str[3] == 's' && str[4] == 't' && i == 5) {
		fprintf(fp2, "CONSTTK const\n");
	}
	else if (str[0] == 'i' && str[1] == 'n' && str[2] == 't' && i == 3) {
		fprintf(fp2, "INTTK int\n");
	}
	else if (str[0] == 'c' && str[1] == 'h' && str[2] == 'a' && str[3] == 'r' && i == 4) {
		fprintf(fp2, "CHARTK char\n");
	}
	else if (str[0] == 'v' && str[1] == 'o' && str[2] == 'i' && str[3] == 'd' && i == 4) {
		fprintf(fp2, "VOIDTK void\n");
	}
	else if (str[0] == 'm' && str[1] == 'a' && str[2] == 'i' && str[3] == 'n' && i == 4) {
		fprintf(fp2, "MAINTK main\n");
	}
	else if (str[0] == 'i' && str[1] == 'f' && i == 2) {
		fprintf(fp2, "IFTK if\n");
	}
	else if (str[0] == 'e' && str[1] == 'l' && str[2] == 's' && str[3] == 'e' && i == 4) {
		fprintf(fp2, "ELSETK else\n");
	}
	else if (str[0] == 'd' && str[1] == 'o' && i == 2) {
		fprintf(fp2, "DOTK do\n");
	}
	else if (str[0] == 'w' && str[1] == 'h' && str[2] == 'i' && str[3] == 'l' && str[4] == 'e' && i == 5) {
		fprintf(fp2, "WHILETK while\n");
	}
	else if (str[0] == 'f' && str[1] == 'o' && str[2] == 'r' && i == 3) {
		fprintf(fp2, "FORTK for\n");
	}
	else if (str[0] == 's' && str[1] == 'c' && str[2] == 'a' && str[3] == 'n' && str[4] == 'f' && i == 5) {
		fprintf(fp2, "SCANFTK scanf\n");
	}
	else if (str[0] == 'p' && str[1] == 'r' && str[2] == 'i' && str[3] == 'n' && str[4] == 't' && str[5] == 'f' && i == 6) {
		fprintf(fp2, "PRINTFTK printf\n");
	}
	else if (str[0] == 'r' && str[1] == 'e' && str[2] == 't' && str[3] == 'u' && str[4] == 'r' && str[5] == 'n' && i == 6) {
		fprintf(fp2, "RETURNTK return\n");
	}
	else {
		if (i > 0) {
			if (testINTCON(str, i) == 1) {
				fprintf(fp2, "INTCON ");
			}
			else {
				fprintf(fp2, "IDENFR ");
			}
			int j;
			for (j = 0; j < i; j++) {
				fprintf(fp2, "%c", str[j]);
			}
			fprintf(fp2, "\n");
		}
	}
}

void test(char* str, FILE* fp2, int i) {
	testKeyWords(str, fp2, i);
	memset(str, '\0', sizeof(char) * 1024);//重置字符数组
}

int testExit(char ch) {

	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ';'
		|| ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']'
		|| ch == '{' || ch == '}') {
		return 1;
	}
	return 0;
}

void testSingleSymbol(char ch, FILE* fp2) {
	switch (ch)
	{
	case '+':
		fprintf(fp2, "PLUS +\n");
		break;
	case '-':
		fprintf(fp2, "MINU -\n");
		break;
	case '*':
		fprintf(fp2, "MULT *\n");
		break;
	case '/':
		fprintf(fp2, "DIV /\n");
		break;
	case ';':
		fprintf(fp2, "SEMICN ;\n");
		break;
	case ',':
		fprintf(fp2, "COMMA ,\n");
		break;
	case '(':
		fprintf(fp2, "LPARENT (\n");
		break;
	case ')':
		fprintf(fp2, "RPARENT )\n");
		break;
	case '[':
		fprintf(fp2, "LBRACK [\n");
		break;
	case ']':
		fprintf(fp2, "RBRACK ]\n");
		break;
	case '{':
		fprintf(fp2, "LBRACE {\n");
		break;
	case '}':
		fprintf(fp2, "RBRACE }\n");
		break;
	default:
		break;
	}
}

void readChar(FILE* fp1, FILE* fp2) {
	char ch1;
	if ((ch1 = fgetc(fp1)) != EOF) {

		fprintf(fp2, "CHARCON %c\n", ch1);
	}
	if ((ch1 = fgetc(fp1)) != EOF) {
		return;
	}
}

void readString(FILE* fp1, FILE* fp2) {
	char ch1;
	fprintf(fp2, "STRCON ");
	while ((ch1 = fgetc(fp1)) != EOF) {
		if (ch1 == '\"') {
			fprintf(fp2, "\n");
			break;
		}
		fprintf(fp2, "%c", ch1);
	}
}

int testDoubleSymbol(char ch, FILE* fp1, FILE* fp2, char* arr, int i) {
	char ch1 = '\0';
	if (i > 0) {
		test(arr, fp2, i);
		i = 0;
	}
	if ((ch1 = fgetc(fp1)) != EOF) {
		if (ch1 == '=') {
			switch (ch)
			{
			case '>':
				fprintf(fp2, "GEQ >=\n");
				break;
			case '=':
				fprintf(fp2, "EQL ==\n");
				break;
			case '<':
				fprintf(fp2, "LEQ <=\n");
				break;
			case '!':
				fprintf(fp2, "NEQ !=\n");
				break;
			default:
				break;
			}
		}
		else { //单符号情况，处理预读进来的字符
			switch (ch)
			{
			case '>':
				fprintf(fp2, "GRE >\n");
				break;
			case '=':
				fprintf(fp2, "ASSIGN =\n");
				break;
			case '<':
				fprintf(fp2, "LSS <\n");
				break;
			default:
				break;
			}

			if (isspace(ch1) || testExit(ch1) == 1) {
				i = 0;
				if (testExit(ch1) == 1) testSingleSymbol(ch1, fp2);
			}
			else if (ch1 == '\'') { //字符常量
				readChar(fp1, fp2);
			}
			else if (ch1 == '\"') { //字符串
				readString(fp1, fp2);
			}
			else {
				arr[i] = ch1;
				i++;
			}
		}
	}
	return i;
}


int main() {
	FILE* fp1;
	FILE* fp2;

	if ((fp1 = fopen("testfile.txt", "r")) == NULL) {
		printf("File cannot be opened\n");
	}
	if ((fp2 = fopen("output.txt", "w+")) == NULL) {
		printf("File cannot be opened\n");
	}
	char ch = '*';
	char string[1024] = { '\0' };
	int i = 0;
	while ((ch = fgetc(fp1)) != EOF) {
		if (isspace(ch) || testExit(ch) == 1) { //检测去除空白符
			test(string, fp2, i);
			i = 0;
			if (testExit(ch) == 1) testSingleSymbol(ch, fp2);
		}
		else if (ch == '\'') { //字符常量
			readChar(fp1, fp2);
		}
		else if (ch == '"') { //字符串
			readString(fp1, fp2);
		}
		else if (ch == '=' || ch == '<' || ch == '>' || ch == '!') {
			i = testDoubleSymbol(ch, fp1, fp2, string, i);
		}
		else {
			string[i] = ch;
			i++;
		}


	}
	fclose(fp1);
	fclose(fp2);
	return 0;

}