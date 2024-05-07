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



















//void listFilesRecursively_2(const char* basePath) 
//{
//    while (1) 
//    {
//        char path[1000];
//        struct dirent* entry;
//        struct stat statbuf;
//
//        DIR* dir = opendir(basePath);
//
//        if (!dir) 
//        {
//            perror("Error opening directory");
//            return;
//        }
//
//        while ((entry = readdir(dir)) != NULL) 
//        {
//            snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);
//
//            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') 
//            {
//                continue;
//            }
//
//            printf("%s\n", path);
//
//            if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) 
//            {
//                listFilesRecursively(path);
//            }
//        }
//
//        closedir(dir);
//        sleep(seconds);
//    }
//}
//void compute_file_checksum(const char* file_path) {
//    unsigned char digest[SHA256_DIGEST_LENGTH];
//
//    FILE* file = fopen(file_path, "rb");
//    if (!file) {
//        perror("Error opening file");
//        return;
//    }
//
//    SHA256_CTX sha256;
//    SHA256_Init(&sha256);
//
//    unsigned char buffer[BUFSIZ];
//    size_t bytesRead;
//
//    while ((bytesRead = fread(buffer, 1, BUFSIZ, file))) {
//        SHA256_Update(&sha256, buffer, bytesRead);
//    }
//
//    SHA256_Final(digest, &sha256);
//
//    fclose(file);
//
//    printf("Checksum for file %s: ", file_path);
//    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//        printf("%02x", digest[i]);
//    }
//    printf("\n");
//}
//
//void listFilesRecursively(const char* basePath) {
//    char path[1000];
//    struct dirent* entry;
//    struct stat statbuf;
//
//    DIR* dir = opendir(basePath);
//
//    if (!dir) {
//        perror("Error opening directory");
//        return;
//    }
//
//    while ((entry = readdir(dir)) != NULL) {
//        snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);
//
//        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') {
//            continue;
//        }
//
//        printf("%s\n", path);
//
//        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
//            listFilesRecursively(path);
//        }
//        else {
//            compute_file_checksum(path);
//        }
//    }
//
//    closedir(dir);
//}

//void checksum(const char* filePath) 
//{
//    FILE* file = fopen(filePath, "rb");
//    if (!file) {
//        perror("Error opening file");
//        return;
//    }
//
//    fseek(file, 0, SEEK_END);
//    long fileSize = ftell(file);
//    fseek(file, 0, SEEK_SET);
//
//    unsigned char buffer[BUFSIZ];
//    size_t bytesRead;
//    SHA256_CTX shaContext;
//    unsigned char hash[SHA256_DIGEST_LENGTH];
//
//    SHA256_Init(&shaContext);
//    while ((bytesRead = fread(buffer, 1, BUFSIZ, file)) > 0) 
//    {
//        SHA256_Update(&shaContext, buffer, bytesRead);
//    }
//    SHA256_Final(hash, &shaContext);
//
//    fclose(file);
//
//    // Convert hash to hex format for better readability
//    char hashHex[2 * SHA256_DIGEST_LENGTH + 1];
//    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
//    {
//        sprintf(&hashHex[i * 2], "%02x", hash[i]);
//    }
//    hashHex[2 * SHA256_DIGEST_LENGTH] = '\0';
//
//    printf("Checksum of file %s: %s\n", filePath, hashHex);
//}







//void entertime()
//{
//
//
//    printf("enter time (seconds(5-100)): ");
//    while (!(scanf("%d", &seconds)) || getchar() != '\n' || (seconds < 5) || seconds > 100)
//    {
//        printf("you entered something wrong, try again\n");
//        rewind(stdin);
//    }
//    printf("seconds = %d", seconds);
//    printf("\n");
//}