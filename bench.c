#define _GNU_SOURCE
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define N  100000


static void
print_dur(struct timespec *start, struct timespec *end, int nlog, const char *sys)
{
	end->tv_sec -= start->tv_sec;
	if ((end->tv_nsec -= start->tv_nsec) < 0) {
		end->tv_sec -= 1;
		end->tv_nsec += 1000000000L;
	}
	printf("%-10s 1<<%i %li.%09li\n", sys, nlog, end->tv_sec, end->tv_nsec);
	fflush(stdout);
}


int
main(void)
{
	char *buf = malloc(1 << 25);
	int i, nlog;
	int rw[2];
	size_t n, p;
	ssize_t r;
	struct timespec start, end;
	struct iovec iov;

	memset(buf, 0, 1 << 25);

	for (nlog = 0; nlog <= 25; nlog++) {
		n = 1 << nlog;

		socketpair(PF_UNIX, SOCK_STREAM, 0, rw);
		if (!fork()) {
			close(rw[1]);
			while (read(rw[0], buf, n) > 0);
			exit(0);
		} else {
			close(rw[0]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			for (i = 0; i < N; i++)
				for (p = 0; p < n; p += (size_t)r)
					if ((r = write(rw[1], buf + p, n - p)) < 0)
						return perror("write <socket>"), 1;
			close(rw[1]);
			wait(NULL);
			clock_gettime(CLOCK_MONOTONIC, &end);
			print_dur(&start, &end, nlog, "socketpair");
		}

		pipe(rw);
		if (!fork()) {
			close(rw[1]);
			while (read(rw[0], buf, n) > 0);
			exit(0);
		} else {
			close(rw[0]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			for (i = 0; i < N; i++)
				for (p = 0; p < n; p += (size_t)r)
					if ((r = write(rw[1], buf + p, n - p)) < 0)
						return perror("write <pipe>"), 1;
			close(rw[1]);
			wait(NULL);
			clock_gettime(CLOCK_MONOTONIC, &end);
			print_dur(&start, &end, nlog, "pipe");
		}

		pipe(rw);
		if (!fork()) {
			close(rw[1]);
			while (read(rw[0], buf, n) > 0);
			exit(0);
		} else {
			close(rw[0]);
			clock_gettime(CLOCK_MONOTONIC, &start);
			for (i = 0; i < N; i++) {
				iov.iov_base = buf;
				iov.iov_len = n;
				for (p = 0; p < n; p += (size_t)r) {
					r = vmsplice(rw[1], &iov, 1, 0);
					if (r < 0)
						return perror("vmsplice <pipe>"), 1;
					iov.iov_base += r;
					iov.iov_len -= (size_t)r;
				}
			}
			close(rw[1]);
			wait(NULL);
			clock_gettime(CLOCK_MONOTONIC, &end);
			print_dur(&start, &end, nlog, "vmsplice");
		}
	}
	return 0;
}
