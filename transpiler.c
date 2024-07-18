#include "transpiler.h"
#include "socket.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char field_map[7][16] = {"Volume", "TC Volume", "Ullage", "Height", "Water", "Temperature", "Water Volume"};

typedef union {
    float f;
    unsigned int h;
} hexfloat;

// extracts date header to string ("hh:mm yy/mm/dd") with null terminator
void extract_header_date(char in[RECV_BUF_SIZE], char out[15]) {
    snprintf(out, 15, "%c%c:%c%c %c%c/%c%c/%c%c", in[13], in[14], in[15], in[16], in[7], in[8], in[9], in[10], in[11], in[12]);
}

void transpile(char in[RECV_BUF_SIZE], char inst[8]) {
    FILE* efile; // export file
    int index = 0; // used to keep track of whereabouts in atg response

    // confirm instruction matches
    if(strncmp(in, inst, 7) != 0) {
         printf("Instruction mismatch during transpiling\n");
	 exit(1);
    }


    efile = fopen("intank.md", "w");
    if(efile == NULL) {
        printf("Error opening export file\n");
	exit(1);
    }

    fprintf(efile, "# In-Tank Delivery Report\n\n");

    char date[15];
    extract_header_date(in, date);
    fprintf(efile, "##### Local date/time **%s**\n\n", date);
    // printf("Received date: %s\n", date);
    index += 17;

    fprintf(efile, "| Tank No. | Product Code | Volume | TC Volume | Ullage | Height | Water | Temperature | Water Volume |\n");
    fprintf(efile, "| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |\n");

    // loop until termination char
    while(in[index + 1] != '&') {
	fprintf(efile, "| "); // initial border, note all fields will finish with a border

        char tank_no_buf[3];
        snprintf(tank_no_buf, sizeof(tank_no_buf), "%c%c", in[index], in[index + 1]);
        int tank_no = atoi(tank_no_buf); // could be represented as char (probably won't more than a hundred tanks at petrol station)
        // printf("Tank no: %d\n", tank_no);
	fprintf(efile, "%d |", tank_no);
        index += 2;

        int product_code = in[index] - '0';
        // printf("Product code: %d\n", product_code);
	fprintf(efile, " %d |", product_code);
        index++;

        index += 4; // ignore status bits

	// grab number of fields in tank data (max 7)
        char field_iter_buf[3];
        snprintf(field_iter_buf, sizeof(field_iter_buf), "%c%c", in[index], in[index + 1]);
        int field_iter = atoi(field_iter_buf);
        // printf("%d fields:\n", field_iter);
        index += 2;

	// loop through tank data fields and append the info
        while(field_iter > 0) {
            char field_buf[9];
            snprintf(field_buf, sizeof(field_buf), "%c%c%c%c%c%c%c%c", in[index], in[index + 1], in[index + 2], in[index + 3], in[index + 4], in[index + 5], in[index + 6], in[index + 7]);
	    int number = (int) strtol(field_buf, NULL, 16);
	    hexfloat hf;
	    hf.h = number;
            // printf("-> %s is %lg\n", field_map[7 - field_iter], field_buf, hf.f);
	    fprintf(efile, " %lg |", hf.f);

            index += 8;
            field_iter--;
        }

	// printf("Tank data finished\n");
	fprintf(efile, "\n");
    }

    fclose(efile);
}
