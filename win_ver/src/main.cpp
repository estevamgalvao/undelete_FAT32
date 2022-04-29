#include <definitions.h>

struct buffer_data
{
    int bytes_per_sector;
    int reserved_sectors;
    int FAT_sectors;
    int offset_files;
    int sectors_per_cluster;
};



int main(int argc, char const *argv[])
{
    std::fstream driver;
    buffer_data buffer_data_;
    driver.open("\\\\.\\P:", std::fstream::in | std::fstream::out | std::fstream::binary);

    if(!driver.good()) {
        perror("not found.");
    }

    char buffer[512];

    driver.seekg(0, std::ios_base::beg);
    driver.read(buffer, 512);

    buffer_data_.bytes_per_sector    = *((unsigned short*) &buffer[11]);
    buffer_data_.reserved_sectors    = *((unsigned short*) &buffer[14]);
    buffer_data_.FAT_sectors         = *((unsigned int*)   &buffer[36]);
    buffer_data_.sectors_per_cluster = buffer[13];
    buffer_data_.offset_files        = buffer_data_.bytes_per_sector * (buffer_data_.reserved_sectors + (buffer_data_.FAT_sectors * 2));

    printf("%d\n", buffer_data_.offset_files);

    driver.seekg(buffer_data_.offset_files, std::ios_base::beg);
    driver.read(buffer, 512);

    int count = 1;
    for (size_t i = 0; i < 16; i++)
    {
        printf("%-3d ", i);
        for (size_t j = 0; j < 32; j++)
        {
            printf("%02x ", (unsigned char) buffer[i*32 + j]);
        }
        printf("\n");
    }        



    
    
    // std::cout << buffer[511] << buffer[510] << std::endl;    
    // driver.seekp(0, std::ios_base::beg);
    // driver.write(buffer, 512);

    return 0;
}
