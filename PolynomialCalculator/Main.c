#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define MAX(x,y)  ((x)>(y)?(x):(y))
#define ABS(x)  ((x)>0? (x) : -(x))

#define MAX_DEGREE 100
#define MAX_STACK_SIZE 100

// 다항식을 표현하는 구조체 타입 polynomial
typedef struct _poly {
	int degree;  // 차수
	int coef[MAX_DEGREE + 1]; // 계수를 저장하는 배열
} polynomial;

//element: 수식의 연산자<+,-,*,(,)> 혹은 피연산자<다항식>
typedef struct {
	polynomial poly;	//피연산자(다항식)를 저장할 경우 사용됨
	char op;	//연산자를 저장할 경우 사용됨
	int	is_op;	//피연산자인지 연산자인지를 표시함
} element;

// ListNode 타입의 정의
typedef struct _ListNode {
	element data;		// 데이터 필드
	struct _ListNode *link;	// 링크 필드
} ListNode;

// List 타입의 정의
typedef struct _List {
	ListNode *head;		// 연결 리스트로 구현
} List;

// StackType의 정의
typedef struct {
	element stack[MAX_STACK_SIZE];
	int top;
}StackType;

void error_msg(char *strMsg);	//에러처리 함수

void print_element(element e);
void print_poly(polynomial P, char symbol);
void print_list(List *pList, char *strLink);	//list를 모니터에 출력
void list_add(List *pList, int pos, element item);	// pos위치에 item을 추가
void list_add_last(List *pList, element item);		// 맨끝에 item을 추가
void list_add_first(List *pList, element item);		// 맨앞에 item을 추가

polynomial poly_zero();
polynomial poly_minus(polynomial P);
polynomial poly_add(polynomial P1, polynomial P2);
polynomial poly_subtract(polynomial P1, polynomial P2);
polynomial poly_multiply(polynomial P1, polynomial P2);

void list_init(List *pList);						// 빈 list로 초기화
void element_init(element * data);	//element 초기화
int list_get_length(List *pList);					// list의 길이를 반환

void infix_to_list(List *pList, char * exp);	//1단계 : 중위수식 -> 리스트에 받음

void init(StackType * s);
int is_empty(StackType * s);
int is_full(StackType * s);
void push(StackType * s, element item);
element pop(StackType * s);
element peek(StackType *s);
int prec(element ch);
void infix_to_postfix(List * L, List * postfix);	//2단계 : 중위수식->후위수식
polynomial eval(List * postfix);	//3단계 : 후위수식 -> 계산


									///////////////////////////////////////////////////////////

									// error_msg
									//  에러메시지를 출력한 후 프로그램을 끝내버린다.
									//  parameter: char *strMsg
									//   에러메시지 strMsg를 표준에러스트림으로 출력하고
									//   exit함수를 호출하여 프로그램을 죽인다.
void error_msg(char *strMsg)
{
	fprintf(stderr, "%s", strMsg);
	exit(1);
}

//print_element
//element타입의 값을 적당한 형식으로 화면에 출력한다.
void print_element(element e)
{
	if (e.is_op)	//e가 현재 연산자이면, e.op를 출력
	{
		printf("%c", e.op);
	}
	else	//피연산자이면,e.poly를 출력
	{
		printf("[");
		print_poly(e.poly, 'X');
		printf("]");
	}
}

// print_poly 
// 다항식 P를 적당한 형식으로 화면에 출력 
// P의 미지수를 표시하기 위해 symbol을 사용함 
// parameter: polynomial P, char symbol 
// return value: void 
// usage: 예를 들어 미지수를 X로 출력하고 싶으면, 
// print_poly(P, 'X'); 와 같은 꼴로 호출 
void print_poly(polynomial P, char symbol)
{
	int i = P.degree;
	if (P.degree <= 0)
	{// 차수가 0 혹은 -1이면, 상수항만 출력한 후 리턴 
		printf("%d", P.coef[0]);
		return;
	}

	/// 최고차항을 먼저 출력 
	// 계수가 -1 혹은 1이면 부호만 출력한다. 
	if (P.coef[i] == -1)
		printf("-%c", symbol);
	else if (P.coef[i] == 1)
		printf("%c", symbol);
	else
		printf("%d%c", P.coef[i], symbol);
	if (i > 1)// 2차항 이상에서만 차수를 표시 
		printf("^%d", i);

	// 나머지 항들에 대해... 
	for (--i; i >= 0; i--)
	{
		// 계수가 0이면 출력하지 않음 
		if (P.coef[i] == 0)
			continue;

		// 계수의 부호를 먼저 출력 
		if (P.coef[i] < 0)
			printf(" - ");
		else if (P.coef[i] > 0)
			printf(" + ");
		// 계수의 절대값을 출력 
		// 단, 계수의 절대값이 1이고 상수항이 아니면 출력할필요 없다. 
		if (ABS(P.coef[i]) != 1 || i == 0)
			printf("%d", ABS(P.coef[i]));

		// 그 뒤에 "symbol^차수"의 형태를 출력 
		// 단, 1차항이면(즉, i==1이면) "^차수"는 생략하며, 
		// 상수항이면(즉, i==0이면) 출력하지 않음 
		if (i>1)
			printf("%c^%d", symbol, i);
		else if (i == 1)
			printf("%c", symbol);
	}
}

