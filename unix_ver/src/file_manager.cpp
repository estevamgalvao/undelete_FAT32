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

    printf("[SYSTEM] - Generating folders.\n");

    /* Generating Folders */
    for (int i = 0; i < count_root_; i++)
    {   
        snprintf(commands, 250, "mkdir -p SD%d", i);
        system(commands);
        folders_++;
        printf("- Folder SD%d created.\n", i);

        if (i == 1) {
            for (int j = 0; j < count_SD1_; j++)
            {
                snprintf(commands, 250, "cd SD%d && mkdir -p SD%d.%d", i, i, j);
                system(commands);
                printf("\t- Folder SD%d.%d created.\n", i, j);
                folders_++;
            }
        }
    }


    return folders_;
}

int FileManager::GenerateFiles1KB(int count_files) {
    if (folders_ < 1) {
        perror("Error! Folders weren't created.");
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
            printf("[1KB] - SD%d/file%d.txt created.\n", i, j);
        }
    }
    return 0;
}

int FileManager::GenerateFiles512(int count_files) {
    if (folders_ < 1) {
        perror("Error! Folders weren't created.");
        return 1;
    }

    char commands[250];
    chdir("/mnt/p");


    int counter = 1;
    while(counter <= count_files) {
        snprintf(commands, 250, "openssl rand 512 > file%d.txt", counter);
        system(commands);
        printf("[512B] - file%d.txt created.\n", counter);
        counter++;
    }

    return 0;
}

int FileManager::GenerateFilesSD1(int count_files) {
    if (folders_ < 1) {
        perror("Error! Folders weren't created.");
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
            // files_multiplier = 1;
            files_multiplier = 2;
            size_multiplier = 4;
            break;
        case 1:
            // files_multiplier = 1;
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
            printf("[%dKB] - SD1.%d/file%d.txt created.\n", size_multiplier, i, j);
        }
    }
    return 0;
}

int FileManager::FillUpSD1_2(int free_space) {
    if (folders_ < 1) {
        perror("Error! Folders weren't created.");
        return 1;
    }

    chdir("/mnt/p/SD1/SD1.2");
    char commands[250];
    int counter = 1;

    snprintf(commands, 250, "openssl rand %d > file%d.txt", free_space, counter);
    system(commands);
    counter++;
    printf("[94MB] - SD1.2/file%d.txt created.\n", counter);

    return 0;
}

int FileManager::DeleteFilesRand(int count_files) {
    if (folders_ < 1) {
        perror("Error! Folders weren't created.");
        return 1;
    }

    char commands[250];
    int n_dir = rand();
    n_dir %= 5;


    snprintf(commands, 250, "/mnt/p/SD%d", n_dir);
    chdir(commands);


    int n_file;
    for (size_t i = 0; i < count_files; i++)
    {
        n_file = rand();
        n_file %= 371;
        snprintf(commands, 250, "rm file%d.txt", n_file);
        system(commands);
        printf("[%dº][1KB] - SD%d/file%d.txt deleted.\n", i, n_dir, n_file);
    }

    chdir("/mnt/p/SD1/SD1.0");
    for (size_t i = 0; i < count_files; i++)
    {
        n_file = rand();
        n_file %= 2000;
        snprintf(commands, 250, "rm file%d.txt", n_file);
        system(commands);
        printf("[%dº][4KB] - SD%d/file%d.txt deleted.\n", i, n_dir, n_file);
    }

    chdir("/mnt/p/SD1/SD1.1");
    for (size_t i = 0; i < count_files; i++)
    {
        n_file = rand();
        n_file %= 3000;
        snprintf(commands, 250, "rm file%d.txt", n_file);
        system(commands);
        printf("[%dº][8KB] - SD%d/file%d.txt deleted.\n", i, n_dir, n_file);
    }
}

