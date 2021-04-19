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
	- finish addEntry()
	- finish getPhysicalAddress() 
		- convert virtual addr to page_number and _offset
		- finish the lookup
	- finish the print command


I'd recommend starting with the pagetable todos, as they seem least dependent on other factors. 
I'd then recommend working on the various incomplete methods in main, adding them one by one to the prompt loop for testing. 
Not sure exactly when the print commands should be done. Maybe before the main methods, or maybe in parallel, if they have matching sections. 


*/

int test; // TODO clean this up before turning in the project

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table);
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory);
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table);
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table);

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
	while (command != "exit") {
        page_table.addEntry(1026, 5);
        page_table.addEntry(1024, 4);
        page_table.addEntry(1028, 13);
        page_table.print();
		// Handle command
		// TODO: implement this!
		
		// First, split the command by spaces (todo: find c++ version of splitString() and the var (array?) to hold it)
		// Iterate through the first section of each command (its name), else { bad command, try again }. 
		// Inside a specific command, error check the input (right number of inputs? Right types? Bounded?) and convert
		// Then use the appropriate method for the command and inform the user as needed. 

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
	//   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK>
	//   - print pid
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
