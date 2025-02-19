/* ************************************************************************** */
/*																			*/
/*														:::	  ::::::::   */
/*   parsing.c										  :+:	  :+:	:+:   */
/*													+:+ +:+		 +:+	 */
/*   By: jchene <jchene@student.42.fr>			  +#+  +:+	   +#+		*/
/*												+#+#+#+#+#+   +#+		   */
/*   Created: 2025/02/18 13:11:12 by jchene			#+#	#+#			 */
/*   Updated: 2025/02/18 15:09:58 by jchene		   ###   ########.fr	   */
/*																			*/
/* ************************************************************************** */

#include "ping.h"

static t_long_options *get_long_opts()
{
	static t_long_options long_opts[] = {
		{"help", no_argument, 0, '?'},
		{"verbose", no_argument, 0, 'v'},
		{"numeric", no_argument, 0, 'n'},
		{"timeout", required_argument, 0, 'w'},
		{"linger", required_argument, 0, 'W'},
		{"size", required_argument, 0, 's'},
		{"ttl", required_argument, 0, 'L'},
		{0, 0, 0, 0}};
	return long_opts;
}

static void handle_option(t_options *opts, int opt_char, char **argv)
{
	switch (opt_char){
	case '?':
		if (strcmp(argv[optind - 1], "--help") != 0 && strcmp(argv[optind - 1], "-?") != 0)
			opts->opt_error = ERR_OPT_ERROR;
		else
			opts->help = TRUE;
		break;
	case 'v':
		opts->verbose = TRUE;
		break;
	case 'n':
		opts->num_output = TRUE;
		break;
	case 'w':
		opts->prgm_deadline = (unsigned)atoi(optarg);
		break;
	case 'W':
		opts->packet_timeout = (unsigned)atoi(optarg);
		break;
	case 's':
		opts->packet_size = (unsigned)atoi(optarg);
		break;
	case 'L':
		opts->ttl = (unsigned)atoi(optarg);
		break;
	default:
		opts->opt_error = ERR_UNKNOWN_ERR;
		break;
	}
}

t_options parse_options(int argc, char **argv)
{
	int opt_char;
	int option_index = 0;
	t_options opts = {
		DEFAULT_HELP, DEFAULT_VERBOSE, DEFAULT_NUM_OUTPUT,
		DEFAULT_OPT_ERROR, DEFAULT_PRGM_DEADLINE, DEFAULT_PACKET_TIMEOUT,
		DEFAULT_PACKET_SIZE, DEFAULT_TTL, DEFAULT_HOST};
	t_long_options *long_options = get_long_opts();
	
	opterr = 0;
	while ((opt_char = getopt_long(argc, argv, "?vnw:W:s:L:", long_options, &option_index)) != -1){
		handle_option(&opts, opt_char, argv);
		if (opts.help || opts.opt_error)
			return opts;
	}
	opts.host = argv[optind];
	if (!opts.host)
		opts.opt_error = ERR_MISSING_HOST;
	return opts;
}
