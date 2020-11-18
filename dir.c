#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "fileList.h"
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;


void dir_thread(pthread_t *thread_group, char *new_path,int i);
void *file_handling(void* f1);
//creates new node
fileNode* newNode(char* path){   
    fileNode *res=malloc(sizeof(fileNode)); 
    res->path=path;   
    res->next=NULL;     
    return res;
}
//prints out linked list
void printList(fileNode *head){   
    fileNode *ptr=head;   

    while(ptr!=NULL){   
        printf("%s-->",ptr->path);   
        ptr=ptr->next;    
        
    }
    printf("\n");
}

//does directory_handling
void* directory_handling( void* s_p ){
        char new_path[100];
        pthread_t *thread_group = malloc(sizeof(pthread_t) * 253);
        int i=0;
        char* start_path=(char *)s_p;
        struct dirent *pDirent;
        DIR *pDir;

        // Ensure we can open directory.

        pDir = opendir (start_path);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", start_path);
            exit(0);
        }

        // Process each entry.

        while ((pDirent = readdir(pDir)) != NULL) {
        if(pDirent->d_type == DT_DIR && pDirent->d_type != DT_REG ){
            if (strcmp(pDirent->d_name, ".") != 0 && strcmp(pDirent->d_name, "..") != 0){
                printf("%s(", pDirent->d_name);
                 strcpy(new_path, start_path);
                 strcat(new_path, "/");
                 strcat(new_path, pDirent->d_name);
                 printf("%s)\n", new_path);
                 dir_thread(thread_group,new_path,i);
                 i++;
            }
            
        }else if(pDirent->d_type == DT_REG){
            fileNode *f1;
             f1 = malloc(sizeof(fileNode));
             printf("%s(", pDirent->d_name);
             strcpy(new_path, start_path);
             strcat(new_path, "/");
             strcat(new_path, pDirent->d_name);
             (*f1).path=new_path;
             pthread_create(&thread_group[i],NULL,file_handling,(void *)f1);
             //file_thread(thread_group,new_path,i);
             i++;
             
  
        }
        //JOINING THREADS
         int j;
        for(j=0;j<i;j++){
            pthread_join(thread_group[j],NULL);
         }
         

        }
        closedir (pDir);
       

        return EXIT_SUCCESS;

}
// does file_handling
void *file_handling(void* f1){
    fileNode *file=(fileNode *)f1;
    fileNode *head;  
    fileNode *tmp;     
    fileNode *ptr; 
    
   
    pthread_mutex_lock(&lock);
        
        ptr=newNode((*file).path);
        tmp=ptr;
        printf("%s)\n", (*file).path);
    pthread_mutex_unlock(&lock);
    tmp->next=head;
    head=tmp;
     
    
    
    printList(head);
    return EXIT_SUCCESS;

}
//creates thread for directory
void dir_thread(pthread_t *thread_group, char *new_path,int i){
    pthread_create(&thread_group[i],NULL,directory_handling,new_path);
}
int main(int argc,char *argv[]){
    
        if (argc != 2) {
            printf ("Provide Directory Please\n");
            exit(0);
        }
        directory_handling(argv[1]);

        return 0;


}
