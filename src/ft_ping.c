/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:21 by jchene            #+#    #+#             */
/*   Updated: 2025/02/20 15:55:09 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static t_err print_error(t_err err, char** argv) {
	switch (err) {
	case ERR_OPT_ERROR:
		fprintf(stderr, "ft_ping: invalid option -- '%s'\nTry '%s --help' for more information.\n", argv[optind - 1], argv[0]);
		break;
	case ERR_MISSING_HOST:
		fprintf(stderr, "ft_ping: missing host operand\nTry './ft_ping --help' for more information.");
		break;
	case ERR_UNKNOWN_ERR:
		fprintf(stderr, "ft_ping: Unknown error... Computer isn't computering.\n");
		break;
	case ERR_SOCK_CREAT_FAIL:
		fprintf(stderr, "ft_ping: Socket creation failed... Computer isn't computering.\n");
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
	case ERR_RECVFROM_FAIL:
		fprintf(stderr, "ft_ping: Recv function failed... Computer isn't computering.\n");
		break;
	default:
		break;
	}
	return err;
}

static t_err print_help() {
	printf("Usage: ./ft_ping [OPTION]... HOST\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf("  -?, --help\t\t\tDisplay this help and exit.\n");
	printf("  -v, --verbose\t\t\tPrint detailed output.\n");
	printf("  -n, --numeric\t\t\tDo not resolve hostnames.\n");
	printf("  -w, --timeout=SECONDS\t\tTime to wait for a response in seconds.\n");
	printf("  -W, --linger=SECONDS\t\tTime to wait for a response in seconds.\n");
	printf("  -s, --size=BYTES\t\tSpecify the number of data bytes to be sent.\n");
	printf("  -L, --ttl=HOPS\t\t\tSpecify the number of hops a packet can traverse.\n");
	return ERR_NO_ERR;
}

int main(int argc, char** argv) {
	t_options opts;
	t_context context;

	opts = parse_options(argc, argv);
	if (opts.opt_error)
		return print_error(opts.opt_error, argv);
	if (opts.help)
		return print_help();

	context = create_threads(opts);
	if (context.net_error)
		return print_error(context.net_error, argv);

	
	close(context.sockfd);
	return ERR_NO_ERR;
}