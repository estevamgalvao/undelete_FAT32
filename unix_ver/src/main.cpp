#include "definitions.h"
#include "file_manager.h"

int main(int, char **)
{
    FileManager file_manager;
    int ret_file_manager;
    srand(time(0));

    ret_file_manager = file_manager.GenerateFolder();
    if (ret_file_manager > 0)
    {
        printf("[SYSTEM] - [%d] Folders created with success.\n\n", ret_file_manager);
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

    // file_manager.GenerateFiles1KB(371);
    // file_manager.GenerateFiles512(N_FILES_ROOT);
    // file_manager.GenerateFilesSD1(N_FILES_ROOT);
    // file_manager.FillUpSD1_2(FREE_SPACE);
    // file_manager.DeleteFilesRand(50);
    file_manager.SetUpFiles40KB();

    return 0;
}
