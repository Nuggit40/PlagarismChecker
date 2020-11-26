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
#include <math.h>
void* file_handling(void* arg);
void* directory_handling(void* arg);
void printList(fileNode* head); 

wordNode* makeWord(char* token){
    wordNode* newWord = (wordNode*)malloc(sizeof(wordNode));
    newWord->occurrence = 1;
    char* wtext = (char*)malloc(sizeof(char) * strlen(token) + 1);
    strcpy(wtext, token);
    newWord->text = wtext;
    newWord->next = NULL;
    return newWord;
}

void printWL(wordNode* wn){
    while(wn != NULL){
        printf("\t\t%s\toccurrances:%d\tprob:%f\n", wn->text, wn->occurrence, wn->probability);
        wn = wn->next;
    }
}

void swapWords(wordNode* w1, wordNode* w2){
    wordNode temp = *w1;
    *w1 = *w2;
    *w2 = temp;
    w1->next = w2;
}

void addToken(fileNode* file, char* token){
    //discard empty tokens or tokens containing only spaces
    if (isspace(token[0]) || token[0] == '\0') return;

    ++file->wordCount;
    
    if(file->wordList == NULL){
        //first word
        file->wordList = makeWord(token);
    } else {
        wordNode* last = file->wordList;
        while(last->next != NULL){
            //increment occurrence if already in list
            if(strcmp(last->text, token) == 0){
                ++last->occurrence;
                break;
            }
            //create new word and swap if list is out of order
            if(strcmp(last->text, token) > 0){
                wordNode* newNode = makeWord(token);
                swapWords(last, newNode);
                break;
            }
            last = last->next;
        }
        if(last->next == NULL){
            //checking last element in the list and swapping if applicable
            if(strcmp(last->text, token) == 0){
                ++last->occurrence;
            } else if(strcmp(last->text, token) > 0){
                wordNode* newNode = makeWord(token);
                swapWords(last, newNode);
            } else {
                last->next = makeWord(token);
            }
        }
    }
}

// reads file in and sets it to a buffer
char* readInFile(fileNode* file)
{
    int fd = open(file->path,O_RDONLY);
    if(fd == -1){
        printf("Cannot open the file %s\n", file->path);
        pthread_exit(NULL);
    }
    int fileSize = lseek(fd, 0, SEEK_END);
    //setting cursor to start of file
    lseek(fd, 0, SEEK_SET);
    char buffer[100];
    buffer[0] = '\0';
    char c;
    int status;
    int wordLen = 0;
    int charCount = 0;
    do {
        status = read(fd, &c, 1);
        if(status > 0){
            ++charCount;
            //make all chars lowercase
            c = tolower(c);
            //add the character to the current buffer
            buffer[wordLen] = c;
            buffer[wordLen + 1] = '\0';
            ++wordLen;
            //printf("buffer currently:'%s'\n", buffer);
            //if the character is a space then then token has ended and we process the token
            if(isspace(c) || isdigit(c) || charCount == fileSize){
                //discard the char that terminated the word
                if(isspace(c) || isdigit(c)){
                    buffer[wordLen-1] = '\0';
                }
                addToken(file, buffer);
                //prepare for the next word
                wordLen = 0;
                buffer[0] = '\0';
                
            }
        }
    } while (status > 0);
    close(fd);
    return NULL;
}

