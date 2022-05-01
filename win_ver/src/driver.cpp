/**
 * driver.cc
 *
 * Class which simulate operations on a FAT32 Device
 *
 * author: Estevam Galvão Albuquerque
 * github: estevamgalvao
 *
 */


#include "driver.h"


Driver::Driver(WCHAR* path) {
    file_path_ = path;

    /* Opening the FAT32 Device as a File */
    handle_file_ = CreateFileW(file_path_,                  //name of the file
                    GENERIC_READ | GENERIC_WRITE,           //open for writing/reading?
                    FILE_SHARE_READ | FILE_SHARE_WRITE,     //sharing mode, none
                    NULL,                                   //default security descriptor
                    OPEN_EXISTING,                          //
                    0,                                      //open as a device
                    NULL);
    

    /* Verifying if the file was opened correctly */
    if (handle_file_ != INVALID_HANDLE_VALUE) {
        printf("-- CreateFile() succeeded. --\n");
    }
    else {
        perror("--CreateFile() failed. --\n");
    }

    bool was_read = this->ReadSector(0);

    /* Verifying if the file was read correctly */
    if (!was_read) {
        printf("-- NOT 512 bytes_read error --\n");
        CloseHandle(handle_file_);
    }
    /* Parsing bytes */
    else {
    bytes_per_sector_    = *((unsigned short*) &buffer_[11]);
    reserved_sectors_    = *((unsigned short*) &buffer_[14]);
    FAT_sectors_         = *((unsigned int*)   &buffer_[36]);
    sectors_per_cluster_ = *((unsigned char*)  &buffer_[13]);
    offset_files_        = bytes_per_sector_ * (reserved_sectors_ + (FAT_sectors_ * 2));
    offset_FAT_          = reserved_sectors_ * bytes_per_sector_;
    }
}

Driver::~Driver() {
    CloseHandle(handle_file_);
}

bool Driver::ReadSector(unsigned int offset) {
    /* Amount of bytes read by ReadFile() */
    DWORD bytes_read;
    /* Jumping the file pointer to the offset informed */
    SetFilePointer(handle_file_, offset, NULL, FILE_BEGIN);
    /* Reading file with the buffer's content */
    ReadFile(handle_file_, buffer_, SECTOR_SIZE, &bytes_read, NULL);

    /* Checking if the correct amount of bytes were read - if not, error */
    if (bytes_read != SECTOR_SIZE) {
        return false;
    }
    else {
        return true;
    }
}

bool Driver::WriteSector(unsigned int offset) {

    /* GETTING ACCESS FOR WRITING AT FILE - LOCK UNLOCK DEVICE*/
    bool IO_permission_flag;
    DWORD bytes_returned;

    IO_permission_flag = DeviceIoControl(handle_file_,
                            FSCTL_LOCK_VOLUME, //FSCTL_DISMOUNT_VOLUME antes p forçar
                            NULL,
                            0,
                            NULL,
                            0,
                            &bytes_returned,
                            NULL);

    if (!IO_permission_flag) {
        perror("[!] LOCK failed.\n");
    }
    else {
        printf("[!] LOCK succeeded.\n");
    }

    /* Amount of bytes written by WriteFile() */
    DWORD bytes_written;
    /* Jumping the file pointer to the offset informed */
    SetFilePointer(handle_file_, offset, NULL, FILE_BEGIN);
    /* Wrinting file with the buffer's content */
    WriteFile(handle_file_, buffer_, SECTOR_SIZE, &bytes_written, NULL);

    IO_permission_flag = DeviceIoControl(handle_file_,
                        FSCTL_UNLOCK_VOLUME,
                        NULL,
                        0,
                        NULL,
                        0,
                        &bytes_returned,
                        NULL);

    if (!IO_permission_flag) {
        perror("[!] UNLOCK failed.\n");
    }
    else {
        printf("[!] UNLOCK succeeded.\n");
    }

    /* Checking if the correct amount of bytes were written - if not, error */
    if (bytes_written != SECTOR_SIZE) {
        return false;
    }
    else {
        return true;
    }

}

bool Driver::DelFile(BYTE* buffer_line) {
    unsigned short starting_cluster         = *((unsigned short*)  &buffer_line[26]);
    unsigned short starting_cluster_high    = *((unsigned short*)  &buffer_line[20]);
    // printf("[6] Starting Cluster: %d\n", starting_cluster); 
    // printf("[7] Starting Cluster High Part: %d\n", starting_cluster_high); 


    //edito o buffer
    // this->ReadSector(offset_files_);
    buffer_line[0] = 0x50;
    // buffer_[11 * 32] = 0xaa;
    this->PrintBuffer();
    this->WriteSector(offset_files_);

    this->ReadSector(offset_FAT_);

    return true;
}

