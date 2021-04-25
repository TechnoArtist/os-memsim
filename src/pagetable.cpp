#include "pagetable.h"
#include <math.h>
#include <string>
#include <cstring>

// TODO find a better solution than copying and renaming the method for each .cpp file
void splitStringPagetable(std::string text, char d, std::vector<std::string>& result); 

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

bool PageTable::entryExists(int32_t pid, int page_number) {
	try {
		_table.at(std::to_string(pid) + "|" + std::to_string(page_number)); 
	} catch (const std::out_of_range& e) {
		return false;
	}
	return true;
}

void PageTable::print()
{
	int i;
	std::cout << " PID  | Page Number | Frame Number" << std::endl;
	std::cout << "------+-------------+--------------" << std::endl;

	std::vector<std::string> keys = sortedKeys();
	
	std::vector<std::string> splitkey; 
	int pid; 
	int pagenum; 
    std::map<std::string,int>::iterator iter;
    iter = _table.begin();
	for (i = 0; i < keys.size(); i++)
	{
		splitStringPagetable(iter->first, '|', splitkey);
		pid = atoi(splitkey[0].c_str());
		pagenum = atoi(splitkey[1].c_str());
		printf("%6i|%13i|%14i\n", pid, pagenum, iter->second);
        iter++;
	}
}

std::map<std::string, int> PageTable::getTable() {
	return _table; 
}


/*
	text: string to split
	d: character delimiter to split `text` on
	result: vector of strings - result will be stored here
*/
void splitStringPagetable(std::string text, char d, std::vector<std::string>& result)
{
	enum states { NONE, IN_WORD, IN_STRING } state = NONE;

	int i;
	std::string token;
	result.clear();
	for (i = 0; i < text.length(); i++)
	{
		char c = text[i];
		switch (state) {
			case NONE:
				if (c != d)
				{
					if (c == '\"')
					{
						state = IN_STRING;
						token = "";
					}
					else
					{
						state = IN_WORD;
						token = c;
					}
				}
				break;
			case IN_WORD:
				if (c == d)
				{
					result.push_back(token);
					state = NONE;
				}
				else
				{
					token += c;
				}
				break;
			case IN_STRING:
				if (c == '\"')
				{
					result.push_back(token);
					state = NONE;
				}
				else
				{
					token += c;
				}
				break;
		}
	}
	if (state != NONE)
	{
		result.push_back(token);
	}
} // splitString()
