/**
 * main
 *
 * The goal of this project is to generate and delete files inside a FAT32 device
 * and at the end, undelete one of them.
 *
 * author: Estevam Galvão Albuquerque
 * github: estevamgalvao
 *
 */

#include "definitions.h"
#include "driver.h"

int main(int argc, char const *argv[])
{
    WCHAR* path = L"\\\\.\\Z:";

    try
    {
        Driver FAT32(path);
        FAT32.PrintBootInfo();
        // FAT32.PrintFileInfo(&FAT32.GetBuffer()[11 * 32]);

        // FAT32.SetFileData("\\pasta1\\pasta1_1\\file2.txt");
        // FAT32.ScanCluster("AILE1   TXT", FAT32.GetOffsetFiles(), false);
        FAT32.LookForFile("\\SD1\\aile128.txt");
        // FAT32.LookForFile("\\12345678.txt");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
