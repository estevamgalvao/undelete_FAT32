#ifndef DRIVER_H
#define DRIVER_H
#include "definitions.h"



class Driver {
    private:
        unsigned int bytes_per_sector_;
        unsigned int reserved_sectors_;
        unsigned int FAT_sectors_;
        unsigned int sectors_per_cluster_;
        unsigned int offset_files_;
        unsigned int offset_FAT_;
        unsigned int offset_current_;
        unsigned int bytes_per_cluster_;

        BYTE buffer_[512];
        WCHAR* file_path_;

        HANDLE handle_file_;

        /* FILE DATA SECTION */
        struct file_data {
            char filename[9];
            char extension[4];
            char parent_paths[PATHS_SIZE][32];
            unsigned int starting_cluster_int;
            unsigned short starting_cluster;
            unsigned short starting_cluster_high;
        } file_data_;


        unsigned int part_;
        
        struct search_pair {
            unsigned int part;
            unsigned int cluster;
        } search_pair_;

        unsigned int search_array_[64];



    public:
        Driver(WCHAR* path);
        virtual ~Driver();

        bool ReadSector(unsigned int offset);
        bool WriteSector(unsigned int offset);

        void SetFileData(char *filepath);
        int ScanCluster(char* filename, unsigned int offset, bool is_file, bool is_deleted);
        void LookForFolder(char *filepath);
        void LookForFile(char *filepath);
        void FindFileContent();
        void RestoreFile(unsigned int n_parts);


        int ConvertChar(char first, char second);
        void AppendSearchPair(unsigned int part, unsigned int cluster);

        void PrintBootInfo();
        void PrintFileData();
        void PrintFileInfo(BYTE* buffer_line);
        void PrintSearchArray();
        void PrintBuffer();
        void PrintSector(unsigned int offset);
        
        unsigned int GetOffsetFiles();
        unsigned int GetOffsetFAT();
        unsigned int GetOffsetCurrent();
        file_data GetFileData();
        BYTE* GetBuffer();
};

#endif // DRIVER_H