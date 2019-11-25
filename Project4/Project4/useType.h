
enum type {
	IDENFR = 0, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
	IFTK, ELSETK, DOTK, WHILETK, FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS,
	MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
	RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
}nowType;

enum boo {
	FALSE = 0, TRUE
};

enum kind {
	INT, CHAR, CONST, ARRAY, STRING, PARA,//参数
	VAR, //变量
	NONE
};

enum chararacter {
	a = 0, b, c, d, e, f, g, h, ii, j, k, l, m, n, o
};

char charact[15] = { 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o' };

char text[36][10] = {
	{""},{""},{""},{""},{"const"},{"int"},{"char"},{"void"},{"main"},{"if"},{"else"},{"do"},{"while"},{"for"},{"scanf"},{"printf"},{"return"},{"+"},{"-"},{"*"},{"/"},{"<"},{"<="},{">"},{">="},{"=="},{"!="},{"="},{";"},{","},{"("},{")"},{"["},{"]"},{"{"},{"}"}
};

char name[36][10] = {
	{"IDENFR"}, {"INTCON"}, {"CHARCON"}, {"STRCON"}, {"CONSTTK"}, {"INTTK"}, {"CHARTK"}, {"VOIDTK"}, {"MAINTK"},
{"IFTK"}, {"ELSETK"}, {"DOTK"}, {"WHILETK"}, {"FORTK"}, {"SCANFTK"}, {"PRINTFTK"}, {"RETURNTK"}, {"PLUS"},
{"MINU"}, {"MULT"}, {"DIV"}, {"LSS"}, {"LEQ"}, {"GRE"}, {"GEQ"}, {"EQL"}, {"NEQ"}, {"ASSIGN"}, {"SEMICN"}, {"COMMA"},{"LPARENT"},{"RPARENT"}, {"LBRACK"}, {"RBRACK"}, {"LBRACE"}, {"RBRACE"}
};

enum opCode {
	PUSH = 36, RET, CALL, BNZ, BZ,ARR,DEVIATION//数组偏移
};

struct mid { // z = x op y
	int op;
	char x[2014];
	char y[2014];
	char z[2014];
	struct mid* next;
}mid = { "'\0'","'\0'","'\0'", NULL };

struct mid* midCode,*mid_now;


struct word {
	char string[1024];
	int type;
	int colNum;
	struct word* next;
	struct word* before;
};

struct symbol {
	char name[1024];
	int value;
	int type;//int,char,string,array
	int kind;//var,para,const
	int size;
	struct symbol* next;
	struct symbol* before;
};

struct func {
	char name[1024];
	int type;//返回值类型，int,char,none
	int valueNum;//参数个数
	int arrValue[20];//参数类型
	struct symbol* begin;
	struct symbol* end;
};

FILE* fp1;
FILE* fp2;

int numCol = 1;//读入文件的行号

//这三个变量在词法分析和语法分析部分指代word链表
struct word* head;
struct word* pre;
struct word* this;

//这两个在program初始化
struct symbol* nowSymbol;

struct symbol* p1, * p2;

struct func* nowFunc;

int nowFunc_index = 0;
int func_test = FALSE;

//符号表
int funcTable_index = -1;

struct func* funcTable[1024];

//id
char id_name[1024] = { '\0' };
int id_type = NONE;
int id_kind = NONE;
int id_value = 0;
int id_size = 1;
struct symbol* id_before = NULL;//这个在函数初始化的时候重置

//func
char func_name[1024] = { '\0' };
int func_type = NONE;
int func_arrValue[20] = { 0 };
int func_index = -1;
int func_return = FALSE;

//exper
int nowExpre = NONE;
int nowItem = NONE;
int nowFactor = NONE;

//var
int var_index = -1;

//word
void readWord();

void initialLink();

//grammar
void back();

void error(struct word* now, int index);

int reFuncState();

int state();

int expre();

int stateColumn();

void program();

//归还var
void backVarIndex() {
	if (var_index > -1) {
		var_index--;
	}
}



