#include "units/memory_unit.hpp"

#include "common.hpp"

MemoryUnit::MemoryUnit() {

}

void MemoryUnit::interface(memory_t memory, Input input, Output* output) {
	switch (input.operation) {
	case load: {
		output->is_busy = true;
		word_t value = memory[input.effective_address];
		output->is_busy = false;
		output->operation = input.operation;
		output->loaded_value = value;
		break;
		}
	case store: {
		output->is_busy = true;
		memory[input.effective_address] = input.to_store_value;
		output->is_busy = false;
		output->operation = input.operation;
		break;
		}
	}
}