//prints out linked list
void printList(fileNode *head){   
    //first entry in the list is NULL
    fileNode *ptr=head;
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
    meanConstruction *ptr=head;
        while(ptr != NULL){   
            printf("\t%s\tmean:%f\n",ptr->text, ptr->mean);
            ptr=ptr->next;    
        }
}
meanConstruction* makeMean(char* text,float mean){
    meanConstruction* newMean = (meanConstruction*)malloc(sizeof(meanConstruction));
    newMean->text = text;
    newMean->next = NULL;
    newMean->mean = mean ;
    return newMean;
}
float getKLD(fileNode* file, meanConstruction* mean);
void getJensenProb(fileNode* file){
    wordNode* wl1 = file->wordList;
    wordNode* wl2 = file->next->wordList;
    //adding first node and creating list
    meanConstruction* meanList = makeMean(wl1->text, (wl1->probability)/2);
    meanConstruction* m = meanList;
    if(wl1->next != NULL){
        wl1 = wl1->next;
    }
    while(wl1 != NULL){
        //printf("%s, %f, %f\n",wl1->text, wl1->probability, wl1->probability/2);
        m->next = makeMean(wl1->text, (wl1->probability)/2.0);
        wl1 = wl1->next;
        m = m->next;
    }
    //reset m pointer
    m = meanList;
    printf("ML after wl1:\n");
    printMeanList(meanList);
    //checking first node of wl2
    while(m->next != NULL){
        if(strcmp(wl2->text, m->text) == 0){
            m->mean += (wl2->probability / 2.0);
            break;
        }
        m = m->next;
    }
    if(m->next == NULL){
        m->next = makeMean(wl2->text, (wl2->probability) / 2.0);
    }
    wl2 = wl2->next;
    while(wl2 != NULL){
        char* curToken = wl2->text;
        //iterate through meanlist and see if curToken is already there
        while(m->next != NULL){
            if(strcmp(curToken, m->text) == 0){
                //found token
                m->mean += (wl2->probability / 2.0);
                break;
            }
            m = m->next;
        }
        //checking last item in list
        if(m->next == NULL){
            if(strcmp(curToken, m->text) == 0){
                //found token
                m->mean += (wl2->probability / 2.0);
                break;
            } else {
                //end of list, add new node
                m->next = makeMean(curToken, (wl2->probability) / 2.0);
            }
        }
        wl2 = wl2->next;
    }
    
    printf("ML after wl2\n");
    printMeanList(meanList);
    float j=getKLD(file,meanList);
    float f=getKLD(file->next,meanList);
    printf("%s KLD:%f\n",file->path,j);
    printf("JSD:%f\n",f/2); //NUMBER NEEDED FOR JSD

}
float getKLD(fileNode* file, meanConstruction* mean){
        wordNode * currWord1=file->wordList;
        meanConstruction* meanList=mean;
        float kld;
        float tmp;
        float i;
          // loop through the meanList, it should be in the same order as the wordList
            while(meanList!=NULL){
                
                
                //sum the kld of the mean and currWord1 stuff
                
                i=(currWord1->probability)/(meanList->mean);
                tmp=log10(i);
               /* printf("Curr Prob %f -->I:%f----> TMP: %f\n",currWord1->probability,i,tmp);
                 printf("Mean:%f\n",meanList->mean);
                 printf("KLD bef: %f\n",currWord1->probability*log10(i));*/
                kld+=((currWord1->probability)*log10(i));
                //move pointers to next position
                if(currWord1->next==NULL){
                   
                    break;
                }else{
                
                currWord1=currWord1->next;
                }
                meanList=meanList->next;
            }
    return kld;
}



typedef struct _threadArg {
    char* path;
    pthread_mutex_t* lock;
    fileNode* flist;
} threadArg;

