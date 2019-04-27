#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {  //从256开始,为了避开ascii
  TK_NOTYPE = 256, TK_HEX, TK_DEC, TK_REG, TK_EQ, TK_NEQ, TK_NEG

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"0x[1-9A-Fa-f][0-9A-Fa-f]*", TK_HEX},
  {"0|[1-9][0-9]*", TK_DEC},
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|ax|cx|dx|bx|sp|bp|si|di|al|cl|dl|bl|ah|ch|dh|bh)", TK_REG},

  {"\\+", '+'},         // 使用单引号
  {"-", '-'},          
  {"\\*", '*'},
  {"\\/", '/'},

  {"\\(", '('},
  {"\\)", ')'},
  
  {"==", TK_EQ},         // equal
  {"!=", TK_NEQ}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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

Token tokens[32];
int nr_token; //已识别出的token数目

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);  //%.*s两个参数宽度+串，指定宽度 强制输出
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if(rules[i].token_type == TK_NOTYPE) //空格直接舍弃
            break;
        if(substr_len>31)  //str溢出 false报错
            assert(0);
        memset(tokens[nr_token].str,'\0',32); //以防万一
        strncpy(tokens[nr_token].str, substr_start, substr_len);// 类似上面的%.*s

        tokens[nr_token].type = rules[i].token_type;
        // Log("Save in type=%d, str=%s",tokens[nr_token].type,tokens[nr_token].str);
        nr_token = nr_token + 1;

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
bool check_parentheses(int p,int q){
    if((tokens[p].str[0]=='(') && (tokens[q].str[0]==')')){
        //左括号记为1 右括号记为-1 
        //总和应该为0 且遍历完之前总和一定不为0，以确保最左和最右匹配
        int count = 0;
        for(int i=p;i<q;i++) //前n-1个数总和应不为0
        {
            if(tokens[i].str[0] == '(')
                count = count + 1;
            if(tokens[i].str[0] == ')')
                count = count - 1;
            if(count == 0)
            {
                //printf("Leftmost and rightmost are not matched\n");
                return false;
            }
        }
        count = count -1; //最后一个右括号
        if(count !=0) //总和应该为0
        {
            printf("Bad parentheses\n");
            assert(0);
        }
        return true;
    }
    else
    {
        // printf("The whole expr was not surrounded\n");
        return false;
    }
}
uint32_t eval(int p,int q){
    if(p>q){        
        printf("Bad expression\n");
        assert(0);
    }
    else if(p==q){
        if(tokens[p].type == TK_HEX){
            uint32_t res;
            sscanf(tokens[p].str,"%x",&res);
            return res;
        }
        else if(tokens[p].type == TK_DEC)
        {
            uint32_t res;
            sscanf(tokens[p].str,"%d",&res);
            return res;
        }
        else{
            printf("Bad expression\n");
            assert(0);
        }
    }
    else if(check_parentheses(p,q) == true){
        return eval(p+1,q-1);
    }
    else{
        int op=0;
        char op_type='\0';
        bool left = false;//出现左括号的flag
        int curr_prev = 3;//当前存的符号优先级
        for(int i=p;i<=q;i++){  //此处为p～q而不是0～q-p
            if(tokens[i].str[0]==')')
            {
                left = false;
                continue;
            }
            if(left)
                continue;
            if(tokens[i].str[0]=='(')
            {
                left = true;
                continue;
            }
            switch(tokens[i].str[0]){
                case '+':if(curr_prev>=1){curr_prev=1;op=i;op_type='+';continue;}
                case '-':if(curr_prev>=1){curr_prev=1;op=i;op_type='-';continue;}
                case '*':if(curr_prev>=2){curr_prev=2;op=i;op_type='*';continue;}
                case '/':if(curr_prev>=2){curr_prev=2;op=i;op_type='/';continue;}
                default:continue;
            }
        }

        uint32_t val1 = eval(p,op-1);
        uint32_t val2 = eval(op+1,q);
        switch(op_type){
            case '+':return val1+val2;
            case '-':return val1-val2;
            case '*':return val1*val2;
            case '/':return val1/val2;
            default:assert(0);
        }
    }
}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  //*success = true;
  /* TODO: Insert codes to evaluate the expression. */
  // TODO();  //什么鬼
  //
  //printf("RESULT=%d\n",eval(0, nr_token-1));

  /*if(tokens[0].type == '-')  //如果是负号而不是减号，那么应该是第一个符号或者左边为(
  {
      tokens[0].type = TK_NEG;
      if(nr_token == 1)
          assert(0);
      uint32_t changed;
      if(tokens[1].type == TK_DEC)  //按照常识 符号后只跟十进制数
      {
          sscanf(tokens[1].str,"%d",&changed);
          changed = -1 * changed;
          itoa(changed,tokens[1].str,10);
      }
      else
          asserts(0);
  }
  for(int i=0;i<nr_token;i++)
  {
      if((tokens[i].type == '-')&&(tokens[i-1].type == '('))
      {
          tokens[i].type = TK_NEG;
          if(i+1==nr_token)
              assert(0);
          uint32_t changed;
          if(tokens[i+1].type == TK_DEC)
          {
              sscanf(tokens[i+1].str,"%d",&changed);
              changed = -1*changed;
              itoa(changed,tokens[i+1].str,10);
          }
          else
              asserts(0);
      }
  }*/
  return eval(0, nr_token-1);
}
