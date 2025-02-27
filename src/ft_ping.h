/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jchene <jchene@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/17 13:22:47 by jchene            #+#    #+#             */
/*   Updated: 2025/02/27 18:32:20 by jchene           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H

# define FT_PING_H
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
# include <fcntl.h>
# include <math.h>

# define FALSE 0
# define TRUE 1

# define ERR_NO_ERR 0
# define ERR_OPT_ERROR 1
# define ERR_MISSING_HOST 2
# define ERR_UNKNOWN_ERR 3
# define ERR_SIGACTION_FAIL 4
# define ERR_SOCK_CREAT_FAIL 5
# define ERR_SETSOCKOPT_FAIL 6
# define ERR_UNKNOWN_HOST 7
# define ERR_THREAD_CREAT_FAIL 8
# define ERR_SENDTO_FAIL 9
# define ERR_SELECT_FAIL 10
# define ERR_RECVFROM_FAIL 11

# define DEFAULT_HELP FALSE
# define DEFAULT_VERBOSE FALSE
# define DEFAULT_NUM_OUTPUT FALSE
# define DEFAULT_OPT_ERROR FALSE
# define DEFAULT_PRGM_DEADLINE 0
# define DEFAULT_PACKET_TIMEOUT 10
# define DEFAULT_PACKET_SIZE 56
# define DEFAULT_TTL 255
# define DEFAULT_HOST NULL

# define PACKET_LIST_MAX_SIZE 65536
# define PRGM_SLEEP 2000

typedef char t_err;
typedef struct option t_long_options;

typedef struct s_options {
	bool		help;
	bool		verbose;
	bool		num_output;
	t_err		opt_error;
	unsigned	prgm_deadline;
	unsigned	packet_timeout;
	unsigned	packet_size;
	unsigned	ttl;
	char* host;
}	t_options;

typedef struct s_packet_info {
	struct timeval send_time;
	unsigned sequence;
}	t_packet_info;

typedef struct s_packet_context {
	struct sockaddr_in sender_addr;
	socklen_t sender_addr_len;
	char buffer[1024];
	int recv_len;
}	t_packet_context;

typedef struct s_packets_stats {
	unsigned sent;
	unsigned received;
	double avg_time;
	double min_time;
	double max_time;
	double dif_squares_sum;
}	t_packet_stats;

typedef struct s_context {
	int sockfd;
	struct sockaddr_in target_addr;
	t_options opts;
	t_packet_stats stats;
	pthread_t send_thread_id;
	pthread_t recv_thread_id;
	pthread_mutex_t running_lock;
	bool running;
	pthread_mutex_t err_lock;
	t_err ctx_error;
	pthread_mutex_t list_lock;
	unsigned packet_list_size;
	t_packet_info packet_list[PACKET_LIST_MAX_SIZE];
}	t_context;

t_options parse_options(int argc, char** argv);
t_context *parent_thread(t_options opts);
void* receive_thread(void* arg);
void* send_thread(void* arg);
t_context *get_context();
void handle_sigint(int sig);

void mutex_set_running(t_context* context, bool running);
void mutex_set_ctx_error(t_context* context, t_err ctx_error);
void mutex_set_packet_info(t_context* context, t_packet_info packet_info);
bool mutex_get_running(t_context* context);
t_err mutex_get_ctx_error(t_context* context);
unsigned mutex_get_list_size(t_context *context);
t_packet_info mutex_get_packet_info_by_seq(t_context* context, unsigned sequence);
t_packet_info mutex_get_packet_info_by_index(t_context* context, unsigned sequence);

unsigned get_oldest_packet(t_context* context);
unsigned short checksum(void* b, int len);
void print_stats(t_packet_stats* stats);

#endif