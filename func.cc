#include "func.h"

void menu()
{
    printf("-------------------------------\n");
    printf("1. Scan and save file checksums\n");
    printf("2. Compare file checksums\n");
    printf("3. Check changed files\n");
    printf("4. Exit\n");
    printf("-------------------------------\n");
}

void logMessage(const char* message)
{
    FILE* logFile = fopen("log.txt", "a");
    if (logFile)
    {
        fprintf(logFile, "%s\n", message);
        fclose(logFile);
    }
}

void calculateMD5(const char* path, unsigned char* md5sum)
{
    FILE* file = fopen(path, "rb");
    if (!file) return;

    MD5_CTX mdContext;
    MD5_Init(&mdContext);

    unsigned char data[1024];
    int bytes;
    while ((bytes = fread(data, 1, 1024, file)) != 0)
    {
        MD5_Update(&mdContext, data, bytes);
    }

    MD5_Final(md5sum, &mdContext);
    fclose(file);
}

void listFilesAndSaveChecksumsToFile(const char* basePath, struct FileData files[], int* fileCount)
{
    char path[1000];
    struct dirent* dp;
    struct stat st;

    DIR* dir = opendir(basePath);

    if (!dir)
    {
        char message[1024];
        snprintf(message, sizeof(message), "Cannot open directory: %s", basePath);
        logMessage(message);
        printf("%s\n", message);
        return;
    }

    FILE* outFile = fopen("checksums.txt", "a");                           // Открываем файл для записи

    if (!outFile)
    {
        char message[1024];
        snprintf(message, sizeof(message), "Error opening output file.");
        logMessage(message);
        printf("%s\n", message);
        closedir(dir);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_name[0] != '.')                                          // Пропускаем файлы и директории, начинающиеся с точки
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (S_ISDIR(st.st_mode))
            {
                listFilesAndSaveChecksumsToFile(path, files, fileCount);   // Рекурсивно обрабатываем поддиректории
            }
            else
            {
                unsigned char md5sum[MD5_DIGEST_LENGTH];
                calculateMD5(path, md5sum);

                fprintf(outFile, "%s ", path);                              // Записываем путь к файлу в файл
                for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
                {
                    fprintf(outFile, "%02x", md5sum[j]);                    // Записываем контрольную сумму в файл
                }
                fprintf(outFile, "\n");

                printf("Scanning file: %s\n", path);                        // Выводим информацию о сканировании в консоль

                char message[1024];
                snprintf(message, sizeof(message), "Scanning file: %s", path);
                logMessage(message);

                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    memcpy(files[*fileCount].md5sum, md5sum, MD5_DIGEST_LENGTH);
                    (*fileCount)++;
                }
                else
                {
                    char message[1024];
                    snprintf(message, sizeof(message), "Max files limit reached.");
                    logMessage(message);
                    printf("%s\n", message);
                    break;
                }
            }
        }
    }

    fclose(outFile);
    closedir(dir);
}

void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH])
{
    char path[1000];
    struct dirent* dp;
    struct stat st;

    DIR* dir = opendir(basePath);

    if (!dir)
    {
        char message[1024];
        snprintf(message, sizeof(message), "Cannot open directory: %s", basePath);
        logMessage(message);
        printf("%s\n", message);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_name[0] != '.')                                               // Пропускаем файлы и директории, начинающиеся с точки
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (S_ISDIR(st.st_mode))
            {
                listFilesRecursively(path, files, fileCount, savedChecksums);   // Рекурсивно обрабатываем поддиректории
            }
            else
            {
                unsigned char md5sum[MD5_DIGEST_LENGTH];
                calculateMD5(path, md5sum);

                printf("Scanning file: %s\n", path);                             // Выводим информацию о сканировании в консоль

                char message[1024];
                snprintf(message, sizeof(message), "Scanning file: %s", path);
                logMessage(message);

                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    memcpy(files[*fileCount].md5sum, md5sum, MD5_DIGEST_LENGTH);
                    (*fileCount)++;
                }
                else
                {
                    char message[1024];
                    snprintf(message, sizeof(message), "Max files limit reached.");
                    logMessage(message);
                    printf("%s\n", message);
                    break;
                }
            }
        }
    }

    closedir(dir);
}

void loadChecksumsFromFile(const char* filename, struct FileData files[], int* fileCount)
{
    FILE* inFile = fopen(filename, "r");
    if (!inFile)
    {
        char message[1024];
        snprintf(message, sizeof(message), "Error opening input file.");
        logMessage(message);
        printf("%s\n", message);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), inFile))
    {
        char* path = strtok(line, " ");
        char* checksum = strtok(NULL, " \n");

        if (path && checksum)
        {
            strcpy(files[*fileCount].path, path);
            for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
            {
                sscanf(&checksum[i * 2], "%2hhx", &files[*fileCount].md5sum[i]);
            }
            (*fileCount)++;
        }
    }

    fclose(inFile);
}

