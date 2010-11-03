#include <avr/io.h>
#include <avr/fuse.h>

FUSES =
{
	/* Don't forget to update F_CPU in hwparam.h if you change CKDIV or CKSEL */
	.low = LFUSE_DEFAULT,
        .high = HFUSE_DEFAULT,
        .extended = EFUSE_DEFAULT,
};

