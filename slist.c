#include<stdio.h>
#include"slist.h"
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include"pattern_matching.h"
void slist_init(slist_t * list){
    slist_head(list)=NULL;
    slist_tail(list)=NULL;
    slist_size(list)=0;

}
void slist_destroy(slist_t * list ,slist_destroy_t dea_t){
    
    slist_node_t* temp; 
    
    while(slist_size(list)>0){// while you not poined to null 
        if(dea_t==0){//leave data
            temp=slist_head(list);
            slist_head(list)= slist_next(slist_head(list));// head pointed to the next and we can free the node
            free(temp);
            slist_size(list)--;
            }
        else{// need to free the data 
            temp=slist_head(list);
            slist_head(list)= slist_next(slist_head(list));
            free(slist_data(temp));
            free(temp);
            slist_size(list)--;
            }
    } 
      
    slist_head(list)=NULL;
    slist_tail(list)=NULL;
    slist_size(list)=0; 
    
   
}
void *slist_pop_first(slist_t * list){
    if(slist_head(list)==NULL){
        printf("the list is empty and you cant pop the first on list\n");
       return NULL;
    }
    void * temp_data;//void * can undel the data on head data on list
    slist_node_t* temp_node;//mode temporery 
    temp_node=slist_head(list);//point to the head
    slist_head(list)= slist_next(slist_head(list));// head point to next
    temp_data=slist_data(temp_node);//temp data get the data from temp
    free(temp_node);// free the node 
    slist_size(list)--;
    if(slist_size(list)==0){
        slist_head(list)=NULL;
        slist_tail(list)=NULL;
    }
    return temp_data;
}
int slist_append(slist_t *list ,void *data){
    slist_node_t* new_node = (slist_node_t*)malloc(sizeof(slist_node_t)*1);
    if(new_node==NULL)
        return -1;
    slist_data(new_node)=data;//new node get the data 
    slist_next(new_node)=NULL;
    if(slist_head(list)==NULL){// list is empty
        slist_head(list)=new_node;// list->head ponter to new node
        slist_tail(list)=new_node;
        slist_size(list)++;// size of list +1
        return 0;
    }
    else{//list not empty
        slist_next(slist_tail(list))=new_node;//tail->next pointer to new node
        slist_tail(list)=slist_next(slist_tail(list));// the tail pointer to next 
        slist_size(list)++;
        return 0;
    }
    return -1;
}
int slist_prepend(slist_t *list ,void *data){
    slist_node_t* new_node = (slist_node_t*)malloc(sizeof(slist_node_t)*1);
    if(new_node==NULL)
        return -1;
    slist_data(new_node)=data;//new node get the data 
    if(slist_size(list)==0){// the list is empty
        slist_head(list)=new_node;// head= new_node
        slist_tail(list)=new_node;
        slist_size(list)++;
        return 0;
    }
    else{
        slist_next(new_node)=slist_head(list);//new_node->next=head 
        slist_head(list)=new_node;//head = new_node
        slist_size(list)++;
        return 0;
    }
    return -1;
}
int slist_append_list(slist_t* slist_dest, slist_t* slist_src){
    if(slist_src==NULL)
        return 0;
    if(slist_dest==NULL)
        return -1;
    int count =slist_size(slist_src);// number of node on the src
    int append_sucses;
    slist_node_t* temp_src = slist_head(slist_src);//pointer to the hesd src
    while(count>0){
        append_sucses=slist_append(slist_dest,slist_data(temp_src));//add to the sest list from src
        if(append_sucses==-1)
            return -1;
        temp_src=slist_next(temp_src);
        count--;
    }
    return 0;
}
