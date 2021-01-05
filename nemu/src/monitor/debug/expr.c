#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t isa_reg_str2val(const char *s, bool *success);
uint32_t paddr_read(paddr_t addr, int len);

enum {
  TK_NOTYPE = 256, TK_EQ, TK_PLUS, 
  TK_REG, TK_DNUM, TK_HNUM, TK_NEQ, TK_AND, TK_DEREF, TK_NEG
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"==", TK_EQ},         // equal
  {"\\+", '+'},         // plus


  {"^\\$0|^\\$ra|^\\$[sgt]p|^\\$t[0-6]|^\\$s[0-9]|^\\$s1[01]|^\\$a[0-7]|^\\$pc", TK_REG}, // regex 
  {"\\b[0-9]+\\b", TK_DNUM}, // decimal
  {"0[xX][0-9a-fA-F]+", TK_HNUM}, // hex
  {"-", '-'}, // minus
  {"\\*", '*'}, // multiple
  {"\\/", '/'}, // divide
  {"\\(", '('}, // left brac
  {"\\)", ')'}, // right brac
  {"!=", TK_NEQ}, // not equal
  {"&&", TK_AND} // and

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool check_parents(uint32_t p, uint32_t q);
static uint32_t operation_position(uint32_t p, uint32_t q);
static uint32_t eval(uint32_t p, uint32_t q);
static bool judge_deref(int type);
static bool judge_neg(int type);


static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  for (int j = 0; j < NR_REGEX; ++j)
  {
    memset(tokens[j].str, '\0', 32);
    tokens[j].type = 0;
  }

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        if (rules[i].token_type == TK_NOTYPE)
        {
          break;
        }

        switch (rules[i].token_type) {
          case TK_DNUM:
          case TK_HNUM:
            if (substr_len > 32)
            {
              printf("ERROR, overflowed bit number\n");
              assert(0);
            }

          case '+':
          case '-':
          case '*':
          case '/':
          case '(':
          case ')':
          case TK_EQ:
          case TK_REG:
          case TK_NEQ:
          case TK_AND:
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;

          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

static bool check_parents(uint32_t p, uint32_t q){
  int flag = 0;
  if (strcmp(tokens[p].str, "(") || strcmp(tokens[q].str, ")"))
  {
    return false;
  }

  p++;
  while(p<q){
    if (strcmp(tokens[p].str, "(") == 0)
    {
      p++;
      flag++;
    }else if (strcmp(tokens[p].str, ")") == 0)
    {
      p++;
      flag--;
      if (flag < 0)
      {
        return false;
      }
    }else p++;
  }
  return flag == 0;
}

static uint32_t operation_position(uint32_t p, uint32_t q){
  assert(p <= q);
  uint32_t i = p;
  uint32_t op = p;
  int pnum = 0;

  for (; i < q; ++i)
  {
    if (strcmp(tokens[i].str, "(") == 0)
    {
      pnum++;
    }

    if (strcmp(tokens[i].str, ")") == 0)
    {
      pnum--;
    }

    if (pnum == 0){
      if (strcmp(tokens[i].str, "+") == 0 || strcmp(tokens[i].str, "-") == 0 ||
        strcmp(tokens[i].str, "*") == 0 || strcmp(tokens[i].str, "/") == 0 ||
            tokens[i].type == TK_DEREF|| tokens[i].type == TK_EQ || 
            tokens[i].type == TK_NEQ ||  tokens[i].type == TK_AND) 
      {
        op = i; 
        break;
      }
    }
  }
  return op;
}

static uint32_t eval(uint32_t p, uint32_t q){
  if (p > q)
  {
    printf("ERROR\n");
    assert(0);
  }

  if (p == q)
  {
    uint32_t value = 0;
    if (tokens[p].type == TK_DNUM)
    {
      value = strtoul(tokens[p].str, NULL, 10);
    }

    if (tokens[p].type == TK_HNUM)
    {
      value = strtoul(tokens[p].str, NULL, 16);
    }

    if (tokens[p].type == TK_REG)
    {
      for (int i = 0; i < strlen(tokens[p].str); ++i)
      {
        tokens[p].str[i] = tokens[p].str[i+1];
      }

      bool success = true;
      value = isa_reg_str2val(tokens[p].str, &success);
    }
    return value;
  }else if (check_parents(p, q) == true)
  {
    printf("check parents correctly, p = %u, q = %u\n", p, q);
    return eval(p+1, q-1);
  }else{
    int op = operation_position(p, q);
    uint32_t temp = 0;
    if (tokens[op].type != TK_DEREF && tokens[p].type != TK_NEG)
    {
      temp = eval(p, op-1);
    }
    uint32_t temp1 = eval(op+1, q);

    switch(tokens[op].type){
      case '+': 
        return temp + temp1;
      case '-':
        return temp - temp1;
      case '*': 
        return temp * temp1;
      case '/':
        return temp / temp1;
      case TK_DEREF:
        return paddr_read(temp1, 4);
      case TK_EQ:
        return temp == temp1;
      case TK_NEQ:
        return temp != temp1;
      case TK_AND:
        return temp && temp1;
      case TK_NEG:
        return -temp1;
      default:
        assert(0);
    }
  }
  return 0;
}

static bool judge_deref(int type){
  switch(type) {
    case '+':
    case '-':
    case '*':
    case '/': 
      return true; 
    default : 
      return false;
  }
}

static bool judge_neg(int type){
  switch(type) {
    case '+':
    case '-':
    case '*':
    case '/': 
    case '(': 
      return true; 
    default : 
      return false;
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  for (int i = 0; i < nr_token; ++i)
  {
    if (tokens[i].type == '*' && (i == 0 || judge_deref(tokens[i-1].type)))
    {
      tokens[i].type = TK_DEREF;
    }

    if (tokens[i].type == '-' && (i == 0 || judge_neg(tokens[i-1].type)))
    {
      tokens[i].type = TK_NEG;
    }
  }

  uint32_t result = eval(0, nr_token-1);

  // printf("Result is %u \n", result);
  return result;
}
