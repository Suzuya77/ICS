#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

static WP* getWP(int N);
static WP* getPreWP(WP *wp);
bool freeWP(int N);
bool watchpointMonitor();
WP* new_wp(char *Expr);
void displayWP(int N);
void displayAllWP();

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}


/* TODO: Implement the functionality of watchpoint */

static WP* getWP(int N){
	WP *wp = head;
	if (wp == NULL)
	{
		return false;
	}

	while(wp){
		if (wp -> NO == N)
		{
			return wp;
		}else
		{
			wp = wp->next;
		}
	}
	return NULL;
}

static WP* getPreWP(WP *wp){
	WP *preWP = head;
	while(preWP != NULL){
		if (preWP->next == wp)
		{
			return preWP;
		}
		else
		{
			preWP = preWP->next;
		}
	}
	return NULL;
}

WP* new_wp(char *Expr){
	if (free_ == NULL)
	{
		printf("No free wp\n");
		return NULL;
	}

	bool success = false;
	expr(Expr, &success);
	if (success != true)
	{
		printf("Expression %s incorrect!\n", Expr);
		return NULL;
	}

	WP *node = free_;
	free_ = free_->next;
	strcpy(node->EXPR, Expr);
	node->next = head;
	head = node;
	return node;
}

bool freeWP(int N){
	WP *wp = getWP(N);
	if (wp == NULL)
	{
		printf("No.%d wp does not exist\n", N);
		return false;
	}

	WP *preWP = getPreWP(wp);

	if (wp == head)
	{
		head = NULL;
	}else
	{
		preWP->next = wp->next;
	}

	wp->next = free_;
	free_ = wp;

	printf("No.%d wp freed\n", N);
	return true;
}

void displayWP(int N){
	WP *wp = getWP(N);
	if (wp == NULL) 
	{
		printf("No.%d wp does not exist\n", N);
		return;
	} 

	printf("No.%d: Expr:%s NewValue:%d\n", wp->NO, wp->EXPR, wp->newValue);
}


void displayAllWP(){
	WP *wp = head;
	if (wp == NULL) 
	{	
		printf("No wp\n");	
		return;
	} 

	while (wp != NULL) 
	{		
		printf("\tNo.%d: \tExpr:%s \tNewValue:%d\n", wp->NO, wp->EXPR, wp->newValue);
		wp = wp->next;
	}
	return;
}

bool watchpointMonitor(){
	WP *wp = head;
	if (wp == NULL) 
	{ 
		return false;
	} else 
	{
		while(wp != NULL)
		{
			bool success = true;
			int newValue = expr(wp->EXPR, &success);
			if (success && newValue != wp->newValue) 
			{			
				printf("Detected watchpoint value changed\n");
				printf(" No.%d\n", wp->NO);	
				printf(" Expression:\t%s\n", wp->EXPR);
				printf(" Old Value:\t%d\n", wp->oldValue);
				printf(" New Value:\t%d\n", newValue);
				wp->oldValue = wp->newValue;
				wp->newValue = newValue;
				return true;			
			}
			wp = wp->next;
		}
		return false;
	}
}