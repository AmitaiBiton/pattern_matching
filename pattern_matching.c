#include "slist.h"
#include"pattern_matching.h"
#include<stdlib.h>
#include <unistd.h>
#include <string.h>
#include<stdio.h>
int pm_state_init(pm_state_t *,pm_int_t id,pm_int_t depth, pm_state_t * fail);
int count_state=0;//couner of state in pm
int pm_state_init(pm_state_t * state , pm_int_t id,pm_int_t depth, pm_state_t * fail ){
    
    state->output =(slist_t*) malloc(sizeof(slist_t));
    if(state->output==NULL)
        return -1;
    slist_init(state->output);
    state->_transitions= (slist_t*)malloc(sizeof(slist_t));
    if(state->_transitions==NULL)
        return -1;
    slist_init(state->_transitions);
    state->id=id;
    state->depth=depth;
    state->fail=fail;

    return 0;
}

int pm_init(pm_t * pm){
    pm_state_t* zero_state =(pm_state_t*) malloc(sizeof(pm_state_t));
    if(zero_state==NULL)
        return -1;
        
    int root = pm_state_init(zero_state ,0,0,NULL);
    if(root==-1){
        free(zero_state);
        return -1;
    }
    pm->zerostate=zero_state;
    pm->newstate=1;
    return 0;

}

pm_state_t* pm_goto_get(pm_state_t *state, unsigned char symbol){
    if(state==NULL)// no state to start
        return NULL;
    
    if(symbol=='\0')// no char 
        return NULL;
    
    if(state->_transitions==NULL)//** have no sons
        return NULL;
    slist_node_t* temp = slist_head(state->_transitions);//head to the sons 
    int i=0;
    pm_labeled_edge_t* temp_edg;
    while(i<slist_size(state->_transitions)){//
        temp_edg=(pm_labeled_edge_t*)slist_data(temp);// get the edge from data on the state
        if(temp_edg->label==symbol){
            return temp_edg->state;//get the next state fron edge 
        }
        temp=slist_next(temp);
        i++;
    }
    return NULL;
}
int pm_goto_set(pm_state_t *from_state,unsigned char symbol,pm_state_t *to_state){
    if(from_state==NULL)
        return -1;
    if(to_state==NULL)
        return -1;
    pm_labeled_edge_t * new_edge = (pm_labeled_edge_t*)malloc(sizeof(pm_labeled_edge_t));// new wdge
    if(new_edge==NULL)
        return -1;
    new_edge->label=symbol;//=get the char
    new_edge->state=to_state;// edge-> state is the final state 
    to_state->depth=from_state->depth+1;// get the depth
    int append_success_transitions=0;
    append_success_transitions=slist_append(from_state->_transitions,new_edge);// add to list of transition for from state the edge
    printf( "Allocating state %d\n",count_state);
    printf("%d -> %c ->  %d\n",from_state->id,symbol,to_state->id);
    return append_success_transitions;
}
int pm_addstring(pm_t * pm,unsigned char * pattern, size_t n){
    if(pattern==NULL)
        return -1;
    if(pm==NULL)
        return -1;
    if(strlen(pattern)!=n){
        printf("in addstring you put string and size not equle");
        return -1;
    }
    pm_state_t* state = pm->zerostate;
    int i=0;
    /*if we allrady have the string on the pm */
    while(pm_goto_get(state,pattern[i])!=NULL && i<n){
        state=pm_goto_get(state,pattern[i]);
        i++;
    }
    /*need to check if  i=n so we have the strinf we need only to update the output */
    if(i==n){
        slist_append(state->output,pattern);
        return 0;
    }
        
    int check=0;
    /* we dont have complite the string OR we dont have any char from the string*/
    while(i<n){
            if(slist_size(state->_transitions)>PM_CHARACTERS)
                return -1;
            pm_state_t *newstate = (pm_state_t*) malloc(sizeof(pm_state_t));
            if(newstate==NULL)
                return -1;
            pm_state_init(newstate,pm->newstate,i,NULL);// create the new state
            count_state=pm->newstate;// update the counter
            check = pm_goto_set(state,pattern[i],newstate);//create the edge between two state
            if(check==-1){// if set fail so the new state fail so need to free
                free(newstate->output);
                free(newstate->_transitions);
                free(newstate);
                return check;
            }
                
            state = newstate;
            pm->newstate++;
            i++; 
    }
    check = slist_append(state->output,pattern);//update the output to the final state 
    return check;
}
 
