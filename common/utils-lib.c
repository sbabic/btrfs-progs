/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#include "kerncompat.h"
#include "common/utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <ioctl.h>
#include "kernel-shared/ctree.h"

/*
 * This function should be only used when parsing command arg, it won't return
 * error to its caller and rather exit directly just like usage().
 */
u64 arg_strtou64(const char *str)
{
	u64 value;
	char *ptr_parse_end = NULL;

	value = strtoull(str, &ptr_parse_end, 0);
	if (ptr_parse_end && *ptr_parse_end != '\0') {
		fprintf(stderr, "ERROR: %s is not a valid numeric value.\n",
			str);
		exit(1);
	}

	/*
	 * if we pass a negative number to strtoull, it will return an
	 * unexpected number to us, so let's do the check ourselves.
	 */
	if (str[0] == '-') {
		fprintf(stderr, "ERROR: %s: negative value is invalid.\n",
			str);
		exit(1);
	}
	if (value == ULLONG_MAX) {
		fprintf(stderr, "ERROR: %s is too large.\n", str);
		exit(1);
	}
	return value;
}

/*
 * For a given:
 * - file or directory return the containing tree root id
 * - subvolume return its own tree id
 * - BTRFS_EMPTY_SUBVOL_DIR_OBJECTID (directory with ino == 2) the result is
 *   undefined and function returns -1
 */
int lookup_path_rootid(int fd, u64 *rootid)
{
	struct btrfs_ioctl_ino_lookup_args args;
	int ret;

	memset(&args, 0, sizeof(args));
	args.treeid = 0;
	args.objectid = BTRFS_FIRST_FREE_OBJECTID;

	ret = ioctl(fd, BTRFS_IOC_INO_LOOKUP, &args);
	if (ret < 0)
		return -errno;

	*rootid = args.treeid;

	return 0;
}

