/*
 * udev.c
 *
 * Userspace devfs
 *
 * Copyright (C) 2003,2004 Greg Kroah-Hartman <greg@kroah.com>
 *
 *	This program is free software; you can redistribute it and/or modify it
 *	under the terms of the GNU General Public License as published by the
 *	Free Software Foundation version 2 of the License.
 * 
 *	This program is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *	General Public License for more details.
 * 
 *	You should have received a copy of the GNU General Public License along
 *	with this program; if not, write to the Free Software Foundation, Inc.,
 *	675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>

#include "libsysfs/sysfs/libsysfs.h"
#include "udev.h"
#include "udev_lib.h"
#include "udev_version.h"
#include "logging.h"
#include "namedev.h"

/* global variables */
char **main_argv;
char **main_envp;


#ifdef LOG
unsigned char logname[LOGNAME_SIZE];
void log_message (int level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	if (format[strlen(format)-1] != '\n')
		printf("\n");
}
#endif

int main(int argc, char *argv[], char *envp[])
{
	char *devpath;
	char temp[NAME_SIZE];
	char *subsystem = "";

	main_argv = argv;
	main_envp = envp;

	info("version %s", UDEV_VERSION);

	if (argv[1] == NULL) {
		info("udevinfo expects the DEVPATH of the sysfs device as a argument");
		goto exit;
	}

	/* initialize our configuration */
	udev_init_config();

	/* remove sysfs_path if given */
	if (strncmp(argv[1], sysfs_path, strlen(sysfs_path)) == 0)
		devpath = argv[1] + strlen(sysfs_path);
	else
		if (argv[1][0] != '/') {
			/* prepend '/' if missing */
			strfieldcpy(temp, "/");
			strfieldcat(temp, argv[1]);
			devpath = temp;
		} else {
			devpath = argv[1];
		}

	info("looking at '%s'", devpath);

	/* we only care about class devices and block stuff */
	if (!strstr(devpath, "class") &&
	    !strstr(devpath, "block")) {
		info("not a block or class device");
		goto exit;
	}

	/* initialize the naming deamon */
	namedev_init();

	if (argv[2] != NULL)
		subsystem = argv[2];

	/* simulate node creation with fake flag */
	udev_add_device(devpath, subsystem, FAKE);

exit:
	return 0;
}