int pm_makeFSM(pm_t * fsm ){
    
    if(fsm==NULL)
        return -1;
    slist_t* Queue =(slist_t*)malloc(sizeof(slist_t));// create Q 
    if(Queue==NULL)
        return -1; 
    slist_init(Queue);
    pm_state_t* pointer_sons; 
    slist_node_t* pointer_transitions = slist_head(fsm->zerostate->_transitions);//pointer to son 
    int check = 0;
    /*put the transitions for  zerostate*/
    for(int i=0;i<slist_size(fsm->zerostate->_transitions);i++){
        //get the sins with goto
        pointer_sons = pm_goto_get(fsm->zerostate,((pm_labeled_edge_t*)slist_data(pointer_transitions))->label);
        //put the sons to Q
        check = slist_append(Queue,pointer_sons);
        if(check ==-1)
            return -1;
        //update the fail=null to zerostate
        pointer_sons->fail=fsm->zerostate;
        printf("Setting f(%d) = %d\n",pointer_sons->id,fsm->zerostate->id);
        // going to next node on the list 
        pointer_transitions=slist_next(pointer_transitions);
    }
    pm_state_t* father_state;
    pm_state_t* son_state; 
    while(slist_head(Queue)!=NULL){//the list is not empty
        pointer_transitions = slist_head(Queue);//pointer to one son of zerostate
        father_state = (pm_state_t*)slist_pop_first(Queue);//pop the first sons
        if(father_state==NULL)
            return -1; 
        pm_labeled_edge_t* edge;
        /*pointer to the sons transitions */
        slist_node_t *father_transitions = slist_head(father_state->_transitions);
        /* put the sons transitions to Q*/
        for(int i=0;i<slist_size(father_state->_transitions);i++){
            edge =slist_data(father_transitions);
            son_state=pm_goto_get(father_state,edge->label);//get the state that have edge between the F and S
            check = slist_append(Queue,son_state);
            if(check==-1)
                return -1;
            pm_state_t* father_state_fail = father_state->fail;
            /*take pointer and search the state that have edge with b */
            while(pm_goto_get(father_state_fail,edge->label)==NULL){
                if(father_state_fail->fail==NULL){//keep looking
                    son_state->fail=fsm->zerostate;
                    break;
                }
                else{//find so update the the fail
                    father_state_fail=father_state_fail->fail;
                }
            }
            pm_state_t* tmp_fail;
            if(pm_goto_get(father_state_fail ,edge->label)!=NULL){//find the state 
                tmp_fail=pm_goto_get(father_state_fail ,edge->label);
                son_state->fail=tmp_fail;//update the failuer state
                if(slist_head(tmp_fail->output)!=NULL){
                    /*update the output for S from failstate of S */
                        check = slist_append_list(son_state->output,son_state->fail->output);//
                        if(check==-1)
                            return -1;
                }
            }
            else{
                //dont find any failstate so you need to update to zerostate
                son_state->fail=fsm->zerostate;
                
            }
            printf("Setting f(%d) = %d\n",son_state->id,son_state->fail->id);
            father_transitions=slist_next(father_transitions);
        }    
    }  
    slist_destroy(Queue,SLIST_LEAVE_DATA);
    free(Queue);
    return 0;
}			   
slist_t* pm_fsm_search(pm_state_t * state,unsigned char * string ,size_t length){
    
    if(state==NULL)
        return NULL;
    if(string==NULL)
        return NULL;
    if(strlen(string)!=length){
        printf("you need to check F-Search the size of string it's a mistake");
        return NULL;
    }
    slist_t* match_list =(slist_t*) malloc(sizeof(slist_t));
    if(match_list==NULL)
        return NULL;
    slist_init(match_list);
    pm_state_t* tmp_state = state;
   
    slist_node_t* tmp_node;
    /*pol over the strinfg and search th state have the char ot any char*/
    for(int i=0; i<length;i++){ 
        /*dont find any char in pm keep looking on pm*/
        while(pm_goto_get(tmp_state,string[i])==NULL && (i<length)){
            if(tmp_state->id!=0){
                tmp_state = tmp_state->fail;
            }
            else{
                i++;
            } 
        }
        /*begin on matching */
        tmp_state=pm_goto_get(tmp_state,string[i]);
        if(tmp_state==NULL){
            break;
        }
        /* if output >0 so maybe we have matching*/
        if(slist_size(tmp_state->output)>0){
            tmp_node = slist_head(tmp_state->output);
            /*going true the list of output to get any match*/
            while(tmp_node!=NULL){
                int size =strlen((char*)slist_data(tmp_node));
                pm_match_t* match = (pm_match_t*)malloc(sizeof(pm_match_t));
                if(match==NULL)
                    return NULL;
                //    create match  
                match->start_pos=i-size+1;//start
                match->end_pos =i;//end
                //=(char*) malloc(sizeof(char)*size);
                match->pattern = (char*)slist_data(tmp_node);
                match->fstate = tmp_state;
                //add match to list_matching 
                slist_append(match_list,match);
                printf("pattern: %s,start at:%d,end at:%d,last state = %d\n",
                match->pattern,match->start_pos,match->end_pos,match->fstate->id);
                tmp_node=slist_next(tmp_node);
            }        
        } 
    }
    return match_list;  
}

