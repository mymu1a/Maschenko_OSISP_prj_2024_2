#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MAX_FILES 1000
#define MD5_DIGEST_LENGTH 16

struct FileData 
{
    char path[1000];
    unsigned char md5sum[MD5_DIGEST_LENGTH];
};

struct ThreadData 
{
    const struct FileData* files;
    int fileCount;
    int threadIndex;
    int numThreads;
    FILE* inFile;
    pthread_mutex_t* mutex;
    int* currentIndex;
};

void listFilesAndSaveChecksumsToFile(const char* basePath, struct FileData files[], int* fileCount);
void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH]);
void compareChecksumsFromFile(const struct FileData files[], int fileCount);
void* compareChecksums(void* arg);

void menu();
void calculateMD5(const char* filePath, unsigned char* md5sum);
int readChecksum(FILE* inFile, char* path, unsigned char* md5sum, pthread_mutex_t* mutex);

#endif FUNC_H
