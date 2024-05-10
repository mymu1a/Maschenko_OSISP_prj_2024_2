#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MAX_FILES 1000
#define MD5_DIGEST_LENGTH 16

void listFilesAndSaveChecksumsToFile(const char* basePath, struct FileData files[], int* fileCount);
void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH]);
void compareChecksumsFromFile(const struct FileData files[], int fileCount);

struct FileData {
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

    printf("MD5 checksum for file %s: ", filePath);
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        printf("%02x", md5sum[i]);
    }
    printf("\n");
}

// Функция сравнения контрольных сумм из массива с данными из файла
void compareChecksumsFromFile(const struct FileData files[], int fileCount)
{
    FILE* inFile = fopen("checksums.txt", "r"); // Открываем файл для чтения

    if (!inFile)
    {
        printf("Error opening input file.\n");
        return;
    }

    char line[1024];
    int i = 0;

    while (fgets(line, sizeof(line), inFile))
    {
        char path[1000];
        unsigned char md5sum[MD5_DIGEST_LENGTH];

        if (sscanf(line, "%s", path) != 1)
        {
            printf("Error reading file path.\n");
            fclose(inFile);
            return;
        }

        int byteCount = 0;
        int byteValue;
        char hexPair[3];

        // Считываем пары символов из строки и преобразуем их в байты контрольной суммы
        for (int j = strlen(path) + 1; j < strlen(line); j += 2)
        {
            strncpy(hexPair, line + j, 2);
            hexPair[2] = '\0';
            sscanf(hexPair, "%x", &byteValue);
            md5sum[byteCount++] = (unsigned char)byteValue;
        }

        // Сравниваем контрольную сумму из файла с контрольной суммой из массива
        int match = memcmp(md5sum, files[i].md5sum, MD5_DIGEST_LENGTH) == 0;

        printf("File: %s\n", path);
        printf("File MD5: ");
        for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
        {
            printf("%02x", md5sum[j]);
        }
        printf("\nArray MD5: ");
        for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
        {
            printf("%02x", files[i].md5sum[j]);
        }
        printf("\nChecksum match: %s\n\n", match ? "Yes" : "No");

        i++;
    }

    fclose(inFile);
}

void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH])
{
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
        if (dp->d_name[0] != '.') // Пропускаем файлы и директории, начинающиеся с точки
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (S_ISDIR(st.st_mode))
            {
                listFilesRecursively(path, files, fileCount, savedChecksums);
            }
            else
            {
                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    calculateMD5(path, files[*fileCount].md5sum);
                    memcpy(savedChecksums[*fileCount], files[*fileCount].md5sum, MD5_DIGEST_LENGTH); 
                    printf("Scanning file: %s\n", path);                                    
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
}

// Функция сканирования директории и сохранения пути к файлу и его контрольной суммы в файл
void listFilesAndSaveChecksumsToFile(const char* basePath, struct FileData files[], int* fileCount)
{
    char path[1000];
    struct dirent* dp;
    struct stat st;

    DIR* dir = opendir(basePath);

    if (!dir)
    {
        printf("Cannot open directory: %s\n", basePath);
        return;
    }

    FILE* outFile = fopen("checksums.txt", "w"); // Открываем файл для записи

    if (!outFile)
    {
        printf("Error opening output file.\n");
        closedir(dir);
        return;
    }

    while ((dp = readdir(dir)) != NULL)
    {
        if (dp->d_name[0] != '.') // Пропускаем файлы и директории, начинающиеся с точки
        {
            sprintf(path, "%s/%s", basePath, dp->d_name);
            stat(path, &st);

            if (!S_ISDIR(st.st_mode))
            {
                unsigned char md5sum[MD5_DIGEST_LENGTH];
                calculateMD5(path, md5sum);

                fprintf(outFile, "%s ", path); // Записываем путь к файлу в файл
                for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
                {
                    fprintf(outFile, "%02x", md5sum[j]); // Записываем контрольную сумму в файл
                }
                fprintf(outFile, "\n");

                printf("Scanning file: %s\n", path); // Выводим информацию о сканировании в консоль

                if (*fileCount < MAX_FILES)
                {
                    strcpy(files[*fileCount].path, path);
                    memcpy(files[*fileCount].md5sum, md5sum, MD5_DIGEST_LENGTH);
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

    fclose(outFile);
    closedir(dir);
}