// poly_zero
//  parameter:
//  return value: 
//       다항식 p(x)=0 을 반환, 0인 다항식의 차수는 -1로 정한다.
polynomial poly_zero()
{
	polynomial r = { -1,{ 0 } };
	return r;
}

// poly_minus
//  다항식 P를 인자로 받아 -P를 리턴한다. (계수의 부호를 바꿈)
//  parameter: polynomial P
//  return value: -P  
polynomial poly_minus(polynomial P)
{
	int i = 0;

	polynomial result = poly_zero();
	result.degree = P.degree;

	for (i = 0; i <= P.degree; i++)
		result.coef[i] = -1 * P.coef[i];

	return result;
}

// poly_add
//  두 다항식의 합을 계산하여 반환
//  parameter: polynomial P1, polynomial P2
//  return value: P1+P2
polynomial poly_add(polynomial P1, polynomial P2)
{
	int i = 0, j = 0;

	polynomial result = poly_zero();
	result.degree = MAX(P1.degree, P2.degree);

	for (i = 0; i <= result.degree; i++)
		result.coef[i] = P1.coef[i] + P2.coef[i];

	while (result.coef[--i] == 0)
		result.degree--;

	return result;
}

// poly_subtract
//  두 다항식의 차를 계산하여 리턴함
//  parameter: polynomial P1, polynomial P2
//  return value: P1-P2
polynomial poly_subtract(polynomial P1, polynomial P2)
{
	polynomial result = poly_zero();
	result = poly_add(P1, poly_minus(P2));

	return result;
}

// poly_multiply
//  두 다항식의 곱을 계산하여 리턴함
//  parameter: polynomial P1, polynomial P2
//  return value: P1*P2
polynomial poly_multiply(polynomial P1, polynomial P2)
{
	polynomial result = poly_zero();
	int i, j;
	result.degree = P1.degree + P2.degree;
	if (result.degree > 100)
		error_msg("차수 100 초과!\n");
	if (P1.degree == 0 && P1.coef[0] == 0 || P2.degree == 0 && P2.coef[0] == 0)
	{
		result.degree = 0;
		result.coef[0] = 0;
		return result;
	}
	else {
		for (i = 0; i <= P1.degree; i++)
			for (j = 0; j <= P2.degree; j++)
				result.coef[i + j] += P1.coef[i] * P2.coef[j];

		return result;
	}
}

// print_list
// 리스트의 내용을 모니터에 출력한다.
// parameter: List *pList, char *strLink
// return value: 없음
// 리스트의 내용은 순서대로 화면에 출력된다.
// strLink는 항목 사이에 들어갈 문자열이다.
// element 타입인 각 항목은 print_element를 호출하여 출력한다.
// 예) print_list(pList, "-->"); 형태로 호출하면 각항목 사이에 --> 가 그려진다.
// 단, pList==NULL 일 경우, 에러메시지를 출력하고 프로그램을 끝낸다.
void print_list(List *pList, char *strLink)
{
	ListNode *cur_node = NULL;
	if (pList == NULL)
		error_msg("print_list: pList==NULL");
	cur_node = pList->head;
	if (cur_node == NULL) // 만약 빈 리스트라면...
	{
		printf("空\n");
		return;
	}
	// 하나라도 있다면...
	// 먼저 맨 앞 항목을 출력
	print_element(cur_node->data);
	cur_node = cur_node->link;
	// 그 다음의 모든 항목들에 대하여..
	while (cur_node != NULL)
	{
		printf("%s", strLink); // strLink를 출력한 후,
		print_element(cur_node->data); // 현재 노드의 데이터를 출력
		cur_node = cur_node->link; // 다음 노드로 이동
	}
	printf("\n");
}

