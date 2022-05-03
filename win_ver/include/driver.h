#ifndef DRIVER_H
#define DRIVER_H
#include "definitions.h"


/* Classe que define o Driver */
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

    public:
        Driver(WCHAR* path);
        virtual ~Driver();

        bool ReadSector(unsigned int offset);
        bool WriteSector(unsigned int offset);

        bool DelFile(BYTE* buffer_line);
        bool RestoreFile(const char* filename);


        int ScanCluster(char* filename, unsigned int offset, bool is_deleted);
        void IdentifyFileSector();
        void SetFileData(char *filepath);
        void LookForFolder(char *filepath);
        void LookForFile(char *filepath);
        


        void SetBufferAtFileSector();


        void PrintBootInfo();
        void PrintFileData();
        void PrintFileInfo(BYTE* buffer_line);

        void PrintBuffer();
        void PrintSector(unsigned int offset);
        


        unsigned int GetOffsetFiles();
        unsigned int GetOffsetFAT();
        unsigned int GetOffsetCurrent();
        BYTE* GetBuffer();

        void EscrevereiQualquerCoisa();
};

#endif // DRIVER_H