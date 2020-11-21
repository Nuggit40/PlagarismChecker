#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "fileList.h"
#include <ctype.h>

// reads file in and sets it to a buffer
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
//creates a newNode of type wordNode * to be added to the linked list
wordNode* newNode(char* value, int i){   
    wordNode *res=malloc(sizeof(wordNode)); 
    res->text=value;   
    res->occurrence=i;
	res->next=NULL;
    return res;
}
//prints out the linked list, for testing pruposes
void printList(wordNode *head){   
    wordNode *ptr=head;   

    while(ptr!=NULL){   
        printf("%s:%d-->",ptr->text,ptr->occurrence);   
        ptr=ptr->next;    
        
    }
    //printf("\n");
}
void word_tok(char *input){
	char* buff = (char*)malloc(sizeof(char));
    int num_toks=0;
	int i = 0,  j=0, inputLength = strlen(input);
	wordNode *ptr=(wordNode *)malloc(sizeof(wordNode *));
	wordNode *head=(wordNode *)malloc(sizeof(wordNode *));
	wordNode *headOG=(wordNode *)malloc(sizeof(wordNode *));
    fileNode *currFile=(fileNode *)malloc(sizeof(fileNode *));
	while( i < inputLength){
        //if it finds a white space or a new line it goes to the next char
		while(isspace(input[i]) || input[i]=='\n' && i < inputLength){
			i++;
		}
		if (i >= inputLength){
			break;
		}
        //if its an alphanumeric it adds it to the buffer
		if(isalnum(input[i])){
			strcpy(buff, "");
			while(isalnum(input[i]) || input[i] == '-' && input[i]!=' '){
				strncat(buff, input + i, 1);
				i++;
			} 
		}
        //creates a new node every time a alphanumeric is tokenized
         ptr=newNode(buff,1);
         //adds the pointer to the word list
         currFile->wordList=ptr;
         num_toks++;
         currFile->wordCount=num_toks;
         //tries to compare the ptr to wordList (DOESN"T WORK)
        if(currFile->wordList == ptr){
             j++;
             ptr->occurrence= j;
         }else {
          //traverse until we find the last file in the list
          fileNode* last ;
        while(last->next != NULL){
            last = last->next;
        }
        last->next = currFile;
    }
        printList(ptr);
          

    }
    printf("\nNumber of tokens: %d\n", currFile->wordCount);
    
    
        
	
}

int main(int argc, char **argv){
	char* f = (char*)malloc(sizeof(char));
    f=readInFile(argv[1]);
	word_tok(f);
 
}

   
