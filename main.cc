#include "func.h"


int main()
{
    struct FileData currentFiles[MAX_FILES];  // ������ ��� ������� ����������� ���� ������
    struct FileData savedFiles[MAX_FILES];    // ������ ��� ����������� ����������� ���� ������
    int currentFileCount = 0;                 // ������� ������� ������
    int savedFileCount = 0;                   // ������� ����������� ������

    while (1)
    {
        int choice;
        menu();
        if (!(scanf("%d", &choice)) || getchar() != '\n' || choice > 3 || choice < 1)
        {
            printf("You entered something wrong, try again\n");
            while (getchar() != '\n');        // ������� ������ �����
            continue;
        }
        switch (choice)
        {
        case 1:
            remove("checksums.txt");          // ������� ������ ����, ���� �� ����������
            currentFileCount = 0;             // ���������� ������� ������� ������
            listFilesAndSaveChecksumsToFile("/home/artur", currentFiles, &currentFileCount);
            break;
        case 2:
            currentFileCount = 0;             // ���������� ������� ������� ������
            listFilesRecursively("/home/artur", currentFiles, &currentFileCount, NULL);
            savedFileCount = 0;               // ���������� ������� ����������� ������
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
