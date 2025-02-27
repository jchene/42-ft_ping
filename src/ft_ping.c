/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:21 by jchene            #+#    #+#             */
/*   Updated: 2025/02/27 18:21:56 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static t_err print_error(t_err err, char** argv) {
	switch (err) {
	case ERR_OPT_ERROR:
		fprintf(stderr, "ft_ping: invalid option -- '%s'\nTry '%s --help' for more information.\n", argv[optind - 1], argv[0]);
		break;
	case ERR_MISSING_HOST:
		fprintf(stderr, "ft_ping: missing host operand\nTry './ft_ping --help' for more information.\n");
		break;
	case ERR_UNKNOWN_ERR:
		fprintf(stderr, "ft_ping: Unknown error... Computer isn't computering.\n");
		break;
	case ERR_SIGACTION_FAIL:
		fprintf(stderr, "ft_ping: Signal action failed... Computer isn't computering.\n");
		break;
	case ERR_SOCK_CREAT_FAIL:
		fprintf(stderr, "ft_ping: Socket creation failed... Do you have root privileges?.\n");
		break;
	case ERR_SETSOCKOPT_FAIL:
		fprintf(stderr, "ft_ping: Socket option setting failed... Computer isn't computering.\n");
		break;
	case ERR_UNKNOWN_HOST:
		fprintf(stderr, "ft_ping: Unknown host.\n");
		break;
	case ERR_THREAD_CREAT_FAIL:
		fprintf(stderr, "ft_ping: Thread creation failed... Computer isn't computering.\n");
		break;
	case ERR_SENDTO_FAIL:
		fprintf(stderr, "ft_ping: Sendto function failed... Computer isn't computering.\n");
		break;
	case ERR_SELECT_FAIL:
		fprintf(stderr, "ft_ping: Select function failed... Computer isn't computering.\n");
		break;
	case ERR_RECVFROM_FAIL:
		fprintf(stderr, "ft_ping: Recv function failed... Computer isn't computering.\n");
		break;
	default:
		break;
	}
	return err;
}

static t_err print_help() {
	printf("Usage: ft_ping [OPTION]... HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -n, --numeric\t\t\tdo not resolve host addresses\n");
	printf("  -L, --ttl=N\t\t\tspecify N as time-to-live\n");
	printf("  -v, --verbose\t\t\tverbose output\n");
	printf("  -w, --timeout=N\t\tstop after N seconds\n");
	printf("  -W, --linger=N\t\tnumber of seconds to wait for response\n");
	printf("  -s, --size=NUMBER\t\tSpecify the number of data bytes to be sent.\n");
	printf("  -?, --help\t\t\tgive this help list\n\n");
	printf("Mandatory or optional arguments to long options are also mandatory or optional for any corresponding short options.\n");
	return ERR_NO_ERR;
}

int main(int argc, char** argv) {
	t_options opts;
	t_context *context;

	opts = parse_options(argc, argv);
	if (opts.opt_error)
		return print_error(opts.opt_error, argv);
	if (opts.help)
		return print_help();

	context = parent_thread(opts);
	if (context->ctx_error)
		return print_error(context->ctx_error, argv);
	
	return ERR_NO_ERR;
}

//Left:
//Numeric
//Verbose