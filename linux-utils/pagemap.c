
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGEMAP_LEN	8
typedef unsigned long  u64;

u64 vir_to_phy(int pid, u64 vaddr)
{
	int fd, ret ;
	int page_size, page_shift = -1;
	u64 offset;
	char filename[64];
	u64 phy_addr;

	page_size = getpagesize();
	offset = (vaddr / page_size) * PAGEMAP_LEN;

	printf("offset = %lu\n", offset);
	sprintf(filename, "/proc/%d/pagemap", pid);
	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		perror("open:");
		return -1;
	}

	if (lseek(fd, offset, SEEK_SET) == -1) {
		perror("lseek:");
		close(fd);
		return -1;
	}

	ret = read(fd, (char *)&phy_addr, PAGEMAP_LEN);
	if (ret < 0) {
		perror("read:");
		close(fd);
		return -1;
	}
	printf("vir_addr = 0x%lx, pagemap = 0x%lx\n", vaddr, phy_addr);
	phy_addr = phy_addr & 0x7fffffffffffff;
	printf("vir_addr = 0x%lx, page_pfn = 0x%lx\n", vaddr, phy_addr);

	offset = vaddr % page_size;

	// calc page_shift
	while (!((1UL << ++page_shift) & page_size));
	
	phy_addr = (u64)((u64)phy_addr << page_shift) + offset;
	printf("vir_addr = 0x%lx, phy_addr = 0x%lx\n", vaddr, phy_addr);

	close(fd);

	return phy_addr;
}

int main(int argc, char **argv)
{
	int pid;
	u64 vaddr;
	char *endptr;
	
	if (argc < 3) {
		printf("Usage: ./pgmap pid vaddr\n");
		exit(0);
	}

	if (argc > 1) {
		pid = atoi(argv[1]);
	}
	if (argc > 2) {
		vaddr = strtoul(argv[2], &endptr, 16);
	}

	printf("pid = %d, vaddr = 0x%lx\n", pid, vaddr);
	

	vir_to_phy(pid, vaddr);
	
	return 0;
}


