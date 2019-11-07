#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

FILE* fp1;
FILE* fp2;

char re[100][128] = { '\0' };

int index1 = 0;

int index2 = 0;

char non[100][128] = { '\0' };

int numCol = 1;//读入文件的行号

enum type
{
	IDENFR = 0, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
	IFTK, ELSETK, DOTK, WHILETK, FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS,
	MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
	RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
}nowType;

enum boo {
	FALSE = 0, TRUE
};

enum kind {
	INT, CHAR, STRING, CONSTINT, CONSTCHAR, NONE, INTARR
};

char text[36][10] = {
	{""},{""},{""},{""},{"const"},{"int"},{"char"},{"void"},{"main"},{"if"},{"else"},{"do"},{"while"},{"for"},{"scanf"},{"printf"},{"return"},{"+"},{"-"},{"*"},{"/"},{"<"},{"<="},{">"},{">="},{"=="},{"!="},{"="},{";"},{","},{"("},{")"},{"["},{"]"},{"{"},{"}"}
};

char name[36][10] = {
	{"IDENFR"}, {"INTCON"}, {"CHARCON"}, {"STRCON"}, {"CONSTTK"}, {"INTTK"}, {"CHARTK"}, {"VOIDTK"}, {"MAINTK"},
{"IFTK"}, {"ELSETK"}, {"DOTK"}, {"WHILETK"}, {"FORTK"}, {"SCANFTK"}, {"PRINTFTK"}, {"RETURNTK"}, {"PLUS"},
{"MINU"}, {"MULT"}, {"DIV"}, {"LSS"}, {"LEQ"}, {"GRE"}, {"GEQ"}, {"EQL"}, {"NEQ"}, {"ASSIGN"}, {"SEMICN"}, {"COMMA"},{"LPARENT"},{"RPARENT"}, {"LBRACK"}, {"RBRACK"}, {"LBRACE"}, {"RBRACE"}
};

struct word {
	int type;
	char string[1024];
	int colNum;
	struct word* next;
	struct word* before;
};

struct word* head;
struct word* pre;
struct word* this;

struct symbol {
	char name[1024];
	int type;//int,char,constInt,constChar
	int value;
	int layer;//层数
};

struct func {
	char name[1024];
	int type;//返回值类型，int,char,none
	int valueNum;//参数个数
	int arrValue[20];//参数类型
};

/*
	符号表，table1存储变量及常量，table2存储函数
*/
int table1_index = 0;
int table2_index = 0;

struct symbol* table1[1024];

struct func* table2[1024];

//当前函数的层数
int nowLayer = 0;

//id
char id_name[1024] = { '\0' };
int id_type = NONE;
int id_value = 0;

//func
char func_name[1024] = { '\0' };
int func_type = NONE;
int func_arrValue[20] = { 0 };
int func_index = 0;

//重置id
void idInitial() {
	memset(id_name,'\0',sizeof(id_name));
	id_type = NONE;
	id_value = 0;
}
//重置func
void funcInitial() {
	memset(func_name, '\0', sizeof(id_name));
	func_type = NONE;
	func_index = 0;
	memset(func_arrValue, 0, sizeof(func_arrValue));
}

void addID() {
	struct symbol* iden;
	iden = (struct symbol*)malloc(sizeof(struct symbol));
	iden->layer = nowLayer;
	iden->type = id_type;
	iden->value = id_value;
	strcpy(iden->name, id_name);
	table1[table1_index] = iden;
	table1_index++;
	idInitial();
}

void error() {

}

int reFuncState();
int state();
int expre();
int stateColumn();

struct word* getSym() {
	struct word* nowWord = this;
	pre = this;
	this = this->next;
	return nowWord;
}

void back() {
	if (this == NULL) {
		this = pre;
	}
	else {
		this = this->before;
		pre = this->before;
	}
}

//<关系运算符>
int relationOp() {
	struct word* now = getSym();
	int type = now->type;
	if (type == LSS || type == LEQ || type == GRE || type == GEQ || type == NEQ || type == EQL) {
		return TRUE;
	}
	else {
		back();
		return FALSE;
	}
}

