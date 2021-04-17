#include "pagetable.h"
#include <math.h>

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
    
	int frame = 0; 
	// Find free frame 
		// NOTE free "frame"? Not free "page"? This is going user-to-mem, not mem-to-user
		// TODO check the above and redo these notes
		// Iterate through the list of virtual memory. That means (TODO how to iterate through virt mem). 
		// If a free space is found, use it. If the end is reached without a free space, pick a victim to replace. 
		// Should be able to see "<FREE SPACE>" on the virtual data? 
	// Find replacement victim, if necessary
	
	// TODO: implement this!
	
	_table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
	// Convert virtual address to page_number and page_offset
    uint32_t n = (uint32_t)log2(_page_size); 
    int page_number = (int)(virtual_address >> n); 
    int page_offset = (int)((_page_size - 1) & virtual_address); 

	// Combination of pid and page number act as the key to look up frame number
	std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
	
	// If entry exists, look up frame number and convert virtual to physical address
	int address = -1;
	if (_table.count(entry) > 0)
	{
		// TODO: implement this!
	}

	return address;
}

void PageTable::print()
{
	int i;

	std::cout << " PID  | Page Number | Frame Number" << std::endl;
	std::cout << "------+-------------+--------------" << std::endl;

	std::vector<std::string> keys = sortedKeys();

	for (i = 0; i < keys.size(); i++)
	{
		// TODO: print all pages
	}
}
