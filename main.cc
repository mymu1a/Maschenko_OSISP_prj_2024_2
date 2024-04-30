#include "main.h"

int main()
{
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
			listFilesRecursively("/home");
			break;
		case 2:
			entertime();
			listFilesRecursively_2("/home");
			return 0;
		case 3:
			printf("close program\n");
			return 0;
		}
	}
	return 0;
}