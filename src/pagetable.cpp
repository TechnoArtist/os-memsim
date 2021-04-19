#include "pagetable.h"
#include <math.h>
#include <string>
#include <cstring>

PageTable::PageTable(int page_size)
{
	_page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
	std::vector<std::string> keys;

	std::map<std::string, int>::iterator it;
	for (it = _table.begin(); it != _table.end(); it++)
	{
		keys.push_back(it->first);
	}

	std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

	return keys;
}

/*
    This is a method to create a fresh virtual page by assigning it to an empty frame. 
    
    Input: pid: The ID of the currently running process, used as an ingredient to find available frames. 
    Input: page_number: The number of the virtual page being allocated, used as an ingredient to find available frames. 
*/
void PageTable::addEntry(uint32_t pid, int page_number)
{
	// Combination of pid and page number act as the key to look up frame number
	std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    bool frameFound = false;
	int frame = 0; 
    while(!(frameFound)) {
        frameFound = true;
        std::map<std::string,int>::iterator it;
        for(it = _table.begin(); it != _table.end() ; it++) {
            //KEY=it->first, Value = it->second
            if (it->second == frame) {
                frameFound = false; 
            }
        }
        if (frameFound) {
            break; 
        }
        frame++;   
    }
	_table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{   
    //using bitshifting
    uint32_t numBits = (uint32_t)log2(_page_size);//num bits for page offset
    int pageNum = (int)(virtual_address >> numBits);
    int offset = (int)((_page_size - 1) & virtual_address);
	// Combination of pid and page number act as the key to look up frame number
	std::string entry = std::to_string(pid) + "|" + std::to_string(pageNum);
	// If entry exists, look up frame number and convert virtual to physical address
	int address = -1;
	if (_table.count(entry) > 0)
	{
		address = _page_size * _table.at(entry) + offset;
	}
	return address;
}

void PageTable::print()
{
	int i;
	std::cout << " PID  | Page Number | Frame Number" << std::endl;
	std::cout << "------+-------------+--------------" << std::endl;

	std::vector<std::string> keys = sortedKeys();

    std::map<std::string,int>::iterator iter;
    iter = _table.begin();
	for (i = 0; i < keys.size(); i++)
	{   //frame number
		printf("%6u|%13i|%14i\n", iter->first, iter->first, iter->second);
        iter++;
	}
}
