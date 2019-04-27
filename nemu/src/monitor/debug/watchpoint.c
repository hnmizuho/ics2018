#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

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
WP* new_wp(){
    if(head == NULL){
        head = free_;
        free_=free_->next;
        head->next = NULL;
        return head;
    }
    else{
        if(free_ == NULL) //没有空闲的了
            assert(0);
        WP* tmp = head;
        while(tmp->next!=NULL){
            tmp = tmp->next;
        }
        tmp->next = free_;
        free_ = free_->next;
        tmp = tmp->next;
        tmp->next = NULL;
        return tmp;
    }
}
void free_wp(int N){
    WP *wp=head;
    while(wp!=NULL)
    {
        if(wp->NO == N)
            break;
        wp = wp->next;
    }
    if(wp == NULL)
    {
        printf("Fail to "); //fail to free wp
        return;
    }
    if(head == wp)
        head = head->next;
    else
    {
        WP* tmp = head;
        while(tmp!=NULL)
        {
            if(tmp->next == wp){
                tmp->next = wp->next;
                break;
            }
            tmp = tmp->next;
        }
    }
    wp->next = free_;
    free_ = wp;
}
void show_wp(){
    printf("Num\tWhat\n");
    WP *tmp = head;
    while(tmp!=NULL)
    {
        printf("%d\t%s\n",tmp->NO,tmp->eexpr);
        tmp = tmp->next;
    }
}
bool check_wp(){  //监视点里某个变动了，返回true
    WP *tmp = head;
    bool had_changed = false;
    while(tmp!=NULL)
    {
        bool *success = false;
        uint32_t new_val = expr(tmp->eexpr,success); 
        if(new_val != tmp->init)
        {
            printf("Watchpoint %d: %s\n",tmp->NO,tmp->eexpr);
            printf("Old value = 0x%08x\nNew value = 0x%08x\n",tmp->init,new_val);
            tmp->init = new_val;//赋上新值
            had_changed = true; //每一个监视点的变化都要输出
        }
        tmp = tmp->next;
    }
    if(had_changed)
        return true;
    return false;
}
