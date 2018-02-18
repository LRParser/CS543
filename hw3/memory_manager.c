#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main()
{

  const int num_pages = 256;
  const int page_size_in_bytes = 256;
  const int num_tlb_entries = 16;
  const int frame_size = 256;
  const int num_frames = 256;
  const int memory_in_bytes = frame_size * num_frames;
  int frames_written = 0;
  int pages_written = 0;

  uint8_t page_table_keys[page_table_num_entries];
  uint8_t page_table_values[page_table_num_entries];

  // This implies each page table entry can store a single byte

  FILE *addresses = fopen("addresses.txt","r");
  char buf[256];
  while(fgets(buf,sizeof(buf),addresses) != NULL)
  {
    //buf[strlen(buf)-1] = '\0';
    uint32_t v_addr = strtoul(buf,NULL,10);

    uint8_t mid_8 = (v_addr & 0xFFFF) >> 8;
    uint8_t low_8 = v_addr & 0xFF;

    uint8_t page_num = mid_8;
    uint8_t offset_num = low_8;
    
    uint16_t phy_addr = frames_written * frame_size + offset_num;

    int frame_num;
    int page_table_hit = 0;

    // Try to find this page in the page table. 

    for (int i=0; i < num_pages; i++)
    {
      if(page_table_keys[i] == page_num)
      {
        frame_num = page_table_values[i];
        page_table_hit = 1;
      }
    }

    if (!page_table_hit)
    {
      frame_num = frames_written;
      frames_written++;
    }

    page_table_keys[pages_written] = page_num;
    page_table_values[pages_written] = frame_num;

    printf("Virtual address: %d, page_num: %d, offset: %d, frames_num: %d, physical address: %d\n",v_addr,page_num, offset, frame_num, phy_addr);

    frames_written++;

    // First, try to get the frame for the page_num from TLB

    // If this fails, see if we can get the frame for the page_num from page table

    // If this also fails, a page fault occurs. In this case, we read a 256-byte page (at page_num) from backing
    // storage, and store it in a page frame in memory.

    //int page = buf_

  }

}