//<无符号整数>
int unsignedInteger() {
	struct word* nowWord = getSym();
	if (nowWord->type == INTCON) {
		if (id_name != '\0') {
			id_value = atoi(nowWord->string);
		}
		return TRUE;
	}
	else {
		back();
		return FALSE;
	}
}

//<整数>
int integer() {
	struct word* nowWord = getSym();
	int type = nowWord->type;
	if (type == MINU || type == PLUS) {
		if (unsignedInteger() == TRUE) {
			if (type == MINU) {
				id_value = -id_value;
			}
			return TRUE;
		}
	}
	else {
		back();
		if (unsignedInteger() == TRUE) {
			return TRUE;
		}
	}
	return FALSE;
}

//<声明头部>
int headState() {
	struct word* did;
	struct word* now = getSym();
	if (now->type == INTTK) {
		func_type = INT;
		if ((did = getSym())->type == IDENFR) {
			strcpy(func_name, did->string);

			strcpy(re[index1], did->string);
			index1++;

			return TRUE;
		}
		else {
			back();//返回did
			back();//返回now
			error();
			return FALSE;
		}
	}
	else if (now->type == CHARTK) {
		func_type = CHAR;
		if ((did = getSym())->type == IDENFR) {
			strcpy(func_name, did->string);

			strcpy(re[index1], did->string);
			index1++;

			return TRUE;
		}
		else {
			back();//返回did
			back();//返回now
			error();
			return FALSE;
		}
	}
	else {
		back();//返回now
		return FALSE;
	}
}

//<类型标识符>不需要输出
int typeIden() {
	struct word* now = getSym();
	if (now->type == INTTK || now->type == CHARTK) {
		back();
		return TRUE;
	}
	else {
		back();
		return FALSE;
	}
}

