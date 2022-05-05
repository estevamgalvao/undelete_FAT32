/**
 * main
 *
 * The goal of this project is to generate and delete files inside a FAT32 device
 * and at the end, undelete one of them.
 *
 * author: Estevam Galvão Albuquerque
 * github: estevamgalvao
 *
 */

#include "definitions.h"
#include "driver.h"

int main()
{
    WCHAR* path = L"\\\\.\\D:";

    try
    {
        Driver FAT32(path);
        int choice;


        printf("\n");
        printf("[SYSTEM] - Select an option:\n");
        printf("[1] - BOOT INFO\n");
        printf("[2] - RESTORE FILE\n");
        printf("[3] - EXIT\n");
        
        while (true)
        {
            scanf("%d", &choice);
            getchar();
            switch (choice)
            {
            case 1:
                FAT32.PrintBootInfo();
                break;
            case 2:
                printf("GIVEN FILE: \\SD0\\moai.txt\n");

                FAT32.LookForFile("\\SD0\\moai.txt");
                FAT32.PrintFileData();
                FAT32.AppendSearchPair(0, FAT32.GetFileData().starting_cluster_int);
                getchar();

                FAT32.FindFileContent();
                FAT32.PrintSearchArray();
                FAT32.RestoreFile(40);

                break;
            case 3:
                return 3;
                break;
            default:
                printf("Invalid option.\n");
                break;
            }
            printf("\n");
            printf("[SYSTEM] - Select an option:\n");
            printf("[1] - BOOT INFO\n");
            printf("[2] - RESTORE FILE\n");
            printf("[3] - EXIT\n");
        }

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