//does directory_handling
void* directory_handling( void* arg ){
        threadArg* args = (threadArg*)arg;
        char* start_path = args->path;
        pthread_mutex_t* lock = args->lock;
        fileNode* fileList = args->flist;
        free(arg);
        pthread_t threads[100];//change later, dont want this static, should find count of files/dirs first and then allocate that much space
        int threadCount = 0;
        struct dirent *pDirent;
        DIR *pDir;
        // Ensure we can open directory.
        pDir = opendir (start_path);
        if (pDir == NULL) {
            printf ("Cannot open directory '%s'\n", start_path);
            exit(0);
        }
        // Reading . and .. directories (throwing them away)
        readdir(pDir);
        readdir(pDir);
        // Process each entry.
        while ((pDirent = readdir(pDir)) != NULL) {
                int pathlen = strlen(start_path) + 1 + strlen(pDirent->d_name);
                char* new_path = (char*)malloc(sizeof(char) * pathlen);
                strcpy(new_path, start_path);
                strcat(new_path, pDirent->d_name);
                threadArg* threadArgs = (threadArg*)malloc(sizeof(threadArg));
                threadArgs->flist = fileList;
                threadArgs->lock = lock;
            if(pDirent->d_type == DT_DIR && pDirent->d_type != DT_REG ){
                strcat(new_path, "/");
                threadArgs->path = new_path;
                pthread_create(&threads[threadCount++], NULL, directory_handling, (void*)threadArgs);
            }else if(pDirent->d_type == DT_REG){
                threadArgs->path = new_path;
                pthread_create(&threads[threadCount++], NULL, file_handling, (void*)threadArgs);
            }
        }
        // done spawning threads, join them all
        int j;
            for(j = 0;j < threadCount; j++){
                pthread_join(threads[j] ,NULL);
        }
        closedir (pDir);
        free(start_path);
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

fileNode* makeFile(char* path){
    fileNode* newFile = (fileNode*)malloc(sizeof(fileNode));
    newFile->path = path;
    newFile->next = NULL;
    newFile->wordList = NULL;
    return newFile;
}

void swapFiles(fileNode* f1, fileNode* f2){
    fileNode temp = *f1;
    *f1 = *f2;
    *f2 = temp;
    f1->next = f2;
}

// does file_handling
void *file_handling(void* arg){
    threadArg* args = (threadArg*)arg;
    char* filePath = args->path;
    pthread_mutex_t* lock = args->lock;
    fileNode* fileList = args->flist;
    free(args);

    int fd = open(filePath,O_RDONLY);
    if(fd == -1) {
        printf("Cannot open: %s", filePath);
        pthread_exit(NULL);
    }

    fileNode* newFile = makeFile(filePath);
    readInFile(newFile);
    addProbabilities(newFile);

    
    //critical section, adding to main file list
    pthread_mutex_lock(lock);
    if(fileList->path == NULL){
        //first file in the list
        *fileList = *newFile;
        //filelist already has been malloc'd
        free(newFile);
    } else {
        fileNode* last = fileList;
        while(last->next != NULL){
            if(last->wordCount > newFile->wordCount){
                swapFiles(last, newFile);
                break;
            }
            last = last->next;
        }
        if(last->next == NULL){
            if(last->wordCount > newFile->wordCount){
                swapFiles(last, newFile);
            } else {
                last->next = newFile;
            }
        }
    }
    pthread_mutex_unlock(lock);
    //end of critical section
    
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
            free(prev->text);
            free(prev);
        }
        free(currentWord);
        fileNode* prevFile = currentFile;
        currentFile = currentFile->next;
        free(prevFile->path);
        free(prevFile);
    }
}

int main(int argc,char *argv[]){
        if (argc != 2) {
            printf ("Provide Directory Please\n");
            exit(0);
        }
        //Beginning of file list
        fileNode* flist = malloc(sizeof(fileNode));
        flist->path = NULL;
        //Shared Lock
        pthread_mutex_t* lock = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
        pthread_mutex_init(lock, NULL);

        char* startdir = (char*)malloc(sizeof(char) * strlen(argv[1]) + 2);
        strcpy(startdir, argv[1]);

        strcat(startdir, "/");

        threadArg* arg = (threadArg*)malloc(sizeof(threadArg));
        arg->path = startdir;
        arg->lock = lock;
        arg->flist = flist;

        pthread_t mainThread;
        pthread_create(&mainThread, NULL, directory_handling, (void*)arg);
        pthread_join(mainThread, NULL);
        fileNode* f=flist;
        //do analysis here
        getJensenProb(f);
        printList(flist);
        cleanList(flist);
        free(lock);
        
        return 0;
}
