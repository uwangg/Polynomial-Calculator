#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>

#define MAX(x,y)  ((x)>(y)?(x):(y))
#define ABS(x)  ((x)>0? (x) : -(x))

#define MAX_DEGREE 100
#define MAX_STACK_SIZE 100

// ���׽��� ǥ���ϴ� ����ü Ÿ�� polynomial
typedef struct _poly {
	int degree;  // ����
	int coef[MAX_DEGREE + 1]; // ����� �����ϴ� �迭
} polynomial;

//element: ������ ������<+,-,*,(,)> Ȥ�� �ǿ�����<���׽�>
typedef struct {
	polynomial poly;	//�ǿ�����(���׽�)�� ������ ��� ����
	char op;	//�����ڸ� ������ ��� ����
	int	is_op;	//�ǿ��������� ������������ ǥ����
} element;

// ListNode Ÿ���� ����
typedef struct _ListNode {
	element data;		// ������ �ʵ�
	struct _ListNode *link;	// ��ũ �ʵ�
} ListNode;

// List Ÿ���� ����
typedef struct _List {
	ListNode *head;		// ���� ����Ʈ�� ����
} List;

// StackType�� ����
typedef struct {
	element stack[MAX_STACK_SIZE];
	int top;
}StackType;

void error_msg(char *strMsg);	//����ó�� �Լ�

void print_element(element e);
void print_poly(polynomial P, char symbol);
void print_list(List *pList, char *strLink);	//list�� ����Ϳ� ���
void list_add(List *pList, int pos, element item);	// pos��ġ�� item�� �߰�
void list_add_last(List *pList, element item);		// �ǳ��� item�� �߰�
void list_add_first(List *pList, element item);		// �Ǿտ� item�� �߰�

polynomial poly_zero();
polynomial poly_minus(polynomial P);
polynomial poly_add(polynomial P1, polynomial P2);
polynomial poly_subtract(polynomial P1, polynomial P2);
polynomial poly_multiply(polynomial P1, polynomial P2);

void list_init(List *pList);						// �� list�� �ʱ�ȭ
void element_init(element * data);	//element �ʱ�ȭ
int list_get_length(List *pList);					// list�� ���̸� ��ȯ

void infix_to_list(List *pList, char * exp);	//1�ܰ� : �������� -> ����Ʈ�� ����

void init(StackType * s);
int is_empty(StackType * s);
int is_full(StackType * s);
void push(StackType * s, element item);
element pop(StackType * s);
element peek(StackType *s);
int prec(element ch);
void infix_to_postfix(List * L, List * postfix);	//2�ܰ� : ��������->��������
polynomial eval(List * postfix);	//3�ܰ� : �������� -> ���


									///////////////////////////////////////////////////////////

									// error_msg
									//  �����޽����� ����� �� ���α׷��� ����������.
									//  parameter: char *strMsg
									//   �����޽��� strMsg�� ǥ�ؿ�����Ʈ������ ����ϰ�
									//   exit�Լ��� ȣ���Ͽ� ���α׷��� ���δ�.
void error_msg(char *strMsg)
{
	fprintf(stderr, "%s", strMsg);
	exit(1);
}

//print_element
//elementŸ���� ���� ������ �������� ȭ�鿡 ����Ѵ�.
void print_element(element e)
{
	if (e.is_op)	//e�� ���� �������̸�, e.op�� ���
	{
		printf("%c", e.op);
	}
	else	//�ǿ������̸�,e.poly�� ���
	{
		printf("[");
		print_poly(e.poly, 'X');
		printf("]");
	}
}

