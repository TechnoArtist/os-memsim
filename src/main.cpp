#include <iostream>
#include <string>
#include <cstring>
#include "mmu.h"
#include "pagetable.h"

/* Master todo list (does not auto-update)

All starter todos are in the .cpp files. 
The .h files are useful reference material. 

main.cpp
	- Prompt loop (prompting the user for input, and handling their commands; the main loop) 
		* This will likely tie together the program
	- createProcess() (the "create" command)
	- allocateVariable() (the "allocate" command)
	- setVariable() (the "set" command)
	- freeVariable() (the "free" command)
	- terminateProcess() (the exit command)

mmu.cpp
	- finish the print command 

pagetable.cpp
	+ finish addEntry()
	+ finish getPhysicalAddress() 
		+ convert virtual addr to page_number and _offset
		+ finish the lookup
	- finish the print command


I'd recommend starting with the pagetable todos, as they seem least dependent on other factors. 
I'd then recommend working on the various incomplete methods in main, adding them one by one to the prompt loop for testing. 
Not sure exactly when the print commands should be done. Maybe before the main methods, or maybe in parallel, if they have matching sections. 

Next focuses: 
	- allocateVariable() 
	- - createProcess() (relies on allocateVariable())
	- ? Pagetable (currently have questions on splitting the string)
	- ? Mmu printing (see above questions)

*/

int test; // TODO clean this up before turning in the project

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);
void splitString(std::string text, char d, std::vector<std::string>& result); 

int main(int argc, char **argv)
{
	// Ensure user specified page size as a command line parameter
	if (argc < 2)
	{
		fprintf(stderr, "Error: you must specify the page size\n");
		return 1;
	}

	// Print opening instuction message
	int page_size = std::stoi(argv[1]);
	printStartMessage(page_size);

	// Create physical 'memory'
	uint32_t mem_size = 67108864;
	void *memory = malloc(mem_size); // 64 MB (64 * 1024 * 1024)

	// Create MMU and Page Table
	Mmu *mmu = new Mmu(mem_size);
	PageTable *page_table = new PageTable(page_size);

	// Prompt loop
	std::string command;
	std::cout << "> ";
	std::getline (std::cin, command);
	std::vector<std::string> split_command; 
	splitString(command, ' ', split_command); 
	while (command != "exit") {
		
		page_table->addEntry(1026, 5);
		page_table->addEntry(1024, 4);
		page_table->addEntry(1025, 8);
		page_table->addEntry(1028, 13);
		page_table->print();
		
	   	
		// Handle commands
		if(split_command[0].compare("print") == 0) {
			// TODO handle command (include input error checking)
			/* print <object>
				If <object> is "mmu", print the MMU memory table
				If <object> is "page", print the page table (do not need to print anything for free frames)
				If <object> is "processes", print a list of PIDs for processes that are still running
				If <object> is a "<PID>:<var_name>", print the value of the variable for that process
					If variable has more than 4 elements, just print the first 4 followed by "... [N items]" (where N is the number of elements)
			*/
		} else if(split_command[0].compare("create") == 0) {
			// TODO handle command (include input error checking)
			/* create <text_size> <data_size>
				Initializes a new process
				Prints the PID
			*/
		} else if(split_command[0].compare("allocate") == 0) {
			// TODO handle command (include input error checking)
			/* allocate <PID> <var_name> <data_type> <number_of_elements>
				Allocated memory on the heap (how much depends on the data type and the number of elements)
				Print the virtual memory address
			*/
		} else if(split_command[0].compare("set") == 0) {
			// TODO handle command (include input error checking)
			/* set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N>
				Set the value for variable <var_name> starting at <offset>
				Note: multiple contiguous values can be set with one command
			*/
		} else if(split_command[0].compare("free") == 0) {
			// TODO handle command (include input error checking)
			/* free <PID> <var_name>
				Deallocate memory on the heap that is associated with <var_name>
			*/
		} else if(split_command[0].compare("terminate") == 0) {
			// TODO handle command (include input error checking)
			/* terminate <PID>
				Kill the specified process
				Free all memory associated with this process
			*/
		} else {
			// TODO "Bad command, try again"
		}
		// exit is handled by the while loop. 

		// Get next command
		std::cout << "> ";
		std::getline (std::cin, command);
	}

	// Clean up
	free(memory);
	delete mmu;
	delete page_table;

	return 0;
}

void printStartMessage(int page_size)
{
	std::cout << "Welcome to the Memory Allocation Simulator! Using a page size of " << page_size << " bytes." << std:: endl;
	std::cout << "Commands:" << std:: endl;
	std::cout << "  * create <text_size> <data_size> (initializes a new process)" << std:: endl;
	std::cout << "  * allocate <PID> <var_name> <data_type> <number_of_elements> (allocated memory on the heap)" << std:: endl;
	std::cout << "  * set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N> (set the value for a variable)" << std:: endl;
	std::cout << "  * free <PID> <var_name> (deallocate memory on the heap that is associated with <var_name>)" << std:: endl;
	std::cout << "  * terminate <PID> (kill the specified process)" << std:: endl;
	std::cout << "  * print <object> (prints data)" << std:: endl;
	std::cout << "	* If <object> is \"mmu\", print the MMU memory table" << std:: endl;
	std::cout << "	* if <object> is \"page\", print the page table" << std:: endl;
	std::cout << "	* if <object> is \"processes\", print a list of PIDs for processes that are still running" << std:: endl;
	std::cout << "	* if <object> is a \"<PID>:<var_name>\", print the value of the variable for that process" << std:: endl;
	std::cout << std::endl;
}

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	
	//   - create new process in the MMU
	uint32_t pid = mmu->createProcess(); 
	
	//   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
	//allocateVariable(); 
	//allocateVariable(); 
	//allocateVariable(); 
	
	//   - print pid
	printf("%i", pid); //TODO formatting
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	//   - find first free space within a page already allocated to this process that is large enough to fit the new variable
	//   - if no hole is large enough, allocate new page(s)
	//   - insert variable into MMU
	//   - print virtual memory address
}

void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
	// TODO: implement this!
	//   - look up physical address for variable based on its virtual address / offset
	//   - insert `value` into `memory` at physical address
	//   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
	//		   multiple elements of an array)
}

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	//   - remove entry from MMU
	//   - free page if this variable was the only one on a given page
}

void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	//   - remove process from MMU
	//   - free all pages associated with given process
}


/*
	text: string to split
	d: character delimiter to split `text` on
	result: vector of strings - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
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

