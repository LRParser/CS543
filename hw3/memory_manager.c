#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main()
{

  int page_table_entries = 256;
  int page_size_in_bytes = 256;
  int tlb_entries = 16;
  int frame_size = 256;
  int num_frames = 256;
  int memory_in_bytes = frame_size * num_frames;

  // This implies each page table entry can store a single byte

  FILE *addresses = fopen("addresses.txt","r");
  char buf[256];
  while(fgets(buf,sizeof(buf),addresses) != NULL)
  {
    //buf[strlen(buf)-1] = '\0';
    uint32_t v_addr = strtoul(buf,NULL,10);

    uint32_t mid_8 = (v_addr & 0xFFFF) >> 8;
    uint32_t low_8 = v_addr & 0xFF;

    uint32_t page_num = mid_8;
    uint32_t offset_num = low_8;

    printf("Virtual address: %d, mid_8: %d, low_8: %d\n",v_addr,mid_8,low_8);

    // First, try to get the frame for the page_num from TLB

    // If this fails, see if we can get the frame for the page_num from page table

    // If this also fails, a page fault occurs. In this case, we read a 256-byte page (at page_num) from backing
    // storage, and store it in a page frame in memory.

    //int page = buf_

  }

}
