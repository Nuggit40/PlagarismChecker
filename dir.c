#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "fileList.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
void* file_handling(void* arg);
void* directory_handling(void* arg);
void printList(fileNode* head); 

// reads file in and sets it to a buffer
char* readInFile(fileNode* file)
{
    int fd = open(file->path,O_RDONLY);
    if(fd == -1){
        printf("Cannot open the file %s\n", file->path);
        return "DNE\0";
    }
    int fileSize = lseek(fd, 0, SEEK_END);
    //setting cursor to start of file
    lseek(fd, 0, SEEK_SET);
    char* buffer = (char*)malloc(sizeof(char) * fileSize);
    int status;
    int offset = 0;
    do {
        status = read(fd, buffer + offset, fileSize - offset);
        if(status > 0 && status < fileSize){
            offset += status;
        }

    } while (status > 0);
    close(fd);
    return buffer;
}

void swapWordData(wordNode* word1, wordNode* word2){

}

void word_tok(char *input, fileNode* currentFile){
    int num_toks = 0;
    int index = 0;
    //convert all uppercase letters to lowercase
    while(index < strlen(input)){
        if(isalpha(input[index])){
            input[index] = tolower(input[index]);
        }
        ++index;
    }
    index = 0;
    while(index < strlen(input)){
        int startIndex = index;
        int endIndex = index;
        //skips whitespace and punctuation
        if((isspace(input[index]) || (ispunct(input[index])) && input[index] != '-') || isdigit(input[index])){
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
        
        ++num_toks;
        //if the wordlist is empty then this is the first entry
        if(currentFile->wordList == NULL){
            currentFile->wordList = curWord;
            curWord->occurrence = 1;
            curWord->next = NULL;
        } else {
            wordNode* c = currentFile->wordList;
            while(c->next != NULL){
                //if the current token is in the list, free the current token and increment the existing word's occurrence
                if(strcmp(curWord->text, c->text) == 0){
                    ++c->occurrence;
                    free(curWord);
                    break;
                }
                c = c->next;
            }
            //checking the last element in the list
            if(c->next == NULL){
               if(strcmp(curWord->text, c->text) == 0){
                    c->occurrence++;
                    free(curWord);
                } else {
                    c->next = curWord;
                    curWord->next = NULL;
                    curWord->occurrence = 1;
                }
            }
        }
        index += tokenLength;
    }
    currentFile->wordCount = num_toks;
}

//prints out linked list
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
void printMeanList(meanConstruction *head){   
    //first entry in the list is NULL
    meanConstruction *ptr=head->next;
        while(ptr != NULL){   
            printf("\t%s\tmean:%f\n",ptr->text, ptr->mean);
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
        meanList=meanList->next;
    }
    
    printMeanList(meanList);

}


typedef struct _threadArg {
    char path[256];
    pthread_mutex_t* lock;
    fileNode* flist;
} threadArg;

//does directory_handling
void* directory_handling( void* arg ){
        threadArg* args = (threadArg*)arg;
        char* start_path = args->path;
        pthread_mutex_t* lock = args->lock;
        fileNode* fileList = args->flist;
        //printf("handling dir %s\n", start_path);
        pthread_t threads[100];//change later, dont want this static, should find count of files/dirs first and then allocate that much space
        int threadCount=0;
        struct dirent *pDirent;
        DIR *pDir;
        // Ensure we can open directory.
        pDir = opendir (start_path);
        // Reading . and .. directories (throwing them away)
        readdir(pDir);
        readdir(pDir);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", start_path);
            exit(0);
        }
        // Process each entry.
        while ((pDirent = readdir(pDir)) != NULL) {
            if(pDirent->d_type == DT_DIR && pDirent->d_type != DT_REG ){
                int pathlen = strlen(start_path) + 2 + strlen(pDirent->d_name);
                char new_path[pathlen];
                strcpy(new_path, start_path);
                strcat(new_path, pDirent->d_name);
                strcat(new_path, "/");
                new_path[pathlen-1] = '\0';
                threadArg* threadArgs = (threadArg*)malloc(sizeof(threadArg));
                strcpy(threadArgs->path, new_path);
                threadArgs->flist = fileList;
                threadArgs->lock = lock;
                pthread_create(&threads[threadCount++], NULL, directory_handling, (void*)threadArgs);
                //directory_handling(threadArgs);
            }else if(pDirent->d_type == DT_REG){
                int pathlen = strlen(start_path) + 1 + strlen(pDirent->d_name);
                char new_path[pathlen];
                strcpy(new_path, start_path);
                strcat(new_path, pDirent->d_name);
                new_path[pathlen-1] = '\0';
                threadArg* threadArgs = (threadArg*)malloc(sizeof(threadArg));
                strcpy(threadArgs->path, new_path);
                threadArgs->flist = fileList;
                threadArgs->lock = lock;
                pthread_create(&threads[threadCount++], NULL, file_handling, (void*)threadArgs);
                // file_handling(threadArgs);
            }
        }
        // done spawning threads, join them all
        int j;
            for(j = 0;j < threadCount; j++){
                pthread_join(threads[j] ,NULL);
        }
        closedir (pDir);
        // return(NULL);
        pthread_exit(NULL);
}
//fills in probabilities for each work to occur on a file
void addProbabilities(fileNode* currentFile){
    wordNode* curWord = currentFile->wordList;
    while(curWord != NULL){
        curWord->probability = ((float)curWord->occurrence / (float)currentFile->wordCount);
        curWord = curWord->next;
    }
}

