#include "definitions.h"
#include "driver.h"

int main(int argc, char const *argv[])
{
    WCHAR* path = L"\\\\.\\Z:";

    try
    {
        Driver FAT32(path);
        FAT32.PrintBuffer();
        FAT32.PrintBootInfo();
        FAT32.EscrevereiQualquerBosta();

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}