//<变量定义>
int varDef() {
	struct word* did, * did1, * did2;
	int judge = FALSE;
	int type;
	if (typeIden() == TRUE) {
		did = getSym();
		type = did->type;
		if ((did1 = getSym())->type == IDENFR) {
			strcpy(id_name, did1->string);
			if ((did2 = getSym())->type == SEMICN || did2->type == COMMA || did2->type == LBRACK) {
				switch (type) {
					case INTTK:
						id_type = INT;
						break;
					case CHARTK:
						id_type = CHAR;
						break;
					default:
						break;
				}
				addID();
				judge = TRUE;
			}
			if (did2->type == LBRACK) {
				switch (type) {
					case INTTK:
						id_type = INTARR;
						break;
					case CHARTK:
						id_type = STRING;
						break;
					default:
						break;
				}
				if (unsignedInteger() == TRUE) {
					back();
					if (atoi((did = getSym())->string) >= 0) {
						//这个无符号整数已经输出
						if ((did = getSym())->type == RBRACK) {
							judge = TRUE;
							addID();
							while ((did = getSym())->type == COMMA) {
								if ((did = getSym())->type == IDENFR) {
									strcpy(id_name, did1->string);
									if ((did = getSym())->type == LBRACK) {
										switch (type) {
											case INTTK:
												id_type = INTARR;
												break;
											case CHARTK:
												id_type = STRING;
												break;
											default:
												break;
										}

										if (unsignedInteger() == TRUE) {
											back();
											if (atoi((did = getSym())->string) >= 0) {
												if ((did = getSym())->type == RBRACK) {
													judge = TRUE;
													addID();
												}
												else {
													judge = FALSE;
													error();//缺少]
													break;
												}
											}
											else {
												judge = FALSE;
												error();//数组长度<0
												break;
											}
										}
										else {
											judge = FALSE;
											error();//[]中不是无符号整数
											break;
										}
									}
									else {
										back();
										switch (type) {
											case INTTK:
												id_type = INT;
												break;
											case CHARTK:
												id_type = CHAR;
												break;
											default:
												break;
										}
										addID();
									}
								}
								else {
									judge = FALSE;
									error();
									break;
								}
							}
							back();
						}
						else {
							judge = FALSE;
							error();//缺少]
						}
					}
					else {
						judge = FALSE;
						error();//数组长度<0
					}
				}
				else {
					judge = FALSE;
					error();//[]中不是无符号整数
				}
			}
			else {
				back();
				if (judge == TRUE) {
					while ((did = getSym())->type == COMMA) {
						if ((did = getSym())->type == IDENFR) {
							strcpy(id_name, did->string);
							judge = TRUE;
							if ((did = getSym())->type == LBRACK) {
								switch (type) {
								case INTTK:
									id_type = INTARR;
									break;
								case CHARTK:
									id_type = STRING;
									break;
								default:
									break;
								}
								if (unsignedInteger() == TRUE) {
									back();
									if (atoi((did = getSym())->string) >= 0) {
										if ((did = getSym())->type == RBRACK) {
											addID();
											judge = TRUE;
										}
										else {
											judge = FALSE;
											error();//缺少]
											break;
										}
									}
									else {
										judge = FALSE;
										error();//数组长度<0
										break;
									}
								}
								else {
									judge = FALSE;
									error();//[]中不是无符号整数
									break;
								}
							}
							else {
								switch (type) {
								case INTTK:
									id_type = INT;
									break;
								case CHARTK:
									id_type = CHAR;
									break;
								default:
									break;
								}
								addID();
								back();
							}
						}
						else {
							judge = FALSE;
							error();
							break;
						}
					}
					back();
				}
				else {
					back();
					back();
				}

			}
		}
		else {
			error();
			back();
			judge = FALSE;
		}
	}
	else {
		judge = FALSE;
	}
	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<变量说明>
int varSpe() {
	struct word* now;
	int judge = FALSE;
	while (varDef() == TRUE) {
		if ((now = getSym())->type == SEMICN) {
			judge = TRUE;
		}
		else {
			error();//缺少;
		}
	}
	if (judge == TRUE) {
		return TRUE;
	}
	else
		return FALSE;

}

//<常量定义>
int conDef() {
	struct word* did;
	int judge = TRUE;
	struct word* now = getSym();
	if (now->type == INTTK) {
		id_type = CONSTINT;
		if ((did = getSym())->type == IDENFR) {
			strcpy(id_name, did->string);
			if ((did = getSym())->type == ASSIGN) {
				if (integer() == TRUE) {
					addID();
					while ((did = getSym())->type == COMMA) {
						if ((did = getSym())->type == IDENFR) {
							strcpy(id_name, did->string);
							if ((did = getSym())->type == ASSIGN) {
								if (integer() == TRUE) {
									addID();
									judge = TRUE;
								}
							}
						}
					}
					back();
					if ((did = getSym())->type == SEMICN && judge == TRUE) {
						back();
						return TRUE;
					}
				}
			}
		}
	}
	else if (now->type == CHARTK) {
		id_type = CONSTCHAR;
		if ((did = getSym())->type == IDENFR) {
			strcpy(id_name, did->string);
			if ((did = getSym())->type == ASSIGN) {
				if ((did = getSym())->type == CHARCON) {
					addID();
					while ((did = getSym())->type == COMMA) {
						if ((did = getSym())->type == IDENFR) {
							strcpy(id_name, did->string);
							if ((did = getSym())->type == ASSIGN) {
								if ((did = getSym())->type == CHARCON) {
									addID();
									judge = TRUE;
								}
							}
						}
					}
					back();
					if ((did = getSym())->type == SEMICN && judge == TRUE) {
						back();
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}

//<常量说明>
int conSpe() {
	struct word* now;
	int judge = FALSE;
	while ((now = getSym())->type == CONSTTK) {
		if (conDef() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				judge = TRUE;
			}
			else {//缺少分号；
				judge = FALSE;
				error();
			}
		}
		else {
			judge = FALSE;
			error();
		}
	}
	back();
	if (judge == TRUE) {
		return TRUE;
	}
	else
		return FALSE;
}

//<因子>
int factor() {
	struct word* did;
	struct word* now = getSym();
	int judge = FALSE;
	if (now->type == IDENFR) {

		if ((did = getSym())->type == LBRACK) {

			if (expre() == TRUE) {
				if ((did = getSym())->type == RBRACK) {
					judge = TRUE;
				}
				else judge = FALSE;
			}
			else judge = FALSE;
		}
		else {
			if (did->type == LPARENT) {
				back();
				back();
				if (reFuncState() == TRUE) {
					judge = TRUE;
				}
			}
			else {
				back();
				back();
				judge = TRUE;
			}
		}
	}
	else if (now->type == LPARENT) {
		if (expre() == TRUE) {
			if ((did = getSym())->type == RPARENT) {
				judge = TRUE;
			}
			else judge = FALSE;
		}
		else judge = FALSE;
	}
	else {
		back();
		if (integer() == TRUE) {
			judge = TRUE;
		}
		else if ((now = getSym())->type == CHARCON) {
			judge = TRUE;
		}
		else {
			back();
			judge = FALSE;
		}
	}
	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<项>
int item() {
	struct word* did;
	int judge = FALSE;
	if (factor() == TRUE) {
		judge = TRUE;
		while ((did = getSym())->type == MULT || did->type == DIV) {
			if (factor() == TRUE) {
				judge = TRUE;
			}
		}
		back();
	}
	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<表达式>
int expre() {
	struct word* now = getSym();
	int judge = FALSE;
	if (now->type == PLUS || now->type == MINU) {
		judge = FALSE;
	}
	else back();

	if (item() == TRUE) {
		judge = TRUE;
		while ((now = getSym())->type == PLUS || now->type == MINU) {
			judge = FALSE;
			if (item() == TRUE) {
				judge = TRUE;
			}
			else judge = FALSE;
		}
		back();
	}
	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<读语句>
int readState() {
	struct word* now, * did;
	int flag = TRUE;
	if ((now = getSym())->type == SCANFTK) {
		if ((did = getSym())->type == LPARENT) {
			if ((did = getSym())->type == IDENFR) {
				while ((did = getSym())->type == COMMA) {
					if ((did = getSym())->type == IDENFR) {
						flag = TRUE;
					}
					else flag = FALSE;
				}
				back();
				if ((did = getSym())->type == RPARENT && flag == TRUE) {
					return TRUE;
				}
			}
		}
	}
	else back();
	return FALSE;
}

//<写语句>
int writeState() {
	struct word* now, * did;
	int flag = FALSE;
	if ((now = getSym())->type == PRINTFTK) {
		if ((did = getSym())->type == LPARENT) {
			if ((did = getSym())->type == STRCON) {
				if ((did = getSym())->type == COMMA) {
					if (expre() == TRUE) {
						if ((did = getSym())->type == RPARENT) {
							flag = TRUE;
						}
					}
				}
				else if (did->type == RPARENT) {
					flag = TRUE;
				}
			}
			else {
				back();
				if (expre() == TRUE) {
					if ((did = getSym())->type == RPARENT) {
						flag = TRUE;
					}
				}
			}

		}
	}
	else back();

	if (flag == TRUE) {
		return TRUE;
	}
	return FALSE;
}

//<返回语句>
int returnState() {
	int flag = FALSE;
	struct word* now, * did;
	if ((now = getSym())->type == RETURNTK) {
		flag = TRUE;
		if ((did = getSym())->type == LPARENT) {
			flag = FALSE;
			if (expre() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					flag = TRUE;
				}
			}
		}
		else back();
	}
	else back();

	if (flag == TRUE) {
		return TRUE;
	}
	return FALSE;
}

//<赋值语句>
int assignState() {
	struct word* now = getSym();
	struct word* did;
	int flag = FALSE;
	if (now->type == IDENFR) {
		if ((did = getSym())->type == ASSIGN) {
			if (expre() == TRUE) {
				flag = TRUE;
			}
		}
		else if (did->type == LBRACK) {
			if (expre() == TRUE) {
				if ((did = getSym())->type == RBRACK) {
					if ((did = getSym())->type == ASSIGN) {
						if (expre() == TRUE) {
							flag = TRUE;
						}
					}
				}
			}
		}
		else flag = FALSE;
	}
	else back();
	if (flag == TRUE) {
		return TRUE;
	}
	else return FALSE;

}

//<条件>
int condition() {
	if (expre() == TRUE) {
		if (relationOp() == TRUE) {
			if (expre() == TRUE) {
				return TRUE;
			}
			else {
				error();
			}
		}
		else {
			return TRUE;
		}
	}
	return FALSE;
}

//<条件语句>
int conditionState() {
	struct word* now = getSym();
	struct word* did;
	int flag = FALSE;
	if (now->type == IFTK) {
		if ((did = getSym())->type == LPARENT) {
			if (condition() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					if (state() == TRUE) {
						if ((did = getSym())->type == ELSETK) {
							if (state() == TRUE) {
								flag = TRUE;
							}
							else {
								error();
								flag = FALSE;
							}
						}
						else {
							back();
							flag = TRUE;
						}
					}
					else error();
				}
				else error();
			}
			else error();
		}
		else error();
	}
	else back();
	if (flag == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<步长>
int step() {
	if (unsignedInteger() == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<循环语句>
int loopState() {
	struct word* now = getSym();
	struct word* did;
	int flag = FALSE;
	if (now->type == WHILETK) {
		if ((did = getSym())->type == LPARENT) {
			if (condition() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					if (state() == TRUE) {
						flag = TRUE;
					}
				}
			}
		}
	}
	else if (now->type == DOTK) {
		if (state() == TRUE) {
			if ((did = getSym())->type == WHILETK) {
				if ((did = getSym())->type == LPARENT) {
					if (condition() == TRUE) {
						if ((did = getSym())->type == RPARENT) {
							flag = TRUE;
						}
					}
				}
			}
		}
	}
	else if (now->type == FORTK) {
		if ((did = getSym())->type == LPARENT) {
			if ((did = getSym())->type == IDENFR) {
				if ((did = getSym())->type == ASSIGN) {
					if (expre() == TRUE) {
						if ((did = getSym())->type == SEMICN) {
							if (condition() == TRUE) {
								if ((did = getSym())->type == SEMICN) {
									if ((did = getSym())->type == IDENFR) {
										if ((did = getSym())->type == ASSIGN) {
											if ((did = getSym())->type == IDENFR) {
												if ((did = getSym())->type == PLUS || did->type == MINU) {
													if (step() == TRUE) {
														if ((did = getSym())->type == RPARENT) {
															if (state() == TRUE) {
																flag = TRUE;
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	else back();

	if (flag == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<值参数表>
int valueParaTable() {
	struct word* did1;
	int flag = FALSE;
	if (expre() == TRUE) {
		flag = TRUE;
		while ((did1 = getSym())->type == COMMA) {
			flag = FALSE;
			if (expre() == TRUE) {
				flag = TRUE;
			}
		}
		back();
	}
	else {
		flag = TRUE;
	}
	if (flag == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<有返回值函数调用语句>
int reFuncState() {
	struct word* did1, * did2, * did3;
	if ((did1 = getSym())->type == IDENFR) {
		int i, judge = FALSE;

		for (i = 0; i < index1; i++) {
			if (strcmp(did1->string, re[i]) == 0) {
				judge = TRUE;
				break;
			}
		}
		if (judge == TRUE) {
			if ((did2 = getSym())->type == LPARENT) {
				if (valueParaTable() == TRUE) {
					if ((did3 = getSym())->type == RPARENT) {
						return TRUE;
					}
				}
			}
			else {
				back();
				back();
			}
		}
		else back();
	}
	else back();
	return FALSE;
}

//<无返回值函数调用语句>
int nonFuncState() {
	struct word* did1, * did2, * did3;
	if ((did1 = getSym())->type == IDENFR) {
		int i, judge = FALSE;

		for (i = 0; i < index2; i++) {
			if (strcmp(did1->string, non[i]) == 0) {
				judge = TRUE;
				break;
			}
		}
		if (judge == TRUE) {
			if ((did2 = getSym())->type == LPARENT) {
				if (valueParaTable() == TRUE) {
					if ((did3 = getSym())->type == RPARENT) {
						return TRUE;
					}
				}
			}
			else {
				back();
				back();
			}
		}
		else back();
	}
	else back();
	return FALSE;
}

//<语句>
int state() {
	struct word* now;
	struct word* did;
	int judge = FALSE;

	if (conditionState() == TRUE) {
		judge = TRUE;
	}
	else if (loopState() == TRUE) {
		judge = TRUE;
	}
	else if ((now = getSym())->type == LBRACE) {
		if (stateColumn() == TRUE) {
			if ((did = getSym())->type == RBRACE) {
				judge = TRUE;
			}
		}
	}
	else {
		back();//回退上一个getSym
		if (reFuncState() == TRUE || nonFuncState() == TRUE || assignState() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				judge = TRUE;
			}
		}
		else if (readState() == TRUE || writeState() == TRUE || returnState() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				judge = TRUE;
			}
		}
		else if ((now = getSym())->type == SEMICN) {
			judge = TRUE;
		}
		else {
			back();
			judge = FALSE;
		}
	}

	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<语句列>
int stateColumn() {
	while (state() == TRUE);
	return TRUE;
}

//<复合语句>
int comState() {
	conSpe();//常量说明
	varSpe();//变量说明
	if (stateColumn() == TRUE) {
		return TRUE;
	}
	return FALSE;
}

//<参数表>
int paraTable() {
	struct word* did;
	int judge = FALSE;
	int type;
	if (typeIden() == TRUE) {
		type = getSym()->type;
		switch (type)
		{
		case INTTK:
			func_arrValue[func_index] = INT;
			func_index++;
			break;
		case CHARTK:
			func_arrValue[func_index] = CHAR;
			func_index++;
			break;
		default:
			break;
		}
		if ((did = getSym())->type == IDENFR) {
			judge = TRUE;
			while ((did = getSym())->type == COMMA) {
				if (typeIden() == TRUE) {
					type = getSym()->type;
					switch (type)
					{
					case INTTK:
						func_arrValue[func_index] = INT;
						func_index++;
						break;
					case CHARTK:
						func_arrValue[func_index] = CHAR;
						func_index++;
						break;
					default:
						break;
					}

					if ((did = getSym())->type == IDENFR) {
						judge = TRUE;
					}
					else {
						error();
						judge = FALSE;
						break;
					}
				}
				else {
					error();
					judge = FALSE;
					break;
				}
			}
			back();
		}
		else {
			error();
			judge = FALSE;
		}
	}
	else judge = TRUE;//参数表为<空>

	if (judge == TRUE) {
		return TRUE;
	}
	else return FALSE;
}

//<有返回值函数定义>
int reFunc() {
	struct word* did;
	if (headState() == TRUE) {
		if ((did = getSym())->type == LPARENT) {
			if (paraTable() == TRUE) {
				/* addFunction() */
				struct func* function;
				function = (struct func*)malloc(sizeof(struct func));
				strcpy(function->name, func_name);
				function->type = func_type;
				function->valueNum = func_index + 1;
				memcpy(function->arrValue, func_arrValue, sizeof(func_arrValue));
				table2[table2_index] = function;
				table2_index++;
				funcInitial();

				if ((did = getSym())->type == RPARENT) {
					if ((did = getSym())->type == LBRACE) {
						if (comState() == TRUE) {
							if ((did = getSym())->type == RBRACE) {
								return TRUE;
							}
							else error();//缺少}
						}
						else error();
					}
					else error();//缺少{
				}
				else error();//缺少)
			}
			else error();
		}
		else error();//缺少(
	}
	return FALSE;
}

//<无返回值函数定义>
int nonFunc() {
	struct word* did;
	struct word* now = getSym();
	if (now->type == VOIDTK) {
		func_type = NONE;
		if ((did = getSym())->type == IDENFR) {
			strcpy(func_name, did->string);
			strcpy(non[index2], did->string);
			index2++;
			if ((did = getSym())->type == LPARENT) {
				if (paraTable() == TRUE) {
					/* addFunction() */
					struct func* function;
					function = (struct func*)malloc(sizeof(struct func));
					strcpy(function->name, func_name);
					function->type = func_type;
					function->valueNum = func_index + 1;
					memcpy(function->arrValue, func_arrValue, sizeof(func_arrValue));
					table2[table2_index] = function;
					table2_index++;
					funcInitial();

					if ((did = getSym())->type == RPARENT) {
						if ((did = getSym())->type == LBRACE) {
							if (comState() == TRUE) {
								if ((did = getSym())->type == RBRACE) {
									return TRUE;
								}
								else error();//缺少}
							}
							else error();
						}
						else error();//缺少{
					}
					else error();//缺少）
				}
				else error();
			}
			else error();//缺少（
		}
		else {
			back();
			back();
		}
	}
	else back();
	return FALSE;
}

//<主函数>
int mainFunc() {
	struct word* did;
	struct word* nowWord = getSym();
	if (nowWord->type == VOIDTK) {
		if ((did = getSym())->type == MAINTK) {
			if ((did = getSym())->type == LPARENT) {
				if ((did = getSym())->type == RPARENT) {
					if ((did = getSym())->type == LBRACE) {
						if (comState() == TRUE) {
							if ((did = getSym())->type == RBRACE) {
								return TRUE;
							}
							else error();//缺少}
						}
						else error();//无<复合语句>
					}
					else error();//无{
				}
				else error();//无)
			}
			else error();//无(
		}
		else error();//无main
	}
	else {
		back();
	}
	return FALSE;

}

//<程序>
void program() {
	this = head;
	int flag = FALSE;
	conSpe();
	varSpe();
	while (reFunc() == TRUE || nonFunc() == TRUE);
	if (mainFunc() == TRUE) {
		flag = TRUE;
	}
	if (flag == TRUE) {
	}
}

void initialLink() {
	head = (struct word*)malloc(sizeof(struct word));
	head->before = NULL;
	pre = head;
}

void addType(int type) {
	struct word* one;
	one = (struct word*)malloc(sizeof(struct word));
	one->type = type;
	one->colNum = numCol;
	strcpy(one->string, text[type]);

	one->before = pre;
	pre->next = one;
	pre = one;
}

void addType2(int type, char* str) {
	struct word* one;
	one = (struct word*) malloc(sizeof(struct word));
	one->type = type;
	one->colNum = numCol;
	strcpy(one->string, str);
	if (type == CHARCON) {
		char c = str[0];
		if ((c >= 47 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 42 || c == 43 || c == 45 || c == 95) {

		}
	}

	one->before = pre;
	pre->next = one;
	pre = one;
}

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

void testKeyWords(char* str, int i) {
	if (str[0] == 'c' && str[1] == 'o' && str[2] == 'n' && str[3] == 's' && str[4] == 't' && i == 5) {
		addType(CONSTTK);
	}
	else if (str[0] == 'i' && str[1] == 'n' && str[2] == 't' && i == 3) {
		addType(INTTK);
	}
	else if (str[0] == 'c' && str[1] == 'h' && str[2] == 'a' && str[3] == 'r' && i == 4) {
		addType(CHARTK);
	}
	else if (str[0] == 'v' && str[1] == 'o' && str[2] == 'i' && str[3] == 'd' && i == 4) {
		addType(VOIDTK);
	}
	else if (str[0] == 'm' && str[1] == 'a' && str[2] == 'i' && str[3] == 'n' && i == 4) {
		addType(MAINTK);
	}
	else if (str[0] == 'i' && str[1] == 'f' && i == 2) {
		addType(IFTK);
	}
	else if (str[0] == 'e' && str[1] == 'l' && str[2] == 's' && str[3] == 'e' && i == 4) {
		addType(ELSETK);
	}
	else if (str[0] == 'd' && str[1] == 'o' && i == 2) {
		addType(DOTK);
	}
	else if (str[0] == 'w' && str[1] == 'h' && str[2] == 'i' && str[3] == 'l' && str[4] == 'e' && i == 5) {
		addType(WHILETK);
	}
	else if (str[0] == 'f' && str[1] == 'o' && str[2] == 'r' && i == 3) {
		addType(FORTK);
	}
	else if (str[0] == 's' && str[1] == 'c' && str[2] == 'a' && str[3] == 'n' && str[4] == 'f' && i == 5) {
		addType(SCANFTK);
	}
	else if (str[0] == 'p' && str[1] == 'r' && str[2] == 'i' && str[3] == 'n' && str[4] == 't' && str[5] == 'f' && i == 6) {
		addType(PRINTFTK);
	}
	else if (str[0] == 'r' && str[1] == 'e' && str[2] == 't' && str[3] == 'u' && str[4] == 'r' && str[5] == 'n' && i == 6) {
		addType(RETURNTK);
	}
	else {
		if (i > 0) {
			if (testINTCON(str, i) == 1) {
				addType2(INTCON, str);
			}
			else {
				addType2(IDENFR, str);
			}
		}
	}
}

void test(char* str, int i) {
	testKeyWords(str, i);
	memset(str, '\0', sizeof(char) * 1024);
}

int testExit(char ch) {

	if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == ';'
		|| ch == ',' || ch == '(' || ch == ')' || ch == '[' || ch == ']'
		|| ch == '{' || ch == '}') {
		return 1;
	}
	return 0;
}

void testSingleSymbol(char ch) {
	switch (ch)
	{
	case '+':
		addType(PLUS);
		break;
	case '-':
		addType(MINU);
		break;
	case '*':
		addType(MULT);
		break;
	case '/':
		addType(DIV);
		break;
	case ';':
		addType(SEMICN);
		break;
	case ',':
		addType(COMMA);
		break;
	case '(':
		addType(LPARENT);
		break;
	case ')':
		addType(RPARENT);
		break;
	case '[':
		addType(LBRACK);
		break;
	case ']':
		addType(RBRACK);
		break;
	case '{':
		addType(LBRACE);
		break;
	case '}':
		addType(RBRACE);
		break;
	default:
		break;
	}
}

void readChar() {
	char ch1;
	char str[2] = { '\0' };
	if ((ch1 = fgetc(fp1)) != EOF) {
		str[0] = ch1;
		addType2(CHARCON, str);
	}
	if ((ch1 = fgetc(fp1)) != EOF) {//读字符常量的第二个'
		return;
	}
}

void readString() {
	char ch1;
	char str[1024] = { '\0' };
	int i = 0;
	while ((ch1 = fgetc(fp1)) != EOF) {
		if (ch1 == '\"') break;
		str[i] = ch1;
		i++;
	}
	addType2(STRCON, str);
}

int testDoubleSymbol(char ch, char* arr, int i) {
	char ch1 = '\0';
	if (i > 0) {
		test(arr, i);
		i = 0;
	}
	if ((ch1 = fgetc(fp1)) != EOF) {
		if (ch1 == '=') {
			switch (ch)
			{
			case '>':
				addType(GEQ);
				break;
			case '=':
				addType(EQL);
				break;
			case '<':
				addType(LEQ);
				break;
			case '!':
				addType(NEQ);
				break;
			default:
				break;
			}
		}
		else {
			switch (ch)
			{
			case '>':
				addType(GRE);
				break;
			case '=':
				addType(ASSIGN);
				break;
			case '<':
				addType(LSS);
				break;
			default:
				break;
			}

			if (isspace(ch1) || testExit(ch1) == 1) {
				i = 0;
				if (testExit(ch1) == 1) testSingleSymbol(ch1);
			}
			else if (ch1 == '\'') {
				readChar();
			}
			else if (ch1 == '\"') {
				readString();
			}
			else {
				arr[i] = ch1;
				i++;
			}
		}
	}
	return i;
}

void readWord() {
	char ch = '*';
	char string[1024] = { '\0' };
	int i = 0;
	while ((ch = fgetc(fp1)) != EOF) {
		if (isspace(ch) || testExit(ch) == 1) {
			test(string, i);
			i = 0;
			if (testExit(ch) == 1) testSingleSymbol(ch);
			if (ch == '\n') numCol++;
		}
		else if (ch == '\'') {
			readChar();
		}
		else if (ch == '"') {
			readString();
		}
		else if (ch == '=' || ch == '<' || ch == '>' || ch == '!') {
			i = testDoubleSymbol(ch, string, i);
		}
		else {
			string[i] = ch;
			i++;
		}
	}

	pre->next = NULL;
	head = head->next;
	head->before = NULL;
}

int main() {

	if ((fp1 = fopen("testfile.txt", "r")) == NULL) {
		printf("File cannot be opened\n");
	}
	if ((fp2 = fopen("error.txt", "w+")) == NULL) {
		printf("File cannot be opened\n");
	}
	initialLink();
	readWord();

	program();

	fclose(fp1);
	fclose(fp2);

	return 0;

}