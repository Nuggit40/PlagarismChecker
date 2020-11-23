#ifndef FILELIST_H
#define FILELIST_H

typedef struct _wordNode {
    char text[50];
    int occurrence;
    float probability;
    struct _wordNode* next;
} wordNode;

typedef struct _fileNode {
    char path[256];
    int wordCount;
    struct _fileNode* next;
    wordNode* wordList;
    float kld;
} fileNode;

typedef struct _meanConstruction {
    char* text;
    double mean;
    struct _meanConstruction* next;
} meanConstruction;

#endif
