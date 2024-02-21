/* Toy synchronizer: Sample template
 * Your synchronizer should implement the three functions listed below. 
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "pe_sync.h"
#include <sys/queue.h>

struct tree_node** roots;
int numberOfSubExpressions;

//if op_name is in a return 1, else 0
int contains(char* a, const char* op_name){
  int i = 0;
  int j = 0;
  
  while(1){
    if(op_name[j] == '\0'){
      return 1;

    }
    
    if(a[i] == '\0')
      return 0;

    if(a[i] == op_name[j]){
      i++;
      j++;
    }
    else{
      j = 0;
      i++;
    }
  }
}


//Binary search to find the leaf node for the given op_name
//Use the contains method written above which checks if the op_name is in the left child or the right child
//Recursively search
struct tree_node* find_node(struct tree_node* cur, const char* op_name){
  if(strcmp(cur->path_expr, op_name) == 0){
    return cur;
  }
  if(cur->isChild == 1)
    return NULL;
  
  if(contains(cur->leftChild->path_expr, op_name))  
    return find_node(cur->leftChild, op_name);
  else if(contains(cur->rightChild->path_expr, op_name))
    return find_node(cur->rightChild, op_name);
  else
    return NULL;
}

//Find the leaf node corresponding to given op_name and apply P and PP according to the prologue
void ENTER_OPERATION(const char *op_name)
{
  struct tree_node* leaf;
  for(int i = 0; i < numberOfSubExpressions; i++){
    leaf = find_node(roots[i], op_name);
    if(leaf != NULL)
      break;
    
  }
  
  if (leaf->prologue->op == P){
    if(strcmp(op_name, "CaregiverLeave\0") == 0)
      sem_wait(&noOfCaregivers);
    sem_wait(leaf->prologue->sem_list->sem);
  }
  else if (leaf->prologue->op == PP){
    sem_wait(leaf->prologue->sem_list->sem);

    *(leaf->prologue->count) += 1;
    if(*(leaf->prologue->count) == 1){
      sem_wait(leaf->prologue->sem_list->next->sem);
      if(strcmp(op_name, "ChildArrive\0") == 0)
        sem_wait(&noOfCaregivers);
    }

    sem_post(leaf->prologue->sem_list->sem);
  }
  else if(leaf->prologue->op == L){
    pthread_mutex_lock(leaf->prologue->lock);

    while(*(leaf->prologue->count) == leaf->prologue->number)
      pthread_cond_wait(leaf->prologue->cond, leaf->prologue->lock); 
    
    *(leaf->prologue->count) += 1;
    if(*(leaf->prologue->count) == 1)
      sem_wait(leaf->prologue->sem_list->sem);

    pthread_mutex_unlock(leaf->prologue->lock);       
  }
}

//Find the leaf node corresponding to given op_name and apply V and VV according to the prologue
void EXIT_OPERATION(const char *op_name)
{
  struct tree_node* leaf;
  for(int i = 0; i < numberOfSubExpressions; i++){
    leaf = find_node(roots[i], op_name);
    if(leaf != NULL)
      break;
  }
  
  if (leaf->epilogue->op == V){
    if(strcmp(op_name, "CaregiverArrive\0") == 0) 
      sem_post(&noOfCaregivers);
    sem_post(leaf->epilogue->sem_list->sem);
  }
  else if (leaf->epilogue->op == VV){
    sem_wait(leaf->epilogue->sem_list->sem);
    *(leaf->epilogue->count) -= 1;
    if(*(leaf->epilogue->count) == 0){
      sem_post(leaf->epilogue->sem_list->next->sem);
      if(strcmp(op_name, "ChildLeave\0") == 0)
        sem_post(&noOfCaregivers);
      
    }
    
    sem_post(leaf->epilogue->sem_list->sem);
  }
  else if(leaf->epilogue->op == U){
    
    pthread_mutex_lock(leaf->epilogue->lock);
    *(leaf->epilogue->count) -= 1;
    
    if(*(leaf->epilogue->count) == 0)
      sem_post(leaf->epilogue->sem_list->sem);
    else
      pthread_cond_broadcast(leaf->epilogue->cond);

    pthread_mutex_unlock(leaf->prologue->lock);       
  }
}

void deep_copy_oper(struct oper* src, struct oper* dest){
  dest->op = src->op;
  dest->sem_list = malloc(sizeof(struct sem_node));
  dest->sem_list->sem = src->sem_list->sem;
  dest->sem_list->sem_number= src->sem_list->sem_number;

  if(src->op == L || src->op == U){
    dest->number = src->number;
    dest->lock = src->lock;
  }
  else if(src->op == PP || src->op == VV){
    dest->count = src->count;
    dest->sem_list->next = malloc(sizeof(struct sem_node));
    dest->sem_list->next->sem = src->sem_list->next->sem;
    dest->sem_list->next->sem_number = src->sem_list->next->sem_number;        
  }
}

//This division method applies the 4 possible rules described in 
//THE SPECIFICATION OF PROCESS SYNCHRONIZATION BY PATH EXPRESSIONS by R.H. Campbell
//There are 4 possible rules. A sequence, a selection, simultaneous execution and procedure name.
//1. Sequence is in the form <path expression 1> ; <path expression 2>
//2. Selection is in the form <path expression 1> + <path expression 2>
//3. Simultaneous Execution is in the form { <path expression> }
//4. Procedure name corresponds to the operation name (No action is required)

//assuming there aren't any path expressions in the form {{{{}}}}
void division(struct tree_node* cur){
  //search for operations ;+{}
  //while searching first take the one that is not inside () {}
  //if that doesn't exist than handle () or {} separately

  int index = 0;
  int leftParenthesesFound = 0;
  int rightParenthesesFound = 0;
  int leftCurlyBracesFound = 0;
  int rightCurlyBracesFound = 0;
  int slashFound = 0;
  int backslashFound = 0;

  cur->isChild = 0;

  for(; index < cur->str_length; index++){
    if(cur->path_expr[index] == '\0')
      break;
    
    if(cur->path_expr[index] == ';' || cur->path_expr[index] == '+'){
      if(leftParenthesesFound == rightParenthesesFound && leftCurlyBracesFound == rightCurlyBracesFound && slashFound == backslashFound)
        break;
    }

    if(cur->path_expr[index] == '(')
      leftParenthesesFound++;
    else if(cur->path_expr[index] == ')')
      rightParenthesesFound++;
    else if(cur->path_expr[index] == '{')
      leftCurlyBracesFound++;
    else if(cur->path_expr[index] == '}')
      rightCurlyBracesFound++;
    else if(cur->path_expr[index] == '/')
      slashFound++;
    else if(cur->path_expr[index] == '\\')
      backslashFound++;
  }

  //If we didn't reach the end of the string this means that we found either ; or +.
  if(cur->path_expr[index] != '\0'){
    cur->isChild = 0;

    //divide the node and split the expression
    struct tree_node* leftChild = malloc(sizeof(struct tree_node)); 
    struct tree_node* rightChild = malloc(sizeof(struct tree_node));

    cur->leftChild = leftChild;
    cur->rightChild = rightChild;

    leftChild->path_expr = malloc(sizeof(char)*(index + 1));//put an extra \n
    rightChild->path_expr = malloc(sizeof(char)*(cur->str_length - index - 1));

    //split the string
    memcpy(leftChild->path_expr, cur->path_expr, index);
    leftChild->path_expr[index] = '\0';
    leftChild->str_length = index + 1;

    memcpy(rightChild->path_expr, cur->path_expr + index + 1, cur->str_length - index - 1);
    rightChild->str_length = cur->str_length - index - 1;


    leftChild->prologue = malloc(sizeof(struct oper));
    leftChild->epilogue = malloc(sizeof(struct oper));
    rightChild->prologue = malloc(sizeof(struct oper));
    rightChild->epilogue = malloc(sizeof(struct oper));

    //The expression is in the sequence form.
    //Create a new semaphore S2 initialized to 0,
    //Replace <path expression 1> ; <path expression 2> with these 2 expressions
    //<path expression 1> V(S2) and P(S2)<path expression 2>
    if(cur->path_expr[index] == ';'){
      //create the prologue and epilogues for the left and right child
      //left child's prologue is same as the current node's prologue (apply deep copy)
      deep_copy_oper(cur->prologue, leftChild->prologue);
      
      leftChild->epilogue->sem_list = malloc(sizeof(struct sem_node));
      leftChild->epilogue->sem_list->next = NULL;
      leftChild->epilogue->sem_list->sem = malloc(sizeof(sem_t));
      sem_init(leftChild->epilogue->sem_list->sem, 0, 0);
      leftChild->epilogue->op = V;

      rightChild->prologue->sem_list = malloc(sizeof(struct sem_node));
      rightChild->prologue->sem_list->next = NULL;
      rightChild->prologue->sem_list->sem = leftChild->epilogue->sem_list->sem;
      rightChild->prologue->op = P;

      deep_copy_oper(cur->epilogue, rightChild->epilogue);
    }
    else if(cur->path_expr[index] == '+'){
      //The expression is in the selection form.
      //Replace OL <path expression 1> + <path expression 2> OR with these 2 expressions
      //<path OL expression 1> OR and OL P(S2)<path expression 2> OR
      //apply deep copy operations
      
      deep_copy_oper(cur->prologue, leftChild->prologue);
      deep_copy_oper(cur->epilogue, leftChild->epilogue);
      deep_copy_oper(cur->prologue, rightChild->prologue);
      deep_copy_oper(cur->epilogue, rightChild->epilogue);
    }

    division(leftChild);
    division(rightChild);
  }
  else{
    //the expression has parenthesis or braces in the left and right side or slash or a procedure name

    //if its in the form (...) remove () and try to parse again
    if(cur->path_expr[0] == '('){
      //remove the left and right parenthesis from the expression. Call the method again
      char* newStr = malloc(sizeof(char)*(cur->str_length - 2));
      memcpy(newStr, cur->path_expr + 1, cur->str_length - 3);
      newStr[cur->str_length - 3] = '\0';
      free(cur->path_expr);
      cur->path_expr = newStr;
      cur->str_length -= 2;

      division(cur);
      return;
    }
    else if(cur->path_expr[0] == '{'){//if its in the form {...} apply the rule
        //The expression is in the simultaneous execution form.
        //create a counter C1 initialized to 0 and a semaphore S3 initialized to 1
        //Replace P(Si) { <path expression> } V(Si) with this expression
        //PP(C1, S3, Si) <path expression> VV(C1, S3, Si)
        
        //remove the left and right braces from the expression. Call the method again
        char* newStr = malloc(sizeof(char)*(cur->str_length - 2));
        memcpy(newStr, cur->path_expr + 1, cur->str_length - 3);
        newStr[cur->str_length - 3] = '\0';
        free(cur->path_expr);
        cur->path_expr = newStr;
        cur->str_length -= 2;


        //prologue (create a new semaphore and a counter)
        //add the semaphore and the counter to the prologue
        struct sem_node* new_sem1 =  malloc(sizeof(struct sem_node));
        new_sem1->next = cur->prologue->sem_list;
        cur->prologue->sem_list = new_sem1;

        cur->prologue->sem_list->sem = malloc(sizeof(sem_t));
        sem_init(cur->prologue->sem_list->sem, 0, 1);
        cur->prologue->op = PP;
        cur->prologue->count = malloc(sizeof(int));
        *(cur->prologue->count) = 0;

        //epilogue (reference the semaphore and the counter created for the prologue)
        struct sem_node* new_sem2 =  malloc(sizeof(struct sem_node));
        new_sem2->next = cur->epilogue->sem_list;
        cur->epilogue->sem_list = new_sem2;
        cur->epilogue->sem_list->sem = cur->prologue->sem_list->sem;

        cur->epilogue->op = VV;
        cur->epilogue->count = cur->prologue->count;
        division(cur);       
    }
    else if(cur->path_expr[0] == '/'){
      //the expression is in the format /#num <path expression> 
      //get the #num
      int i = 1;
      while(cur->path_expr[i] < 48 || cur->path_expr[i] > 57)
        i++;

      char* num = malloc(sizeof(char)*(i+1));
      memcpy(num, cur->path_expr + 1, i);
      num[i] = '\0';


      //remove the left and right slashes
      char* newStr = malloc(sizeof(char)*(cur->str_length - (i+1)));
      memcpy(newStr, cur->path_expr+i+1, cur->str_length - i - 3);
      newStr[cur->str_length - (i+2)] = '\0';
      free(cur->path_expr);
      cur->path_expr = newStr;
      cur->str_length = cur->str_length - (i+1);

      cur->prologue->count = malloc(sizeof(int));
      *(cur->prologue->count) = 0;
      //prologue
      cur->prologue->op = L;
      cur->prologue->lock = malloc(sizeof(pthread_mutex_t));
      cur->prologue->cond = malloc(sizeof(pthread_cond_t));
      pthread_cond_init(cur->prologue->cond, NULL);
      pthread_mutex_init(cur->prologue->lock, NULL);
      cur->prologue->number = atoi(num);


      //epilogue
      cur->epilogue->op = U;
      cur->epilogue->lock = cur->prologue->lock;
      cur->epilogue->cond = cur->prologue->cond;
      cur->epilogue->number = atoi(num);

      cur->epilogue->count = cur->prologue->count;

      division(cur);
    }
    else{//last case. The expression is an operation. Mark the node as a leaf and return.
      cur->isChild = 1;
      return;
    }     
  }
}

void remove_extra_spaces_from_path_expression(char** expr, int expr_index){
  int index = 0;
  int noOfSpaces = 0;
  while(expr[expr_index][index] != '\0'){
    if(expr[expr_index][index] == ' ')
      noOfSpaces++;
    
    index++;
  }

  if(noOfSpaces > 2){
    char* new_expr = malloc(index+1 - (noOfSpaces - 2));
 
    int firstSpace = 4;
    int lastSpace = index - 4;
  
    int i = 0;
    int j = 0;

    while(i <= index){
      if(expr[expr_index][i] == ' ' && (i != firstSpace && i != lastSpace)){
        i++;
      }
      else{
        new_expr[j] = expr[expr_index][i];
        i++;
        j++;
      }
    }
  
    free(expr[expr_index]);
    expr[expr_index] = new_expr;
  }
}


void INIT_SYNCHRONIZER(const char *path_exp)
{ 
  printf("Initializing Synchronizer with path_exp %s\n", path_exp);

  //Count the number of path word in the expression
  numberOfSubExpressions = 0;
  int index = 4;
  while(path_exp[index] != '\0'){
    if(path_exp[index] == ' ' && path_exp[index-1] == 'h' && path_exp[index - 2] == 't' && path_exp[index - 3] == 'a' && path_exp[index - 4] == 'p')
      numberOfSubExpressions++;

    index++;
  }

  roots = malloc(sizeof(struct root*)*numberOfSubExpressions);
  int* startIndexOfExprs = malloc(sizeof(int)*numberOfSubExpressions);
  int index2 = 0;
  index = 0;
  while(path_exp[index] != '\0'){
    if(path_exp[index] == ' ' && path_exp[index-1] == 'h' && path_exp[index - 2] == 't' && path_exp[index - 3] == 'a' && path_exp[index - 4] == 'p')
      startIndexOfExprs[index2++] = index - 4;
    
    index++;
  }

  char** exprs = malloc(sizeof(char*)*numberOfSubExpressions);
  for(int i = 0; i < numberOfSubExpressions - 1; i++){
    exprs[i] = malloc(sizeof(char)*(startIndexOfExprs[i+1] - startIndexOfExprs[i]));
    memcpy(exprs[i], path_exp+ startIndexOfExprs[i], startIndexOfExprs[i+1] - startIndexOfExprs[i] - 1);
    exprs[i][startIndexOfExprs[i+1] - startIndexOfExprs[i] - 1] = '\0';

    //check if expression contains any extra spaces
    remove_extra_spaces_from_path_expression(exprs, i);
  }

  //The last expression is until the end of the string. Handle that case separately.
  int length = 0;
  while(path_exp[length] != '\0')
    length++;

  exprs[numberOfSubExpressions - 1] = malloc(sizeof(char)*(length + 1 - startIndexOfExprs[numberOfSubExpressions-1]));
  memcpy(exprs[numberOfSubExpressions-1], path_exp + startIndexOfExprs[numberOfSubExpressions-1], length - startIndexOfExprs[numberOfSubExpressions-1]);
  exprs[numberOfSubExpressions-1][length - startIndexOfExprs[numberOfSubExpressions-1]] = '\0';

  //check if expression contains any extra spaces
  remove_extra_spaces_from_path_expression(exprs, numberOfSubExpressions - 1);


  for(int i = 0; i < numberOfSubExpressions; i++){
     //Apply the first rule. Replace path and end operations with semaphore operations.
    length = 0;
    while(exprs[i][length] != '\0')
      length++;

    //Initialize the root node.
    roots[i] = malloc(sizeof(struct tree_node));
    roots[i]->prologue = malloc(sizeof(struct oper));
    roots[i]->epilogue = malloc(sizeof(struct oper));

    //Remove path and end keywords.
    roots[i]->path_expr = malloc(length*sizeof(char));
    memcpy(roots[i]->path_expr, exprs[i] + 5, length - 9);
    roots[i]->path_expr[length - 9] = '\0';
    roots[i]->str_length = length-8;

    //Create prologue.
  
    //Applying Stage 1
    //Select a unique semaphore S1 initialized to 1 and 
    //replace the path and end keywords with P(S1) and V(S1) respectively

    //create prologue
    roots[i]->prologue->sem_list = malloc(sizeof(struct sem_node));

    roots[i]->prologue->sem_list->next = NULL;
    roots[i]->prologue->sem_list->sem = malloc(sizeof(sem_t));
    sem_init(roots[i]->prologue->sem_list->sem, 0, 1);

    roots[i]->prologue->op = P;

    //create epilogue 
    //(epilogue uses the same semaphore with prologue so just storing the reference to it is enough)
    roots[i]->epilogue->sem_list = malloc(sizeof(struct sem_node));
    roots[i]->epilogue->sem_list->next = NULL;
    roots[i]->epilogue->sem_list->sem = roots[i]->prologue->sem_list->sem;
    roots[i]->epilogue->op = V;

    //call the recursive division method
    division(roots[i]);  
  }

  //caregiver problem extra semaphore
  sem_init(&noOfCaregivers, 0, 0);
}
