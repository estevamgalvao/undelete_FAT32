#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <Windows.h>
#include <filesystem>

#define PATH "/mnt/p"
#define SECTOR_SIZE 512
#define FAT_ENTRIES_PER_SECTOR 128
#define LINE_SIZE 32
#define N_LINES 16