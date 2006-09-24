	import	elfloader_onload

	code32
	rseg	CODE
entry	stmfd	sp!,{lr}
	ldr	r0,=struc
	swi	140
	ldmfd	sp!,{pc}
	
	rseg	CODE:DATA(4)
	dcd	0xB0C2D3E4,0xb0ba02,entry,0
	dcd	0,0,0,0
	dcb	0
extension
	dcb	"elf"
	alignrom 2
small_icons
	dcd	0x3f5
	dcd	0
big_icons
	dcd	0x439
	dcd	0

struc	dcd	extension
	dcd	0x5
	dcd	0x59C1200
	dcd	small_icons
	dcd	big_icons
	dcd	0xBB
	dcd	0x197
	dcd	0x7FFFC112
	dcd	elfloader_onload
	dcd	0
	
	end
