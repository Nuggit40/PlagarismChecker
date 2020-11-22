#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "fileList.h"
#include <ctype.h>
#include <errno.h>

void func(fileNode* f){
    wordNode* wn = f->wordList;
    if(wn != NULL){
        while(wn != NULL){
            printf("%s\t%d\n", wn->text, wn->occurrence);
            wn = wn->next;
        }
    }
}
char* readInFile(fileNode* file)
{
    char* fileName = file->path;
    char* buffer = (char* )(malloc(sizeof(char)));
    buffer[0] = '\0';
    char c;
    int fd = open(fileName,O_RDONLY);
    //printf("%s\n", strerror(errno));
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



void word_tok(char *input, fileNode* currentFile){
    //input = "hello hello hi hi";
    int num_toks = 0;
    int index = 0;
    //convert all lowercase letters to uppercase
    while(index < strlen(input)){
        if(isalpha(input[index])){
            input[index] = toupper(input[index]);
        }
        ++index;
    }
    index = 0;
    while(index < strlen(input)){
        int startIndex = index;
        int endIndex = index;
        //skips whitespace and punctuation
        if((isspace(input[index]) || ispunct(input[index])) && input[index] != '-'){
            ++index;
            continue;
        }
        char* c = input + startIndex;
        //while reading valid word chars
        while(isalpha(input[endIndex]) || input[endIndex] == '-'){
            ++endIndex;
        }
        //copying the array contents into curWord->txt
        int tokenLength = endIndex - startIndex;
        char* str = (char*)malloc(sizeof(char)*tokenLength + 1);
        memcpy(str, &input[startIndex], tokenLength);
        str[tokenLength] = '\0';
        wordNode* curWord = (wordNode*)malloc(sizeof(wordNode));
        memcpy(curWord->text, str, tokenLength+1);
        free(str);

        //if the wordlist is empty then this is the first entry
        if(currentFile->wordList == NULL){
            //printf("%s is the first entry\n", curWord->text);
            currentFile->wordList = curWord;
            curWord->occurrence = 1;
            curWord->next = NULL;
        } else {
            wordNode* c = currentFile->wordList;
            while(c->next != NULL){
                if(strcmp(curWord->text, c->text) == 0){
                    c->occurrence++;
                    free(curWord);
                    break;
                }
                c = c->next;
            }
            if(c->next == NULL){
               if(strcmp(curWord->text, c->text) == 0){
                    c->occurrence++;
                    free(curWord);
                } else {
                    c->next = curWord;
                    curWord->occurrence = 1;
                }
            }
        }
        //printf("startindex:%d\tendIndex:%d\t\n",startIndex,endIndex);
        //printf("tokenlength of %s:%d\n", currentFile->path,tokenLength);
        //printf("token is: %s\n", curWord->text);
        index += tokenLength;
    }
}
int main(int argc, char **argv){
	fileNode* fnode;
    strcpy(fnode->path, "testdir/a.txt");
    //printf("%s\n", strerror(errno));
    char* buff = readInFile(fnode);
    //printf("%s\n", buff);
	word_tok(buff, fnode);
    func(fnode);
}

   
