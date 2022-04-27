#include "definitions.h"
#include "file_manager.h"

int main(int argc, char const *argv[])
{
    FileManager file_manager;

    file_manager.GenerateFolder();
    file_manager.GenerateFiles1KB(N_FILES_ROOT);
    file_manager.GenerateFilesSD1(N_FILES_ROOT);
    
    return 0;
}
