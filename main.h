#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

void listFilesRecursively(const char* basePath);
int seconds;

void menu()
{
	printf("------------\n");
    printf("1. scan\n");
    printf("2. scan ( enter time )\n");
	printf("3. exit\n");
	printf("------------\n");
	printf("enter your choice (1 2 3): ");
}

void entertime()
{


	printf("enter time (seconds(5-100)): ");
	while (!(scanf("%d", &seconds)) || getchar() != '\n' || (seconds < 5) || seconds > 100)
	{
		printf("you entered something wrong, try again\n");
		rewind(stdin);
	}
	printf("seconds = %d", seconds);
	printf("\n");
}

void listFilesRecursively(const char* basePath) 
{
    char path[1000];
    struct dirent* entry;
    struct stat statbuf;

    DIR* dir = opendir(basePath);

    if (!dir) 
    {
        perror("Error opening directory");
        return;
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.')
        {
            continue;
        }

        printf("%s\n", path);

        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) 
        {
            listFilesRecursively(path);
        }
    }

    closedir(dir);
}

void listFilesRecursively_2(const char* basePath) 
{
    while (1) 
    {
        char path[1000];
        struct dirent* entry;
        struct stat statbuf;

        DIR* dir = opendir(basePath);

        if (!dir) 
        {
            perror("Error opening directory");
            return;
        }

        while ((entry = readdir(dir)) != NULL) 
        {
            snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);

            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || entry->d_name[0] == '.') 
            {
                continue;
            }

            printf("%s\n", path);

            if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) 
            {
                listFilesRecursively(path);
            }
        }

        closedir(dir);
        sleep(seconds);
    }
}
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

void checksum(const char* filePath) 
{
    FILE* file = fopen(filePath, "rb");
    if (!file) {
        perror("Error opening file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char buffer[BUFSIZ];
    size_t bytesRead;
    SHA256_CTX shaContext;
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_Init(&shaContext);
    while ((bytesRead = fread(buffer, 1, BUFSIZ, file)) > 0) 
    {
        SHA256_Update(&shaContext, buffer, bytesRead);
    }
    SHA256_Final(hash, &shaContext);

    fclose(file);

    // Convert hash to hex format for better readability
    char hashHex[2 * SHA256_DIGEST_LENGTH + 1];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) 
    {
        sprintf(&hashHex[i * 2], "%02x", hash[i]);
    }
    hashHex[2 * SHA256_DIGEST_LENGTH] = '\0';

    printf("Checksum of file %s: %s\n", filePath, hashHex);
}