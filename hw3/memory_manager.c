#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define DEBUG 0
int main()
{

  const int num_pages = 256;
  const int page_size_in_bytes = 256;
  const int num_tlb_entries = 16;
  const int frame_size = 256;
  const int num_frames = 256;
  const int memory_in_bytes = frame_size * num_frames;

  // This is a value that we use to initialize all int size fields, for ease in debugging
  const int INIT_VAL = 9999;

  int frames_written = 0;
  int pages_written = 0;

  int page_table_keys[num_pages];
  uint8_t page_table_values[num_pages];

  // This implies that the data that we read in at each physical address is a single byte
  char physical_memory[num_frames][frame_size];

  // Ensure physical memory is initialized properly
  for (int i=0; i<num_frames; i++)
  {
    for (int j=0; j<frame_size; j++)
    {
      physical_memory[i][j] = 255;
    }
  }


  // Initialize all page table keys and values to 0
  for(int i=0; i<num_pages; i++)
  {
    page_table_keys[i] = INIT_VAL;
    page_table_values[i] = INIT_VAL;
  }

  // Ensure we can open the backing store
  FILE *fp;
  fp = fopen("BACKING_STORE.bin","rb");
  
  if (fp == NULL)
  {
    printf("Unable to open backing store\n");
    exit(1);
  }


  FILE *addresses = fopen("addresses.txt","r");
  char buf[256];
  while(fgets(buf,sizeof(buf),addresses) != NULL)
  {
    uint32_t v_addr = strtoul(buf,NULL,10);

    uint8_t mid_8 = (v_addr & 0xFFFF) >> 8;
    uint8_t low_8 = v_addr & 0xFF;

    uint8_t page_num = mid_8;
    uint8_t offset_num = low_8;
    

    int frame_num = 0;
    int page_table_hit = 0;
    char frame_contents[frame_size];

    // First, try to get the frame for the page_num from TLB
    // To be implemented


    // If this fails, see if we can get the frame for the page_num from page table

    for (int i=0; i < num_pages; i++)
    {
      if(page_table_keys[i] == page_num)
      {
        frame_num = page_table_values[i];
        memcpy(frame_contents,physical_memory[frame_num],sizeof(char) * frame_size);
        page_table_hit = 1;
        if (DEBUG)
        {
          printf("Page table hit for page %d returns frame %d\n",page_num,frame_num);
        }
        break;
      }
    }

    // If this also fails, a page fault occurs. In this case, we read a 256-byte page (at page_num) from the backing store
    if (!page_table_hit)
    {
      // We store the data at the next available frame
      frame_num = pages_written;
      if (DEBUG)
      {
        printf("pages_written is: %d storing at: %d\n",pages_written,frame_num);
      }
      fseek(fp, sizeof(char)*frame_size*page_num, SEEK_SET);
      fread(frame_contents, sizeof(char), frame_size, fp);
      
      // Store in physical memory to reduce likelihood of having to read from backing store again on proximate future attempts to retrieve this page/frame combination
      for (int i=0; i < frame_size; i++)
      {
        physical_memory[frame_num][i] = frame_contents[i];
      }
      page_table_keys[pages_written] = page_num;
      page_table_values[pages_written] = frame_num;
      pages_written++;
    }

    uint16_t phy_addr = frame_num * frame_size + offset_num;

    char byte_at_addr = frame_contents[offset_num];

    if(DEBUG)
    {
      printf("Virtual address: %d, page_num: %d, frame_num: %d, offset: %d, physical address: %d, value: %d\n",v_addr,page_num, frame_num, offset_num, phy_addr, byte_at_addr);
    }
    else
    {
      printf("Virtual address: %d Physical address: %d Value: %d\n",v_addr,phy_addr,byte_at_addr);
    }



    // storage, and store it in a page frame in memory.

    //int page = buf_

  }

}
