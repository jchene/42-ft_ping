/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:22:47 by jchene            #+#    #+#             */
/*   Updated: 2025/02/18 15:56:53 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H

# define PING_H
# define _GNU_SOURCE

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <string.h>
# include <getopt.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <errno.h>
# include <signal.h>
# include <sys/time.h>
# include <pthread.h>

# define FALSE 0
# define TRUE 1

# define ERR_OPT_ERROR 2
# define ERR_MISSING_HOST 3
# define ERR_UNKNOWN_ERR 4
# define ERR_SOCKET 5
# define ERR_UNKNOWN_HOST 6

# define DEFAULT_HELP FALSE
# define DEFAULT_VERBOSE FALSE
# define DEFAULT_NUM_OUTPUT FALSE
# define DEFAULT_OPT_ERROR FALSE
# define DEFAULT_PRGM_DEADLINE 0
# define DEFAULT_PACKET_TIMEOUT 10
# define DEFAULT_PACKET_SIZE 56
# define DEFAULT_TTL 255
# define DEFAULT_HOST NULL

typedef char t_err;

typedef struct s_options {
	bool		help;
	bool		verbose;
	bool		num_output;
	t_err		opt_error;
	unsigned	prgm_deadline;
	unsigned	packet_timeout;
	unsigned	packet_size;
	unsigned	ttl;
	char		*host;
}	t_options;

typedef struct s_context { 
	int sockfd;
	struct sockaddr_in target_addr;
	t_err net_error;
	t_options opts;
	bool running;
	pthread_mutex_t lock;
}	t_context;

t_options parse_options(int argc, char **argv);

#endif