// list_init
//  빈 list로 초기화한다. 
//  이 함수는 List 타입의 변수를 선언한 후 최초에 한번만 호출되어야 한다.
//  parameters: List *pList
//  return value: 없음
//   단, pList==NULL 인 경우 에러메시지를 출력하고 프로그램을 끝낸다.
void list_init(List *pList)
{
	if (pList == NULL)	// 입력받은 리스트가 NULL이면 프로그램 종료
		error_msg("list_init: pList == NULL");

	pList->head = NULL;
}

// list_get_length
//  리스트의 길이를 계산하여 반환
//  parameter: List *pList
//  return value: pList의 항목 개수를 계산하여 리턴
//   단, pList==NULL 인 경우 에러메시지를 출력하고 프로그램을 끝낸다.
int list_get_length(List *pList)
{
	int cnt = 0;
	ListNode * cur_node = NULL;
	if (pList == NULL)	// 입력받은 리스트가 NULL이면 프로그램 종료
		error_msg("list_get_length : pList == NULL");
	if (pList->head == NULL)
		cnt = 0;
	else
	{
		cur_node = pList->head;
		while (cur_node->link != NULL)	//마지막노드까지 찾아줌
		{
			cur_node = cur_node->link;
			cnt++;
		}
		cnt++;	//마지막노드의수까지 더해줌
	}
	return cnt;
}

// list_add
//  리스트의 pos위치에 항목 item을 삽입한다.
//  parameter: List *pList, int pos, element item
//  return value: 없음
//    pos==0이면 맨 앞에, pos==list_get_length(pList)이면 맨 뒤에 추가된다.
//    단, 삽입에 실패할 경우, (pos 값이 부적절하거나) 에러메시지를 출력하고 프로그램을 끝낸다.
void list_add(List *pList, int pos, element item)
{
	int i;
	ListNode * new_node = (ListNode *)malloc(sizeof(ListNode));	//새로운노드 동적할당
	ListNode * cur_node = NULL;
	new_node->data = item;	//노드의 data부분에 item대입
	new_node->link = NULL;
	if (list_get_length(pList)<pos || pos<0)	//pos가 범위를 벗어났을때
		error_msg("list_add: pos isn't proper value");
	if (pList == NULL)
		error_msg("list_add: pList == NULL");
	if (new_node == NULL || pList == NULL)
		error_msg("list_add:memory allocation failed");

	if (pos == 0)	//제일 앞에 삽입할경우
	{
		list_add_first(pList, item);
	}
	else if (pos == list_get_length(pList) - 1)	//제일 마지막에 삽입할경우
	{
		list_add_last(pList, item);
	}
	else	//pos번 위치에 삽입할경우
	{
		cur_node = pList->head;
		for (i = 0; i<pos - 1; i++)	//삽입할위치의 앞의노드까지 찾아줌
			cur_node = cur_node->link;
		new_node->link = cur_node->link;
		cur_node->link = new_node;
	}
}

// list_add_last
//  리스트의 맨 끝에 항목 item을 삽입한다.
//  parameter: List *pList, element item
//  return value: 없음
//    단, 삽입에 실패할 경우, 적당한 에러메시지를 출력하고 프로그램을 끝낸다.
void list_add_last(List *pList, element item)
{
	ListNode * last_node = (ListNode *)malloc(sizeof(ListNode));
	ListNode * cur_node = NULL;	//head포인터 초기화
	last_node->data = item;	//노드의data부분에 item대입
	last_node->link = NULL;
	if (pList->head == NULL)	//빈 노드일때..
	{
		pList->head = last_node;
	}
	else	//마지막 노드를 찾아줌
	{
		cur_node = pList->head;
		while (cur_node->link != NULL)
			cur_node = cur_node->link;
		cur_node->link = last_node;
	}
}

// list_add_first
//  리스트의 맨 앞에 항목 item을 삽입한다.
//  parameter: List *pList, element item
//  return value: 없음
//    단, 삽입에 실패할 경우, 적당한 에러메시지를 출력하고 프로그램을 끝낸다.
void list_add_first(List *pList, element item)
{
	ListNode * new_node = (ListNode *)malloc(sizeof(ListNode));
	new_node->data = item;
	new_node->link = NULL;
	if (pList->head == NULL)
		pList->head = new_node;
	else
	{
		new_node->link = pList->head;
		pList->head = new_node;
	}
}

