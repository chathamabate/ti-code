// #define HAS_PRINTF YES

#include <tice.h>
#include <graphx.h>

#include <stdio.h>
#include <stdlib.h>
#include <fileioc.h>

typedef struct data_cell {
    uint8_t x;
    uint8_t y;
} dc;

void bgc(void) {
    os_PutStrFull("Collecting Garbage!");
    os_NewLine();
}

void agc(void) {
    os_PutStrFull("GC Complete!");
    os_NewLine();
}

/* Main function, called first */
int main(void)
{
    os_ClrHome();

    ti_SetGCBehavior(bgc, agc);

    // First can we store and read variables???
    ti_var_t data = ti_Open("TestData", "w");

    if (data == 0) {
        os_PutStrFull("Can't Create Data!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    dc points[3];
    uint8_t i;

    for (i = 0; i < 3; i++) {
        points[i].x = i;
        points[i].y = 2 - i;
    }

    size_t written = ti_Write(points, sizeof(dc), 3, data);

    if (written != 3) {
        os_PutStrFull("Did not write all chunks!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    os_PutStrFull("Data Written!");
    os_NewLine();

    // Move data to the Archive.
    if (ti_SetArchiveStatus(1, data)) {
        os_PutStrFull("Data Archived!");
    } else {
        os_PutStrFull("Data Archive Failed!");
    }
    // void ti_SetGCBehavior(void (*before)(void), void (*after)(void));

    os_NewLine();

    if (!ti_Close(data)) {
        os_PutStrFull("Failed To Close Data!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    // Now attempt to read it from the archive...
    // Moves it to RAM... w and a+, not r tho apparently...
    data = ti_Open("TestData", "r+"); // This is important!!!!
    // This moves data out of archive into the RAM.
    // It must be returned to Archive after editing!!!

    if (data == 0) {
        os_PutStrFull("Failed to unarchive!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    dc read_points[3];
    if (ti_Read(read_points, sizeof(dc), 3, data) != 3) {
        os_PutStrFull("Failed to read Data!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    ti_Close(data);

    char pnt_strs[20];

    sprintf(pnt_strs, "(%d, %d) (%d, %d) (%d, %d)", 
        read_points[0].x, read_points[0].y,
        read_points[1].x, read_points[1].y,
        read_points[2].x, read_points[2].y
    );

    os_PutStrFull(pnt_strs);
    os_NewLine();

    if (ti_Rename("TestData-TEMP", "TestData")) {
        os_PutStrFull("Failed to rename!");
        while (os_GetCSC() != sk_Clear);
        return 1;
    }

    while (os_GetCSC() != sk_Clear);
    return 0;

    /* Clear the homescreen */
    // os_ClrHome();

    // char buff[10];
    // sprintf(buff, "%d", sizeof(int)); // ints are 3 bytes because of ez80 architecture!

    // os_PutStrLine(buff);

    // gfx_Begin();

    // /* Set the text colors */
    // gfx_SetTextFGColor(6);
    // gfx_SetTextBGColor(255);

    // /* Print some scaled text */
    // gfx_SetTextScale(4, 4);

    // test_run();


    // /* Waits for a key */
    // while (!os_GetCSC());

    // /* End graphics drawing */
    // gfx_End();

    return 0;
}
