#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <openssl/md5.h>
#include <openssl/evp.h>

#define MAX_FILES 1000
#define MD5_DIGEST_LENGTH 16

void listFilesRecursively(const char* basePath, struct FileData files[], int* fileCount, unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH]);
void compareChecksums(const struct FileData files[], int fileCount, const unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH]);

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

// нахождение контрольной суммы
void calculateMD5(const char* filePath, unsigned char* md5sum)
{
//    printf("START calculateMD5\n");
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
    printf("MD5     checksum for file %s: ", filePath);
    for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
    {
        printf("%02x", md5sum[i]);
    }
    printf("\n");
//    printf("END calculateMD5\n");
}


//  сравнение
void compareChecksums(const struct FileData files[], int fileCount, const unsigned char savedChecksums[][MD5_DIGEST_LENGTH])
{
    printf("START compareChecksums\n");

    for (int i = 0; i < fileCount; i++)
    {
        unsigned char currentChecksum[MD5_DIGEST_LENGTH];
        calculateMD5(files[i].path, currentChecksum);

        printf("File: %s\n", files[i].path);
        printf("Saved Checksum: ");
        for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
        {
            printf("%02x", savedChecksums[i][j]);
        }
        printf("\n");
        printf("Current Checksum: ");
        for (int j = 0; j < MD5_DIGEST_LENGTH; j++)
        {
            printf("%02x", currentChecksum[j]);
        }
        printf("\n");

        if (memcmp(currentChecksum, savedChecksums[i], MD5_DIGEST_LENGTH) != 0)
        {
            printf("Checksum mismatch for file: %s\n", files[i].path);
        }
    }

    printf("END compareChecksums\n");
}

// скан директории
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
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
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
                    memcpy(savedChecksums[*fileCount], files[*fileCount].md5sum, MD5_DIGEST_LENGTH); // Сохраняем контрольную сумму
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
}
