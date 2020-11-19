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

meanConstruction getMeanProb(fileNode file1, fileNode file2){

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
}