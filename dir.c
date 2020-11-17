#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;





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
                pthread_create(&thread_group[i],NULL,directory_handling,new_path);
                i++;
            }
            
        }else if(pDirent->d_type == DT_REG){
             printf("%s(", pDirent->d_name,start_path);
             
            strcpy(new_path, start_path);
            strcat(new_path, "/");
            strcat(new_path, pDirent->d_name);
            printf("%s)\n", new_path);
        }


        
         int j;
        for(j=0;j<i;j++){
            pthread_join(thread_group[j],NULL);
         }

        }
        closedir (pDir);
       

        return EXIT_SUCCESS;

}

int main(int argc,char *argv[]){
    
        if (argc != 2) {
            printf ("Provide Directory Please\n");
            exit(0);
        }
        directory_handling(argv[1]);

     
    
        return 0;


}
