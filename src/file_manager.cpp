#include "file_manager.h"
#include "definitions.h"

FileManager::FileManager() {
    folders_ = 0;
    count_root_ = N_FOLDERS_ROOT;
    count_SD1_ = N_FOLDERS_SD1;
}

FileManager::~FileManager() {}

int FileManager::GenerateFolder() {
    char commands[250];

    chdir("/mnt/p");

    /* Generating Folders */
    for (int i = 0; i < count_root_; i++)
    {   
        snprintf(commands, 250, "mkdir SD%d", i);
        system(commands);
        folders_++;

        if (i == 1) {
            for (int j = 0; j < count_SD1_; j++)
            {
                snprintf(commands, 250, "cd SD%d && mkdir SD%d.%d", i, i, j);
                system(commands);
                folders_++;
            }
        }
    }

    return 0;
}

int FileManager::GenerateFiles1KB(int count_files) {
    if (folders_ < 1) {
        printf("Error! Folders weren't created.");
        return 1;
    }

    chdir("/mnt/p");
    
    char commands[250];

    for (int i = 0; i < count_root_; i++)
    {
        for (int j = 1; j <= count_files; j++)
        {
            snprintf(commands, 250, "cd SD%d && openssl rand 1000 > file%d.txt", i, j);
            /* Criando 371 arquivos de 1KB dentro de cada subdiretório em root */
            system(commands);
        }
    }
    return 0;
}

int FileManager::GenerateFilesSD1(int count_files) {
    if (folders_ < 1) {
        printf("Error! Folders weren't created.");
        return 1;
    }

    chdir("/mnt/p/SD1");
    
    char commands[250];

    /* Running through the SD1 subfolders except by the SD1.2 (videos only) */
    for (int i = 0; i < count_SD1_ - 1; i++)
    {
        int files_multiplier, size_multiplier;
        switch (i)
        {
        case 0:
            files_multiplier = 2;
            size_multiplier = 4;
            break;
        case 1:
            files_multiplier = 3;
            size_multiplier = 8;
            break;
        default:
            printf("Error accessing SD1 subfolders.");
            break;
        }
        for (int j = 1; j <= count_files*files_multiplier; j++)
        {
            snprintf(commands, 250, 
            "cd SD1.%d && openssl rand %d000 > file%d.txt", i, size_multiplier, j);
            system(commands);
        }
    }
    return 0;
}