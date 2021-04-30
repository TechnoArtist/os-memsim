#ifndef __MMU_H_
#define __MMU_H_

#include <iostream>
#include <string>
#include <vector>

enum DataType : uint8_t {FreeSpace, Char, Short, Int, Float, Long, Double};

typedef struct Variable {
	std::string name;
	DataType type;
	uint32_t virtual_address;
	uint32_t size;
} Variable;

typedef struct Process {
	uint32_t pid;
	std::vector<Variable*> variables;
} Process;

class Mmu {
private:
	uint32_t _next_pid;
	uint32_t _max_size;
	std::vector<Process*> _processes;
	uint32_t _remainingMemory;

public:
	Mmu(int memory_size);
	~Mmu();

	uint32_t createProcess();
	void addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address);
	void print();
	std::vector<Process*> getProcesses(); 
	Variable* findVariable(uint32_t pid, std::string var_name); 
	Process* findPID(uint32_t pid); 
	int isOnlyVar(uint32_t pid, int pageNum, int page_size);
	uint32_t getRemainingMemory();
	void setRemainingMemory(uint32_t all_vars_size);
};

#endif // __MMU_H_
