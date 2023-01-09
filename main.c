#include <stdio.h>
#include <time.h>

#include "vmachine.h"
#include "instrgen.h"

int main(void) {
	struct {
		uint32_t *buf;
		size_t    len;
	} instrs = {0};
	instrs.buf = calloc(1000, sizeof *instrs.buf);
	
	/// Program: basic loop
	/// for( int i=0; i < 100M; i++ ) {}
	/// 100000 = 100K | 1000000 = 1M | 10000000 = 10M | 100000000 = 100M | 1000000000 = 1B
	enum { amount = 1000000000 };
	instrs.len += vmachine_instr_gen(&instrs.buf[instrs.len], immB, ( union VMachineVal ){ .uint64 = amount }); /// 16 bytes
	instrs.len += vmachine_instr_gen(&instrs.buf[instrs.len], jeq, 5); /// 12 bytes
	instrs.len += vmachine_instr_gen(&instrs.buf[instrs.len], incA); /// 8 bytes
	instrs.len += vmachine_instr_gen(&instrs.buf[instrs.len], jmp, -8); /// 12 bytes
	instrs.len += vmachine_instr_gen(&instrs.buf[instrs.len], ret); /// 8 bytes
	
	struct VMachineState vmach = { 0 };
	//const clock_t start     = clock();
	uint8_t stackmem[2048] = {0};
	vmachine_run(sizeof stackmem, &stackmem[0], &instrs.buf[0], &vmach);
	//const float64_t elapsed = ( float64_t )(clock() - start) / ( float64_t )(CLOCKS_PER_SEC);
	
	//const clock_t start2     = clock();
	//for( volatile size_t i = 0; i < amount; i++ ) {}
	//const float64_t elapsed2 = ( float64_t )(clock() - start2) / ( float64_t )(CLOCKS_PER_SEC);
	
	//printf("Done Executing | rA: %" PRIi64 " | elapsed: '%f' milliseconds | reference: '%f' milliseconds\n", vmach.saved_ra.int64, elapsed * 1000.0, /*elapsed2 **/ 1000.0);
}
