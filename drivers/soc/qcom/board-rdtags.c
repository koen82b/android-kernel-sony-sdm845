/*
 *
 * Author: Nilsson, Stefan 2 <stefan2.nilsson@sonymobile.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
/*
 * Copyright (C) 2015 Sony Mobile Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 */

#include <linux/rdtags.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <asm/setup.h>
#include <linux/io.h>
#include <linux/init.h>
#include "board-rdtags.h"

#define RDTAGS_TAG_MAGIC 0xBADFAD01
#define NBR_OF_ELEMENTS 3

struct rdtags_tag_const {
	const char const key[16];
	const char const value[64];
};

struct rdtags_build_tags {
	const unsigned int magic;
	const unsigned int size;
	const struct rdtags_tag_const tag_array[NBR_OF_ELEMENTS];
};

static char slot_suffix[3] = {0};
static int __init boot_slot_suffix_param(char *str)
{
	if (!str)
		return -EINVAL;

	if (!strncmp(str, "_a", 2) || !strncmp(str, "_b", 2))
		strlcpy(slot_suffix, str, 3);

	return 0;
}

early_param("androidboot.slot_suffix", boot_slot_suffix_param);

static const struct rdtags_build_tags rdtags_build_info = {
	RDTAGS_TAG_MAGIC,
	sizeof(rdtags_build_info),
	{
		{
			"build_product",
			INFO_PRODUCT
		},
		{
			"build_variant",
			INFO_VARIANT
		},
		{
			"build_id",
			INFO_BUILDID
		}
	}
};

static int board_rdtags_init(int ramdump_mode)
{
	int nbr_tags = 0;
	int i = 0;

	if (!ramdump_mode) {
		for (i = 0; i < NBR_OF_ELEMENTS; i++) {
			const char *key;
			const unsigned char *value;
			unsigned int size = 0;

			key = rdtags_build_info.tag_array[i].key;
			value = rdtags_build_info.tag_array[i].value;
			size = strnlen(value,
				sizeof(rdtags_build_info.tag_array[i].value));
			if (!rdtags_add_tag(key, value, size))
				nbr_tags++;
		}

		if (slot_suffix[0]) {
			rdtags_add_tag("slot_suffix", slot_suffix,
					sizeof(slot_suffix));
			nbr_tags++;
		}
	}

	return nbr_tags;
}

struct rdtags_platform_data rdtags_platdata = {
	.platform_init	= board_rdtags_init,
};
