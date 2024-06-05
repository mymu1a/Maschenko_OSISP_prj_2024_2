#include "func.h"


int main()
{
    struct FileData currentFiles[MAX_FILES];  // Массив для текущих контрольных сумм файлов
    struct FileData savedFiles[MAX_FILES];    // Массив для сохраненных контрольных сумм файлов
    int currentFileCount = 0;                 // Счетчик текущих файлов
    int savedFileCount = 0;                   // Счетчик сохраненных файлов

    while (1)
    {
        int choice;
        menu();
        if (!(scanf("%d", &choice)) || getchar() != '\n' || choice > 3 || choice < 1)
        {
            printf("You entered something wrong, try again\n");
            while (getchar() != '\n');        // Очистка буфера ввода
            continue;
        }
        switch (choice)
        {
        case 1:
            remove("checksums.txt");          // Удаляем старый файл, если он существует
            currentFileCount = 0;             // Сбрасываем счетчик текущих файлов
            listFilesAndSaveChecksumsToFile("/home/artur", currentFiles, &currentFileCount);
            break;
        case 2:
            currentFileCount = 0;             // Сбрасываем счетчик текущих файлов
            listFilesRecursively("/home/artur", currentFiles, &currentFileCount, NULL);
            savedFileCount = 0;               // Сбрасываем счетчик сохраненных файлов
            loadChecksumsFromFile("checksums.txt", savedFiles, &savedFileCount);
            compareChecksumsFromFile(currentFiles, savedFiles, currentFileCount, savedFileCount);
            break;
        case 3:
            viewChangedFiles(currentFiles, currentFileCount);
            break;
        case 4:
            printf("Close program\n");
            return 0;
        }
    }
    return 0;
}
