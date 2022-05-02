// license:BSD-3-Clause
// copyright-holders:Juergen Buchmueller
/*****************************************************************************
 *
 *   Portable I8085A disassembler
 *
 *   TODO:
 *   - add I8080?
 *
 *****************************************************************************/

#include "emu.h"
#include "8085dasm.h"

u32 i8085_disassembler::opcode_alignment() const
{
	return 1;
}

offs_t i8085_disassembler::disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params)
{
	offs_t flags = 0;
	uint8_t op;
	unsigned prevpc = pc;
	switch (op = opcodes.r8(pc++))
	{
		case 0x00: util::stream_format(stream, "nop"); break;
		case 0x01: util::stream_format(stream, "lxi  b,$%04x", params.r16(pc)); pc+=2; break;
		case 0x02: util::stream_format(stream, "stax b"); break;
		case 0x03: util::stream_format(stream, "inx  b"); break;
		case 0x04: util::stream_format(stream, "inr  b"); break;
		case 0x05: util::stream_format(stream, "dcr  b"); break;
		case 0x06: util::stream_format(stream, "mvi  b,$%02x", params.r8(pc)); pc++; break;
		case 0x07: util::stream_format(stream, "rlc"); break;
		case 0x08: util::stream_format(stream, "dsub (*)"); break;
		case 0x09: util::stream_format(stream, "dad  b"); break;
		case 0x0a: util::stream_format(stream, "ldax b"); break;
		case 0x0b: util::stream_format(stream, "dcx  b"); break;
		case 0x0c: util::stream_format(stream, "inr  c"); break;
		case 0x0d: util::stream_format(stream, "dcr  c"); break;
		case 0x0e: util::stream_format(stream, "mvi  c,$%02x", params.r8(pc)); pc++; break;
		case 0x0f: util::stream_format(stream, "rrc"); break;
		case 0x10: util::stream_format(stream, "asrh (*)"); break;
		case 0x11: util::stream_format(stream, "lxi  d,$%04x", params.r16(pc)); pc+=2; break;
		case 0x12: util::stream_format(stream, "stax d"); break;
		case 0x13: util::stream_format(stream, "inx  d"); break;
		case 0x14: util::stream_format(stream, "inr  d"); break;
		case 0x15: util::stream_format(stream, "dcr  d"); break;
		case 0x16: util::stream_format(stream, "mvi  d,$%02x", params.r8(pc)); pc++; break;
		case 0x17: util::stream_format(stream, "ral"); break;
		case 0x18: util::stream_format(stream, "rlde (*)"); break;
		case 0x19: util::stream_format(stream, "dad  d"); break;
		case 0x1a: util::stream_format(stream, "ldax d"); break;
		case 0x1b: util::stream_format(stream, "dcx  d"); break;
		case 0x1c: util::stream_format(stream, "inr  e"); break;
		case 0x1d: util::stream_format(stream, "dcr  e"); break;
		case 0x1e: util::stream_format(stream, "mvi  e,$%02x", params.r8(pc)); pc++; break;
		case 0x1f: util::stream_format(stream, "rar"); break;
		case 0x20: util::stream_format(stream, "rim"); break;
		case 0x21: util::stream_format(stream, "lxi  h,$%04x", params.r16(pc)); pc+=2; break;
		case 0x22: util::stream_format(stream, "shld $%04x", params.r16(pc)); pc+=2; break;
		case 0x23: util::stream_format(stream, "inx  h"); break;
		case 0x24: util::stream_format(stream, "inr  h"); break;
		case 0x25: util::stream_format(stream, "dcr  h"); break;
		case 0x26: util::stream_format(stream, "mvi  h,$%02x", params.r8(pc)); pc++; break;
		case 0x27: util::stream_format(stream, "daa"); break;
		case 0x28: util::stream_format(stream, "ldeh $%02x (*)", params.r8(pc)); pc++; break;
		case 0x29: util::stream_format(stream, "dad  h"); break;
		case 0x2a: util::stream_format(stream, "lhld $%04x", params.r16(pc)); pc+=2; break;
		case 0x2b: util::stream_format(stream, "dcx  h"); break;
		case 0x2c: util::stream_format(stream, "inr  l"); break;
		case 0x2d: util::stream_format(stream, "dcr  l"); break;
		case 0x2e: util::stream_format(stream, "mvi  l,$%02x", params.r8(pc)); pc++; break;
		case 0x2f: util::stream_format(stream, "cma"); break;
		case 0x30: util::stream_format(stream, "sim"); break;
		case 0x31: util::stream_format(stream, "lxi  sp,$%04x", params.r16(pc)); pc+=2; break;
		case 0x32: util::stream_format(stream, "sta  $%04x", params.r16(pc)); pc+=2; break;
		case 0x33: util::stream_format(stream, "inx  sp"); break;
		case 0x34: util::stream_format(stream, "inr  m"); break;
		case 0x35: util::stream_format(stream, "dcr  m"); break;
		case 0x36: util::stream_format(stream, "mvi  m,$%02x", params.r8(pc)); pc++; break;
		case 0x37: util::stream_format(stream, "stc"); break;
		case 0x38: util::stream_format(stream, "ldes $%02x (*)", params.r8(pc)); pc++; break;
		case 0x39: util::stream_format(stream, "dad  sp"); break;
		case 0x3a: util::stream_format(stream, "lda  $%04x", params.r16(pc)); pc+=2; break;
		case 0x3b: util::stream_format(stream, "dcx  sp"); break;
		case 0x3c: util::stream_format(stream, "inr  a"); break;
		case 0x3d: util::stream_format(stream, "dcr  a"); break;
		case 0x3e: util::stream_format(stream, "mvi  a,$%02x", params.r8(pc)); pc++; break;
		case 0x3f: util::stream_format(stream, "cmc"); break;
		case 0x40: util::stream_format(stream, "mov  b,b"); break;
		case 0x41: util::stream_format(stream, "mov  b,c"); break;
		case 0x42: util::stream_format(stream, "mov  b,d"); break;
		case 0x43: util::stream_format(stream, "mov  b,e"); break;
		case 0x44: util::stream_format(stream, "mov  b,h"); break;
		case 0x45: util::stream_format(stream, "mov  b,l"); break;
		case 0x46: util::stream_format(stream, "mov  b,m"); break;
		case 0x47: util::stream_format(stream, "mov  b,a"); break;
		case 0x48: util::stream_format(stream, "mov  c,b"); break;
		case 0x49: util::stream_format(stream, "mov  c,c"); break;
		case 0x4a: util::stream_format(stream, "mov  c,d"); break;
		case 0x4b: util::stream_format(stream, "mov  c,e"); break;
		case 0x4c: util::stream_format(stream, "mov  c,h"); break;
		case 0x4d: util::stream_format(stream, "mov  c,l"); break;
		case 0x4e: util::stream_format(stream, "mov  c,m"); break;
		case 0x4f: util::stream_format(stream, "mov  c,a"); break;
		case 0x50: util::stream_format(stream, "mov  d,b"); break;
		case 0x51: util::stream_format(stream, "mov  d,c"); break;
		case 0x52: util::stream_format(stream, "mov  d,d"); break;
		case 0x53: util::stream_format(stream, "mov  d,e"); break;
		case 0x54: util::stream_format(stream, "mov  d,h"); break;
		case 0x55: util::stream_format(stream, "mov  d,l"); break;
		case 0x56: util::stream_format(stream, "mov  d,m"); break;
		case 0x57: util::stream_format(stream, "mov  d,a"); break;
		case 0x58: util::stream_format(stream, "mov  e,b"); break;
		case 0x59: util::stream_format(stream, "mov  e,c"); break;
		case 0x5a: util::stream_format(stream, "mov  e,d"); break;
		case 0x5b: util::stream_format(stream, "mov  e,e"); break;
		case 0x5c: util::stream_format(stream, "mov  e,h"); break;
		case 0x5d: util::stream_format(stream, "mov  e,l"); break;
		case 0x5e: util::stream_format(stream, "mov  e,m"); break;
		case 0x5f: util::stream_format(stream, "mov  e,a"); break;
		case 0x60: util::stream_format(stream, "mov  h,b"); break;
		case 0x61: util::stream_format(stream, "mov  h,c"); break;
		case 0x62: util::stream_format(stream, "mov  h,d"); break;
		case 0x63: util::stream_format(stream, "mov  h,e"); break;
		case 0x64: util::stream_format(stream, "mov  h,h"); break;
		case 0x65: util::stream_format(stream, "mov  h,l"); break;
		case 0x66: util::stream_format(stream, "mov  h,m"); break;
		case 0x67: util::stream_format(stream, "mov  h,a"); break;
		case 0x68: util::stream_format(stream, "mov  l,b"); break;
		case 0x69: util::stream_format(stream, "mov  l,c"); break;
		case 0x6a: util::stream_format(stream, "mov  l,d"); break;
		case 0x6b: util::stream_format(stream, "mov  l,e"); break;
		case 0x6c: util::stream_format(stream, "mov  l,h"); break;
		case 0x6d: util::stream_format(stream, "mov  l,l"); break;
		case 0x6e: util::stream_format(stream, "mov  l,m"); break;
		case 0x6f: util::stream_format(stream, "mov  l,a"); break;
		case 0x70: util::stream_format(stream, "mov  m,b"); break;
		case 0x71: util::stream_format(stream, "mov  m,c"); break;
		case 0x72: util::stream_format(stream, "mov  m,d"); break;
		case 0x73: util::stream_format(stream, "mov  m,e"); break;
		case 0x74: util::stream_format(stream, "mov  m,h"); break;
		case 0x75: util::stream_format(stream, "mov  m,l"); break;
		case 0x76: util::stream_format(stream, "hlt"); break;
		case 0x77: util::stream_format(stream, "mov  m,a"); break;
		case 0x78: util::stream_format(stream, "mov  a,b"); break;
		case 0x79: util::stream_format(stream, "mov  a,c"); break;
		case 0x7a: util::stream_format(stream, "mov  a,d"); break;
		case 0x7b: util::stream_format(stream, "mov  a,e"); break;
		case 0x7c: util::stream_format(stream, "mov  a,h"); break;
		case 0x7d: util::stream_format(stream, "mov  a,l"); break;
		case 0x7e: util::stream_format(stream, "mov  a,m"); break;
		case 0x7f: util::stream_format(stream, "mov  a,a"); break;
		case 0x80: util::stream_format(stream, "add  b"); break;
		case 0x81: util::stream_format(stream, "add  c"); break;
		case 0x82: util::stream_format(stream, "add  d"); break;
		case 0x83: util::stream_format(stream, "add  e"); break;
		case 0x84: util::stream_format(stream, "add  h"); break;
		case 0x85: util::stream_format(stream, "add  l"); break;
		case 0x86: util::stream_format(stream, "add  m"); break;
		case 0x87: util::stream_format(stream, "add  a"); break;
		case 0x88: util::stream_format(stream, "adc  b"); break;
		case 0x89: util::stream_format(stream, "adc  c"); break;
		case 0x8a: util::stream_format(stream, "adc  d"); break;
		case 0x8b: util::stream_format(stream, "adc  e"); break;
		case 0x8c: util::stream_format(stream, "adc  h"); break;
		case 0x8d: util::stream_format(stream, "adc  l"); break;
		case 0x8e: util::stream_format(stream, "adc  m"); break;
		case 0x8f: util::stream_format(stream, "adc  a"); break;
		case 0x90: util::stream_format(stream, "sub  b"); break;
		case 0x91: util::stream_format(stream, "sub  c"); break;
		case 0x92: util::stream_format(stream, "sub  d"); break;
		case 0x93: util::stream_format(stream, "sub  e"); break;
		case 0x94: util::stream_format(stream, "sub  h"); break;
		case 0x95: util::stream_format(stream, "sub  l"); break;
		case 0x96: util::stream_format(stream, "sub  m"); break;
		case 0x97: util::stream_format(stream, "sub  a"); break;
		case 0x98: util::stream_format(stream, "sbb  b"); break;
		case 0x99: util::stream_format(stream, "sbb  c"); break;
		case 0x9a: util::stream_format(stream, "sbb  d"); break;
		case 0x9b: util::stream_format(stream, "sbb  e"); break;
		case 0x9c: util::stream_format(stream, "sbb  h"); break;
		case 0x9d: util::stream_format(stream, "sbb  l"); break;
		case 0x9e: util::stream_format(stream, "sbb  m"); break;
		case 0x9f: util::stream_format(stream, "sbb  a"); break;
		case 0xa0: util::stream_format(stream, "ana  b"); break;
		case 0xa1: util::stream_format(stream, "ana  c"); break;
		case 0xa2: util::stream_format(stream, "ana  d"); break;
		case 0xa3: util::stream_format(stream, "ana  e"); break;
		case 0xa4: util::stream_format(stream, "ana  h"); break;
		case 0xa5: util::stream_format(stream, "ana  l"); break;
		case 0xa6: util::stream_format(stream, "ana  m"); break;
		case 0xa7: util::stream_format(stream, "ana  a"); break;
		case 0xa8: util::stream_format(stream, "xra  b"); break;
		case 0xa9: util::stream_format(stream, "xra  c"); break;
		case 0xaa: util::stream_format(stream, "xra  d"); break;
		case 0xab: util::stream_format(stream, "xra  e"); break;
		case 0xac: util::stream_format(stream, "xra  h"); break;
		case 0xad: util::stream_format(stream, "xra  l"); break;
		case 0xae: util::stream_format(stream, "xra  m"); break;
		case 0xaf: util::stream_format(stream, "xra  a"); break;
		case 0xb0: util::stream_format(stream, "ora  b"); break;
		case 0xb1: util::stream_format(stream, "ora  c"); break;
		case 0xb2: util::stream_format(stream, "ora  d"); break;
		case 0xb3: util::stream_format(stream, "ora  e"); break;
		case 0xb4: util::stream_format(stream, "ora  h"); break;
		case 0xb5: util::stream_format(stream, "ora  l"); break;
		case 0xb6: util::stream_format(stream, "ora  m"); break;
		case 0xb7: util::stream_format(stream, "ora  a"); break;
		case 0xb8: util::stream_format(stream, "cmp  b"); break;
		case 0xb9: util::stream_format(stream, "cmp  c"); break;
		case 0xba: util::stream_format(stream, "cmp  d"); break;
		case 0xbb: util::stream_format(stream, "cmp  e"); break;
		case 0xbc: util::stream_format(stream, "cmp  h"); break;
		case 0xbd: util::stream_format(stream, "cmp  l"); break;
		case 0xbe: util::stream_format(stream, "cmp  m"); break;
		case 0xbf: util::stream_format(stream, "cmp  a"); break;
		case 0xc0: util::stream_format(stream, "rnz"); flags = STEP_OUT | STEP_COND; break;
		case 0xc1: util::stream_format(stream, "pop  b"); break;
		case 0xc2: util::stream_format(stream, "jnz  $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xc3: util::stream_format(stream, "jmp  $%04x", params.r16(pc)); pc+=2; break;
		case 0xc4: util::stream_format(stream, "cnz  $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xc5: util::stream_format(stream, "push b"); break;
		case 0xc6: util::stream_format(stream, "adi  $%02x", params.r8(pc)); pc++; break;
		case 0xc7: util::stream_format(stream, "rst  0"); flags = STEP_OVER; break;
		case 0xc8: util::stream_format(stream, "rz"); flags = STEP_OUT | STEP_COND; break;
		case 0xc9: util::stream_format(stream, "ret"); flags = STEP_OUT; break;
		case 0xca: util::stream_format(stream, "jz   $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xcb: util::stream_format(stream, "rstv 8 (*)"); flags = STEP_OVER | STEP_COND; break;
		case 0xcc: util::stream_format(stream, "cz   $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xcd: util::stream_format(stream, "call $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER; break;
		case 0xce: util::stream_format(stream, "aci  $%02x", params.r8(pc)); pc++; break;
		case 0xcf: util::stream_format(stream, "rst  1"); flags = STEP_OVER; break;
		case 0xd0: util::stream_format(stream, "rnc"); flags = STEP_OUT | STEP_COND; break;
		case 0xd1: util::stream_format(stream, "pop  d"); break;
		case 0xd2: util::stream_format(stream, "jnc  $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xd3: util::stream_format(stream, "out  $%02x", params.r8(pc)); pc++; break;
		case 0xd4: util::stream_format(stream, "cnc  $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xd5: util::stream_format(stream, "push d"); break;
		case 0xd6: util::stream_format(stream, "sui  $%02x", params.r8(pc)); pc++; break;
		case 0xd7: util::stream_format(stream, "rst  2"); flags = STEP_OVER; break;
		case 0xd8: util::stream_format(stream, "rc"); flags = STEP_OUT | STEP_COND; break;
		case 0xd9: util::stream_format(stream, "shlx d (*)"); break;
		case 0xda: util::stream_format(stream, "jc   $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xdb: util::stream_format(stream, "in   $%02x", params.r8(pc)); pc++; break;
		case 0xdc: util::stream_format(stream, "cc   $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xdd: util::stream_format(stream, "jnx  $%04x (*)", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xde: util::stream_format(stream, "sbi  $%02x", params.r8(pc)); pc++; break;
		case 0xdf: util::stream_format(stream, "rst  3"); flags = STEP_OVER; break;
		case 0xe0: util::stream_format(stream, "rpo"); flags = STEP_OUT | STEP_COND; break;
		case 0xe1: util::stream_format(stream, "pop  h"); break;
		case 0xe2: util::stream_format(stream, "jpo  $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xe3: util::stream_format(stream, "xthl"); break;
		case 0xe4: util::stream_format(stream, "cpo  $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xe5: util::stream_format(stream, "push h"); break;
		case 0xe6: util::stream_format(stream, "ani  $%02x", params.r8(pc)); pc++; break;
		case 0xe7: util::stream_format(stream, "rst  4"); flags = STEP_OVER; break;
		case 0xe8: util::stream_format(stream, "rpe"); flags = STEP_OUT | STEP_COND; break;
		case 0xe9: util::stream_format(stream, "pchl"); break;
		case 0xea: util::stream_format(stream, "jpe  $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xeb: util::stream_format(stream, "xchg"); break;
		case 0xec: util::stream_format(stream, "cpe  $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xed: util::stream_format(stream, "lhlx d (*)"); break;
		case 0xee: util::stream_format(stream, "xri  $%02x", params.r8(pc)); pc++; break;
		case 0xef: util::stream_format(stream, "rst  5"); flags = STEP_OVER; break;
		case 0xf0: util::stream_format(stream, "rp"); flags = STEP_OUT | STEP_COND; break;
		case 0xf1: util::stream_format(stream, "pop  psw"); break;
		case 0xf2: util::stream_format(stream, "jp   $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xf3: util::stream_format(stream, "di"); break;
		case 0xf4: util::stream_format(stream, "cp   $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xf5: util::stream_format(stream, "push psw"); break;
		case 0xf6: util::stream_format(stream, "ori  $%02x", params.r8(pc)); pc++; break;
		case 0xf7: util::stream_format(stream, "rst  6"); flags = STEP_OVER; break;
		case 0xf8: util::stream_format(stream, "rm"); flags = STEP_OUT | STEP_COND; break;
		case 0xf9: util::stream_format(stream, "sphl"); break;
		case 0xfa: util::stream_format(stream, "jm   $%04x", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xfb: util::stream_format(stream, "ei"); break;
		case 0xfc: util::stream_format(stream, "cm   $%04x", params.r16(pc)); pc+=2; flags = STEP_OVER | STEP_COND; break;
		case 0xfd: util::stream_format(stream, "jx   $%04x (*)", params.r16(pc)); pc+=2; flags = STEP_COND; break;
		case 0xfe: util::stream_format(stream, "cpi  $%02x", params.r8(pc)); pc++; break;
		case 0xff: util::stream_format(stream, "rst  7"); flags = STEP_OVER; break;
	}
	return (pc - prevpc) | flags | SUPPORTED;
}