// print_poly 
// ���׽� P�� ������ �������� ȭ�鿡 ��� 
// P�� �������� ǥ���ϱ� ���� symbol�� ����� 
// parameter: polynomial P, char symbol 
// return value: void 
// usage: ���� ��� �������� X�� ����ϰ� ������, 
// print_poly(P, 'X'); �� ���� �÷� ȣ�� 
void print_poly(polynomial P, char symbol)
{
	int i = P.degree;
	if (P.degree <= 0)
	{// ������ 0 Ȥ�� -1�̸�, ����׸� ����� �� ���� 
		printf("%d", P.coef[0]);
		return;
	}

	/// �ְ������� ���� ��� 
	// ����� -1 Ȥ�� 1�̸� ��ȣ�� ����Ѵ�. 
	if (P.coef[i] == -1)
		printf("-%c", symbol);
	else if (P.coef[i] == 1)
		printf("%c", symbol);
	else
		printf("%d%c", P.coef[i], symbol);
	if (i > 1)// 2���� �̻󿡼��� ������ ǥ�� 
		printf("^%d", i);

	// ������ �׵鿡 ����... 
	for (--i; i >= 0; i--)
	{
		// ����� 0�̸� ������� ���� 
		if (P.coef[i] == 0)
			continue;

		// ����� ��ȣ�� ���� ��� 
		if (P.coef[i] < 0)
			printf(" - ");
		else if (P.coef[i] > 0)
			printf(" + ");
		// ����� ���밪�� ��� 
		// ��, ����� ���밪�� 1�̰� ������� �ƴϸ� ������ʿ� ����. 
		if (ABS(P.coef[i]) != 1 || i == 0)
			printf("%d", ABS(P.coef[i]));

		// �� �ڿ� "symbol^����"�� ���¸� ��� 
		// ��, 1�����̸�(��, i==1�̸�) "^����"�� �����ϸ�, 
		// ������̸�(��, i==0�̸�) ������� ���� 
		if (i>1)
			printf("%c^%d", symbol, i);
		else if (i == 1)
			printf("%c", symbol);
	}
}

// poly_zero
//  parameter:
//  return value: 
//       ���׽� p(x)=0 �� ��ȯ, 0�� ���׽��� ������ -1�� ���Ѵ�.
polynomial poly_zero()
{
	polynomial r = { -1,{ 0 } };
	return r;
}

// poly_minus
//  ���׽� P�� ���ڷ� �޾� -P�� �����Ѵ�. (����� ��ȣ�� �ٲ�)
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
//  �� ���׽��� ���� ����Ͽ� ��ȯ
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
//  �� ���׽��� ���� ����Ͽ� ������
//  parameter: polynomial P1, polynomial P2
//  return value: P1-P2
polynomial poly_subtract(polynomial P1, polynomial P2)
{
	polynomial result = poly_zero();
	result = poly_add(P1, poly_minus(P2));

	return result;
}

