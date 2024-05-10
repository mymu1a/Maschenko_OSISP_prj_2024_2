#include "main.h"
#include <unistd.h>

int main()
{
	struct FileData files[MAX_FILES];								// объявляем массив структур для хранения файлов и их хешей
	int fileCount = 0;												// объявляем переменную для отслеживания количества файлов	Й

	while (1)
	{
		int chois;
		menu();
		if (!(scanf("%d", &chois)) || getchar() != '\n' || chois > 3 || chois < 1)
		{
			printf("you entered something wrong, try again\n");
			while (getchar() != '\n');								// Очищаем буфер ввода до символа новой строки
			continue;												// Продолжаем цикл заново
		}
		switch (chois)
		{
		case 1:
			listFilesRecursively("/home/artur/test", files, &fileCount);
			break;
		case 2:
			scanDirectory("/home/artur/test", files, &fileCount);
			compareChecksums(files, fileCount);
			break;
		case 3:
			printf("close program\n");
			return 0;
		}
	}
	return 0;
}