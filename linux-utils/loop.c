
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>

#include <limits.h>
#include <getopt.h>


static int thread_mode;
static int timeout;
static int delay;
static int child_count = 1;
static int duration;
static int foreground;
static pthread_t *pchild;

static void *loop(void *args)
{
	if (!thread_mode)
		printf("create loop task success, pid %d\n", getpid());
	else
		printf("create loop task success, tid %d\n", getpid());
	while(1);
}

static pthread_t create_task(void *(*func)(void *), void *args)
{
	pthread_attr_t attr;
	pthread_t childid;
	int ret;

	if (!thread_mode) {
		/* process mode */
		/* Fork the receiver. */
		switch (fork()) {
		case -1:
			err(EXIT_FAILURE, "fork()");
			break;
		case 0:
			func(args);
			exit(0);
			break;
		default:
			break;
		}

		return (pthread_t)0;
	}

	if (pthread_attr_init(&attr) != 0)
		err(EXIT_FAILURE, "pthread_attr_init:");

	ret = pthread_create(&childid, &attr, func, args);
	if (ret != 0)
		err(EXIT_FAILURE, "pthread_create failed");

	return childid;
}

static void reap_task(pthread_t id)
{
	int proc_status;
	void *thread_status;

	if (!thread_mode) {
		/* process mode */
		wait(&proc_status);
		if (!WIFEXITED(proc_status))
			exit(1);
	} else {
		pthread_join(id, &thread_status);
	}
}

static int run_loop_tasks(void)
{
	int i;

	for (i=0; i<child_count; i++)
		pchild[i] = create_task(loop, (void*)i);
	for (i=0; i<child_count; i++)
		reap_task(pchild[i]);
}

static void usage(void)
{
	printf("loop propram for test.\n\n"
		"-f|--foreground        Run in the foreground\n"
		"-H|--thread-mode       create thread tasks\n"
		"-t|--duration          Exit after running @timeout seconds\n"
		"-d|--delay             Delays delay seconds before creating\n"
		"-h|--help              Show usage information\n"
		);
}

static struct option opts[] = {
	{ "foreground", no_argument, NULL, 'F' },
	{ "thread_mode", no_argument, NULL, 'H' },
	{ "duration", optional_argument, NULL, 't' },
	{ "delay", no_argument, NULL, 'd' },
	{ "number", no_argument, NULL, 'n' },
	{ "help", no_argument, NULL, 'h' },
	{ },
};

int main(int argc, char **argv)
{
	int c;
	int ret;

	while ((c = getopt_long(argc, argv, "FhHt:w:", opts, NULL)) != -1)
		switch (c) {
		case 'F':
			foreground = 1;
			break;
		case 'H':
			thread_mode = 1;
			break;
		case 'h':
			usage();
			exit(0);
			break;
		case 'd':
			delay = atoi(optarg);
			break;
		case 'n':
			child_count = atoi(optarg);
			break;
		case 't':
			if (optarg)
				duration = atoi(optarg);
			break;
		default:
			printf("%s: Invalid option '%c'\n", argv[0], optopt);
			break;

	}

	pchild = malloc(child_count * sizeof(pthread_t));
	if (!pchild) {
		err(EXIT_FAILURE, "main:malloc()");
	}

	run_loop_tasks();
	return 0;
}

