# REQUIRES: loongarch
# Check handling of TLS related relocations in a reduced .debug_info section.

# RUN: llvm-mc -filetype=obj -triple=loongarch32-unknown-elf %s -o %t.o
# RUN: ld.lld %t.o -o %t
# RUN: llvm-objdump -s -t %t | FileCheck %s

# CHECK:      Contents of section .debug_info:
# CHECK-NEXT:  {{.*}} 08000000 05000104 00000000

	.text
	.globl	_Z8get_tvarv
	.p2align	2
	.type	_Z8get_tvarv,@function
_Z8get_tvarv:
	ret

	.type	_ZZ8get_tvarvE4tvar,@object
	.section	.tbss,"awT",@nobits
	.p2align	2, 0x0
_ZZ8get_tvarvE4tvar:
	.word	0

	.section	.debug_info,"",@progbits
	.word	.Ldebug_info_end0-.Ldebug_info_start0 # Length of Unit
.Ldebug_info_start0:
	.half	5                               # DWARF version number
	.byte	1                               # DWARF Unit Type
	.byte	4                               # Address Size (in bytes)
	.dtprelword	_ZZ8get_tvarvE4tvar
.Ldebug_info_end0:
