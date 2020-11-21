#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "fileList.h"
#include <ctype.h>


char* readInFile(char* fileName)
{
   char* buffer = (char* )(malloc(sizeof(char)));
   buffer[0] = '\0';
    char c;
    int fd = open(fileName,O_RDONLY);
    int status;
    int counter = 0;
    if (fd!=-1){
        do{
                status =  read(fd, &c, 1);
                if (status<=0){
                    break;
                }
                else{   
                    int len = strlen(buffer);
                    buffer =  realloc(buffer,(len+ 2)*sizeof(char));
                    buffer[len] = c;
                    buffer[len+1] = '\0';
                }
            }while(status >0);
         
            close(fd);
        return buffer; 
    }
    printf("Cannot open the file / File was deleted\n");
    char*dne = "DNE\0";
    return dne;
}
wordNode* newNode(char* value, int i){   
    wordNode *res=malloc(sizeof(wordNode)); 
    res->text=value;   
    res->occurrence=i;
	res->next=NULL;
    return res;
}
void printList(wordNode *head){   
    wordNode *ptr=head;   

    while(ptr!=NULL){   
        printf("%s-->",ptr->text);   
        ptr=ptr->next;    
        
    }
    printf("\n");
}
void end(wordNode *head)
{
    wordNode *p,*q;
    p=malloc(sizeof(wordNode));
    p->next=NULL;
    q=head;
    while(q->next!=NULL)
    {
        q = q->next;
    }
    q->next = p;
}
void word_tok(char *input){
	char* buff = (char*)malloc(sizeof(char));
	int i = 0, inputLength = strlen(input);
	wordNode *ptr=(wordNode *)malloc(sizeof(wordNode *));
	wordNode *head=(wordNode *)malloc(sizeof(wordNode *));
	wordNode *headOG=(wordNode *)malloc(sizeof(wordNode *));
	while( i < inputLength){
		while(isspace(input[i]) || input[i]=='\n' && i < inputLength){
			i++;
		}
		if (i >= inputLength){
			break;
		}
		if(isalnum(input[i])){
			strcpy(buff, "");
			while(isalnum(input[i]) || input[i] == '-'){
				strncat(buff, input + i, 1);
				i++;
			}
		}
        head=newNode(buff,1); 
        end(head);
        printList(head);
	}
}

int main(int argc, char **argv){
	char* f = (char*)malloc(sizeof(char));
    f=readInFile(argv[1]);
	word_tok(f);
 
}

   
