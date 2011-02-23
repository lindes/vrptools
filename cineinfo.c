#include <stdio.h>

#include "vrptools.h"

int main(int argc, char *argv[])
{
    int i;

    for(i = 1; i < argc; ++i)
    {
        VRP_Handle handle;

        printf("--=> %s <=--\n", argv[i]);

        if(!(handle = read_cine(argv[i])))
        {
            fprintf(stderr, "Failed to get handle on %s\n", argv[i]);
            continue;
        }
        printf("Name: %s\n", handle->name);
        /* printf("%s: %p\n", argv[i], handle); */
        /* printf("header: %p\n", handle->header); */
        /* printf("mmapped_region: %p\n", handle->mmapped_region); */
        /* printf("Type: %p\n", &(handle->header->Type)); */
        printf("Type: %2.2s\n", (const char *)&(handle->header->Type));
        printf("Headersize: %d\n", handle->header->Headersize);

        free_cine_handle(handle);
    }

    return(0);
}
