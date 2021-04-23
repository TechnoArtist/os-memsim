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

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table);
void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int page_size);
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
			uint32_t pid = (uint32_t)atoi(split_command[1].c_str()); 
			// TODO Error checking: Is an int? 
			// 		Error checking: PID > 1024
			
			std::string var_name = split_command[2]; // TODO Error checking: If var name already exists, print "Error: variable already exists"
			
			// FreeSpace, Char, Short, Int, Float, Long, Double
			DataType type; 
			if (split_command[3].c_str() == "FreeSpace") { 
				type = DataType::FreeSpace; 
			} else if (split_command[3].c_str() == "Short") {
				type = DataType::Short; 
			} else if (split_command[3].c_str() == "Char") {
				type = DataType::Char; 
			} else if (split_command[3].c_str() == "Int") {
				type = DataType::Int; 
			} else if (split_command[3].c_str() == "Float") {
				type = DataType::Float; 
			} else if (split_command[3].c_str() == "Long") {
				type = DataType::Long; 
			} else if (split_command[3].c_str() == "Double") {
				type = DataType::Double; 
			} else {
				// TODO: ERROR bad data type
			}
			
			uint32_t num_elements = (uint32_t)atoi(split_command[4].c_str()); // TODO error checking: Is an int? Is greater than 0? 
			
			
			// When allocating: Error if allocation would exceed system memory (and skip allocating)
			allocateVariable(pid, var_name, type, num_elements, mmu, page_table, page_size); 
			
			/* TODO Error checking: 
				1024 < Page size < 32,768
				Page size power of 2 (try log2()^2, and see if it changes, or try log2() and see if it's an int; Math.log_2() returns a double)
					10.0, 11.0, 12.0, 13.0, 14.0, or 15.0
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
	//allocateVariable(pid, , DataType::, , mmu, page_table, );
	//allocateVariable();
	//allocateVariable();
	/*
	Assign a PID - unique number (start at 1024 and increment up)
	Allocate some amount of startup memory for the process
		Text/Code: size of binary executable - user specified number (2048 - 16384 bytes)
		Data/Globals: size of global variables - user specified number (0 - 1024 bytes)
		Stack: constant (65536 bytes)
	*/
	// TODO question: What's the difference between allocating for these three variables? Are they allocated like normal variables, or are they not the same? 
	
	//   - print pid
	printf("%i", pid); //TODO formatting
}

void allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int page_size)
{
	//	 - determine how much space the variables need
	int single_var_size; 
	int all_vars_size; 
	if (type == DataType::Char) {
		single_var_size = 1; 
	} else if (type == DataType::Short) {
		single_var_size = 2; 
	} else if (type == DataType::Int || type == DataType::Float) {
		single_var_size = 4; 
	} else if (type == DataType::Long || type == DataType::Double) {
		single_var_size = 8; 
	} 
	all_vars_size = num_elements * single_var_size; 
	//   - find first free space within a page already allocated to this process that is large enough to fit the new variable
	Process* process; 
	int* page_remaining_sizes; // Start each int at page_size
	int current_page = 0; 
	int free_page = -1; 
	// Find the process in the mmu
	std::vector<Process*> process_list = mmu->getProcesses(); 
	for (int i = 0; i < process_list.size(); i++) {
		if (process_list[i]->pid == pid) {
			process = process_list[i]; 
		}
	}
	// Iterate through each variable in the process
	page_remaining_sizes[0] = page_size; 
	for (int i = 0; i < process->variables.size(); i++) {
		// Iterate through each page...? Remember to set remaining_size at page_size when flipping pages
		// TODO if (something), page flip
		{
			// Check if the current page is what we need
			if (page_remaining_sizes[current_page] >= all_vars_size) {
				free_page = current_page; 
				break; 
			}
			// Flip the page 
			current_page++; 
			page_remaining_sizes[current_page] = page_size; 
		}
		// add variable sizes to page
		page_remaining_sizes[current_page] -= process->variables[i]->size; 	
		// TODO question: Are the variables sorted by virtual address, or do they jump around pages? 
	}
	//   - if no hole is large enough, allocate new page(s)
	if (free_page == -1) {
		free_page = /*TODO largest page plus one*/
		page_table->addEntry(pid, free_page); // TODO do we need to do the checking for victims here, or in this method? 
	}
	//   - insert variable into MMU
	// TODO question: How to find the virtual address? 
	uint32_t virt_addr; // TODO 
    uint32_t numBits = (uint32_t)log2(page_size);//num bits for page offset
    virt_addr = (uint32_t)free_page << numBits;
    virt_addr = (uint32_t)(page_size - 1) & offset;
	mmu->addVariableToProcess(pid, var_name, type, all_vars_size, virt_addr); 
	//   - print virtual memory address
	printf("%i\n", virt_addr);
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

