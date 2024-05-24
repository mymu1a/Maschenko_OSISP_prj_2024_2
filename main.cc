#include "main.h"

int main()
{
	struct FileData files[MAX_FILES];								
	int fileCount = 0;												
	unsigned char savedChecksums[MAX_FILES][MD5_DIGEST_LENGTH];		

	while (1)
	{
		int chois;
		menu();
		if (!(scanf("%d", &chois)) || getchar() != '\n' || chois > 3 || chois < 1)
		{
			printf("you entered something wrong, try again\n");
			while (getchar() != '\n');								
			continue;												
		}
		switch (chois)
		{
		case 1:
			listFilesAndSaveChecksumsToFile("/home/artur/test", files, &fileCount);
			break;
		case 2:
			listFilesRecursively("/home/artur/test", files, &fileCount, savedChecksums);
			compareChecksumsFromFile(files, fileCount);
			break;
		case 3:
			printf("close program\n");
			return 0;
		}
	}
	return 0;
}