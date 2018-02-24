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

  uint8_t page_table_keys[num_pages];
  uint8_t page_table_values[num_pages];
  char physical_memory[num_frames][frame_size];


  // Initialize all page table keys and values to 0
  for(int i=0; i<num_pages; i++)
  {
    page_table_keys[i] = 0;
    page_table_values[i] = 0;
  }

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
    

    int frame_num = 0;
    int page_table_hit = 0;
    char* frame_contents;

    // First, try to get the frame for the page_num from TLB
    // To be implemented


    // If this fails, see if we can get the frame for the page_num from page table

    for (int i=0; i < num_pages; i++)
    {
      if(page_table_keys[i] == page_num)
      {
        frame_num = page_table_values[i];
        frame_contents = physical_memory[frame_num];
        page_table_hit = 1;
      }
    }

    // If this also fails, a page fault occurs. In this case, we read a 256-byte page (at page_num) from the backing store
    if (!page_table_hit)
    {
      frame_num = frames_written;

      FILE *fp;
      if ((fp = fopen("BACKING_STORE.bin","rb")) != NULL)
      {
        fseek(fp, sizeof(char)*frame_size*page_num, SEEK_SET);
        fread(frame_contents, sizeof(char), frame_size, fp);
        fclose(fp);
      }

      frames_written++;
    }

    page_table_keys[pages_written] = page_num;
    page_table_values[pages_written] = frame_num;

    uint16_t phy_addr = frame_num * frame_size + offset_num;

    char byte_at_addr = frame_contents[offset_num];

    printf("Virtual address: %d, page_num: %d, frame_num: %d, offset: %d, physical address: %d, value: %d\n",v_addr,page_num, frame_num, offset_num, phy_addr, byte_at_addr);




    // storage, and store it in a page frame in memory.

    //int page = buf_

  }

}
