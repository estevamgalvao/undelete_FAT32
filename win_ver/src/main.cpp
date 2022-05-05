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
                // std::cout << ((memcmp("PARTE", "PARTE", 5))) << std::endl;
                getchar();

                FAT32.FindFileContent();
                FAT32.PrintSearchArray();
                getchar();
                FAT32.RestoreFile(40);

                // FAT32.LookForFile("\\SD0");

                break;
            case 3:
                return 3;
                break;
            default:
                printf("Invalid option.\n");
                break;
            }
            // system("clear");
            printf("\n");
            printf("[SYSTEM] - Select an option:\n");
            printf("[1] - BOOT INFO\n");
            printf("[2] - RESTORE FILE\n");
            printf("[3] - EXIT\n");
        }




        // FAT32.PrintFileInfo(&FAT32.GetBuffer()[11 * 32]);

        // FAT32.SetFileData("\\pasta1\\pasta1_1\\file2.txt");
        // FAT32.ScanCluster("AILE1   TXT", FAT32.GetOffsetFiles(), false);

        // FAT32.LookForFile("\\12345678.txt");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
