#ifndef FUNC_H
#define FUNC_H

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MAX_FILES 1000
#define MD5_DIGEST_LENGTH 16

struct FileData
{
    char path[1000];
    unsigned char md5sum[MD5_DIGEST_LENGTH];
    int changed; // Флаг для отслеживания изменений
};

struct ThreadData 
{
    const struct FileData* currentFiles;
    int currentFileCount;
    const struct FileData* savedFiles;
    int savedFileCount;
    int threadIndex;
    int numThreads;
    pthread_mutex_t* mutex;
    int* currentIndex;
};

void menu();
void logMessage(const char* message);
void calculateMD5(const char* path, unsigned char* md5sum);
void listFilesAndSaveChecksumsToFile(const char* basePath, struct FileData files[], int* fileCount);
void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH]);
void loadChecksumsFromFile(const char* filename, struct FileData files[], int* fileCount);
void compareChecksumsFromFile(const struct FileData currentFiles[], const struct FileData savedFiles[], int currentFileCount, int savedFileCount);
void* compareChecksums(void* arg);
void viewChangedFiles(const struct FileData changedFiles[], int changedFileCount);

#endif // FUNC_H
