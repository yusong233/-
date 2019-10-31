#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <memory.h>
#include <string.h>

FILE* fp1;
FILE* fp2;

char re[100][128] = { '\0' };

int index1 = 0;

int index2 = 0;

char non[100][128] = { '\0' };

enum type
{
	IDENFR = 0, INTCON, CHARCON, STRCON, CONSTTK, INTTK, CHARTK, VOIDTK, MAINTK,
	IFTK, ELSETK, DOTK, WHILETK, FORTK, SCANFTK, PRINTFTK, RETURNTK, PLUS,
	MINU, MULT, DIV, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN, SEMICN, COMMA, LPARENT,
	RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
}nowType;

enum boo {
	FALSE = 0, TRUE
}boolean;

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
	struct word* next;
	struct word* before;
};

struct word* head;
struct word* pre;
struct word* this;

void error() {

}

int reFuncState();
int state();
int expre();
int stateColumn();

void printR(struct word* now) {
	fprintf(fp2, "%s %s\n", name[now->type], now->string);
}

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
		printR(now);
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
		printR(nowWord);
		fprintf(fp2, "<无符号整数>\n");
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
		printR(nowWord);
		if (unsignedInteger() == TRUE) {
			fprintf(fp2, "<整数>\n");
			return TRUE;
		}
	}
	else {
		back();
		if (unsignedInteger() == TRUE) {
			fprintf(fp2, "<整数>\n");
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
		if ((did = getSym())->type == IDENFR) {
			printR(now);
			printR(did);
			strcpy(re[index1], did->string);
			index1++;
			fprintf(fp2, "<声明头部>\n");
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
		if ((did = getSym())->type == IDENFR) {
			printR(now);
			printR(did);
			strcpy(re[index1], did->string);
			index1++;
			fprintf(fp2, "<声明头部>\n");
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
	if (typeIden() == TRUE) {
		did = getSym();
		if ((did1 = getSym())->type == IDENFR) {
			if ((did2 = getSym())->type == SEMICN || did2->type == COMMA || did2->type == LBRACK) {
				judge = TRUE;
				printR(did);
				printR(did1);
			}

			if (did2->type == LBRACK) {
				printR(did2);
				if (unsignedInteger() == TRUE) {
					back();
					if (strcmp((did = getSym())->string, "0") >= 0) {
						//这个无符号整数已经输出
						if ((did = getSym())->type == RBRACK) {
							printR(did);
							judge = TRUE;
							while ((did = getSym())->type == COMMA) {
								printR(did);
								if ((did = getSym())->type == IDENFR) {
									printR(did);
									if ((did = getSym())->type == LBRACK) {
										printR(did);
										if (unsignedInteger() == TRUE) {
											back();
											if (strcmp((did = getSym())->string, "0") >= 0) {
												if ((did = getSym())->type == RBRACK) {
													printR(did);
													judge = TRUE;
												}
												else {
													judge = FALSE;
													error();
													break;
												}
											}
											else {
												judge = FALSE;
												error();
												break;
											}
										}
										else {
											judge = FALSE;
											error();
											break;
										}
									}
									else back();
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
							error();
						}
					}
					else {
						judge = FALSE;
						error();
					}
				}
				else {
					judge = FALSE;
					error();
				}
			}
			else {
				back();
				if (judge == TRUE) {
					while ((did = getSym())->type == COMMA) {
						printR(did);
						if ((did = getSym())->type == IDENFR) {
							printR(did);
							judge = TRUE;
							if ((did = getSym())->type == LBRACK) {
								printR(did);
								if (unsignedInteger() == TRUE) {
									back();
									if (strcmp((did = getSym())->string, "0") >= 0) {
										if ((did = getSym())->type == RBRACK) {
											printR(did);
											judge = TRUE;
										}
										else {
											judge = FALSE;
											error();
											break;
										}
									}
									else {
										judge = FALSE;
										error();
										break;
									}
								}
								else {
									judge = FALSE;
									error();
									break;
								}
							}
							else {
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
		fprintf(fp2, "<变量定义>\n");
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
			printR(now);
			judge = TRUE;
		}
	}
	if (judge == TRUE) {
		fprintf(fp2, "<变量说明>\n");
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
		printR(now);
		if ((did = getSym())->type == IDENFR) {
			printR(did);
			if ((did = getSym())->type == ASSIGN) {
				printR(did);
				if (integer() == TRUE) {
					while ((did = getSym())->type == COMMA) {
						printR(did);
						if ((did = getSym())->type == IDENFR) {
							printR(did);
							if ((did = getSym())->type == ASSIGN) {
								printR(did);
								if (integer() == TRUE) {
									judge = TRUE;
								}
								else error();
							}
							else error();
						}
						else error();
					}
					back();
					if ((did = getSym())->type == SEMICN && judge == TRUE) {
						fprintf(fp2, "<常量定义>\n");
						back();
						return TRUE;
					}
					else error();
				}
				else error();
			}
			else error();
		}
		else error();
	}
	else if (now->type == CHARTK) {
		printR(now);
		if ((did = getSym())->type == IDENFR) {
			printR(did);
			if ((did = getSym())->type == ASSIGN) {
				printR(did);
				if ((did = getSym())->type == CHARCON) {
					printR(did);
					while ((did = getSym())->type == COMMA) {
						printR(did);
						if ((did = getSym())->type == IDENFR) {
							printR(did);
							if ((did = getSym())->type == ASSIGN) {
								printR(did);
								if ((did = getSym())->type == CHARCON) {
									printR(did);
									judge = TRUE;
								}
								else error();
							}
							else error();
						}
						else error();
					}
					back();
					if ((did = getSym())->type == SEMICN && judge == TRUE) {
						fprintf(fp2, "<常量定义>\n");
						back();
						return TRUE;
					}
					else error();
				}
				else error();
			}
			else error();
		}
		else error();
	}
	else error();
	return FALSE;
}

//<常量说明>
int conSpe() {
	struct word* now;
	int judge = FALSE;
	while ((now = getSym())->type == CONSTTK) {
		printR(now);
		if (conDef() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				printR(now);
				judge = TRUE;
			}
			else {
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
		fprintf(fp2, "<常量说明>\n");
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
			printR(now);
			printR(did);

			if (expre() == TRUE) {
				if ((did = getSym())->type == RBRACK) {
					printR(did);
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
				printR(getSym());
				judge = TRUE;
			}
		}
	}
	else if (now->type == LPARENT) {
		printR(now);
		if (expre() == TRUE) {
			if ((did = getSym())->type == RPARENT) {
				printR(did);
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
			printR(now);
			judge = TRUE;
		}
		else {
			back();
			judge = FALSE;
		}
	}
	if (judge == TRUE) {
		fprintf(fp2, "<因子>\n");
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
			printR(did);
			if (factor() == TRUE) {
				judge = TRUE;
			}
		}
		back();
	}
	if (judge == TRUE) {
		fprintf(fp2, "<项>\n");
		return TRUE;
	}
	else return FALSE;
}

//<表达式>
int expre() {
	struct word* now = getSym();
	int judge = FALSE;
	if (now->type == PLUS || now->type == MINU) {
		printR(now);
		judge = FALSE;
	}
	else back();

	if (item() == TRUE) {
		judge = TRUE;
		while ((now = getSym())->type == PLUS || now->type == MINU) {
			printR(now);
			judge = FALSE;
			if (item() == TRUE) {
				judge = TRUE;
			}
			else judge = FALSE;
		}
		back();
	}
	if (judge == TRUE) {
		fprintf(fp2, "<表达式>\n");
		return TRUE;
	}
	else return FALSE;
}

//<读语句>
int readState() {
	struct word* now, * did;
	int flag = TRUE;
	if ((now = getSym())->type == SCANFTK) {
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if ((did = getSym())->type == IDENFR) {
				printR(did);
				while ((did = getSym())->type == COMMA) {
					printR(did);
					if ((did = getSym())->type == IDENFR) {
						printR(did);
						flag = TRUE;
					}
					else flag = FALSE;
				}
				back();
				if ((did = getSym())->type == RPARENT && flag == TRUE) {
					printR(did);
					fprintf(fp2, "<读语句>\n");
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
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if ((did = getSym())->type == STRCON) {
				printR(did);
				fprintf(fp2, "<字符串>\n");
				if ((did = getSym())->type == COMMA) {
					printR(did);
					if (expre() == TRUE) {
						if ((did = getSym())->type == RPARENT) {
							printR(did);
							flag = TRUE;
						}
					}
				}
				else if (did->type == RPARENT) {
					printR(did);
					flag = TRUE;
				}
			}
			else {
				back();
				if (expre() == TRUE) {
					if ((did = getSym())->type == RPARENT) {
						printR(did);
						flag = TRUE;
					}
				}
			}

		}
	}
	else back();

	if (flag == TRUE) {
		fprintf(fp2, "<写语句>\n");
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
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			flag = FALSE;
			printR(did);
			if (expre() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					flag = TRUE;
					printR(did);
				}
			}
		}
		else back();
	}
	else back();

	if (flag == TRUE) {
		fprintf(fp2, "<返回语句>\n");
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
		printR(now);
		if ((did = getSym())->type == ASSIGN) {
			printR(did);
			if (expre() == TRUE) {
				flag = TRUE;
			}
		}
		else if (did->type == LBRACK) {
			printR(did);
			if (expre() == TRUE) {
				if ((did = getSym())->type == RBRACK) {
					printR(did);
					if ((did = getSym())->type == ASSIGN) {
						printR(did);
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
		fprintf(fp2, "<赋值语句>\n");
		return TRUE;
	}
	else return FALSE;

}

//<条件>
int condition() {
	if (expre() == TRUE) {
		if (relationOp() == TRUE) {
			if (expre() == TRUE) {
				fprintf(fp2, "<条件>\n");
				return TRUE;
			}
			else {
				error();
			}
		}
		else {
			fprintf(fp2, "<条件>\n");
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
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if (condition() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					printR(did);
					if (state() == TRUE) {
						if ((did = getSym())->type == ELSETK) {
							printR(did);
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
		fprintf(fp2, "<条件语句>\n");
		return TRUE;
	}
	else return FALSE;
}

//<步长>
int step() {
	if (unsignedInteger() == TRUE) {
		fprintf(fp2, "<步长>\n");
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
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if (condition() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					printR(did);
					if (state() == TRUE) {
						flag = TRUE;
					}
				}
			}
		}
	}
	else if (now->type == DOTK) {
		printR(now);
		if (state() == TRUE) {
			if ((did = getSym())->type == WHILETK) {
				printR(did);
				if ((did = getSym())->type == LPARENT) {
					printR(did);
					if (condition() == TRUE) {
						if ((did = getSym())->type == RPARENT) {
							printR(did);
							flag = TRUE;
						}
					}
				}
			}
		}
	}
	else if (now->type == FORTK) {
		printR(now);
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if ((did = getSym())->type == IDENFR) {
				printR(did);
				if ((did = getSym())->type == ASSIGN) {
					printR(did);
					if (expre() == TRUE) {
						if ((did = getSym())->type == SEMICN) {
							printR(did);
							if (condition() == TRUE) {
								if ((did = getSym())->type == SEMICN) {
									printR(did);
									if ((did = getSym())->type == IDENFR) {
										printR(did);
										if ((did = getSym())->type == ASSIGN) {
											printR(did);
											if ((did = getSym())->type == IDENFR) {
												printR(did);
												if ((did = getSym())->type == PLUS || did->type == MINU) {
													printR(did);
													if (step() == TRUE) {
														if ((did = getSym())->type == RPARENT) {
															printR(did);
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
		fprintf(fp2, "<循环语句>\n");
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
			printR(did1);
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
		fprintf(fp2, "<值参数表>\n");
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
				printR(did1);
				printR(did2);
				if (valueParaTable() == TRUE) {
					if ((did3 = getSym())->type == RPARENT) {
						printR(did3);
						fprintf(fp2, "<有返回值函数调用语句>\n");
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
				printR(did1);
				printR(did2);
				if (valueParaTable() == TRUE) {
					if ((did3 = getSym())->type == RPARENT) {
						printR(did3);
						fprintf(fp2, "<无返回值函数调用语句>\n");
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
		printR(now);
		if (stateColumn() == TRUE) {
			if ((did = getSym())->type == RBRACE) {
				printR(did);
				judge = TRUE;
			}
		}
	}
	else {
		back();//回退上一个getSym
		if (reFuncState() == TRUE || nonFuncState() == TRUE || assignState() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				printR(now);
				judge = TRUE;
			}
		}
		else if (readState() == TRUE || writeState() == TRUE || returnState() == TRUE) {
			if ((now = getSym())->type == SEMICN) {
				printR(now);
				judge = TRUE;
			}
		}
		else if ((now = getSym())->type == SEMICN) {
			printR(now);
			judge = TRUE;
		}
		else {
			back();
			judge = FALSE;
		}
	}

	if (judge == TRUE) {
		fprintf(fp2, "<语句>\n");
		return TRUE;
	}
	else return FALSE;
}

//<语句列>
int stateColumn() {
	while (state() == TRUE);
	fprintf(fp2, "<语句列>\n");
	return TRUE;
}

//<复合语句>
int comState() {
	conSpe();//常量说明
	varSpe();//变量说明
	if (stateColumn() == TRUE) {
		fprintf(fp2, "<复合语句>\n");
		return TRUE;
	}
	return FALSE;
}

//<参数表>
int paraTable() {
	struct word* did;
	int judge = FALSE;
	if (typeIden() == TRUE) {
		printR(getSym());//输出类型标识符
		if ((did = getSym())->type == IDENFR) {
			printR(did);
			judge = TRUE;
			while ((did = getSym())->type == COMMA) {
				printR(did);
				if (typeIden() == TRUE) {
					printR(getSym());//输出类型标识符
					if ((did = getSym())->type == IDENFR) {
						printR(did);
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
		fprintf(fp2, "<参数表>\n");
		return TRUE;
	}
	else return FALSE;
}

//<有返回值函数定义>
int reFunc() {
	struct word* did;
	if (headState() == TRUE) {
		if ((did = getSym())->type == LPARENT) {
			printR(did);
			if (paraTable() == TRUE) {
				if ((did = getSym())->type == RPARENT) {
					printR(did);
					if ((did = getSym())->type == LBRACE) {
						printR(did);
						if (comState() == TRUE) {
							if ((did = getSym())->type == RBRACE) {
								printR(did);
								fprintf(fp2, "<有返回值函数定义>\n");
								return TRUE;
							}
							else error();
						}
						else error();
					}
					else error();
				}
				else error();
			}
			else error();
		}
		else error();
	}
	return FALSE;
}

//<无返回值函数定义>
int nonFunc() {
	struct word* did;
	struct word* now = getSym();
	if (now->type == VOIDTK) {
		if ((did = getSym())->type == IDENFR) {
			printR(now);
			printR(did);
			strcpy(non[index2], did->string);
			index2++;
			if ((did = getSym())->type == LPARENT) {
				printR(did);
				if (paraTable() == TRUE) {
					if ((did = getSym())->type == RPARENT) {
						printR(did);
						if ((did = getSym())->type == LBRACE) {
							printR(did);
							if (comState() == TRUE) {
								if ((did = getSym())->type == RBRACE) {
									printR(did);
									fprintf(fp2, "<无返回值函数定义>\n");
									return TRUE;
								}
								else error();
							}
							else error();
						}
						else error();
					}
					else error();
				}
				else error();
			}
			else error();
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
			printR(nowWord);
			printR(did);
			if ((did = getSym())->type == LPARENT) {
				printR(did);
				if ((did = getSym())->type == RPARENT) {
					printR(did);
					if ((did = getSym())->type == LBRACE) {
						printR(did);
						if (comState() == TRUE) {
							if ((did = getSym())->type == RBRACE) {
								printR(did);
								fprintf(fp2, "<主函数>\n");
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
		fprintf(fp2, "<程序>\n");
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
	strcpy(one->string, text[type]);

	one->before = pre;
	pre->next = one;
	pre = one;
}

void addType2(int type, char* str) {
	struct word* one;
	one = (struct word*) malloc(sizeof(struct word));
	one->type = type;
	strcpy(one->string, str);

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

void print() {
	struct word* now;
	now = head;
	while (now != NULL) {
		printf("%s %s\n", name[now->type], now->string);
		now = now->next;
	}

}

int main() {

	if ((fp1 = fopen("testfile.txt", "r")) == NULL) {
		printf("File cannot be opened\n");
	}
	if ((fp2 = fopen("output.txt", "w+")) == NULL) {
		printf("File cannot be opened\n");
	}
	initialLink();
	readWord();
	
	program();

	fclose(fp1);
	fclose(fp2);

	return 0;

}