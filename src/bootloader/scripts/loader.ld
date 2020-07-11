ENTRY(main)
SECTIONS
{
    .text :
    {
        *(.text.main);
        *(.text*);
    }
}