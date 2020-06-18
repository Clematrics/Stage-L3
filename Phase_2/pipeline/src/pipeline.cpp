#include "pipeline.hpp"

Pipeline::Pipeline() {}


void Pipeline::pipeline() {
	fetch_stage.fetch();
	decode_stage.decode();
}
