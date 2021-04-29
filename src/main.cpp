#include <iostream>
#include <string>
#include <cstring>
#include <math.h>
#include "mmu.h"
#include "pagetable.h"

/* Master todo list (does not auto-update)

All starter todos are in the .cpp files. 
The .h files are useful reference material. 

main.cpp
	~ Prompt loop (prompting the user for input, and handling their commands; the main loop) 
		* This will likely tie together the program
	~ createProcess() (the "create" command)
	~ allocateVariable() (the "allocate" command)
	~ setVariable() (the "set" command)
	~ freeVariable() (the "free" command)
	~ terminateProcess() (the exit command)

mmu.cpp
	+ finish the print command 

pagetable.cpp
	+ finish addEntry()
	+ finish getPhysicalAddress() 
		+ convert virtual addr to page_number and _offset
		+ finish the lookup
	+ finish the print command


TODO somewhere, combine adjacent free spaces

Consider moving method prints from end-of-method to return values, printing in the prompt loop
Consider creating a method for converting datatype to size

*/

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table, int page_size);
uint32_t allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int page_size);
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
		
		if(split_command[0].compare("print") == 0) {
			// print <object>
			if (split_command.size() < 2) {
				printf("Error: Missing argument. Please select an option to print ('help' for details).\n"); 
			} else {
				std::vector<std::string> special_case; 
				splitString(split_command[1], ':', special_case);
				if (split_command[1] == "mmu") {
					mmu->print();
				} else if (split_command[1] == "page") {
					page_table->print();
				} else if (split_command[1] == "processes") {
					std::vector<Process*> processList = mmu->getProcesses();
					for (int i = 0; i < processList.size(); i++) {
						printf("%i\n", processList[i]->pid);
					}
				} else if (special_case.size() > 1) {
					Variable* var = mmu->findVariable(atoi(special_case[0].c_str()), special_case[1]); 
					int item_size; 
					int offset; 
					int physical_address; 
					
					switch (var->type) {
						case DataType::Char: 
							item_size = 1; 
							break; 
						case DataType::Short: 
							item_size = 2; 
							break; 
						case DataType::Int: 
						case DataType::Float: 
							item_size = 4; 
							break; 
						case DataType::Long: 
						case DataType::Double: 
							item_size = 8; 
							break; 
					}
					// TODO question: How to account for page breaks here, in the print? Do we need to? 
					int num_elements = var->size/item_size; 
					void* value; 
					for (int i = 0; i < 4; i++) {
						offset = i * item_size; 
						physical_address = page_table->getPhysicalAddress(atoi(special_case[0].c_str()), var->virtual_address + offset); 
						memcpy(value, memory + physical_address, item_size); 
						printf("%i\n", value); 
					}
					if (num_elements >= 4) {
						printf("... [%i items]\n", num_elements - 4); 
					}	
				} else {
					printf("Error: Invalid argument %s. Please select a valid argument (see 'help' for details).\n", split_command[1].c_str());
				}
			}
		} else if(split_command[0].compare("create") == 0) {
			// create <text_size> <data_size>
			int text_size = (uint32_t)atoi(split_command[1].c_str()); 
			int data_size = (uint32_t)atoi(split_command[2].c_str()); 
			if ((text_size <= 2048) || (text_size >= 16384)) {
				printf("error: text size out of bounds (2048 to 16384 bytes)\n"); 
			} else if ((data_size <= 0) || (data_size >= 1024)) {
				printf("error: data size out of bounds (0 to 1024 bytes)\n"); 
			} else {
				createProcess(text_size, data_size, mmu, page_table, page_size); 
			}	
		} else if(split_command[0].compare("allocate") == 0) {
			// allocate <PID> <var_name> <data_type> <number_of_elements>
			uint32_t pid = (uint32_t)atoi(split_command[1].c_str()); 
			std::string var_name = split_command[2]; 
			DataType type; 
			uint32_t num_elements = (uint32_t)atoi(split_command[4].c_str()); 
			//TODO question ask about freespace capitalization.
			if (split_command[3] == "FreeSpace") { 
				type = DataType::FreeSpace; 
			} else if (split_command[3] == "short") {
				type = DataType::Short; 
			} else if (split_command[3] == "char") {
				type = DataType::Char; 
			} else if (split_command[3] == "int") {
				type = DataType::Int; 
			} else if (split_command[3] == "float") {
				type = DataType::Float; 
			} else if (split_command[3] == "long") {
				type = DataType::Long; 
			} else if (split_command[3] == "double") {
				type = DataType::Double; 
			} else {
				printf("Error: Data type not recognized. Please enter a valid data type\n");
			}
			
			if (mmu->findPID(pid) == nullptr) {
				printf("error: pid not found\n"); 
			} else if (mmu->findVariable(pid, var_name) != nullptr) {
				printf("error: variable already exists\n"); 
			} else {
				uint32_t address = allocateVariable(pid, var_name, type, num_elements, mmu, page_table, page_size); 
				if(address != -1) printf("%i\n", address); 
			}
			
		} else if(split_command[0].compare("set") == 0) {
			// TODO handle command 
			/* set <PID> <var_name> <offset> <value_0> <value_1> <value_2> ... <value_N>
				Set the value for variable <var_name> starting at <offset>
				Note: multiple contiguous values can be set with one command
			*/
			int 		pid		 = atoi(split_command[1].c_str()); 
			std::string var_name = split_command[2]; 
			int 		offset	 = atoi(split_command[3].c_str()); 
			std::vector<std::string> values; 
			Process* 	proc	 = mmu->findPID(pid); 
			Variable* 	var		 = mmu->findVariable(pid, var_name); 
			for (int i = 0; i < split_command.size() - 4; i++) {
				values[i] = split_command[i+4]; 
			}	
			if (proc == nullptr) {
				printf("error: process not found\n"); 
			} else if (var == nullptr) {
				printf("error: variable not found\n"); 
			} else {
				for (int i = 0; i < values.size(); i++) {
					//setVariable(pid, var_name, offset, values[i], mmu, page_table, memory); 
					// TODO question: see "how to interact with void pointers" question
				}
			}		
		} else if(split_command[0].compare("free") == 0) {
			// free <PID> <var_name>
			uint32_t pid = atoi(split_command[1].c_str());
			std::string var_name = split_command[2];
			if (mmu->findPID(pid) == nullptr) {
				printf("error: process not found\n");
			} else if(mmu->findVariable(pid, var_name) == nullptr) {
				printf("error: variable not found\n");
			} else {
				freeVariable(pid, var_name, mmu, page_table);
			}
		} else if(split_command[0].compare("terminate") == 0) {
			// TODO handle command (include input error checking)
			/* terminate <PID>
				Kill the specified process
				Free all memory associated with this process
			*/
			uint32_t pid = atoi(split_command[1].c_str());
			terminateProcess(pid, mmu, page_table);
		} else {
			printf("error: command not recognized\n"); 
		}
		// exit is handled by the while loop.
		// Get next command
		std::cout << "> ";
		std::getline (std::cin, command);
		splitString(command, ' ', split_command);
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

