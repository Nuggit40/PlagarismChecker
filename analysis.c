#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "fileList.h"

#define CHANGE_RED printf("\033[0;31m");
#define CHANGE_YELLOW printf("\033[0;33m");
#define CHANGE_GREEN printf("\033[0;32m");
#define CHANGE_CYAN printf("\033[0;36m");
#define CHANGE_BLUE printf("\033[0;34m");
#define RESET_COLOR printf("\033[0m");


typedef struct _meanConstruction {
    char* text;
    double mean;
    struct _meanConstruction* next;
} meanConstruction;
void printList(fileNode *head){   
    //first entry in the list is NULL
    fileNode *ptr=head->next;
        while(ptr != NULL){   
            printf("\t%s\ttotal tokens:%d\n",ptr->path, ptr->wordCount);
            //print out wordlist
            wordNode* wn = ptr->wordList;
                while(wn != NULL){
                    printf("\t\t%s\toccurrances:%d\tprob:%f\n", wn->text, wn->occurrence, wn->probability);
                    wn = wn->next;
                }
            ptr=ptr->next;    
        }
}

void getJensenProb(fileNode* file1, fileNode* file2){
    wordNode * currWord1=file1->wordList;
    wordNode*  currWord2=file2->wordList;
    meanConstruction* meanList = (meanConstruction*)malloc(sizeof(meanConstruction));
    // first loop runs through second list given in
    while(currWord2!=NULL ){
        //second loop one by one compares the parts from the first list to the second list
        while(currWord1!=NULL){
            // checks if the second list is empty
            if(currWord2 ==NULL){
                    break;
            }else if(currWord1->text==currWord2->text){ // checks to see if the they are the same word
                meanList->text=currWord1->text;
                meanList->mean=((currWord1->probability+currWord2->probability)/2);
                currWord1=currWord1->next;
                currWord2=currWord2->next;

            }else{ //if word not in list 
                meanList->text=currWord1->text;
                meanList->mean=((currWord1->probability)/2);
                currWord1=currWord1->next;
                currWord2=currWord2->next;
            }
        }
    }
    meanList=meanList->next;
    printList(meanList);

}



int main(){
    fileNode b;
    wordNode hi_b = {"hi", 0, 0.5, NULL};
    wordNode there_b = {"there", 0, 0.5, NULL};
    hi_b.next = &there_b;
    b.wordList = &hi_b;

    wordNode* cur = b.wordList;
    while(cur != NULL){
        printf("%s\n", cur->text);
        cur = cur->next;
    }
    CHANGE_RED
    printf("red\n");
    CHANGE_YELLOW
    printf("yellow\n");
    CHANGE_GREEN
    printf("green\n");
    CHANGE_CYAN
    printf("cyan\n");
    CHANGE_BLUE
    printf("blue\n");
    RESET_COLOR
    getJensenProb("","");
}