bool Driver::RestoreFile(const char* filename) {

    for (size_t i = 0; i < N_LINES; i++)
    {   
        if(buffer_[i*32] == 0xe5) {
            if(!(memcmp(filename+1, &buffer_[i*32+1], 10))) {
                // printf("Achei o mesmo arquivo.\n");
                buffer_[i*32] = 0x41;

                offset_current_ = offset_files_; // Como lidar com o fato de que ele pode não estar no primeiro setor
                this->WriteSector(offset_current_);

                unsigned short starting_cluster         = *((unsigned short*)  &buffer_[i*32 + 26]);
                unsigned short starting_cluster_high    = *((unsigned short*)  &buffer_[i*32 + 20]);
                // printf("[6] Starting Cluster: %d\n", starting_cluster); 
                // printf("[7] Starting Cluster High Part: %d\n", starting_cluster_high);

                offset_current_ = offset_FAT_;
                // * starting_cluster; // Como entraria o cluster_high aqui?
                this->ReadSector(offset_current_);
                this->PrintBuffer();
                buffer_[starting_cluster*4]     = (unsigned char) 0xFF;
                buffer_[starting_cluster*4 + 1] = (unsigned char) 0xFF;
                buffer_[starting_cluster*4 + 2] = (unsigned char) 0xFF;
                buffer_[starting_cluster*4 + 3] = (unsigned char) 0x0F;
                this->PrintBuffer();

                this->WriteSector(offset_current_);

            }
        }
    }

    return true;
}


void Driver::PrintBootInfo() {
    printf("\n[-] BOOT SECTOR INFO\n");
    printf("[1] Bytes per sector: %u\n", bytes_per_sector_);    
    printf("[2] Reserved sectors: %u\n", reserved_sectors_);    
    printf("[3] FAT sectors: %u\n", FAT_sectors_);         
    printf("[4] Sectors per cluster: %u\n", sectors_per_cluster_); 
    printf("[5] FAT offset: %u\n", offset_FAT_);
    printf("[6] Files offset: %u\n", offset_files_);
}

void Driver::PrintFileInfo(BYTE* buffer_line) {
    // filename[8] = 0 to indicate to C %s argument to stop printing a string
    char filename[9];                       memcpy(filename, buffer_line, 8); filename[8] = 0;
    char extension[4];                      memcpy(extension, buffer_line+8, 3); extension[3] = 0;
    unsigned char attributes_bitstream      = *((unsigned char*)   &buffer_line[11]);
    // unsigned int reserved                = *((unsigned char*)  &buffer_line[12]);
    unsigned short time                     = *((unsigned short*)  &buffer_line[14]);
    unsigned short date                     = *((unsigned short*)  &buffer_line[16]);
    unsigned short starting_cluster         = *((unsigned short*)  &buffer_line[26]);
    unsigned short starting_cluster_high    = *((unsigned short*)  &buffer_line[20]);
    unsigned int file_size                  = *((unsigned int*)  &buffer_line[28]);

    printf("\n[-] FILE INFO\n");
    printf("[1] Filename: %s\n", filename);
    printf("[2] Extension: %s\n", extension);
    printf("[3] Attributes bitstream: %x\n", attributes_bitstream);    
    printf("[4] Time: %d\n", time);
    printf("[5] Date: %d\n", date);
    printf("[6] Starting Cluster: %d\n", starting_cluster); 
    printf("[7] Starting Cluster High Part: %d\n", starting_cluster_high);         
    printf("[8] File Size: %u\n", file_size);
}

void Driver::PrintBuffer() {
    printf("\n");
    for (int i = 0; i < 16; i++)
    {
        printf("%-3d ", i);
        for (int j = 0; j < 32; j++)
        {
            printf("%02x ", buffer_[i*32 + j]);
        }
        printf("\n");
    }
}

void Driver::PrintSector(unsigned int offset) {
    this->ReadSector(offset);
    this->PrintBuffer();
}


unsigned int Driver::GetOffsetFiles() {
    return offset_files_;
}
unsigned int Driver::GetOffsetFAT() {
    return offset_FAT_;
}
unsigned int Driver::GetOffsetCurrent() {
    return offset_current_;
}
BYTE* Driver::GetBuffer() {
    return buffer_;
}



void Driver::IdentifyFileSector() {};