// poly_multiply
//  �� ���׽��� ���� ����Ͽ� ������
//  parameter: polynomial P1, polynomial P2
//  return value: P1*P2
polynomial poly_multiply(polynomial P1, polynomial P2)
{
	polynomial result = poly_zero();
	int i, j;
	result.degree = P1.degree + P2.degree;
	if (result.degree > 100)
		error_msg("���� 100 �ʰ�!\n");
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
// ����Ʈ�� ������ ����Ϳ� ����Ѵ�.
// parameter: List *pList, char *strLink
// return value: ����
// ����Ʈ�� ������ ������� ȭ�鿡 ��µȴ�.
// strLink�� �׸� ���̿� �� ���ڿ��̴�.
// element Ÿ���� �� �׸��� print_element�� ȣ���Ͽ� ����Ѵ�.
// ��) print_list(pList, "-->"); ���·� ȣ���ϸ� ���׸� ���̿� --> �� �׷�����.
// ��, pList==NULL �� ���, �����޽����� ����ϰ� ���α׷��� ������.
void print_list(List *pList, char *strLink)
{
	ListNode *cur_node = NULL;
	if (pList == NULL)
		error_msg("print_list: pList==NULL");
	cur_node = pList->head;
	if (cur_node == NULL) // ���� �� ����Ʈ���...
	{
		printf("��\n");
		return;
	}
	// �ϳ��� �ִٸ�...
	// ���� �� �� �׸��� ���
	print_element(cur_node->data);
	cur_node = cur_node->link;
	// �� ������ ��� �׸�鿡 ���Ͽ�..
	while (cur_node != NULL)
	{
		printf("%s", strLink); // strLink�� ����� ��,
		print_element(cur_node->data); // ���� ����� �����͸� ���
		cur_node = cur_node->link; // ���� ���� �̵�
	}
	printf("\n");
}

// list_init
//  �� list�� �ʱ�ȭ�Ѵ�. 
//  �� �Լ��� List Ÿ���� ������ ������ �� ���ʿ� �ѹ��� ȣ��Ǿ�� �Ѵ�.
//  parameters: List *pList
//  return value: ����
//   ��, pList==NULL �� ��� �����޽����� ����ϰ� ���α׷��� ������.
void list_init(List *pList)
{
	if (pList == NULL)	// �Է¹��� ����Ʈ�� NULL�̸� ���α׷� ����
		error_msg("list_init: pList == NULL");

	pList->head = NULL;
}

// list_get_length
//  ����Ʈ�� ���̸� ����Ͽ� ��ȯ
//  parameter: List *pList
//  return value: pList�� �׸� ������ ����Ͽ� ����
//   ��, pList==NULL �� ��� �����޽����� ����ϰ� ���α׷��� ������.
int list_get_length(List *pList)
{
	int cnt = 0;
	ListNode * cur_node = NULL;
	if (pList == NULL)	// �Է¹��� ����Ʈ�� NULL�̸� ���α׷� ����
		error_msg("list_get_length : pList == NULL");
	if (pList->head == NULL)
		cnt = 0;
	else
	{
		cur_node = pList->head;
		while (cur_node->link != NULL)	//������������ ã����
		{
			cur_node = cur_node->link;
			cnt++;
		}
		cnt++;	//����������Ǽ����� ������
	}
	return cnt;
}

// list_add
//  ����Ʈ�� pos��ġ�� �׸� item�� �����Ѵ�.
//  parameter: List *pList, int pos, element item
//  return value: ����
//    pos==0�̸� �� �տ�, pos==list_get_length(pList)�̸� �� �ڿ� �߰��ȴ�.
//    ��, ���Կ� ������ ���, (pos ���� �������ϰų�) �����޽����� ����ϰ� ���α׷��� ������.
void list_add(List *pList, int pos, element item)
{
	int i;
	ListNode * new_node = (ListNode *)malloc(sizeof(ListNode));	//���ο��� �����Ҵ�
	ListNode * cur_node = NULL;
	new_node->data = item;	//����� data�κп� item����
	new_node->link = NULL;
	if (list_get_length(pList)<pos || pos<0)	//pos�� ������ �������
		error_msg("list_add: pos isn't proper value");
	if (pList == NULL)
		error_msg("list_add: pList == NULL");
	if (new_node == NULL || pList == NULL)
		error_msg("list_add:memory allocation failed");

	if (pos == 0)	//���� �տ� �����Ұ��
	{
		list_add_first(pList, item);
	}
	else if (pos == list_get_length(pList) - 1)	//���� �������� �����Ұ��
	{
		list_add_last(pList, item);
	}
	else	//pos�� ��ġ�� �����Ұ��
	{
		cur_node = pList->head;
		for (i = 0; i<pos - 1; i++)	//��������ġ�� ���ǳ����� ã����
			cur_node = cur_node->link;
		new_node->link = cur_node->link;
		cur_node->link = new_node;
	}
}

// list_add_last
//  ����Ʈ�� �� ���� �׸� item�� �����Ѵ�.
//  parameter: List *pList, element item
//  return value: ����
//    ��, ���Կ� ������ ���, ������ �����޽����� ����ϰ� ���α׷��� ������.
void list_add_last(List *pList, element item)
{
	ListNode * last_node = (ListNode *)malloc(sizeof(ListNode));
	ListNode * cur_node = NULL;	//head������ �ʱ�ȭ
	last_node->data = item;	//�����data�κп� item����
	last_node->link = NULL;
	if (pList->head == NULL)	//�� ����϶�..
	{
		pList->head = last_node;
	}
	else	//������ ��带 ã����
	{
		cur_node = pList->head;
		while (cur_node->link != NULL)
			cur_node = cur_node->link;
		cur_node->link = last_node;
	}
}

// list_add_first
//  ����Ʈ�� �� �տ� �׸� item�� �����Ѵ�.
//  parameter: List *pList, element item
//  return value: ����
//    ��, ���Կ� ������ ���, ������ �����޽����� ����ϰ� ���α׷��� ������.
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

//element �ʱ�ȭ
void element_init(element * data)
{
	data->is_op = -1;
	data->op = NULL;
	data->poly = poly_zero();
}

//1�ܰ� : ������ ��ũ�帮��Ʈ�� �ٲ���
void infix_to_list(List *pList, char * exp)
{
	int a = 0, b = 0, pos;
	int i = 0, j = 0, k = 0;	//a��b�� 0���� �ʱ�ȭ
	int len = strlen(exp);
	char ch[20], *toke = NULL;
	element item;
	for (i = 0; i <= len; i++)
	{
		//exp[b]�� ������ Ȥ�� ���ڿ��� ���̸�
		if (exp[b] == '+' || exp[b] == '-' || exp[b] == '*' || exp[b] == '/' || exp[b] == '(' || exp[b] == ')' || exp[b] == '\0')
		{
			element_init(&item);	//item �ʱ�ȭ
			if (a == b)	//�ǿ����ڰ� ���̿� ����
			{
				//exp[b]�� element������ �ٲ㼭 ��� �߰�
				item.op = exp[b];
				item.is_op = 1;
				list_add_last(pList, item);
			}
			else
			{
				for (j = a; j<b; j++)	//a���� b-1���� ���ڿ��� ch�� �ű�
					ch[j - a] = exp[j];
				ch[j - a] = NULL;	//ch�� ���� �˷���
				if (strchr(ch, 'X') == NULL)	//X�� ���ٸ� pos=-1
					pos = -1;
				else	//X�� �ִٸ� pos�� ��ġ ����
					pos = strchr(ch, 'X') - ch;
				if (pos == -1)	//X�� ���ٸ�..
				{
					item.poly.degree = 0;
					item.poly.coef[item.poly.degree] = atoi(ch);
				}
				else if (pos == 0)	//X�� ��ġ�� 0��°��� ���� ��
				{
					if (ch[1] == '^')	//'^'�̸� X^������ ����
					{
						toke = strtok(ch, "^");
						toke = strtok(NULL, "^");	//���� �κи� �ڸ�
						if (atoi(toke) > 100)
							error_msg("���� 100 �ʰ�!\n");
						item.poly.degree = atoi(toke);	//���� ����
						item.poly.coef[item.poly.degree] = 1;	//����� 1����
					}
					else	//[X]�� ����
					{
						item.poly.degree = 1;
						item.poly.coef[item.poly.degree] = 1;
					}
				}
				else if (pos == b - a - 1)	//���X�� ����
				{
					toke = strtok(ch, "X");
					item.poly.degree = 1;
					item.poly.coef[1] = atoi(toke);
				}
				else	//[���X^����]�� ����
				{
					toke = strtok(ch, "X");
					k = atoi(toke);
					toke = strtok(NULL, "^");
					if (atoi(toke) > 100)	//������ 100�� �ʰ��Ұ�� ���� ����� ����
						error_msg("degree is 100 over!");
					item.poly.degree = atoi(toke);
					item.poly.coef[item.poly.degree] = k;
				}
				item.is_op = 0;	//�ǿ������̹Ƿ� 0����
				list_add_last(pList, item);	//���׽� ����Ʈ �ڿ� �߰�

				element_init(&item);	//item �ʱ�ȭ
				item.op = exp[b];	//exp[b]�� item�� �����ڿ� ����
				item.is_op = 1;	//�������̹Ƿ� 1
				list_add_last(pList, item);	//exp[b]�� ����Ʈ �ڿ� �߰�
			}
			b++;
			a = b;
		}
		else	//�����ڰ� �ƴ϶��
			b++;
	}
}

// ���� �ʱ�ȭ �Լ�
void init(StackType * s)
{
	s->top = -1;
}

// ���� ���� ���� �Լ�
int is_empty(StackType * s)
{
	return (s->top == -1);
}

// ��ȭ ���� ���� �Լ�
int is_full(StackType * s)
{
	return (s->top == (MAX_STACK_SIZE));
}

// �����Լ�
void push(StackType * s, element item)
{
	if (is_full(s))
		error_msg("���� ��ȭ ����\n");
	else s->stack[++(s->top)] = item;
}

// �����Լ�
element pop(StackType * s)
{
	if (is_empty(s))
		error_msg("���� ���� ����\n");
	else return s->stack[(s->top)--];
}

// ��ũ�Լ�
element peek(StackType *s)
{
	if (is_empty(s))
		error_msg("���� ���� ����\n");
	else return s->stack[s->top];
}

// �켱���� �˻� �Լ�
int prec(element ch)
{
	switch (ch.op) {
	case '+': case '-':
		return 1;
		break;
	case '*': case '/':	//+-���� �켱���� ����
		return 2;
		break;
	case '(':	// �ǿ����ں��� �켱������ �۾ƾ���
		return 0;
		break;
		// ')'�� �Է¹����� pop�ϹǷ� �ʿ����
	}
}

// 2�ܰ� : �������� -> ��������
void infix_to_postfix(List * L, List * postfix)
{
	int i = 0;
	int len = list_get_length(L);	//����Ʈ L�� ����
	element item, top_op;
	StackType s;
	ListNode * p_node = NULL;
	p_node = L->head;
	init(&s); // ���� �ʱ�ȭ
	while (1) {
		element_init(&item);	//item �ʱ�ȭ
		if (p_node->data.is_op == 1)	// �������̸�
		{

			switch (p_node->data.op) {
			case '+': case '-': case '*': case '/': // ������
													// ���ÿ� �ִ� �������� �켱������ �� ũ�ų� ������ ����Ʈ postfix�� ��� �߰�
				while (!is_empty(&s) && (prec(p_node->data) <= prec(peek(&s))))
				{
					item = pop(&s);	//pop�� ���� item�� ���� �� ����Ʈ postfix�� ��� �߰�
					list_add_last(postfix, item);
				}
				push(&s, p_node->data);
				break;
			case '(': // ���� ��ȣ
				push(&s, p_node->data);
				break;
			case ')': // ������ ��ȣ
				top_op = pop(&s);
				// ���� ��ȣ�� ���������� ����Ʈ postfix�� ��� �߰�
				while (top_op.op != '(') {
					item = top_op;
					top_op = pop(&s);
					list_add_last(postfix, item);
				}
				break;
			}
		}
		else	// �ǿ�����
		{
			item = p_node->data;
			list_add_last(postfix, item);
		}
		p_node = p_node->link;	//�������� �Ѿ
		if (p_node == NULL)
			break;
	}
	while (!is_empty(&s)) // ���ÿ� ����� �����ڵ��� postfix�� �ű�
	{
		item = pop(&s);
		list_add_last(postfix, item);
	}
}

// 3�ܰ� : �������� ����Լ�
polynomial eval(List * postfix)
{
	int len = list_get_length(postfix), i;
	element op1, op2, result;
	StackType s;
	ListNode * p_node = NULL;
	p_node = postfix->head;
	init(&s);
	element_init(&op1);	//op1,op2,result �ʱ�ȭ
	element_init(&op2);
	element_init(&result);
	for (i = 0; i<len; i++) {
		if (p_node->data.is_op == 0)	// �ǿ������̸�
			push(&s, p_node->data);
		else { //�������̸� �ǿ����ڸ� ���ÿ��� ����
			op2 = pop(&s);	//�ǿ����� �ΰ��� ����
			op1 = pop(&s);
			switch (p_node->data.op) { //������ �����ϰ� ���ÿ� ����
			case '+':	// '+'�� ��� poly_add�� �̿��� ���� ���
				result.poly = poly_add(op1.poly, op2.poly);
				push(&s, result);
				break;
			case '-':	// '-'�� ��� poly_subtract�� �̿��� ���� ���
				result.poly = poly_subtract(op1.poly, op2.poly);
				push(&s, result);
				break;
			case '*':	// '*'�� ��� poly_multiply�� �̿��� ���� ���
				result.poly = poly_multiply(op1.poly, op2.poly);
				push(&s, result);
				break;
			}
		}
		p_node = p_node->link;	// �������� �Ѿ
	}
	return pop(&s).poly;	// ���ÿ� �����ִ� ���� ��� ���
}

void main()
{
	List L;	// List Ÿ���� ���� ����
	List postfix;	// �������� ����Ʈ
	char str[100];

	list_init(&L);	// List �ʱ�ȭ, ���� �� ����Ʈ
	list_init(&postfix);	// �������� ����Ʈ �ʱ�ȭ

	printf("�Է�: ");	//���ڿ� �Է¹���
	gets(str);

	printf("1�ܰ���: ");
	infix_to_list(&L, str);	// 1�ܰ� : �������� ����Ʈ�� �ޱ�
	print_list(&L, "");	// ��� �غ���

	printf("2�ܰ���: ");
	infix_to_postfix(&L, &postfix);	// 2�ܰ� : ��������->��������
	print_list(&postfix, "");	// ��� �غ���

	printf("�����: ");
	print_poly(eval(&postfix), 'X');	// 3�ܰ� : �������� ����� ���
	printf("\n");
}