//element 초기화
void element_init(element * data)
{
	data->is_op = -1;
	data->op = NULL;
	data->poly = poly_zero();
}

//1단계 : 수식을 링크드리스트로 바꿔줌
void infix_to_list(List *pList, char * exp)
{
	int a = 0, b = 0, pos;
	int i = 0, j = 0, k = 0;	//a와b를 0으로 초기화
	int len = strlen(exp);
	char ch[20], *toke = NULL;
	element item;
	for (i = 0; i <= len; i++)
	{
		//exp[b]가 연산자 혹은 문자열의 끝이면
		if (exp[b] == '+' || exp[b] == '-' || exp[b] == '*' || exp[b] == '/' || exp[b] == '(' || exp[b] == ')' || exp[b] == '\0')
		{
			element_init(&item);	//item 초기화
			if (a == b)	//피연산자가 사이에 없음
			{
				//exp[b]를 element형으로 바꿔서 노드 추가
				item.op = exp[b];
				item.is_op = 1;
				list_add_last(pList, item);
			}
			else
			{
				for (j = a; j<b; j++)	//a부터 b-1까지 문자열을 ch로 옮김
					ch[j - a] = exp[j];
				ch[j - a] = NULL;	//ch의 끝을 알려줌
				if (strchr(ch, 'X') == NULL)	//X가 없다면 pos=-1
					pos = -1;
				else	//X가 있다면 pos의 위치 저장
					pos = strchr(ch, 'X') - ch;
				if (pos == -1)	//X가 없다면..
				{
					item.poly.degree = 0;
					item.poly.coef[item.poly.degree] = atoi(ch);
				}
				else if (pos == 0)	//X의 위치가 0번째라면 제일 앞
				{
					if (ch[1] == '^')	//'^'이면 X^차수의 형식
					{
						toke = strtok(ch, "^");
						toke = strtok(NULL, "^");	//차수 부분만 자름
						if (atoi(toke) > 100)
							error_msg("차수 100 초과!\n");
						item.poly.degree = atoi(toke);	//차수 대입
						item.poly.coef[item.poly.degree] = 1;	//계수에 1대입
					}
					else	//[X]의 형식
					{
						item.poly.degree = 1;
						item.poly.coef[item.poly.degree] = 1;
					}
				}
				else if (pos == b - a - 1)	//계수X의 형식
				{
					toke = strtok(ch, "X");
					item.poly.degree = 1;
					item.poly.coef[1] = atoi(toke);
				}
				else	//[계수X^차수]의 형식
				{
					toke = strtok(ch, "X");
					k = atoi(toke);
					toke = strtok(NULL, "^");
					if (atoi(toke) > 100)	//차수가 100이 초과할경우 오류 출력후 종료
						error_msg("degree is 100 over!");
					item.poly.degree = atoi(toke);
					item.poly.coef[item.poly.degree] = k;
				}
				item.is_op = 0;	//피연산자이므로 0대입
				list_add_last(pList, item);	//다항식 리스트 뒤에 추가

				element_init(&item);	//item 초기화
				item.op = exp[b];	//exp[b]를 item의 연산자에 대입
				item.is_op = 1;	//연산자이므로 1
				list_add_last(pList, item);	//exp[b]를 리스트 뒤에 추가
			}
			b++;
			a = b;
		}
		else	//연산자가 아니라면
			b++;
	}
}

// 스택 초기화 함수
void init(StackType * s)
{
	s->top = -1;
}

// 공백 상태 검출 함수
int is_empty(StackType * s)
{
	return (s->top == -1);
}

// 포화 상태 검출 함수
int is_full(StackType * s)
{
	return (s->top == (MAX_STACK_SIZE));
}

// 삽입함수
void push(StackType * s, element item)
{
	if (is_full(s))
		error_msg("스택 포화 에러\n");
	else s->stack[++(s->top)] = item;
}

// 삭제함수
element pop(StackType * s)
{
	if (is_empty(s))
		error_msg("스택 공백 에러\n");
	else return s->stack[(s->top)--];
}

// 피크함수
element peek(StackType *s)
{
	if (is_empty(s))
		error_msg("스택 공백 에러\n");
	else return s->stack[s->top];
}

