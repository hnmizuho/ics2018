#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);//readline返回值由malloc分配,释放需要free
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);//行编辑的一常用功能 历史记录
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static struct {
  char *name;
  char *description;
  int (*m_handler) (char *);  //函数指针 返回值(新类型)(参数列表)
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step N steps", cmd_si },
  { "info", "Print regs' or watchpoint's state", cmd_info },
  { "x", "Scan the mem", cmd_x },
  { "p", "Expression evaluation", cmd_p },
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0])) //长度

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");  //同一个字符串，第二次要用NULL
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}


static int cmd_si(char *args) {
    int step;
    char *arg = strtok(NULL, " ");
    if (arg == NULL)
        step=1;
    else
        step=atoi(arg); //atoi atof sscanf
    cpu_exec(step);
    return 0;
}
static int cmd_info(char *args) {
    char *subcmd[] = {"r","w"};
    int SUBCMD_SIZE = 2;
    int i;

    char *arg = strtok(NULL, " ");
    if (arg != NULL){
        for(i=0;i<SUBCMD_SIZE;i++)
            if(strcmp(arg,subcmd[i]) == 0){
                if(i == 0)
                {
                    int j;
                    for(j=0;j<8;j++)
                        printf("%s\t\t0x%08x\n",regsl[j],cpu.gpr[j]._32);
                    for(j=0;j<8;j++) 
                        printf("%s\t\t0x%04x\n",regsw[j],cpu.gpr[j]._16);
                    for(j=0;j<8;j++) 
                        printf("%s\t\t0x%02x\n",regsb[j],cpu.gpr[j%4]._8[j/4]);
                }
                else if(i == 1)
                    printf("w");
                return 0;
            }
        printf("Unknown command '%s'\n", arg);
        return 0;
    }
    printf("Lack of parameter!\n");
    return 0;

}
static int cmd_x(char *args) {
    int N;
    char* expr;

    char *arg = strtok(NULL, " ");
    if(arg == NULL)
    {
        printf("Lack of parameter!\n");
        return 0;
    }
    N = atoi(arg);
    if(N==0){
        printf("Unknown command '%s'\n",arg);
        return 0;  //N=0时可能不是数字
    }
    //printf("%d\n",N);

    expr = strtok(NULL, " ");
    if(expr == NULL)
    {
        printf("Lack of parameter!\n");
        return 0;
    }
    //printf("%s\n",expr);

    vaddr_t addr;
    sscanf(expr,"%x",&addr);//尝试使用标准格式化输出
    for(int i=0;i<N;i++)
    {
        printf("%08x:\t%08x\n",addr,vaddr_read(addr,4));
        addr = addr+4;
    }
    return 0;
}

static int cmd_p(char *args) {
   // bool *success=false;
    // char *arg = strtok(NULL, " "); //参数只有1个
    if(args == NULL)
    {
        printf("Lack of parameter!\n");
        return 0;
    }printf("%s",args);
    /*
    uint32_t res = expr(args,success);
    if(*success == false)
        printf("Lexical analysis error");
    else
        printf("%d\n",res);
        */
    return 0;
}
void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);//指针指向末尾'\0'

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;//指针指向命令剩余部分的开头
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].m_handler(args) < 0) { return; } //用于 q 退出
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