void compareChecksumsFromFile(const struct FileData currentFiles[], const struct FileData savedFiles[], int currentFileCount, int savedFileCount)
{
    int numThreads = 4;                                     // можно изменить это значение на желаемое количество потоков
    pthread_t threads[numThreads];
    struct ThreadData threadData[numThreads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    int currentIndex = 0;
    struct FileData changedFiles[MAX_FILES];                // Массив для хранения измененных файлов
    int changedFileCount = 0;                               // Счетчик измененных файлов

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < numThreads; i++)
    {
        threadData[i].currentFiles = currentFiles;
        threadData[i].currentFileCount = currentFileCount;
        threadData[i].savedFiles = savedFiles;
        threadData[i].savedFileCount = savedFileCount;
        threadData[i].threadIndex = i;
        threadData[i].numThreads = numThreads;
        threadData[i].mutex = &mutex;
        threadData[i].currentIndex = &currentIndex;

        char message[1024];
        snprintf(message, sizeof(message), "Starting thread %d", i);
        logMessage(message);

        pthread_create(&threads[i], NULL, compareChecksums, &threadData[i]);
    }

    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(threads[i], NULL);

        char message[1024];
        snprintf(message, sizeof(message), "Thread %d finished", i);
        logMessage(message);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Time taken: %.2f seconds\n", elapsed);

    char message[1024];
    snprintf(message, sizeof(message), "Time taken for comparison: %.2f seconds", elapsed);
    logMessage(message);
}

void* compareChecksums(void* arg)
{
    struct ThreadData* data = (struct ThreadData*)arg;

    while (1)
    {
        pthread_mutex_lock(data->mutex);
        if (*data->currentIndex >= data->currentFileCount)
        {
            pthread_mutex_unlock(data->mutex);
            //usleep(1);
            break;
        }

        int fileIndex = *data->currentIndex;
        (*data->currentIndex)++;
        pthread_mutex_unlock(data->mutex);

        //usleep(1);

        if (fileIndex % data->numThreads != data->threadIndex)
        {
            continue;
        }

        const struct FileData* currentFile = &data->currentFiles[fileIndex];
        const struct FileData* savedFile = NULL;

        for (int i = 0; i < data->savedFileCount; i++)
        {
            if (strcmp(currentFile->path, data->savedFiles[i].path) == 0)
            {
                savedFile = &data->savedFiles[i];
                break;
            }
        }

        if (savedFile)
        {
            int match = memcmp(currentFile->md5sum, savedFile->md5sum, MD5_DIGEST_LENGTH) == 0;

            printf("Thread %d - File: %s\n", data->threadIndex, currentFile->path);
            printf("Thread %d - Current MD5: ", data->threadIndex);
            for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
            {
                printf("%02x", currentFile->md5sum[j]);
            }
            printf("\nThread %d - Saved MD5: ", data->threadIndex);
            for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
            {
                printf("%02x", savedFile->md5sum[j]);
            }

            printf("\nThread %d - Checksum match: %s\n", data->threadIndex, match ? "Yes" : "No");

            char message[1024];
            snprintf(message, sizeof(message), "Thread %d - File: %s - Checksum match: %s", data->threadIndex, currentFile->path, match ? "Yes" : "No");
            logMessage(message);

            if (!match)
            {
                // Файл изменился, добавляем его в массив changedFiles
                pthread_mutex_lock(data->mutex);
                strcpy(changedFiles[changedFileCount].path, currentFile->path);
                memcpy(changedFiles[changedFileCount].md5sum, currentFile->md5sum, MD5_DIGEST_LENGTH);
                changedFileCount++;
                pthread_mutex_unlock(data->mutex);
            }
        }
        else
        {
            printf("Thread %d - File: %s - No saved checksum found.\n", data->threadIndex, currentFile->path);

            char message[1024];
            snprintf(message, sizeof(message), "Thread %d - File: %s - No saved checksum found.", data->threadIndex, currentFile->path);
            logMessage(message);
        }
    }

    return NULL;
}

void viewChangedFiles(const struct FileData changedFiles[], int changedFileCount)
{
    printf("Changed Files:\n");
    for (int i = 0; i < changedFileCount; i++)
    {
        printf("%s\n", changedFiles[i].path);
    }
}
