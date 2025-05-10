#include "code.h"
#include <stdio.h>
#include <string.h>

static const string bin_header = S("\tglobal\t_start\n"
                                   "\tsection\t.text\n"

                                   "_exit:\n"

                                   "\tpush\tr9\n"
                                   "\tpush\tr8\n"
                                   "\tpush\trcx\n"
                                   "\tpush\trdx\n"
                                   "\tpush\trsi\n"
                                   "\tpush\trdi\n"

                                   "\tmov\trax, 1\n"
                                   "\tmov\trdi, 1\n"
                                   "\tmov\trsi, rsp\n"
                                   "\tmov\trdx, 48\n"
                                   "\tsyscall\n"

                                   "\tmov\trax, 60\n"
                                   "\txor\trdi, rdi\n"
                                   "\tsyscall\n"

                                   "_start:\n"

                                   "\tmov\trdi, 0\n"
                                   "\tmov\trsi, 0\n"
                                   "\tmov\trdx, 0\n"
                                   "\tmov\trcx, 0\n"
                                   "\tmov\tr8, 0\n"
                                   "\tmov\tr9, 0\n"
                                   "\tjmp\tmain\n");
static const string operations[] = {S("mov"), S("add"), S("je"), S("jmp")};
static const string registers[] = {S("rdi"), S("rsi"), S("rdx"),
                                   S("rcx"), S("r8"),  S("r9")};

static int emit_instr(char *loc, int n, instr ins)
{
        string op = operations[ins.type];
        string dst = ins.dst.type == TOK_REGISTER ? registers[ins.dst.i_value]
                                                  : ins.dst.s_value;
        string src = ins.src.type == TOK_REGISTER ? registers[ins.src.i_value]
                                                  : ins.src.s_value;
        switch (ins.type) {
        case I_MOV:
                return snprintf(loc, n, "\t%.*s\t%.*s, %.*s\n", op.len, op.addr,
                                dst.len, dst.addr, src.len, src.addr);
        case I_ADD:
                return snprintf(loc, n, "\t%.*s\t%.*s, %.*s\n", op.len, op.addr,
                                dst.len, dst.addr, src.len, src.addr);
        case I_JEQ:
                return snprintf(loc, n, "\tcmp\t%.*s, 0\n\tje\t%.*s\n", src.len,
                                src.addr, dst.len, dst.addr);
        case I_JMP:
                return snprintf(loc, n, "\t%.*s\t%.*s\n", op.len, op.addr,
                                dst.len, dst.addr);
        }
}

static const int part_sz = 256;
string code_generation(arena *a, program prog)
{
        string code;
        int potential = bin_header.len + part_sz;
        code.addr = make(a, char, potential);
        code.len = bin_header.len;
        memcpy(code.addr, bin_header.addr, code.len);

        int n;
        int i = 0;
        int j = 0;
        char *loc;
        instr ins;
        int rewind;
        string label;
        string op;
        instr_seq seq;
        while (i < prog.len) {
                seq = prog.blocks[i++];
                label = seq.label.s_value;

                rewind = code.len;
                n = potential - code.len;
                loc = code.addr + code.len;
                code.len += snprintf(loc, n, "%.*s:", label.len, label.addr);
                if (code.len >= potential) {
                        steal(a, char, part_sz);
                        potential += part_sz;
                        code.len = rewind;
                        i--;
                }

                j = 0;
                while (j < seq.len) {
                        ins = seq.arr[j++];
                        rewind = code.len;
                        n = potential - code.len;
                        loc = code.addr + code.len;
                        code.len += emit_instr(loc, n, ins);
                        if (code.len >= potential) {
                                steal(a, char, part_sz);
                                potential += part_sz;
                                code.len = rewind;
                                j--;
                        }
                }
        }

        return code;
}
