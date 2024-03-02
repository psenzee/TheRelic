#include "core/global.h"
#include "core/file.h"

#include <stdio.h>
#include <stdlib.h>

#include <mach/mach.h>
#include <mach/mach_host.h>

#include <unistd.h>

Profiler  *profiler = 0;
//GameState *state    = 0;

//float _g_inputSensitivity = 100.0f;
//float _g_tiltFactor       = 1.0f;

const char *_g_readOnlyPath  = "./data";
const char *_g_readWritePath = "./save";

extern "C" 
{
/*
static const char *makePath(const char *path, const char *filename)
{
    static char complete[1024];
    sprintf(complete, "%s/%s", path, filename);
    return complete;
}

const char *globalTranslatePath(const char *filename)
{
    return makePath(_g_readOnlyPath, filename);
}

const char *globalTranslateReadWritePath(const char *filename)
{
    return makePath(_g_readWritePath, filename);
}

const char *globalReadTextFile(const char *filename)
{
    file::buffer_t b = file::read_file(globalTranslatePath(filename));
    if (!b.data)
    {
        printf("Unable to load text file '%s'\n", filename);
        return 0;
    }
    printf("Loading text file '%s' and leaking!!\n", filename);
    return (const char *)b.data;
}
*/

unsigned GetFreeMemory()
{
	mach_port_t host_port;
	mach_msg_type_number_t host_size;
	vm_size_t pagesize;
	host_port = mach_host_self();
	host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
	host_page_size(host_port, &pagesize);
	vm_statistics_data_t vm_stat;
	if (host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) != KERN_SUCCESS) 
	{
		printf("Failed to fetch vm statistics");
		return 0;
	}
	/* Stats in bytes */
	unsigned mem_free = vm_stat.free_count * pagesize;
	return mem_free;
}
	
static bool _gDidReceiveLowMemoryWarning = false;

void SetReceivedLowMemoryWarning(bool value)
{
	if (value == true)
	{
	    printf("** LOW MEMORY WARNING ** (%u bytes available)\n", GetFreeMemory());
        sleep(0); // give the scheduler time to do other stuff
	}
	_gDidReceiveLowMemoryWarning = value;
}
	
bool ReceivedLowMemoryWarning()
{
    return _gDidReceiveLowMemoryWarning;
}
	
bool FreeMemory(int threshold)
{
	enum { MAX_MBS = 256, CHUNK_SIZE = 1024 * 1024 }; // 1 MB at a time
	if (threshold >= MAX_MBS * CHUNK_SIZE)
	{
        printf("FreeMemory: requested too much memory!\n");			
		return false; // if you're asking for more than the max just shut up
	}
    // $TODO remove this line before Apple reviews it!!	
	printf("FreeMemory: attempting to free memory\n");	
	int original = (int)GetFreeMemory();
	if (original >= threshold)
	{
        // $TODO remove this line before Apple reviews it!!
		printf("FreeMemory: %d bytes already available (%d requested), freeing unnecessary\n", original, threshold);
		return true;
	}
	void *chunks[MAX_MBS];
	int   chunkCount = 0;
	
	while (!ReceivedLowMemoryWarning() && chunkCount < MAX_MBS)
	{
		chunks[chunkCount] = malloc(CHUNK_SIZE);
		if (!chunks[chunkCount])
			break;
		printf("FreeMemory: pointer returned %p\n", chunks[chunkCount]);
		try {
		if (chunkCount - 1 >= 0)
   	        memset(chunks[chunkCount - 1], chunkCount, CHUNK_SIZE); // fill the previous one with something
		}
		catch (...) { break; }
		sleep(0); // give the scheduler time to do other stuff
		chunkCount++;
	}
	// k, back a couple meg out, so the OS has wiggle room
	for (int i = 0; i < 2; i++)
	{
		free(chunks[--chunkCount]);
	}
	// wait up to n seconds
	for (int i = 0; i < 10 && chunkCount < MAX_MBS && !ReceivedLowMemoryWarning(); i++)
		sleep(1); // wait for the inevitable low memory warning
	// $TODO remove this line before Apple reviews it!!
	printf("FreeMemory: %d bytes allocated and about to be freed\n", chunkCount * CHUNK_SIZE);
	for (int i = 0; i < chunkCount; i++)
		free(chunks[i]);
	SetReceivedLowMemoryWarning(false); // ok, understood
	int now = (int)GetFreeMemory();
	// $TODO remove this line before Apple reviews it!!
	printf("FreeMemory: freed %d bytes\n", now - original);
	return now >= threshold;
}
		

}
