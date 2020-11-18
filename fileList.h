#ifndef FILELIST_H
#define FILELIST_H

typedef struct _wordNode {
    char* text;
    int occurrence;
    float probability;
    struct _wordNode* next;
} wordNode;

typedef struct _fileNode {
    char* path;
    int wordCount;
    struct _fileNode* next;
    wordNode* wordList;
    float kld;
} fileNode;

#endif