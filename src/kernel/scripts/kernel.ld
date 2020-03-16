ENTRY(_start)
SECTIONS
{
	.text 0x100000 : 
	{
		*(.text)
		. = ALIGN(4096);
	}

	.data : 
	{
		*(.text)
		. = ALIGN(4096);
	}

	.bss :
	{
		*(.bss)
		. = ALIGN(4096);
	}
}