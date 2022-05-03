#include <definitions.h>

struct buffer_data
{
    int bytes_per_sector;
    int reserved_sectors;
    int FAT_sectors;
    int offset_files;
    int sectors_per_cluster;
};


long long int current_offset;



void PrettyPrinterBuffer(char* buffer) {
    for (int i = 0; i < 16; i++)
    {
        printf("%-3d ", i);
        for (int j = 0; j < 32; j++)
        {
            printf("%02x ", (unsigned char) buffer[i*32 + j]);
        }
        printf("\n");
    }
}

int* NameToHex(const char* file_name) {
    static int ret[3];

    ret[0] = *((unsigned short*) &file_name[0]);
    ret[1] = *((unsigned short*) &file_name[4]);
    ret[2] = *((unsigned short*) &file_name[8]);

    return ret;
}

void func (const char* file_name, const char* line) {
    if(line[0] == 0xE5) {
        return;
    }
    else {
        if (strncmp(file_name, line, 11) == 0)
        {
            for (size_t j = 0; j < 11; j++)
            {
                // if (j == 8) {
                //     continue;
                // }
                printf("%02x ", (unsigned char) file_name[j]);
            }
            printf("\n");
        }
    }
}


bool func1 (const char* file_name, const char* line) {
    if((unsigned char) line[0] == 0xe5) {
        if (strncmp(file_name+1, line+1, 10) == 0)
        {
            for (size_t j = 0; j < 11; j++)
            {
                // if (j == 8) {
                //     continue;
                // }
                printf("%02x ", (unsigned char) file_name[j]);
            }
            printf("\n");
            return true;
        }
    }
    return false;
}

void recover(std::fstream &Driver, char* buffer, int offset, buffer_data buffer_data_) {
    int FAT_index, FAT_sector;
    int FAT_offset = buffer_data_.reserved_sectors * buffer_data_.bytes_per_sector;

    char local_buffer[512];
    Driver.seekg(0);

    buffer[offset] = 0x41;

    PrettyPrinterBuffer(buffer);

    std::cout << "tellg: " << Driver.tellg() << std::endl;
    std::cout << "tellp: " << Driver.tellp() << std::endl;
    std::cout << "rdstate: " << Driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;

    printf("[Escrita FILES]Current Offset: %lld\n", current_offset);
    /* CREIO QUE O ERRO ESTEJA AQUI POIS NÃO ESTÁ ESCREVENDO NO DISCO */
    Driver.seekp(current_offset);

    std::cout << "tellg: " << Driver.tellg() << std::endl;
    std::cout << "tellp: " << Driver.tellp() << std::endl;
    std::cout << "rdstate: " << Driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;

    Driver.write(buffer, 512);
    // Driver.flush();
    if (Driver.bad())
    {
        perror("Error Driver.\n");
    }


    FAT_index = (*((unsigned short*) &buffer[offset + 0x1A]))%128; // índice na tabela FAT dentro de um setor (primeiro, segundo, terceiro elemento na tabela)
    FAT_sector = (*((unsigned short*) &buffer[offset + 0x1A]))/128; // índice do setor FAT (primeiro, segundo, terceiro setor)
    
    current_offset = FAT_offset + FAT_sector*buffer_data_.bytes_per_sector;
    printf("[ESCRITA FAT]Current Offset: %lld\n", current_offset);
    Driver.seekg(current_offset);
    Driver.read(local_buffer, 512);

    local_buffer[FAT_index*4]     = (unsigned char) 0xFF;
    local_buffer[FAT_index*4 + 1] = (unsigned char) 0xFF;
    local_buffer[FAT_index*4 + 2] = (unsigned char) 0xFF;
    local_buffer[FAT_index*4 + 3] = (unsigned char) 0x0F;

    PrettyPrinterBuffer(local_buffer);

    Driver.seekg(current_offset);
    /* CREIO QUE O ERRO ESTEJA AQUI POIS NÃO ESTÁ ESCREVENDO NO DISCO */
    std::cout << "tellg: " << Driver.tellg() << std::endl;
    std::cout << "tellp: " << Driver.tellp() << std::endl;
    std::cout << "rdstate: " << Driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;
    Driver.write(local_buffer, 512);
    // Driver.flush();

    //std::ofstream  Driver("filecopy.txt"", std::ios::binary);
}




