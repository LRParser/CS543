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
  const int INIT_VAL = -9999;

  // We keep track of the number of TLB hits and page faults
  int tlb_hit_count = 0;
  int page_fault_count = 0;

  int frames_written = 0;
  int pages_written = 0;
  int tlb_entries_written = 0;

  int page_table_keys[num_pages];
  int page_table_values[num_pages];
  int page_table_access_times[num_pages];

  int tlb_keys[num_tlb_entries];
  int tlb_values[num_tlb_entries];
  int tlb_access_times[num_tlb_entries];

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

  // Ensure the TLB is initialized properly
  for (int i=0; i<num_tlb_entries; i++)
  {
    tlb_keys[i] = tlb_values[i] = tlb_access_times[i] = INIT_VAL;
  }

  // Initialize all page table keys and values to 0
  for(int i=0; i<num_pages; i++)
  {
    page_table_keys[i] = page_table_values[i] = page_table_access_times[i] = INIT_VAL;
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
  int current_time = 0;
  while(fgets(buf,sizeof(buf),addresses) != NULL)
  {
    uint32_t v_addr = strtoul(buf,NULL,10);

    uint8_t mid_8 = (v_addr & 0xFFFF) >> 8;
    uint8_t low_8 = v_addr & 0xFF;

    uint8_t page_num = mid_8;
    uint8_t offset_num = low_8;
    

    int frame_num = 0;
    int tlb_hit = 0;
    int page_table_hit = 0;
    char frame_contents[frame_size];

    // First, try to get the frame for the page_num from TLB
    for (int i=0; i < num_tlb_entries; i++)
    {
      if (tlb_keys[i] == page_num)
      {
        frame_num = tlb_values[i];
        memcpy(frame_contents, physical_memory[frame_num], sizeof(char) * frame_size);
        // Update the access time for LRU algorithm; this makes this entry less likely to be removed in the future
        tlb_access_times[i] = current_time;
        tlb_hit = 1;
        tlb_hit_count++;
      }
    }


    // If this fails, see if we can get the frame for the page_num from page table

    if(!tlb_hit)
    {
      for (int i=0; i < num_pages; i++)
      {
        if(page_table_keys[i] == page_num)
        {
          frame_num = page_table_values[i];
          memcpy(frame_contents,physical_memory[frame_num],sizeof(char) * frame_size);
          page_table_access_times[i] = current_time;
          page_table_hit = 1;
          if (DEBUG)
          {
            printf("Page table hit for page %d returns frame %d\n",page_num,frame_num);
          }
          break;
        }
      }

      if (page_table_hit)
      {
        // If we did not find in TLB but did in page table, add a TLB entry for next time
        int tlb_entry_to_update = tlb_entries_written;

        if (tlb_entries_written >= num_tlb_entries)
        {
          int time_stamp_lowest = 99999;
          for (int i=0; i<num_tlb_entries; i++)
          {
            if (tlb_access_times[i] < time_stamp_lowest)
            {
              time_stamp_lowest = tlb_access_times[i];
            }
          }

          for (int i=0; i<num_tlb_entries; i++)
          {
            if (tlb_access_times[i] == time_stamp_lowest)
            {
              tlb_entry_to_update = i;
            }
          }
        }
        tlb_keys[tlb_entry_to_update] = page_num;
        tlb_values[tlb_entry_to_update] = frame_num;
        tlb_access_times[tlb_entry_to_update] = current_time;
       }
    }



    // If this also fails, a page fault occurs. In this case, we read a 256-byte page (at page_num) from the backing store
    if (!page_table_hit && !tlb_hit)
    {

      page_fault_count++;

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

      int page_to_update = pages_written;

      int tlb_entry_to_update = tlb_entries_written;

      if (tlb_entries_written >= num_tlb_entries)
      {
        // Find the LRU TLB entry
        int time_stamp_lowest = 99999;
        for (int i=0; i<num_tlb_entries; i++)
        {
          if (tlb_access_times[i] < time_stamp_lowest)
          {
            time_stamp_lowest = tlb_access_times[i];
          }
        }

        for (int i=0; i<num_tlb_entries; i++)
        {
          if (tlb_access_times[i] == time_stamp_lowest)
          {
            tlb_entry_to_update = i;
          }
        }
      }

      if(pages_written >= num_pages)
      {
        // Find the LRU page
        int time_stamp_lowest = 99999;
        for (int i=0; i<num_pages; i++)
        {
          if (page_table_access_times[i] <= time_stamp_lowest)
          {
            time_stamp_lowest = page_table_access_times[i];
          }
        }
        for (int i=0; i<num_pages; i++)
        {
          if (page_table_access_times[i] == time_stamp_lowest)
          {
            page_to_update = i;
          }
        }
      }

      tlb_keys[tlb_entry_to_update] = page_num;
      tlb_values[tlb_entry_to_update] = frame_num;
      tlb_access_times[tlb_entry_to_update] = current_time;
     
      tlb_entries_written++;

      page_table_keys[page_to_update] = page_num;
      page_table_values[page_to_update] = frame_num;
      page_table_access_times[page_to_update] = current_time;

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

    current_time++;

  }

  // Because current time just increases for each read address, at the end it has the number of entries processed. So our
  // rates are simply our counts divided by the total number (e.g., the current_time)

  printf("Page-fault rate in percent: %f\n",100 * (page_fault_count/(float)current_time));
  printf("TLB hit rate in percent: %f\n", 100 * (tlb_hit_count/(float)current_time));

}
