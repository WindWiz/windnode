#include <avr/io.h>
#include <avr/fuse.h>

FUSES =
{
	/* Don't forget to update F_CPU in hwparam.h if you change CKDIV or CKSEL */
	.low = (FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0),
        .high = HFUSE_DEFAULT,
        .extended = EFUSE_DEFAULT,
};

