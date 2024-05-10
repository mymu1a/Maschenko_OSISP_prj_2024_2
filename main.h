#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MAX_FILES 1000
#define MD5_DIGEST_LENGTH 16

void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount);
void compareChecksums(const struct FileData files[], int fileCount);
void writeChecksumsToFile(struct FileData files[], int fileCount);

struct FileData 
{
    char path[1000];
    unsigned char md5sum[MD5_DIGEST_LENGTH];
};

void menu()
{
	printf("------------\n");
    printf("1. scan\n");
    printf("2. compare\n");
    printf("3. exit\n");
	printf("------------\n");
	printf("enter your choice (1 2 3): ");
}

// нахождение контрольной суммы
void calculateMD5(const char* filePath, unsigned char* md5sum)
{
    FILE* file = fopen(filePath, "rb");
    if (!file)
    {
        printf("Error opening file: %s\n", filePath);
        return;
    }

    EVP_MD_CTX* md_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md_ctx, EVP_md5(), NULL);

    unsigned char buffer[1024];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, 1024, file)) != 0)
    {
        EVP_DigestUpdate(md_ctx, buffer, bytesRead);
    }

    EVP_DigestFinal_ex(md_ctx, md5sum, NULL);
    EVP_MD_CTX_free(md_ctx);

    fclose(file);

    // Вывод контрольной суммы в stdout
    printf("MD5 checksum for file %s: ", filePath);
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        printf("%02x", md5sum[i]);
    }
    printf("\n");
}


// сравнение
void compareChecksums(const struct FileData files[], int fileCount)
{
    printf("START compareChecksums\n");

    // Сначала сканируем выбранную директорию и собираем контрольные суммы файлов
    struct FileData scannedFiles[MAX_FILES]; // Массив для сканированных файлов
    int scannedFileCount = 0;

    // Открываем файл с контрольными суммами
    FILE* checksumsFile = fopen("checksums.txt", "r");
    if (!checksumsFile)
    {
        printf("Error opening checksums file.\n");
        return;
    }
    printf("/////////////////////////////////////////////\n");
    if (fgets(line, sizeof(line), checksumsFile) == NULL)
    {
        printf("Error reading line from checksums.txt\n");
        fclose(checksumsFile);
        return;
    }

    printf("Read line from checksums.txt: %s\n", line);
    printf("fgets result: %d\n", fgets(line, sizeof(line), checksumsFile) != NULL);
    printf("lineCount: %d\n", lineCount);
    printf("fileCount: %d\n", fileCount);
    printf("/////////////////////////////////////////////\n");
    char line[1024];
    int lineCount = 0;

    while (fgets(line, sizeof(line), checksumsFile) != NULL && lineCount < scannedFileCount)
    {
        char savedPath[1024];
        char savedChecksum[MD5_DIGEST_LENGTH * 2 + 1]; // +1 for null terminator

        sscanf(line, "%s %s", savedPath, savedChecksum);

        if (strcmp(savedPath, scannedFiles[lineCount].path) != 0)
        {
            printf("File path mismatch in checksums file.\n");
            fclose(checksumsFile);
            return;
        }

        // Преобразование md5sum к строке для сравнения
        char currentChecksumString[MD5_DIGEST_LENGTH * 2 + 1]; // +1 for null terminator
        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
        {
            sprintf(currentChecksumString + (i * 2), "%02x", scannedFiles[lineCount].md5sum[i]);
        }
        currentChecksumString[MD5_DIGEST_LENGTH * 2] = '\0'; // Null terminator

        if (strcmp(savedChecksum, currentChecksumString) != 0)
        {
            printf("Checksum mismatch for file: %s\n", scannedFiles[lineCount].path);
        }

        lineCount++;
    }

    printf("compareChecksums: Finished reading checksums file.\n");
    fclose(checksumsFile);

    if (lineCount < scannedFileCount)
    {
        printf("Not enough checksum entries in the file.\n");
    }

    printf("END compareChecksums\n");
}

// скан директории
void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount)
{
    FILE* clearFile = fopen("checksums.txt", "w");  // Очищаем файл перед началом записи
    fclose(clearFile);

    char path[1000];
    struct dirent* dp;
    struct stat st;

    DIR* dir = opendir(basePath);
    if (!dir)
    {
        printf("Cannot open directory: %s\n", basePath);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (S_ISDIR(st.st_mode))
            {
                listFilesRecursively(path, files, fileCount);
            }
            else
            {
                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    calculateMD5(path, files[*fileCount].md5sum);
                    printf("Scanning file: %s\n", path);                                    // Выводим путь в stdout
                    (*fileCount)++;
                }
                else
                {
                    printf("Max files limit reached.\n");
                    break;
                }
            }
        }
    }

    closedir(dir);

    // Записываем контрольные суммы в файл
    writeChecksumsToFile(files, *fileCount);
}

// Функция для записи контрольных сумм в файл
void writeChecksumsToFile(struct FileData files[], int fileCount)
{
    FILE* file = fopen("checksums.txt", "w");
    if (!file)
    {
        printf("Error opening file: checksums.txt\n");
        return;
    }

    for (int i = 0; i < fileCount; i++)
    {
        fprintf(file, "%s %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
            files[i].path,
            files[i].md5sum[0], files[i].md5sum[1], files[i].md5sum[2], files[i].md5sum[3],
            files[i].md5sum[4], files[i].md5sum[5], files[i].md5sum[6], files[i].md5sum[7],
            files[i].md5sum[8], files[i].md5sum[9], files[i].md5sum[10], files[i].md5sum[11],
            files[i].md5sum[12], files[i].md5sum[13], files[i].md5sum[14], files[i].md5sum[15]);
    }

    fclose(file);
}

// сканирование
void scanDirectory(const char* basePath, struct FileData files[], int* fileCount)
{
    printf("START scanDirectory\n");
    char path[1000];
    struct dirent* dp;
    struct stat st;

    DIR* dir = opendir(basePath);

    if (!dir)
    {
        printf("Cannot open directory: %s\n", basePath);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (S_ISDIR(st.st_mode))
            {
                scanDirectory(path, files, fileCount);
            }
            else
            {
                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    calculateMD5(path, files[*fileCount].md5sum);
                    printf("Scanning file: %s\n", path); // Выводим путь в stdout
                    (*fileCount)++;
                }
                else
                {
                    printf("Max files limit reached.\n");
                    break;
                }
            }
        }
    }

    closedir(dir);
    printf("END scanDirectory\n");
}