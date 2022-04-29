#include <definitions.h>

int main(int argc, char const *argv[])
{
    std::fstream driver;
    driver.open("\\\\.\\P:", std::fstream::in | std::fstream::out | std::fstream::binary);

    if(!driver.good()) {
        perror("not found.");
    }

    char buffer[1024];

    // driver.seekg(0, std::ios_base::beg);
    driver.read(buffer, 1024);
    int count = 1;
    for (size_t i = 0; i < 1024; i++)
    {
        printf("%02x ", (unsigned char) buffer[i]);
        count++;
        if (count == 20) {
            printf("\n");
            count = 1;
        }
    }        



    
    
    // std::cout << buffer[511] << buffer[510] << std::endl;    
    // driver.seekp(0, std::ios_base::beg);
    // driver.write(buffer, 512);

    return 0;
}
