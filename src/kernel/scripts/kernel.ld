ENTRY(_start)
SECTIONS
{
	.init 0x100000:
	{
		PROVIDE(__INIT_BEGIN = .);
		*(.init)
		*(.init*)
		. = ALIGN(4096);
		PROVIDE(__INIT_END = .);
	}

	.text : 
	{
		*(.text)
		. = ALIGN(4096);
	}

	.rodata : 
	{
		*(.rodata)
		. = ALIGN(4096);
	}

	.data : 
	{
		*(.data)
		. = ALIGN(4096);
	}

	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}

	PROVIDE(__KERNEL_END__ = .);
}
