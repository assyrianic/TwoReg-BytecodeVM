#include "vmachine.h"

//#define VMACHINE_DBG

#ifdef VMACHINE_DBG
static FILE *vm_output;
static void prep_vm_output() {
	vm_output = fopen("vmachine_output.txt", "w+");
}
static void close_vm_output() {
	fclose(vm_output);
	vm_output = NULL;
}
#	define VMACHINE_OPCODE    fprintf(vm_output, "%s -> rA: '%u' | rB: '%u'\n", __func__, rA.uint32, rB.uint32);
#else
#	define VMACHINE_OPCODE 
#endif

void vmachine_run(
	const size_t          stksize,
	uint8_t               stkmem[static stksize],
	const uint32_t        ip[static 1],
	struct VMachineState *restrict state
) {
#ifdef VMACHINE_DBG
	prep_vm_output();
#endif
	uintptr_t sp = ( uintptr_t )(&stkmem[stksize - sizeof(union VMachineVal)]);
	uintptr_t fp = sp;
	uintptr_t lr = ( uintptr_t )(NULL);
    union VMachinePtr pc = { .uint32 = &ip[0] };
	union VMachineVal rA = {0}, rB = {0};
	VMACHINE_OPCODE
	vmachine_exec(INSTR_ARGS);
#ifdef VMACHINE_DBG
	close_vm_output();
#endif
}

void vmachine_exec(INSTR_PARAMS) {
	VMACHINE_OPCODE
	( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

void func_halt(INSTR_PARAMS) {
	VMACHINE_OPCODE
	state->saved_ra = rA;
}
void func_nop(INSTR_PARAMS) {
	VMACHINE_OPCODE
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

void func_immA(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA = *pc.val++;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_immB(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rB = *pc.val++;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_jeq(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    if( rA.uint64==rB.uint64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}

void func_incA(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 += 1;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_jmp(INSTR_PARAMS) {
	VMACHINE_OPCODE
	const int_fast32_t offset = *pc.int32++;
    pc.uint32 += offset;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_incB(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rB.uint64 += 1;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

void func_decA(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 -= 1;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_decB(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rB.uint64 -= 1;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

void func_zero(INSTR_PARAMS) {
	VMACHINE_OPCODE
	rA.uint64 = rB.uint64 = 0;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

/// arithmetic ops.
void func_add(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.int64 += rB.int64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_sub(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.int64 -= rB.int64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_mul(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.int64 *= rB.int64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_divi(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 /= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_mod(INSTR_PARAMS) {
	VMACHINE_OPCODE
	rA.uint64 %= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

void func__and(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 &= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func__or(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 |= rB.int64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func__xor(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 ^= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_sll(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 <<= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_srl(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 >>= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_sra(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.int64 >>= rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}

/// function call ops
void func_call(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    lr = ( uintptr_t )(pc.uint8);
	pc.uint32 += offset;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_callr(INSTR_PARAMS) {
	VMACHINE_OPCODE
    lr = ( uintptr_t )(pc.uint8);
	pc.uint8 = ( const uint8_t* )(rA.uintptr);
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_ret(INSTR_PARAMS) {
	VMACHINE_OPCODE
	pc.uint8 = ( const uint8_t* )(lr);
	if( pc.uint8==NULL ) {
		func_halt(INSTR_ARGS);
	} else {
  		( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
	}
}

/// Jumps
void func_jnz(INSTR_PARAMS) {
	VMACHINE_OPCODE
	const int_fast32_t offset = *pc.int32++;
    if( rA.uint64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}
void func_jz(INSTR_PARAMS) {
	VMACHINE_OPCODE
	const int_fast32_t offset = *pc.int32++;
    if( !rA.uint64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}
void func_jlt(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    if( rA.int64 < rB.int64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}
void func_jltu(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    if( rA.uint64 < rB.uint64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}
void func_jle(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    if( rA.int64 <= rB.int64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}
void func_jleu(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t offset = *pc.int32++;
    if( rA.uint64 <= rB.uint64 ) {
        pc.uint32 += offset;
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    } else {
        ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
    }
}

void func_swap(INSTR_PARAMS) {
	VMACHINE_OPCODE
	const union VMachineVal tmp = rA;
    rA = rB;
	rB = tmp;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_slt(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 = rA.int64 < rB.int64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_sltu(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 = rA.uint64 < rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_cmp(INSTR_PARAMS) {
	VMACHINE_OPCODE
    rA.uint64 = rA.uint64==rB.uint64;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_pushlr(INSTR_PARAMS) {
	VMACHINE_OPCODE
	sp -= sizeof rA;
    union VMachineVal *const restrict rsp = ( union VMachineVal* )(sp);
    rsp->uintptr = lr;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_poplr(INSTR_PARAMS) {
	VMACHINE_OPCODE
    union VMachineVal *const restrict rsp = ( union VMachineVal* )(sp);
	sp += sizeof rA;
    lr = rsp->uintptr;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_enter(INSTR_PARAMS) {
	VMACHINE_OPCODE
    const int_fast32_t i = *pc.int32++;
    sp -= sizeof fp;
    union VMachineVal *const restrict rsp = ( union VMachineVal* )(sp);
    rsp->uintptr = fp; /// push bp
    fp = sp; /// mov bp, sp
	sp -= i * sizeof rA;
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
void func_leave(INSTR_PARAMS) {
	VMACHINE_OPCODE
    sp = fp; /// mov sp, bp
    const union VMachineVal *const restrict rsp = ( const union VMachineVal* )(sp);
    sp += sizeof fp;
    fp = rsp->uintptr; /// pop bp
    ( *( InstrFunc* )(*pc.uint64++) )(INSTR_ARGS);
}
