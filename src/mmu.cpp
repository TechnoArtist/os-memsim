#include "mmu.h"

Mmu::Mmu(int memory_size)
{
	_next_pid = 1024;
	_max_size = memory_size;
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
	for (i = 0; i < _processes.size(); i++)
	{
		for (j = 0; j < _processes[i]->variables.size(); j++)
		{
			Variable* var = _processes[i]->variables[j]; 
<<<<<<< Updated upstream
			if (var->type != DataType::FreeSpace) printf("%6i %15s 0x%14x %5i\n", _processes[i]->pid, var->name.c_str(), var->virtual_address, var->size);
=======
			if (var->type != DataType::FreeSpace) printf("%6i %15s %14x %5i\n", _processes[i]->pid, var->name.c_str(), var->virtual_address, var->size);
			// TODO double check formatting
>>>>>>> Stashed changes
		}
	}
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
