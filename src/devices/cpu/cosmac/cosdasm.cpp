// license:BSD-3-Clause
// copyright-holders:Curt Coder
/***************************************************************************

    cosdasm.c

    Simple RCA COSMAC disassembler.
    Written by Curt Coder

***************************************************************************/

#include "emu.h"
#include "cosdasm.h"

#define CDP1801_OPCODE(...) \
	util::stream_format(stream, __VA_ARGS__)

#define CDP1802_OPCODE(...) \
	if (m_variant < TYPE_1802) stream << "illegal"; else util::stream_format(stream, __VA_ARGS__)

offs_t cosmac_disassembler::implied(const uint8_t opcode)
{
	return opcode & 0x0f;
}

offs_t cosmac_disassembler::immediate(offs_t &pc, const data_buffer &params)
{
	return params.r8(pc++);
}

offs_t cosmac_disassembler::double_immediate(offs_t &pc, const data_buffer &params)
{
	u16 res = params.r8(pc) << 8 | params.r8(pc + 1);
	pc += 2;
	return res;
}

offs_t cosmac_disassembler::short_branch(offs_t base_pc, offs_t &pc, const data_buffer &params)
{
	return (base_pc & 0xff00) | immediate(pc, params);
}

offs_t cosmac_disassembler::long_branch(offs_t &pc, const data_buffer &params)
{
	u16 res = params.r8(pc) << 8 | params.r8(pc + 1);
	pc += 2;
	return res;
}

offs_t cosmac_disassembler::short_skip(offs_t pc)
{
	return pc + 1;
}

offs_t cosmac_disassembler::long_skip(offs_t pc)
{
	return pc + 2;
}


u32 cosmac_disassembler::opcode_alignment() const
{
	return 1;
}

cosmac_disassembler::cosmac_disassembler(int variant) : m_variant(variant)
{
}

void cosmac_disassembler::disassemble_68(std::ostream &stream, offs_t base_pc, offs_t &pc, const data_buffer &opcodes, const data_buffer &params)
{
	if (m_variant < TYPE_1802)
		stream << "INP 0";
	else if (m_variant < TYPE_1805)
		stream << "illegal";
	else
	{
		uint8_t op2 = opcodes.r8(pc++);

		switch (op2)
		{
		case 0x00: stream << "STPC"; break;
		case 0x01: stream << "DTC"; break;
		case 0x02: stream << "SPM2"; break;
		case 0x03: stream << "SCM2"; break;
		case 0x04: stream << "SPM1"; break;
		case 0x05: stream << "SCM1"; break;
		case 0x06: stream << "LDC"; break;
		case 0x07: stream << "STM"; break;
		case 0x08: stream << "GEC"; break;
		case 0x09: stream << "ETQ"; break;
		case 0x0a: stream << "XIE"; break;
		case 0x0b: stream << "XID"; break;
		case 0x0c: stream << "CIE"; break;
		case 0x0d: stream << "CID"; break;
		case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
		case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
			util::stream_format(stream, "DJNZ R%d, %04X", implied(op2), short_branch(base_pc, pc, params)); break;
		case 0x3e: util::stream_format(stream, "BCI %04X", short_branch(base_pc, pc, params)); break;
		case 0x3f: util::stream_format(stream, "BXI %04X", short_branch(base_pc, pc, params)); break;
		case 0x60: case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67:
		case 0x68: case 0x69: case 0x6a: case 0x6b: case 0x6c: case 0x6d: case 0x6e: case 0x6f:
			util::stream_format(stream, "RLXA R%d", implied(op2)); break;
		case 0x74: stream << "DADC"; break;
		case 0x76: stream << "DSAV"; break;
		case 0x77: stream << "DSMB"; break;
		case 0x7c: util::stream_format(stream, "DACI #%02X", immediate(pc, params)); break;
		case 0x7f: util::stream_format(stream, "DSBI #%02X", immediate(pc, params)); break;
		case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
		case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
			util::stream_format(stream, "SCAL R%d, %04X", implied(op2), long_branch(pc, params)); break;
		case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
		case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
			util::stream_format(stream, "SRET R%d", implied(op2)); break;
		case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
		case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
			util::stream_format(stream, "RSXD R%d", implied(op2)); break;
		case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
		case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
			util::stream_format(stream, "RNX R%d", implied(op2)); break;
		case 0xc0: case 0xc1: case 0xc2: case 0xc3: case 0xc4: case 0xc5: case 0xc6: case 0xc7:
		case 0xc8: case 0xc9: case 0xca: case 0xcb: case 0xcc: case 0xcd: case 0xce: case 0xcf:
			util::stream_format(stream, "RLDI R%d, #%04X", implied(op2), double_immediate(pc, params)); break;
		case 0xf4: stream << "DADD"; break;
		case 0xf7: stream << "DSM"; break;
		case 0xfc: util::stream_format(stream, "DADI #%02X", immediate(pc, params)); break;
		case 0xff: util::stream_format(stream, "DSMI #%02X", immediate(pc, params)); break;
		default: stream << "illegal"; break;
		}
	}
}