void pm_destroy(pm_t * pm){
    if(pm==NULL)
        return ;
        /*list to old the states like Q*/
    slist_t* free_data =(slist_t*) malloc(sizeof(slist_t));
    if(free_data==NULL)
        return;
    int check =0;
    slist_init(free_data);
    pm_state_t* state = pm->zerostate;
    slist_node_t* tmp = slist_head(state->_transitions);//pointer to transitions of zerostate
    for(int i=0;i<slist_size(state->_transitions);i++){//pol over to transitions of zero state
        check= slist_append(free_data, ((pm_labeled_edge_t*) slist_data(tmp))->state);//add sons to Q
        if(check==-1)
            return ;
        tmp=slist_next(tmp);
    }
    pm_state_t *f_state;
    pm_state_t *s_state;
    /*after  we get the sons we want to add the sons of sons to Q*/
    while(slist_size(free_data)>0){ 
        f_state=(pm_state_t*)slist_pop_first(free_data);//get the first
        if(slist_size(f_state->_transitions)>0){//if have any edge so need to 
            slist_node_t* tmp= slist_head(f_state->_transitions);
            int j=0;  
            while(j<slist_size(f_state->_transitions)){
                s_state= ((pm_labeled_edge_t*) slist_data(tmp))->state;
                check=slist_append(free_data, s_state);
                if(check==-1)
                    return;
                j++;
                tmp=slist_next(tmp);
            } 
        }    
        slist_destroy(f_state->output,SLIST_LEAVE_DATA);
        free(f_state->output);
        slist_destroy(f_state->_transitions,SLIST_FREE_DATA);
        free(f_state->_transitions);
        f_state->fail=NULL;
        free(f_state);    
    }
    slist_destroy(pm->zerostate->output, SLIST_LEAVE_DATA);
    free(pm->zerostate->output);
    slist_destroy(pm->zerostate->_transitions, SLIST_FREE_DATA);
    free(pm->zerostate->_transitions);
    pm->zerostate->fail = NULL;
    free(pm->zerostate);
    slist_destroy(free_data, SLIST_LEAVE_DATA);
    free(free_data);
}