// does file_handling
void *file_handling(void* arg){
    threadArg* args = (threadArg*)arg;
    char* filePath = args->path;
    pthread_mutex_t* lock = args->lock;
    fileNode* fileList = args->flist;
    //initting current file
    fileNode* currentFile = (fileNode*)malloc(sizeof(fileNode));
    strcpy(currentFile->path, filePath);
    currentFile->wordCount = 0;
    currentFile->next = NULL;
    currentFile->wordList = NULL;
    //critical section, adding to main file list
    pthread_mutex_lock(lock);
    //traverse until we find the last file in the list
    fileNode* last = fileList;
    while(last->next != NULL){
        last = last->next;
    }
    last->next = currentFile;
    pthread_mutex_unlock(lock);
    //end of critical section
    char* fileContent = readInFile(currentFile);
    if(fileContent != "DNE\0"){
        word_tok(fileContent, currentFile);
        addProbabilities(currentFile);
    }
    pthread_exit(NULL);
}
//frees the filelist and each file's words
void cleanList(fileNode* fileList){
    //first element is NULL and gets freed at a later point
    fileNode* currentFile = fileList->next;
    while(currentFile != NULL){
        //free the wordlist of the current file
        wordNode* currentWord = currentFile->wordList;
        while(currentWord != NULL){
            wordNode* prev = currentWord;
            currentWord = currentWord->next;
            free(prev);
        }
        free(currentWord);

        fileNode* prevFile = currentFile;
        currentFile = currentFile->next;
        free(prevFile);
    }
}

int main(int argc,char *argv[]){
        if (argc != 2) {
            printf ("Provide Directory Please\n");
            exit(0);
        }
        //Beginning of file list
        fileNode* flist = (fileNode*)malloc(sizeof(fileNode));
        //Shared Lock
        pthread_mutex_t* lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(lock, NULL);
        char* startdir = argv[1];
        strcat(startdir, "/");
        threadArg* arg = (threadArg*)malloc(sizeof(threadArg));
        strcpy(arg->path, startdir);
        arg->lock = lock;
        arg->flist = flist;
        pthread_t mainThread;
        pthread_create(&mainThread, NULL, directory_handling, (void*)arg);
        pthread_join(mainThread, NULL);
        printList(flist);
        cleanList(flist);
        free(flist);
        free(lock);
        
        return 0;
}