// 우선순위 검사 함수
int prec(element ch)
{
	switch (ch.op) {
	case '+': case '-':
		return 1;
		break;
	case '*': case '/':	//+-보다 우선순위 높음
		return 2;
		break;
	case '(':	// 피연산자보다 우선순위가 작아야함
		return 0;
		break;
		// ')'는 입력받으면 pop하므로 필요없음
	}
}

// 2단계 : 중위수식 -> 후위수식
void infix_to_postfix(List * L, List * postfix)
{
	int i = 0;
	int len = list_get_length(L);	//리스트 L의 길이
	element item, top_op;
	StackType s;
	ListNode * p_node = NULL;
	p_node = L->head;
	init(&s); // 스택 초기화
	while (1) {
		element_init(&item);	//item 초기화
		if (p_node->data.is_op == 1)	// 연산자이면
		{

			switch (p_node->data.op) {
			case '+': case '-': case '*': case '/': // 연산자
													// 스택에 있는 연산자의 우선순위가 더 크거나 같으면 리스트 postfix에 노드 추가
				while (!is_empty(&s) && (prec(p_node->data) <= prec(peek(&s))))
				{
					item = pop(&s);	//pop한 것을 item에 넣은 후 리스트 postfix에 노드 추가
					list_add_last(postfix, item);
				}
				push(&s, p_node->data);
				break;
			case '(': // 왼쪽 괄호
				push(&s, p_node->data);
				break;
			case ')': // 오른쪽 괄호
				top_op = pop(&s);
				// 왼쪽 괄호를 만날때까지 리스트 postfix에 노드 추가
				while (top_op.op != '(') {
					item = top_op;
					top_op = pop(&s);
					list_add_last(postfix, item);
				}
				break;
			}
		}
		else	// 피연산자
		{
			item = p_node->data;
			list_add_last(postfix, item);
		}
		p_node = p_node->link;	//다음노드로 넘어감
		if (p_node == NULL)
			break;
	}
	while (!is_empty(&s)) // 스택에 저장된 연산자들을 postfix에 옮김
	{
		item = pop(&s);
		list_add_last(postfix, item);
	}
}

// 3단계 : 후위수식 계산함수
polynomial eval(List * postfix)
{
	int len = list_get_length(postfix), i;
	element op1, op2, result;
	StackType s;
	ListNode * p_node = NULL;
	p_node = postfix->head;
	init(&s);
	element_init(&op1);	//op1,op2,result 초기화
	element_init(&op2);
	element_init(&result);
	for (i = 0; i<len; i++) {
		if (p_node->data.is_op == 0)	// 피연산자이면
			push(&s, p_node->data);
		else { //연산자이면 피연산자를 스택에서 제거
			op2 = pop(&s);	//피연산자 두개를 꺼냄
			op1 = pop(&s);
			switch (p_node->data.op) { //연산을 수행하고 스택에 저장
			case '+':	// '+'일 경우 poly_add를 이용해 덧셈 계산
				result.poly = poly_add(op1.poly, op2.poly);
				push(&s, result);
				break;
			case '-':	// '-'일 경우 poly_subtract를 이용해 뺄셈 계산
				result.poly = poly_subtract(op1.poly, op2.poly);
				push(&s, result);
				break;
			case '*':	// '*'일 경우 poly_multiply를 이용해 곱셈 계산
				result.poly = poly_multiply(op1.poly, op2.poly);
				push(&s, result);
				break;
			}
		}
		p_node = p_node->link;	// 다음노드로 넘어감
	}
	return pop(&s).poly;	// 스택에 남아있는 계산된 결과 출력
}

void main()
{
	List L;	// List 타입의 변수 선언
	List postfix;	// 후위수식 리스트
	char str[100];

	list_init(&L);	// List 초기화, 현재 빈 리스트
	list_init(&postfix);	// 후위수식 리스트 초기화

	printf("입력: ");	//문자열 입력받음
	gets(str);

	printf("1단계후: ");
	infix_to_list(&L, str);	// 1단계 : 중위수식 리스트에 받기
	print_list(&L, "");	// 출력 해보기

	printf("2단계후: ");
	infix_to_postfix(&L, &postfix);	// 2단계 : 중위수식->후위수식
	print_list(&postfix, "");	// 출력 해보기

	printf("계산결과: ");
	print_poly(eval(&postfix), 'X');	// 3단계 : 후위수식 계산후 출력
	printf("\n");
}