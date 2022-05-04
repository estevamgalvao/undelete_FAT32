#include "definitions.h"
#include "file_manager.h"

int main()
{
    FileManager file_manager;
    int ret_file_manager;
    srand(time(0));

    // chdir("/mnt/p");
    // system("ls -la");
    // system("mkdir sotest");
    // mkdir("sotest", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);

    ret_file_manager = file_manager.GenerateFolder();
    if (ret_file_manager > 0)
    {
        printf("[SYSTEM] - [%d] Folders created with success.\n", ret_file_manager);
        printf("[SYSTEM] - Press ENTER to continue: \n");
        printf("\t-> [SD0]   371    *    1KB  FILES.TXT\n");
        printf("\t-> [SD1]   371    *    1KB  FILES.TXT\n");
        printf("\t-> [SD2]   371    *    1KB  FILES.TXT\n");
        printf("\t-> [SD3]   371    *    1KB  FILES.TXT\n");
        printf("\t-> [SD4]   371    *    1KB  FILES.TXT\n");
        printf("\t-> [ROOT]  1000   *    512B FILES.TXT\n");
        printf("\t-> [SD1.0] 2000   *    4KB  FILES.TXT\n");
        printf("\t-> [SD1.1] 3000   *    8KB  FILES.TXT\n");
        printf("\t-> [SD1.2] 1      *    94MB FILE.TXT\n");
        getchar();
    }
    else {
        return -1;
    }

    file_manager.GenerateFiles1KB(371);
    file_manager.GenerateFiles512(N_FILES_ROOT);
    file_manager.GenerateFilesSD1(N_FILES_ROOT);
    file_manager.FillUpSD1_2(FREE_SPACE);



    return 0;
}
