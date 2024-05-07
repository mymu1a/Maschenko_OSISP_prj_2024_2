#include "main.h"
#include <unistd.h>

int main()
{
	struct FileData files[MAX_FILES];								// объявляем массив структур для хранения файлов и их хешей
	int fileCount = 0;												// объявляем переменную для отслеживания количества файлов	Й
	unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH];		// Массив для сохранения контрольных сумм

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
			listFilesRecursively("/home/artur/test", files, &fileCount, savedChecksums);
			break;
		case 2:
			compareChecksums(files, fileCount, savedChecksums);
			break;
		case 3:
			printf("close program\n");
			return 0;
		}
	}
	return 0;
}