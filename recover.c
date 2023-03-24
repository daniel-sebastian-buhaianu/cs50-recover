#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BLOCK_SIZE 512
#define OUTFILE_NAME_LEN 7

typedef uint8_t BYTE;

void set_filename(short num, char file[OUTFILE_NAME_LEN]);

int main(int argc, char *argv[])
{
	// Ensure proper usage
	if (argc != 2)
	{
		printf("Usage: ./recover IMAGE\n");
		return 1;
	}

	// Remember filenames
	char *infile = argv[1];
	char outfile[OUTFILE_NAME_LEN];

	// Open input file
	FILE *inptr = fopen(infile, "r");
	if (inptr == NULL)
	{
		printf("Could not open %s.\n", infile);
		return 2;
	}

	// Alocate memory for buffer
	BYTE *buffer = malloc(BLOCK_SIZE * sizeof(BYTE));


	short images_count = -1;

	// Initialise output file pointer
	FILE *outptr = NULL;

	// Read each block of 512 bytes into buffer
	while (fread(buffer, sizeof(BYTE), BLOCK_SIZE, inptr) == BLOCK_SIZE)
	{
		if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff)
		{
			// check if buffer[3]'s first four bits are 1110 (JPEG signature)
			if ( (buffer[3] & (1<<5)) && (buffer[3] & (1<<6)) && (buffer[3] & (1<<7)) && !(buffer[3] & (1<<4)) )
			{
				// close last .jpg file (if exists)
				if (outptr != NULL)
				{
					fclose(outptr);
				}

				// found a valid image, increase count
				images_count++;

				// set outfile name to "###.jpg" where ### = images_count
				set_filename(images_count, outfile);

				// open output file
				outptr = fopen(outfile, "w");
				if (outptr == NULL)
				{
					fclose(inptr);
					printf("Could not create %s.\n", outfile);
					return 3;
				}

				// write bytes to .jpg file
				fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, outptr);
			}
			else
			{
				if (outptr != NULL)
				{
					// write bytes to .jpg file
					fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, outptr);
				}
				else
				{
					// reset to initial value
					outptr = NULL;
				}
			}
		}
		else
		{
			if (outptr != NULL)
			{
				// write bytes to .jpg file
				fwrite(buffer, sizeof(BYTE), BLOCK_SIZE, outptr);
			}
			else
			{
				// reset to initial value
				outptr = NULL;
			}
		}
	}

	// free memory for buffer
	free(buffer);

	// close infile
	fclose(inptr);

	// success
	return 0;
}

// Set file name to "###.jpg" where ### == num (0-999)
void set_filename(short num, char file[OUTFILE_NAME_LEN])
{
	if (num >= 0 && num < 10)
	{
		sprintf(file, "00%hi.jpg", num);
	}
	else if (num >= 10 && num < 100)
	{
		sprintf(file, "0%hi.jpg", num);
	}
	else if (num >= 100 && num < 1000)
	{
		sprintf(file, "%hi.jpg", num);
	}
}