/*
	Creates a newly running process in the mmu. 
	
	@param text_size	The size of the "text" section of memory. 
	@param data_size 	The size of the "data" section of memory. 
	@param mmu			A link to the mmu. 
	@param page_table	A link to the page table. 
	@param page_size	The size of each page. 
*/
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table, int page_size)
{
	uint32_t pid = mmu->createProcess(); 
	allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table, page_size); 
	allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table, page_size);
	allocateVariable(pid, "<STACK>", DataType::Char, 65536, mmu, page_table, page_size);
	printf("%i\n", pid);
}

/*
	Creates and allocates a variable into a section of free space, adding pages as needed. 
	
	@param pid			The ID of the process to allocate for. 
	@param var_name		The name of the variable to create. 
	@param type			The type of variable being created (e.g. Int). 
	@param num_elements The number of elements to create in the variable. 
	@param mmu			A link to the mmu. 
	@param page_table	A link to the page table. 
	@param page_size	The size of each page. 
	@return address	The location the variable was allocated to, or -1 if failed. 
*/
uint32_t allocateVariable(uint32_t pid, std::string var_name, DataType type, uint32_t num_elements, Mmu *mmu, PageTable *page_table, int page_size)
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
	Process* process; 
	Variable* free_space = nullptr; 
	uint32_t address; 
	uint32_t end_of_address; 
	std::vector<Process*> process_list = mmu->getProcesses(); 
	for (int i = 0; i < process_list.size(); i++) {
		if (process_list[i]->pid == pid) {
			process = process_list[i]; 
		}
	}
	for (int i = 0; i < process->variables.size(); i++) {
		if (process->variables[i]->type == DataType::FreeSpace) {
			free_space = process->variables[i]; 
			//check if the space is split across pages, and whether that still fits the variables
			uint32_t address = process->variables[i]->virtual_address; 
			uint32_t first_page = address >> (uint32_t)log2(page_size);
			//nextpage == page virtual address maps to + 1
			uint32_t next_page = first_page + 1;  
			//convert next page back to virtual address
			uint32_t next_page_address = next_page << (uint32_t)log2(page_size); 
			//subtract virtual address from next page from our converted
			uint32_t pageOverlapTestResultAddressStage = next_page_address - address; 	
			//take result and mod with data size
			uint32_t pageOverlapTestResult = pageOverlapTestResultAddressStage % single_var_size; 
			//add result to our current virtual address
			uint32_t offset_address = address + pageOverlapTestResult; 
			uint32_t end_of_address = address + all_vars_size - 1; 
			uint32_t last_page = end_of_address >> (uint32_t)log2(page_size);
			//recheck size
			uint32_t offset_size = end_of_address - offset_address; 
			//TODO quesion: how does this set up look? walk through process and check we are computing/checking proper sizes.
			if (process->variables[i]->size > all_vars_size) {
				free_space->size -= all_vars_size; 
				free_space->virtual_address += all_vars_size; 
				mmu->addVariableToProcess(pid, var_name, type, all_vars_size, offset_address); 
				uint32_t first_page = offset_address >> (uint32_t)log2(page_size); 
				uint32_t last_page = end_of_address >> (uint32_t)log2(page_size); 
				for (int j = first_page; j <= last_page; j++) {
					// Note: "entry" refers to a page with a specific pid. 
					if(!page_table->entryExists(pid, j)) {
						page_table->addEntry(pid, j);  
					} 
				}
				break; 
			} else if (process->variables[i]->size == all_vars_size) {
				// This is exactly the right size space, replace it. 
				for (int j = first_page; j <= last_page; j++) {
					// Note: "entry" refers to a page with a specific pid. 
					if(!page_table->entryExists(pid, j)) {
						page_table->addEntry(pid, j);  
					} 
				}
				break; 
			}
		}
	}
	if (free_space == nullptr) {
		printf("Error: allocation would exceed system memory\n");
		return -1; 
	} else {
		return address; 
	}	
}

