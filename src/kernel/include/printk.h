#ifndef __ARCUS_INCLUDE_PRINTK
#define __ARCUS_INCLUDE_PRINTK
namespace arcus
{
	enum pt_state {
		start,
		flag,
		decimal_int,
		hex_int,
		long_flag,
		decimal_long,
		hex_long,
		str,
		ch
	};

	void printk(const char* s, ...);

	void clear_text_screnn();
}
#endif
