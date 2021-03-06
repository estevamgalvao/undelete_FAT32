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
        printf("[SYSTEM] CreateFile() succeeded.\n");
    }
    else {
        perror("[SYSTEM] CreateFile() failed.\n");
    }

    bool was_read = this->ReadSector(0);

    /* Verifying if the file was read correctly */
    if (!was_read) {
        printf("[SYSTEM] NOT 512 bytes_read error.\n");
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
    bytes_per_cluster_   = bytes_per_sector_ * sectors_per_cluster_;
    }
    printf("[SYSTEM] FAT32 Info loaded.\n");

    part_ = 0;
}

Driver::~Driver() {
    CloseHandle(handle_file_);
}

/* Read 512 bytes at the given offset and load it on buffer_ */
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

/* Write 512 bytes from buffer_ at the given offset */
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

/* Scan a cluster, at a given offset, looking for a deleted or not deleted file by its name */
int Driver::ScanCluster(char* filename, unsigned int offset, bool is_file, bool is_deleted = true) {
    this->ReadSector(offset);
    int first_byte = filename[0];
    if (is_deleted) {
        first_byte = 0xe5;
    }


    for (size_t h = 0; h < sectors_per_cluster_; h++)
    {
        for (size_t i = 0; i < N_LINES; i++)
        {
            if(buffer_[i*32] == first_byte) {
                if(!(memcmp(filename+1, &buffer_[i*32+1], 10))) {
                    printf("[SYSTEM] FOUND: ");
                    std::cout << filename << std::endl;
                    if (!is_file)
                    {
                        file_data_.starting_cluster         = *((unsigned short*)  &buffer_[i*32 + 26]);
                        file_data_.starting_cluster_high    = *((unsigned short*)  &buffer_[i*32 + 20]);
                        file_data_.starting_cluster_int     = (unsigned int) file_data_.starting_cluster_high;
                        file_data_.starting_cluster_int     <<= 16;
                        file_data_.starting_cluster_int     |= (unsigned int) file_data_.starting_cluster;
                    }
                    return 0;
                }
            }
        }
        offset += 512;
        this->ReadSector(offset);
    }
    return -1;
}


/* Look for a file by name and update the initial_cluster_(s) attributes with the result.
*  NOTE: The value assigned to the attributes is the initial CONTENT cluster of the given file.
*/
void Driver::LookForFile(char* filepath) {
    this->SetFileData(filepath);
    unsigned int FAT_sector = 0;
    offset_current_ = offset_files_;

    this->PrintFileData();
    this->ReadSector(offset_current_);
    this->PrintBuffer();

    int scan_ret;
    for (int i = PATHS_SIZE - 1; i >= 0; i--)
    {
        if (!isblank(file_data_.parent_paths[i][0]) && file_data_.parent_paths[i][0] != 0 )
        {
            printf("[SYSTEM] Looking for: %s\n", file_data_.parent_paths[i]);
            if (i == 0) // Estou procurando meu arquivo de texto
            {
                scan_ret = this->ScanCluster(file_data_.parent_paths[i], offset_current_, true, true);
            }
            else // Estou procurando pastas
            { 
                scan_ret = this->ScanCluster(file_data_.parent_paths[i], offset_current_, false, false);
            }
            
            if (scan_ret != -1)
            {
                offset_current_ = offset_files_ + (bytes_per_cluster_ * (file_data_.starting_cluster_int - 2));
            }
            else {
                offset_current_ = offset_FAT_;
                FAT_sector = file_data_.starting_cluster_int/FAT_ENTRIES_PER_SECTOR;
                offset_current_ = offset_FAT_ + FAT_sector * bytes_per_sector_;

                this->ReadSector(offset_current_);
                file_data_.starting_cluster_int %= FAT_ENTRIES_PER_SECTOR;
                
                file_data_.starting_cluster_int = ((unsigned int*)buffer_)[file_data_.starting_cluster_int];
                file_data_.starting_cluster = (unsigned short) file_data_.starting_cluster_int;
                file_data_.starting_cluster_high = (unsigned short) (file_data_.starting_cluster_int >> 16);
                
                offset_current_ = offset_files_ + (bytes_per_cluster_ * (file_data_.starting_cluster_int - 2));
                i++; // I still wanna look for the same filename
            }
        }
    }
}

/* Take a path from a file and treat the name to the correct format => example.txt -> EXAMPLE  TXT
Also translate it into a struct file_data and store it inside the object */
void Driver::SetFileData(char* filepath) {
    std::filesystem::path path(filepath);

    snprintf(file_data_.filename, 9, "%-8s", path.stem().string().c_str());
    if (path.has_extension())
    {
        snprintf(file_data_.extension, 4, "%-3s", path.extension().string().substr(1,3).c_str());
    }
    else {
        snprintf(file_data_.extension, 4, "%-3s", "");
    }
    
    // std::cout << file_data_.filename << std::endl;
    // std::cout << file_data_.extension << std::endl;


    char aux[32];
    aux[0] = 0; // Strcat could point out an error, with this I show the string is empty
    strcat_s(aux, 32, file_data_.filename);
    strcat_s(aux, 32, file_data_.extension);
    // std::cout << aux << std::endl;

    for (size_t i = 0; i < 8; i++)
    {
        file_data_.filename[i] = (char) toupper(file_data_.filename[i]);
    }

    for (size_t i = 0; i < 3; i++)
    {
        file_data_.extension[i] = (char) toupper(file_data_.extension[i]);
    }

    for (size_t i = 0; i < 32; i++)
    {
        aux[i] = (char) toupper(aux[i]);
    }
    
    memcpy(file_data_.parent_paths[0], aux, 32);
    // std::cout << (memcmp(parent_paths_[0],"TESTE3  ", 8)) << std::endl;
    // std::cout << file_data_.parent_paths[0] << std::endl;
    for (size_t i = 1; i < PATHS_SIZE; i++)
    {
        path = path.parent_path();
        if (path.parent_path().has_parent_path()) {
                snprintf(aux, 12, "%-11s", path.stem().string().c_str());
                for (size_t j = 0; j < 32; j++)
                    {
                        aux[j] = (char) toupper(aux[j]);
                    }
                memcpy(file_data_.parent_paths[i], aux, 32);
        }
    }

    file_data_.starting_cluster = 2;
    file_data_.starting_cluster_high = 0;
    file_data_.starting_cluster_int = 2;
}


