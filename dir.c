#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "fileList.h"

void *file_handling(void* arg);

//prints out linked list
void printList(fileNode *head){   
    fileNode *ptr=head;   
    while(ptr!=NULL){   
        printf("\t%s\n",ptr->path);   
        ptr=ptr->next;    
    }
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
        printf("handling dir %s\n", start_path);
        pthread_t threads[100];//change later, dont want this static, should find count of files/dirs first and then allocate that much space
        int threadCount=0;
        
        struct dirent *pDirent;
        
        DIR *pDir;
        
        // Ensure we can open directory.
        pDir = opendir (start_path);
        
        //printf("dir made it here\n");
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
            }
        }
        //done spawning threads, join them all
        int j;
            for(j = 0;j < threadCount; j++){
                pthread_join(threads[j] ,NULL);
        }
        closedir (pDir);
        pthread_exit(NULL);
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
    printf("handling file %s\n", currentFile->path);
    pthread_mutex_lock(lock);
    //check if we are the first file in the list
    if(fileList->path[0] == '\0'){
        *fileList = *currentFile;
    } else {
        //traverse until we find the last file in the list
        fileNode* last = fileList;
        while(last->next != NULL){
            last = last->next;
        }
        last->next = currentFile;
    }
    pthread_mutex_unlock(lock);
    //now we can read currentFile and add tokens to its wordlist
    return NULL;
    pthread_exit(NULL);
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
        free(flist);
        free(lock);
        return 0;
}
