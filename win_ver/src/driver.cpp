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
        perror("UNLOCK failed.\n");
    }
    else {
        perror("UNLOCK succeeded.\n");
    }

    /* Amount of bytes written by WriteFile() */
    DWORD bytes_written;
    /* Jumping the file pointer to the offset informed */
    SetFilePointer(handle_file_, offset, NULL, FILE_BEGIN);
    /* Wrinting file with the buffer's content */
    ReadFile(handle_file_, buffer_, SECTOR_SIZE, &bytes_written, NULL);

    IO_permission_flag = DeviceIoControl(handle_file_,
                        FSCTL_UNLOCK_VOLUME,
                        NULL,
                        0,
                        NULL,
                        0,
                        &bytes_returned,
                        NULL);

    if (!IO_permission_flag) {
        perror("UNLOCK failed.\n");
    }
    else {
        perror("UNLOCK succeeded.\n");
    }

    /* Checking if the correct amount of bytes were written - if not, error */
    if (bytes_written != SECTOR_SIZE) {
        return false;
    }
    else {
        return true;
    }

}


void Driver::PrintBootInfo() {
    printf("\n[-] BOOF SECTOR INFO\n");
    printf("[1] Bytes per sector: %u\n", bytes_per_sector_);    
    printf("[2] Reserved sectors: %u\n", reserved_sectors_);    
    printf("[3] FAT sectors: %u\n", FAT_sectors_);         
    printf("[4] Sectors per cluster: %u\n", sectors_per_cluster_); 
    printf("[5] Files offset: %u\n", offset_files_);
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

void Driver::EscrevereiQualquerCoisa() {

    bool FLAG;


    DWORD bytes_returned;
    FLAG = DeviceIoControl(handle_file_,
                            FSCTL_LOCK_VOLUME, //FSCTL_DISMOUNT_VOLUME antes
                            NULL,
                            0,
                            NULL,
                            0,
                            &bytes_returned,
                            NULL);

    if (!FLAG) {
        perror("LOCK failed.");
    }
    else {
        perror("LOCK succeeded.");
    }

    current_offset_ = offset_files_;
    this->ReadSector(current_offset_);
    SetFilePointer(handle_file_, current_offset_, NULL, FILE_BEGIN);
    // buffer_[12*32] = 0x41;
    memset(buffer_, 1, 512);
    DWORD bytes_written;
    WriteFile(handle_file_, buffer_, SECTOR_SIZE, &bytes_written, NULL);

    FLAG = DeviceIoControl(handle_file_,
                        FSCTL_UNLOCK_VOLUME,
                        NULL,
                        0,
                        NULL,
                        0,
                        &bytes_returned,
                        NULL);

    if (!FLAG) {
        perror("UNLOCK failed.");
    }
    else {
        perror("UNLOCK succeeded.");
    }
}