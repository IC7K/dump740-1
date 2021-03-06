/* Part of dump740, a UVD protocol decoder for RTLSDR devices.
 *
 * Copyright (c) 2016 Alexandr Ivanov <alexandr.sky@gmail.com>
 *
 * This file is free software: you may copy, redistribute and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "options.h"
#include <argp.h>
#include <stdlib.h>


static struct argp_option argp_options[] = {
	{"device-index",	'd', "<index>",		0, "Select RTLSDR device" },
	{"freq",			'f', "<hz>",		0, "Set frequency (default: 740 MHz);\nYou can use 'M' and 'K' as reduction for million and thousand respectively" },
	{"freq-correction",	'c', "<ppm>",		0, "Set frequency correction (default: 0)" },
	{"gain",			'g', "<db>",		0, "Set gain (default: max gain, use 'A' for auto-gain)"},
	{"ifile",			'i', "<filename>",	0, "Read data from file"},
	{"raw",				'r', 0,				0, "Show only raw messages"},
	{"log-level",		'l', "<0-4>",		0, "Minimum log level: 0 - DEBUG, 1 - INFO, 2 - WARNING, 3 - FATAL"},
#ifdef TEST
	{"dump",			'D', 0,				0, "Dump raw blocks data to file."},
	{"blocks-statistic",'B', 0,				0, "Show blocks statistic."},
#endif
	{ 0 }
};


options_t options;


static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	options_t *options = state->input;
	int len, mult = 1;

	switch (key) {
		case 'd':
			options->dev_index = atoi(arg);
			break;
		case 'f':
			len = strlen(arg);
			if (len < 1)
				fatal("Incorrect argument for freq-correction: '%s'\n", arg);
			if (arg[len-1] == 'M')
				mult = 1000000;
			else if (arg[len-1] == 'K')
				mult = 1000;
			options->freq = atoi(arg) * mult;
			break;
		case 'c':
			options->freq_correction = atoi(arg);
			break;
		case 'g':
			if (strcmp(arg, "A") == 0)
				options->gain = ARG_GAIN_AUTO;
			else
				options->gain = atof(arg) * 10;
			break;
		case 'i':
			options->ifile = arg;
			break;
		case 'r':
			options->raw = 1;
			break;
		case 'l':
			options->log_level = atoi(arg);
			break;
#ifdef TEST
		case 'D':
			options->dump = 1;
			break;
		case 'B':
			options->bstat = 1;
			break;
#endif
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int parse_args(int argc, char **argv)
{
	char program_desc[128];

	snprintf(program_desc, sizeof(program_desc),
				"Program for receiving and decoding UVD protocol.\n"
				"Version: %d.%d.%d.\n"
				"Example: %s -d 1 -f 730M -g 42.1 -a -r",
				VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH,
				argv[0]);

	options.dev_index = 0;
	options.freq = DEFAULT_FREQUENCY;
	options.freq_correction = 0;
	options.gain = ARG_GAIN_MAX;
	options.ifile = NULL;
	options.raw = 0;
	options.log_level = DEFAULT_LOG_LEVEL;
#ifdef TEST
	options.dump = 0;
	options.bstat = 0;
#endif

	struct argp argp = {argp_options, parse_opt, "", program_desc};

	argp_parse(&argp, argc, argv, 0, 0, &options);
}