int main()
{

    std::fstream driver;
    buffer_data buffer_data_;
    driver.open("\\\\.\\P:", std::fstream::in | std::fstream::out | std::fstream::binary);

    if(!driver.good()) {
        perror("not found.");
    }

    char buffer[512];

    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;
    driver.seekg(0);
    driver.read(buffer, 512);
    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;
    // // memset(buffer, '0', 512);

    // driver.seekp(0, std::ios_base::beg);
    // driver.write(buffer, 512);

    driver.seekg(0);

    buffer_data_.bytes_per_sector    = *((unsigned short*) &buffer[11]);
    buffer_data_.reserved_sectors    = *((unsigned short*) &buffer[14]);
    buffer_data_.FAT_sectors         = *((unsigned int*)   &buffer[36]);
    buffer_data_.sectors_per_cluster = buffer[13];
    buffer_data_.offset_files        = buffer_data_.bytes_per_sector * (buffer_data_.reserved_sectors + (buffer_data_.FAT_sectors * 2));

    printf("%d\n", buffer_data_.bytes_per_sector);
    printf("%d\n", buffer_data_.reserved_sectors);
    printf("%d\n", buffer_data_.FAT_sectors);
    printf("%d\n", buffer_data_.sectors_per_cluster);
    
    printf("%d\n", buffer_data_.offset_files);
    

    current_offset = buffer_data_.offset_files;
    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;
    driver.seekg(current_offset);
    driver.read(buffer, 512);
    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;

    driver.seekg(0);

    for (int i = 0; i < 16; i++)
    {
        printf("%-3d ", i);
        for (int j = 0; j < 32; j++)
        {
            printf("%02x ", (unsigned char) buffer[i*32 + j]);
        }
        printf("\n");
        if(func1("APAGA1~1TXT", &buffer[i*32])) {
            recover(driver, buffer, i*32, buffer_data_);
        }
    }


    printf("BUFFER ORIGINAL\n");
    PrettyPrinterBuffer(buffer);

    // std::cout << buffer[511] << buffer[510] << std::endl;    
    // driver.seekp(0, std::ios_base::beg);
    // driver.write(buffer, 512);

    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;


    current_offset = buffer_data_.offset_files;
    driver.seekp(current_offset);

    std::cout << "tellg: " << driver.tellg() << std::endl;
    std::cout << "tellp: " << driver.tellp() << std::endl;
    std::cout << "rdstate: " << driver.rdstate() << std::endl;
    std::cout << "------------------------------- " << std::endl;
    driver.write(buffer, 512);
    driver.close();
    return 0;
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

    offset_current_ = offset_files_;
    this->ReadSector(offset_current_);
    SetFilePointer(handle_file_, offset_current_, NULL, FILE_BEGIN);
    buffer_[11*32] = 0xaa;
    // memset(buffer_, 1, 512);
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




bool Driver::RestoreFile(const char* filename) {

    for (size_t i = 0; i < N_LINES; i++)
    {   

        if(!(memcmp(filename+1, &buffer_[i*32+1], 10))) {
            printf("I've found it.\n");
            for (size_t j = 1; j < 10; j++)
            {
                printf("F: %02x\nB: %02x\n", filename[j], buffer_[i*32 + j]);
            }
        }
        printf("\n");
    }

    for (size_t j = 0; j < 11; j++)
    {
        printf("%02x ", (const char*) buffer_[11*32 + j]);
    }


    bool compare = memcmp(filename, &buffer_[11*32], 11);
    printf("FLAG: %d", compare);
    return true;
}


void Driver::LookForFile(char* filepath) {
    int scan_cluster_ret;
    int FAT_sector;

    this->SetFileData(filepath);
    offset_current_ = offset_files_;

    starting_cluster_int_ = 12;

    scan_cluster_ret = this->ScanCluster(filename_);
    if (scan_cluster_ret == -1)
    {
        FAT_sector = starting_cluster_int_/FAT_ENTRIES_PER_SECTOR;
        offset_current_ = offset_FAT_ + FAT_sector * bytes_per_sector_;
        starting_cluster_int_ = starting_cluster_int_%FAT_ENTRIES_PER_SECTOR;

        this->ReadSector(offset_current_);
        this->PrintBuffer();
        starting_cluster_int_ = buffer_[starting_cluster_int_*4];
        std::cout << "Starting cluster: " << starting_cluster_int_ << std::endl;
    }
}