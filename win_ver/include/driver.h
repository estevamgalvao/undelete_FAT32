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

        BYTE buffer_[512];
        WCHAR* file_path_;

        HANDLE handle_file_;

    public:
        Driver(WCHAR* path);
        virtual ~Driver();

        bool ReadSector(unsigned int offset);
        bool WriteSector(unsigned int offset);

        bool DelFile(BYTE* buffer_line);
        bool RestoreFile(const char* filename);

        void PrintBootInfo();
        void PrintFileInfo(BYTE* buffer_line);

        void PrintBuffer();
        void PrintSector(unsigned int offset);
        
        void IdentifyFileSector();

        unsigned int GetOffsetFiles();
        unsigned int GetOffsetFAT();
        unsigned int GetOffsetCurrent();
        BYTE* GetBuffer();

        void EscrevereiQualquerCoisa();
};

#endif // DRIVER_H