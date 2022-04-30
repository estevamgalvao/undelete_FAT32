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
        unsigned int current_offset_;

        BYTE buffer_[512];
        WCHAR* file_path_;

        HANDLE handle_file_;

    public:
        Driver(WCHAR* path);
        virtual ~Driver();

        bool ReadSector(unsigned int offset);
        bool WriteSector(unsigned int offset);
        void PrintBootInfo();
        void PrintBuffer();
        void EscrevereiQualquerCoisa();
};

#endif // DRIVER_H