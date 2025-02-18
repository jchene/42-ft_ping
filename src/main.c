/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:52:21 by jchene            #+#    #+#             */
/*   Updated: 2025/02/18 16:02:15 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ping.h"

static t_err print_error(t_err err, char **argv){
	switch (err){
	case ERR_OPT_ERROR:
		fprintf(stderr, "ft_ping: invalid option -- '%s'\nTry '%s --help' for more information.\n", argv[optind - 1], argv[0]);
		break;
	case ERR_MISSING_HOST:
		fprintf(stderr, "ft_ping: missing host operand\nTry './ft_ping --help' for more information.");
		break;
	case ERR_UNKNOWN_ERR:
		fprintf(stderr, "ft_ping: Unknown error... Computer isn't computering.\n");
		break;
	case ERR_SOCKET:
		fprintf(stderr, "ft_ping: Socket creation failed... Computer isn't computering.\n");
		break;
	case ERR_UNKNOWN_HOST:
		fprintf(stderr, "ft_ping: Unknown host.\n");
		break;
	default:
		break;
	}
	return err;
}

static t_context init_context(t_options opts){
	t_context context;
	struct sockaddr_in target_addr;
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	if (sockfd < 0){
		context.net_error = ERR_SOCKET;
		return context;
	}
	context.sockfd = sockfd;
	memset(&target_addr, 0, sizeof(target_addr));
	target_addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, opts.host, &target_addr.sin_addr) <= 0) {
		struct hostent *host = gethostbyname(opts.host);
		if (!host) {
			close(sockfd);
			context.net_error = ERR_UNKNOWN_HOST;
			return context;
		}
		memcpy(&target_addr.sin_addr, host->h_addr, host->h_length);
	}
	context.target_addr = target_addr;
	context.opts = opts;
	context.running = true;
	pthread_mutex_init(&context.lock, NULL);
	return context;
}

int main(int argc, char **argv){
	t_options opts;
	t_context context;

	opts = parse_options(argc, argv);
	if (opts.opt_error)
		return print_error(opts.opt_error, argv);

	context = init_context(opts);
	if (context.net_error)
		return print_error(context.net_error, argv);
	
	close(context.sockfd);
	return 0;
}