#include "definitions.h"
#include "file_manager.h"

int main(int argc, char const *argv[])
{
    FileManager file_manager;


    // chdir("/mnt/p");
    // system("ls -la");
    // system("mkdir sotest");
    // mkdir("sotest", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);

    file_manager.GenerateFolder();
    file_manager.GenerateFiles1KB(N_FILES_ROOT);
    file_manager.GenerateFilesSD1(N_FILES_ROOT);
    
    return 0;
}