/*
	Changes the values of a given element in a given variable. 
	
	@param pid			The ID of the process to search for the variable in. 
	@param var_name		The name of the variable to search for. 
	@param offset		The location offset of the element. 
	@param value		The new value to put in the element. 
	@param mmu			A link to the mmu. 
	@param page_table	A link to the page table. 
	@param memory		A link to the simulated system memory. 
*/
void setVariable(uint32_t pid, std::string var_name, uint32_t offset, void *value, Mmu *mmu, PageTable *page_table, void *memory)
{
	// TODO: implement this!
	//   - look up physical address for variable based on its virtual address / offset
	//   - insert `value` into `memory` at physical address
	//   * note: this function only handles a single element (i.e. you'll need to call this within a loop when setting
	//		   multiple elements of an array)	
	int physical_address = page_table->getPhysicalAddress(pid, mmu->findVariable(pid, var_name)->virtual_address + offset); 
	memcpy((char*)memory+physical_address,value, mmu->findVariable(pid,var_name)->type); 
	
}

/*
	Clears a variable from taking up memory. 
	
	@param pid			The ID of the process to search for the variable in. 
	@param var_name 	The name of the variable to be freed. 
	@param mmu			A link to the mmu. 
	@param page_table	A link to the page table. 
*/
void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	//   - remove entry from MMU
	Variable* toRemove = mmu->findVariable(pid, var_name);
	uint32_t virtualAdd = toRemove->virtual_address;
	uint32_t endAdd = virtualAdd + toRemove->size; 
	uint32_t numBits = (uint32_t)log2(page_table->getPageSize());//num bits for page offset
    int currentPageNum = (int)(virtualAdd >> numBits);
	int endingPageNum = (int)(endAdd >> numBits); 
	toRemove->type = DataType::FreeSpace;
	toRemove->name = "<FREE_SPACE>";
	//TODO question: Are we doing this properly?
	for (int i = currentPageNum; i < endingPageNum; i++) {
		if (mmu->isOnlyVar(pid,currentPageNum, page_table->getPageSize()) == 1) { 
			page_table->deletePage(pid, toRemove->virtual_address);
		}
	} 
	for (int i = 0; i < mmu->getProcesses().size(); i++) {
		if (mmu->getProcesses()[i]->pid == pid) {
			Process* proc = mmu->getProcesses()[i]; 
			Variable* var; 
			for (int j = 0; j < proc->variables.size(); j++) {
				var = proc->variables[j]; 
				if (var->name == var_name) {
					//TODO question: DO we need to update just the sizes or the virtual address?
					//If freespace comes before variable merge
					if (j > 1 && proc->variables[j-1]->type == DataType::FreeSpace) {
						proc->variables[j-1]->size = proc->variables[j-1]->size + var->size;
					}
					//If freespace comes after our variable merge
					if (j+1 < mmu->getProcesses().size() && proc->variables[j+1]->type == DataType::FreeSpace) {
						var->size = var->size + proc->variables[j+1]->size;
					}
				}//if
			}//for
			break; 
		}//if
	}//for

}


/*
	Terminates a currently running process and frees up memory it was using. 
	@param pid			The ID of the process to terminate. 
	@param mmu			A link to the mmu. 
	@param page_table	A link to the page table. 
*/
void terminateProcess(uint32_t pid, Mmu *mmu, PageTable *page_table)
{
	Process* proc = mmu->findPID(pid); 
	for (int i = 0; i < proc->variables.size(); i++) {
		freeVariable(pid, proc->variables[i]->name, mmu, page_table); 
	}
	// TODO: implement this!
	//   - remove process from MMU
	Process* toRemove = mmu->findPID(pid);
	page_table->deleteProcessPages(pid);
	//   - free all pages associated with given process
}

/*
	Turns a std::string into a vector<std::string>, splitting it based on the given delimiter. 
	
	@param text		The string to split. 
	@param d 		The character delimiter to split `text` on. 
	@param result	The vector of strings - result will be stored here. 
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