offs_t cosmac_disassembler::disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params)
{
	offs_t base_pc = pc;
	uint32_t flags = 0;

	uint8_t opcode = opcodes.r8(pc++);

	switch (opcode)
	{
	case 0x00: CDP1801_OPCODE("IDL"); break;
	case 0x01: case 0x02: case 0x03: case 0x04: case 0x05: case 0x06: case 0x07:
	case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d: case 0x0e: case 0x0f:
		CDP1801_OPCODE("LDN R%d", implied(opcode)); break;
	case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15: case 0x16: case 0x17:
	case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d: case 0x1e: case 0x1f:
		CDP1801_OPCODE("INC R%d", implied(opcode)); break;
	case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25: case 0x26: case 0x27:
	case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d: case 0x2e: case 0x2f:
		CDP1801_OPCODE("DEC R%d", implied(opcode)); break;
	case 0x30: CDP1801_OPCODE("BR %04X", short_branch(base_pc, pc, params)); break;
	case 0x32: CDP1801_OPCODE("BZ %04X", short_branch(base_pc, pc, params)); break;
	case 0x33: CDP1801_OPCODE("BDF %04X", short_branch(base_pc, pc, params)); break;
	case 0x34: CDP1801_OPCODE("B1 %04X", short_branch(base_pc, pc, params)); break;
	case 0x35: CDP1801_OPCODE("B2 %04X", short_branch(base_pc, pc, params)); break;
	case 0x36: CDP1801_OPCODE("B3 %04X", short_branch(base_pc, pc, params)); break;
	case 0x37: CDP1801_OPCODE("B4 %04X", short_branch(base_pc, pc, params)); break;
	case 0x38: CDP1801_OPCODE("SKP %04X", short_skip(pc)); break;
	case 0x3a: CDP1801_OPCODE("BNZ %04X", short_branch(base_pc, pc, params)); break;
	case 0x3b: CDP1801_OPCODE("BNF %04X", short_branch(base_pc, pc, params)); break;
	case 0x3c: CDP1801_OPCODE("BN1 %04X", short_branch(base_pc, pc, params)); break;
	case 0x3d: CDP1801_OPCODE("BN2 %04X", short_branch(base_pc, pc, params)); break;
	case 0x3e: CDP1801_OPCODE("BN3 %04X", short_branch(base_pc, pc, params)); break;
	case 0x3f: CDP1801_OPCODE("BN4 %04X", short_branch(base_pc, pc, params)); break;
	case 0x40: case 0x41: case 0x42: case 0x43: case 0x44: case 0x45: case 0x46: case 0x47:
	case 0x48: case 0x49: case 0x4a: case 0x4b: case 0x4c: case 0x4d: case 0x4e: case 0x4f:
		CDP1801_OPCODE("LDA R%d", implied(opcode)); break;
	case 0x50: case 0x51: case 0x52: case 0x53: case 0x54: case 0x55: case 0x56: case 0x57:
	case 0x58: case 0x59: case 0x5a: case 0x5b: case 0x5c: case 0x5d: case 0x5e: case 0x5f:
		CDP1801_OPCODE("STR R%d", implied(opcode)); break;
	case 0x61: CDP1801_OPCODE("OUT 1"); break;
	case 0x62: CDP1801_OPCODE("OUT 2"); break;
	case 0x63: CDP1801_OPCODE("OUT 3"); break;
	case 0x64: CDP1801_OPCODE("OUT 4"); break;
	case 0x65: CDP1801_OPCODE("OUT 5"); break;
	case 0x66: CDP1801_OPCODE("OUT 6"); break;
	case 0x67: CDP1801_OPCODE("OUT 7"); break;
	case 0x69: CDP1801_OPCODE("INP 1"); break;
	case 0x6a: CDP1801_OPCODE("INP 2"); break;
	case 0x6b: CDP1801_OPCODE("INP 3"); break;
	case 0x6c: CDP1801_OPCODE("INP 4"); break;
	case 0x6d: CDP1801_OPCODE("INP 5"); break;
	case 0x6e: CDP1801_OPCODE("INP 6"); break;
	case 0x6f: CDP1801_OPCODE("INP 7"); break;
	case 0x70: CDP1801_OPCODE("RET"); flags = STEP_OUT; break;
	case 0x71: CDP1801_OPCODE("DIS"); flags = STEP_OUT; break;
	case 0x78: CDP1801_OPCODE("SAV"); break;
	case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: case 0x87:
	case 0x88: case 0x89: case 0x8a: case 0x8b: case 0x8c: case 0x8d: case 0x8e: case 0x8f:
		CDP1801_OPCODE("GLO R%d", implied(opcode)); break;
	case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
	case 0x98: case 0x99: case 0x9a: case 0x9b: case 0x9c: case 0x9d: case 0x9e: case 0x9f:
		CDP1801_OPCODE("GHI R%d", implied(opcode)); break;
	case 0xa0: case 0xa1: case 0xa2: case 0xa3: case 0xa4: case 0xa5: case 0xa6: case 0xa7:
	case 0xa8: case 0xa9: case 0xaa: case 0xab: case 0xac: case 0xad: case 0xae: case 0xaf:
		CDP1801_OPCODE("PLO R%d", implied(opcode)); break;
	case 0xb0: case 0xb1: case 0xb2: case 0xb3: case 0xb4: case 0xb5: case 0xb6: case 0xb7:
	case 0xb8: case 0xb9: case 0xba: case 0xbb: case 0xbc: case 0xbd: case 0xbe: case 0xbf:
		CDP1801_OPCODE("PHI R%d", implied(opcode)); break;
	case 0xd0: case 0xd1: case 0xd2: case 0xd3: case 0xd4: case 0xd5: case 0xd6: case 0xd7:
	case 0xd8: case 0xd9: case 0xda: case 0xdb: case 0xdc: case 0xdd: case 0xde: case 0xdf:
		CDP1801_OPCODE("SEP R%d", implied(opcode)); flags = STEP_OVER; break;
	case 0xe0: case 0xe1: case 0xe2: case 0xe3: case 0xe4: case 0xe5: case 0xe6: case 0xe7:
	case 0xe8: case 0xe9: case 0xea: case 0xeb: case 0xec: case 0xed: case 0xee: case 0xef:
		CDP1801_OPCODE("SEX R%d", implied(opcode)); break;
	case 0xf0: CDP1801_OPCODE("LDX"); break;
	case 0xf1: CDP1801_OPCODE("OR"); break;
	case 0xf2: CDP1801_OPCODE("AND"); break;
	case 0xf3: CDP1801_OPCODE("XOR"); break;
	case 0xf4: CDP1801_OPCODE("ADD"); break;
	case 0xf5: CDP1801_OPCODE("SD"); break;
	case 0xf6: CDP1801_OPCODE("SHR"); break;
	case 0xf7: CDP1801_OPCODE("SM"); break;
	case 0xf8: CDP1801_OPCODE("LDI #%02X", immediate(pc, params)); break;
	case 0xf9: CDP1801_OPCODE("ORI #%02X", immediate(pc, params)); break;
	case 0xfa: CDP1801_OPCODE("ANI #%02X", immediate(pc, params)); break;
	case 0xfb: CDP1801_OPCODE("XRI #%02X", immediate(pc, params)); break;
	case 0xfc: CDP1801_OPCODE("ADI #%02X", immediate(pc, params)); break;
	case 0xfd: CDP1801_OPCODE("SDI #%02X", immediate(pc, params)); break;
	case 0xff: CDP1801_OPCODE("SMI #%02X", immediate(pc, params)); break;
	// CDP1802
	case 0x31: CDP1802_OPCODE("BQ %04X", short_branch(base_pc, pc, params)); break;
	case 0x39: CDP1802_OPCODE("BNQ %04X", short_branch(base_pc, pc, params)); break;
	case 0x60: util::stream_format(stream, m_variant < TYPE_1802 ? "OUT 0" : "IRX"); break;
	case 0x68: disassemble_68(stream, base_pc, pc, opcodes, params); break;
	case 0x72: CDP1802_OPCODE("LDXA"); break;
	case 0x73: CDP1802_OPCODE("STXD"); break;
	case 0x74: CDP1802_OPCODE("ADC"); break;
	case 0x75: CDP1802_OPCODE("SDB"); break;
	case 0x76: CDP1802_OPCODE("SHRC"); break;
	case 0x77: CDP1802_OPCODE("SMB"); break;
	case 0x79: CDP1802_OPCODE("MARK"); break;
	case 0x7a: CDP1802_OPCODE("REQ"); break;
	case 0x7b: CDP1802_OPCODE("SEQ"); break;
	case 0x7c: CDP1802_OPCODE("ADCI #%02X", immediate(pc, params)); break;
	case 0x7d: CDP1802_OPCODE("SDBI #%02X", immediate(pc, params)); break;
	case 0x7e: CDP1802_OPCODE("SHLC"); break;
	case 0x7f: CDP1802_OPCODE("SMBI #%02X", immediate(pc, params)); break;
	case 0xc0: CDP1802_OPCODE("LBR %04X", long_branch(pc, params)); break;
	case 0xc1: CDP1802_OPCODE("LBQ %04X", long_branch(pc, params)); break;
	case 0xc2: CDP1802_OPCODE("LBZ %04X", long_branch(pc, params)); break;
	case 0xc3: CDP1802_OPCODE("LBDF %04X", long_branch(pc, params)); break;
	case 0xc4: CDP1802_OPCODE("NOP"); break;
	case 0xc5: CDP1802_OPCODE("LSNQ %04X", long_skip(pc)); break;
	case 0xc6: CDP1802_OPCODE("LSNZ %04X", long_skip(pc)); break;
	case 0xc7: CDP1802_OPCODE("LSNF %04X", long_skip(pc)); break;
	case 0xc8: CDP1802_OPCODE("LSKP %04X", long_skip(pc)); break;
	case 0xc9: CDP1802_OPCODE("LBNQ %04X", long_skip(pc)); break;
	case 0xca: CDP1802_OPCODE("LBNZ %04X", long_skip(pc)); break;
	case 0xcb: CDP1802_OPCODE("LBNF %04X", long_skip(pc)); break;
	case 0xcc: CDP1802_OPCODE("LSIE %04X", long_skip(pc)); break;
	case 0xcd: CDP1802_OPCODE("LSQ %04X", long_skip(pc)); break;
	case 0xce: CDP1802_OPCODE("LSZ %04X", long_skip(pc)); break;
	case 0xcf: CDP1802_OPCODE("LSDF %04X", long_skip(pc)); break;
	case 0xfe: CDP1802_OPCODE("SHL"); break;
	//
	default:   CDP1801_OPCODE("illegal"); break;
	}

	return (pc - base_pc) | flags | SUPPORTED;
}
