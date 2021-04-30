#include "mmu.h"
#include <math.h>

Mmu::Mmu(int memory_size)
{
	_next_pid = 1024;
	_max_size = memory_size;
	_remainingMemory = memory_size;
}

Mmu::~Mmu()
{
}

uint32_t Mmu::createProcess()
{
	Process *proc = new Process();
	proc->pid = _next_pid;

	Variable *var = new Variable();
	var->name = "<FREE_SPACE>";
	var->type = DataType::FreeSpace;
	var->virtual_address = 0;
	var->size = _max_size;
	proc->variables.push_back(var);

	_processes.push_back(proc);

	_next_pid++;
	return proc->pid;
}

void Mmu::addVariableToProcess(uint32_t pid, std::string var_name, DataType type, uint32_t size, uint32_t address)
{
	int i;
	Process *proc = NULL;
	for (i = 0; i < _processes.size(); i++)
	{
		if (_processes[i]->pid == pid)
		{
			proc = _processes[i];
		}
	}

	Variable *var = new Variable();
	var->name = var_name;
	var->type = type;
	var->virtual_address = address;
	var->size = size;
	if (proc != NULL)
	{
		proc->variables.push_back(var);
	}
}

void Mmu::print()
{
	int i, j;
	std::cout << " PID  | Variable Name | Virtual Addr | Size" << std::endl;
	std::cout << "------+---------------+--------------+------------" << std::endl;
	for (i = 0; i < _processes.size(); i++) {
		for (j = 0; j < _processes[i]->variables.size(); j++) {
			Variable* var = _processes[i]->variables[j]; 
			if (var->type != DataType::FreeSpace) printf(" %4i | %13s |   0x%08x | %10i\n", _processes[i]->pid, var->name.c_str(), var->virtual_address, var->size);
		}
	}
}

//pid, page
//loop over all variables that aren't free space see which pages they are on, have a counter to see if return count if count == 1 its to remove
int Mmu::isOnlyVar(uint32_t pid, int pageNum, int page_size) {
	int counter = 0;
	Process* checker = findPID(pid);
	for (int i = 0; i < checker->variables.size(); i++) {
		uint32_t virtualAdd = checker->variables[i]->virtual_address;
		uint32_t numBits = (uint32_t)log2(page_size);//num bits for page offset
    	int currentPageNum = (int)(virtualAdd >> numBits);
		if (currentPageNum == pageNum && checker->variables[i]->type != DataType::FreeSpace){
			counter++;
		}
	}
	return counter;
}

uint32_t Mmu::getRemainingMemory(){
	return _remainingMemory;
}

void Mmu::setRemainingMemory(uint32_t all_vars_size) {
	_remainingMemory = _remainingMemory - all_vars_size;
}

std::vector<Process*> Mmu::getProcesses() {
	return _processes; 
}

Variable* Mmu::findVariable(uint32_t pid, std::string var_name) {
	for (int i = 0; i < _processes.size(); i++) {
		if (_processes[i]->pid == pid) {
			Process* proc = _processes[i]; 
			Variable* var; 
			for (int j = 0; j < proc->variables.size(); j++) {
				var = proc->variables[j]; 
				if (var->name == var_name) {
					return var; 
				}//if
			}//for
			break; 
		}//if
	}//for
	return nullptr; 
} // variableExists()

Process* Mmu::findPID(uint32_t pid) {
	for (int i = 0; i < _processes.size(); i++) {
		if (_processes[i]->pid == pid) {
			return _processes[i]; 
		}//if
	}//for
	return nullptr; 
} // pidExists()
