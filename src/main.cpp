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
	- setVariable() (the "set" command)
	- freeVariable() (the "free" command)
	- terminateProcess() (the exit command)

mmu.cpp
	+ finish the print command 

pagetable.cpp
	+ finish addEntry()
	+ finish getPhysicalAddress() 
		+ convert virtual addr to page_number and _offset
		+ finish the lookup
	+ finish the print command
	+ add new method entryExists()

Next steps: 
- Finish that last section of the prompt loop print
- Set up some kind of skeleton (even if it's shitty code) of the set command
- Meet with teacher again, wait for reply (will meet at 2:30pm, then work)

*/

void printStartMessage(int page_size);
void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table, int page_size);
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
		
		if(split_command[0].compare("print") == 0) {
			// TODO handle command (include input error checking)
			/* print <object>
				If <object> is "mmu", print the MMU memory table
				If <object> is "page", print the page table (do not need to print anything for free frames)
				If <object> is "processes", print a list of PIDs for processes that are still running
				If <object> is a "<PID>:<var_name>", print the value of the variable for that process
					If variable has more than 4 elements, just print the first 4 followed by "... [N items]" (where N is the number of elements)
			*/
			if(split_command.size() < 2) {
				// print default
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
						// TODO account for page breaks
					int soFar_variables = 0;
					void* value; 
					for (int i = 0; i < var->size/item_size; i++) {
						offset = i * item_size; 
						if (soFar_variables <= 4) {
							// Calculate remaining variables and print (see assignment formatting)
							// Convert virtual address to physical address per item in variable
							physical_address = page_table->getPhysicalAddress(atoi(special_case[0].c_str()), var->virtual_address + offset); 
							memcpy(value, memory + physical_address, item_size);  
							printf("%i\n",value);   
						} else {
							printf("... [%i items]\n", (var->size/item_size)-i);
							break;
						}
						soFar_variables++;
					}
					// Find the physical address (watch out for page breaks)
						// Find the start of the virtual address, and the distance to next page break
						// Find the variable data size
					// Print the size-chunk of the variable, based on type, until it reaches 4 or runs out
					// If it reaches 4, calculate remaining variables and print the number (formatted)	
				} else {
					printf("Error: Please select a valid option\n");
				}
			}
		
		} else if(split_command[0].compare("create") == 0) {
			// create <text_size> <data_size>
			int text_size = (uint32_t)atoi(split_command[1].c_str()); 
			int data_size = (uint32_t)atoi(split_command[2].c_str()); 
			
			if ((text_size < 2048) || (text_size > 16384)) {
				printf("error: text size out of bounds (2048 to 16384 bytes)\n"); 
			} else if ((data_size < 0) || (data_size > 1024)) {
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
				//TODO question: If we print extra errors will that cause problems testing?
				printf("Error: Data type not recognized please enter a valid data type\n");
			}
			
			if (mmu->findPID(pid) == nullptr) {
				printf("error: pid not found\n"); 
			} else if (mmu->findVariable(pid, var_name) != nullptr) {
				printf("error: variable already exists\n"); 
			} else {
				// When allocating: Error if allocation would exceed system memory (and skip allocating)
				allocateVariable(pid, var_name, type, num_elements, mmu, page_table, page_size); 
			}
			
			
		} else if(split_command[0].compare("set") == 0) {
			// TODO handle command 
			/* error checking: 
			 - if <PID> does not exist, print "error: process not found"
			 - if <var_name> does not exist, print "error: variable not found"
			*/
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
				printf("error: process not found"); 
			} else if (var == nullptr) {
				printf("error: variable not found"); 
			} else {
				for (int i = 0; i < values.size(); i++) {
					//setVariable(pid, var_name, offset, values[i], mmu, page_table, memory); 
					// TODO see "how to interact with void pointers" question
				}
			}
			
			
		} else if(split_command[0].compare("free") == 0) {
			// TODO handle command 
			/* error checking: 
			 - if <PID> does not exist, print "error: process not found"
			 - if <var_name> does not exist, print "error: variable not found"
			*/
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

void createProcess(int text_size, int data_size, Mmu *mmu, PageTable *page_table, int page_size)
{	/*
	Assign a PID - unique number (start at 1024 and increment up)
	Allocate some amount of startup memory for the process
		Text/Code: size of binary executable - user specified number (2048 - 16384 bytes)
		Data/Globals: size of global variables - user specified number (0 - 1024 bytes)
		Stack: constant (65536 bytes)
	*/
	//   - create new process in the MMU
	uint32_t pid = mmu->createProcess(); 
	//   - allocate new variables for the <TEXT>, <GLOBALS>, and <STACK> 
	allocateVariable(pid, "<TEXT>", DataType::Char, text_size, mmu, page_table, page_size); 
	allocateVariable(pid, "<GLOBALS>", DataType::Char, data_size, mmu, page_table, page_size);
	allocateVariable(pid, "<STACK>", DataType::Char, 65536, mmu, page_table, page_size);
	//   - print the pid
	printf("%i\n", pid);
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
	Process* process; 
	Variable* free_space = nullptr; 
	uint32_t address; 
	uint32_t end_of_address; 
	//   - find first free space within a page already allocated to this process that is large enough to fit the new variable
	std::vector<Process*> process_list = mmu->getProcesses(); 
	for (int i = 0; i < process_list.size(); i++) {
		if (process_list[i]->pid == pid) {
			process = process_list[i]; 
		}
	}
	for (int i = 0; i < process->variables.size(); i++) {
		if (process->variables[i]->type == DataType::FreeSpace) {
			// TODO check if the space is split across pages, and whether that still fits the variables
			if (process->variables[i]->size > all_vars_size) {
				uint32_t address = process->variables[i]->virtual_address; 
				uint32_t end_of_address = address + all_vars_size - 1; 
				free_space = process->variables[i]; 
				free_space->size -= all_vars_size; 
				free_space->virtual_address += all_vars_size; 
				mmu->addVariableToProcess(pid, var_name, type, all_vars_size, address); 
				uint32_t first_page = address >> (uint32_t)log2(page_size); 
				uint32_t last_page = end_of_address >> (uint32_t)log2(page_size); 
				for (int j = first_page; j <= last_page; j++) {
					// Note: "entry" refers to a page with a specific pid. 
					if(!page_table->entryExists(pid, j)) {
						page_table->addEntry(pid, j);  
					} 
				}
				break; 
			} else if (process->variables[i]->size == all_vars_size) {
				// TODO This is exactly the right size space, replace it
				uint32_t address = process->variables[i]->virtual_address; 
				uint32_t end_of_address = address + all_vars_size - 1;
				uint32_t first_page = address >> (uint32_t)log2(page_size); 
				uint32_t last_page = end_of_address >> (uint32_t)log2(page_size);
				for (int j = first_page; j <= last_page; j++) {
					// Note: "entry" refers to a page with a specific pid. 
					if(!page_table->entryExists(pid, j)) {
						page_table->addEntry(pid, j);  
					} 
				}
				free_space = process->variables[i]; 
				free_space->name = var_name; 
				free_space->type = type; 
				
			}
		}
	}
	if (!(free_space == nullptr)) {
		// - print virtual memory address
		printf("%i\n", address);
	} else {
		printf("Error: allocation would exceed system memory\n");
	}
	
}

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

void freeVariable(uint32_t pid, std::string var_name, Mmu *mmu, PageTable *page_table)
{
	// TODO: implement this!
	//   - remove entry from MMU
	Variable* toRemove = mmu->findVariable(pid, var_name);
	toRemove->type = DataType::FreeSpace;
	toRemove->name = "<FREE_SPACE>"; 
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