void Driver::RestoreFile(unsigned int n_parts) 
{
    offset_current_ = offset_files_ + (search_array_[0] - 2) * bytes_per_cluster_;
    this->ReadSector(offset_current_);
    this->PrintBuffer();
    getchar();
    for (size_t h = 0; h < sectors_per_cluster_; h++)
    {
        for (size_t i = 0; i < N_LINES; i++)
        {
            if(buffer_[i*32] == 0xe5) {
                if(!(memcmp(file_data_.parent_paths[0]+1, &buffer_[i*32+1], 10))) {

                    buffer_[i*32] = 0x4d;
                    this->WriteSector(offset_current_);
                    break;
                }
            }
        }
        offset_current_ += 512;
 
        this->ReadSector(offset_current_);
    }
    this->PrintFileData();
    


    search_array_[n_parts+1] = 0x0FFFFFFF;
    for (unsigned int i = 1; i <= n_parts; i++)
    {
        unsigned int cluster = search_array_[i];
        unsigned int FAT_index;
        
        FAT_index = cluster%128; 
        offset_current_ = offset_FAT_ + ((cluster/128) * bytes_per_sector_);


        this->ReadSector(offset_current_);
        ((unsigned int*)(buffer_))[FAT_index] = search_array_[i+1];
        this->WriteSector(offset_current_);
        printf("\t - %u written at %4u | %6u on FAT.\n", search_array_[i+1], FAT_index, cluster);
    }


}

/* Fill up search_array with parts and clusters from my target file [<part, cluster>] */
/* Note:    First element is the cluster from the folder where the header of my file is.
            Last element is 0x0FFFFFFF to write at FAT pointing that my file ended */
void Driver::FindFileContent() 
{
    printf("[SYSTEM] Filling up the search_array.\n");
    char local_buffer[512];
    /* Sector inside offset_FAT that I'm in */
    unsigned int FAT_sector = 0;
    int counter_parts = 1;
    offset_current_ = offset_FAT_;

    this->ReadSector(offset_current_);
    memcpy(local_buffer, buffer_, 512);
    
    while (counter_parts <= 40)
    {
        for (unsigned int i = 0; i < 128; i++)
        {
            if (((unsigned int*)local_buffer)[i] == 0) // If I've found a empty space inside the FAT
            {
                offset_current_ = offset_files_ + ((i + (FAT_sector * 128)  - 2) * bytes_per_cluster_);
                this->ReadSector(offset_current_);


                if (!(memcmp("PARTE", &buffer_[0], 5)))
                {
                    this->AppendSearchPair(this->ConvertChar(buffer_[5], buffer_[6]), (i + (FAT_sector * 128)));
                    counter_parts++;
                }
            }

        }
        
        FAT_sector++;
        offset_current_ = offset_FAT_ + (FAT_sector*bytes_per_sector_);
        this->ReadSector(offset_current_);
        memcpy(local_buffer, buffer_, 512);
    }
    printf("[SYSTEM] search_array filled up.\n");
}





int Driver::ConvertChar(char first, char second) {
    int result;

    result = 10 * (first - '0');
    result += second - '0';

    return result;
}

void Driver::AppendSearchPair(unsigned int part, unsigned int cluster) {
    search_array_[part] = cluster; 
}


void Driver::PrintSearchArray() {
    for (unsigned int i = 0; i <= 40; i++)
    {
        unsigned int part = i;
        unsigned int cluster = search_array_[i];
        printf("[%2u] < %u, %u >\n", i, part, cluster);
    }
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

void Driver::PrintFileData() {
    printf("\n[-] FILEDATA STRUCT INFO\n");
    printf("[1] Filename: %s\n", file_data_.filename);
    printf("[2] Extension: %s\n", file_data_.extension);
    for (int i = 0; i < PATHS_SIZE; i++)
    {
        printf("[3.%d] Paths-%d: %s\n", i, i, file_data_.parent_paths[i]);
    }
    printf("[5] Starting Cluster: %u\n", file_data_.starting_cluster); 
    printf("[6] Starting Cluster High Part: %u\n", file_data_.starting_cluster_high);          
    // file_data_.starting_cluster_int = (unsigned int) file_data_.starting_cluster_high;
    // file_data_.starting_cluster_int <<= 16;
    // file_data_.starting_cluster_int |= (unsigned int) file_data_.starting_cluster;
    printf("[7] Starting Cluster INT (FULL): %u\n", file_data_.starting_cluster_int);          
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
    int k = 0;
    bool red = true;
    for (int i = 0; i < 16; i++)
    {
        printf("%-3d ", i);
        for (int j = 0; j < 32; j++)
        {
            if (red)
            {
                printf("\033[31m");
            }
            printf("%02x ", buffer_[i*32 + j]);
            k++;
            if (k == 4)
            {
                printf("\033[0m");
                red = !red;
                k = 0;
            }
            
            
        }
        printf("\n");
    }
    printf("\n");
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
Driver::file_data Driver::GetFileData() {
    return file_data_;
}
BYTE* Driver::GetBuffer() {
    return buffer_;
}
