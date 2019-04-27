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
    if(free_->NO == 0){
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
    while(wp->next!=NULL)
    {
        if(wp->NO == N)
            break;
        wp = wp->next;
    }
    if(head == wp)
        head = head->next;
    else
    {
        WP* tmp = head;
        while(tmp->next!=NULL)
        {
            if(tmp->next == wp){
                tmp->next = wp->next;
                break;
            }
            tmp = tmp->next;
        }
    }

    if(wp->NO<free_->NO)
    {
        wp->next = free_;
        free_ = wp;
    }
    else
    {
        WP *tmp1 = free_;
        while(tmp1->next!=NULL){
            if(tmp1->next->NO>wp->NO)
            {
                wp->next = tmp1->next;
                tmp1->next = wp;
            }
            tmp1 = tmp1->next;
        }
    }
}

