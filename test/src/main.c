// #define HAS_PRINTF YES

#include <tice.h>
#include <graphx.h>

#include <stdio.h>
#include <stdlib.h>
#include <fileioc.h>

#define HALT(c) \
    os_PutStrFull(c); \
    while (os_GetCSC() != sk_Clear); \
    return 1;

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

    if(!ti_Delete("MyDataTemp")) {
        os_PutStrFull("Could not delete!");
        os_NewLine();
    }

    // First can we store and read variables???
    ti_var_t data = ti_Open("MyData", "w");

    if (data == 0) {
        HALT("Can't Create Data!");
    }

    dc points[3];
    uint8_t i;

    for (i = 0; i < 3; i++) {
        points[i].x = i;
        points[i].y = 2 - i;
    }

    size_t written = ti_Write(points, sizeof(dc), 3, data);

    if (written != 3) {
        HALT("Did not write all chunks!");
    }

    os_PutStrFull("Data Written!");
    os_NewLine();

    // Move data to the Archive. (MUST BE DONE WHILE DATA IS OPEN!)
    if (ti_SetArchiveStatus(1, data)) {
        os_PutStrFull("Data Archived!");
    } else {
        os_PutStrFull("Data Archive Failed!");
    }
    os_NewLine();

    if (!ti_Close(data)) {
        HALT("Failed To Close Data!");
    }

    uint8_t code = ti_Rename("MyData", "MyDataTemp");

    if (code == 1) {
        HALT("Rename: Already Exists!");
    } else if (code == 2) {
        HALT("Rename: Other error!");
    }

    // Renaming works !!!!! 
    // EVEN FOR ARCHIVED FILES!!!!!
    os_PutStrFull("Rename Success!");
    os_NewLine();

    // // Now attempt to read it from the archive...
    // // Moves it to RAM... w and a+, not r tho apparently...
    // data = ti_Open("TestData", "r+"); // This is important!!!!
    // // This moves data out of archive into the RAM.
    // // It must be returned to Archive after editing!!!

    // if (data == 0) {
    //     os_PutStrFull("Failed to unarchive!");
    //     while (os_GetCSC() != sk_Clear);
    //     return 1;
    // }

    // dc read_points[3];
    // if (ti_Read(read_points, sizeof(dc), 3, data) != 3) {
    //     os_PutStrFull("Failed to read Data!");
    //     while (os_GetCSC() != sk_Clear);
    //     return 1;
    // }

    // ti_Close(data);

    // char pnt_strs[20];

    // sprintf(pnt_strs, "(%d, %d) (%d, %d) (%d, %d)", 
    //     read_points[0].x, read_points[0].y,
    //     read_points[1].x, read_points[1].y,
    //     read_points[2].x, read_points[2].y
    // );

    // os_PutStrFull(pnt_strs);
    // os_NewLine();

    // if (ti_Rename("TestData-TEMP", "TestData")) {
    //     os_PutStrFull("Failed to rename!");
    //     while (os_GetCSC() != sk_Clear);
    //     return 1;
    // }

    while (os_GetCSC() != sk_Clear);

    return 0;
}
