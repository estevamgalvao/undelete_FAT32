#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "definitions.h"

class FileManager {
    private:
        int folders_;
        int count_root_;
        int count_SD1_;

        
    public:
        FileManager();
        virtual ~FileManager();

        int GenerateFolder();
        int GenerateFiles512(int count_files);
        int GenerateFiles1KB(int count_files);
        int GenerateFilesSD1(int count_files);
        int FillUpSD1_2(int free_space);
        int DeleteFilesRand(int count_files);


};


#endif //FILE_MANAGER_H;