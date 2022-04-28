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
    char aux_command[50];

    system("mkdir root");

    snprintf(commands, 250, "cd root && ");
    snprintf(commands, 250, "cd /mnt/d && ");

    /* Generating Folders */
    for (int i = 0; i < count_root_; i++)
    {   
        snprintf(aux_command, 50, "mkdir SD%d", i);
        strcat(commands, aux_command);

        system(commands);
        folders_++;
        snprintf(commands, 250, "cd root && ");
        // snprintf(commands, 250, "cd mnt/d && ");

        if (i == 1) {
            snprintf(aux_command, 50, "cd SD%d && ", i);
            strcat(commands, aux_command);

            system(commands);

            for (int j = 0; j < count_SD1_; j++)
            {
                snprintf(aux_command, 50, "mkdir SD%d.%d", i, j);
                strcat(commands, aux_command);
                
                system(commands);
                folders_++;

                snprintf(commands, 250, "cd root && cd SD%d && ", i);
                // snprintf(commands, 250, "cd mnt/d && cd SD%d && ", i);
            }
            
            snprintf(commands, 250, "cd root && ");
            // snprintf(commands, 250, "cd mnt/d && ");
        }
    }

    return 0;
}

int FileManager::GenerateFiles1KB(int count_files) {
    if (folders_ < 1) {
        printf("Error! Folders weren't created.");
        return 1;
    }
    
    char commands[250];

    for (int i = 0; i < count_root_; i++)
    {
        for (int j = 1; j <= count_files; j++)
        {
            snprintf(commands, 250, "cd root && cd SD%d && openssl rand 1000 > file%d.txt", i, j);
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
    
    char commands[250];

    /* Running through the SD1 subfolders except by the SD1.2 (videos only) */
    for (int i = 0; i < count_SD1_ - 1; i++)
    {
        int times_files, times_size;
        switch (i)
        {
        case 0:
            times_files = 2;
            times_size = 4;
            break;
        case 1:
            times_files = 3;
            times_size = 8;
            break;
        default:
            printf("Error accessing SD1 subfolders.");
            break;
        }
        for (int j = 1; j <= count_files*times_files; j++)
        {
            snprintf(commands, 250, 
            "cd root && cd SD1 && cd SD1.%d && openssl rand %d000 > file%d.txt", i, times_size, j);
            system(commands);
        }
    }
    return 0;
}