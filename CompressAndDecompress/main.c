#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#include "getopt.h"
#include "deflate.h"

#define MIN_LA_SIZE 2       /* min lookahead size */
#define MAX_LA_SIZE 255     /* max lookahead size */
#define MIN_SB_SIZE 0       /* min search buffer size */
#define MAX_SB_SIZE 65535   /* max search buffer size */

#define windowSize 60
#define bufferSize 40
#define arraySize bufferSize + windowSize


int findMatch(unsigned char window[], unsigned char str[], int strLen) {
    int j, k, pos = -1;

    for (int i = 0; i <= windowSize - strLen; i++) {
        pos = k = i;

        for (j = 0; j < strLen; j++) {
            if (str[j] == window[k])
                k++;
            else
                break;
        }
        if (j == strLen)
            return pos;
    }

    return -1;
}


int lz77_compress(char* inputPath) {
    FILE *fileInput;
    FILE *fileOutput;
    bool last = false;
    int inputLength = 0;
    int outputLength = 0;
    int endOffset = 0;
    int pos = -1;
    int i, size, shift, c_in;
    size_t bytesRead = (size_t) -1;
    unsigned char c;
    unsigned char array[arraySize];
    unsigned char window[windowSize];
    unsigned char buffer[bufferSize];
    unsigned char loadBuffer[bufferSize];
    unsigned char str[bufferSize];

    // Open I/O files
    char path[30] = "input/";
    strcat(path, inputPath);
    fileInput = fopen(inputPath, "rb");
    fileOutput = fopen("lz77_compressed.txt", "wb");

    // If unable to open file, return alert
    if (!fileInput) {
        fprintf(stderr, "Unable to open fileInput %s", inputPath);
        return 0;
    }

    // Get fileInput length
    fseek(fileInput, 0, SEEK_END);
    inputLength = ftell(fileInput);
    fseek(fileInput, 0, SEEK_SET);


    // If file is empty, return alert
    if (inputLength == 0)
        return 3;

    // If file length is smaller than arraySize, not worth processing
    if (inputLength < arraySize)
        return 2;

    // Load array with initial bytes
    fread(array, 1, arraySize, fileInput);

    // Write the first bytes to output file
    fwrite(array, 1, windowSize, fileOutput);

    // LZ77 logic beginning
    while (true) {
        if ((c_in = fgetc(fileInput)) == EOF)
            last = true;
        else
            c = (unsigned char) c_in;

        // Load window (dictionary)
        for (int k = 0; k < windowSize; k++)
            window[k] = array[k];

        // Load buffer (lookahead)
        for (int k = windowSize, j = 0; k < arraySize; k++, j++) {
            buffer[j] = array[k];
            str[j] = array[k];
        }

        // Search for longest match in window
        if (endOffset != 0) {
            size = bufferSize - endOffset;
            if (endOffset == bufferSize)
                break;
        }
        else {
            size = bufferSize;
        }

        pos = -1;
        for (i = size; i > 0; i--) {
            pos = findMatch(window, str, i);
            if (pos != -1)
                break;
        }

        // No match found
        // Write only one byte instead of two
        // 255 -> offset = 0, match = 0
        if (pos == -1) {
            fputc(255, fileOutput);
            fputc(buffer[0], fileOutput);
            shift = 1;
        }
        // Found match
        // offset = windowSize - position of match
        // i = number of match bytes
        // endOffset = number of bytes in lookahead buffer not to be considered (EOF)
        else {
            fputc(windowSize - pos, fileOutput);
            fputc(i, fileOutput);
            if (i == bufferSize) {
                shift = bufferSize + 1;
                if (!last)
                    fputc(c, fileOutput);
                else
                    endOffset = 1;
            }
            else {
                if (i + endOffset != bufferSize)
                    fputc(buffer[i], fileOutput);
                else
                    break;
                shift = i + 1;
            }
        }

        // Shift buffers
        for (int j = 0; j < arraySize - shift; j++)
            array[j] = array[j + shift];
        if (!last)
            array[arraySize - shift] = c;

        if (shift == 1 && last)
            endOffset++;

        // If (shift != 1) -> read more bytes from file
        if (shift != 1) {
            // Load loadBuffer with new bytes
            bytesRead = fread(loadBuffer, 1, (size_t) shift - 1, fileInput);

            // Load array with new bytes
            // Shift bytes in array, then splitted into window[] and buffer[] during next iteration
            for (int k = 0, l = arraySize - shift + 1; k < shift - 1; k++, l++)
                array[l] = loadBuffer[k];

            if (last) {
                endOffset += shift;
                continue;
            }

            if (bytesRead < shift - 1)
                endOffset = shift - 1 - bytesRead;
        }
    }

    // Get fileOutput length
    fseek(fileOutput, 0, SEEK_END);
    outputLength = ftell(fileOutput);
    fseek(fileOutput, 0, SEEK_SET);

    fprintf(stdout, "\nSize of the file compressed by lz77 is: %d bytes\n", outputLength);

    // Close I/O files
    fclose(fileInput);
    fclose(fileOutput);

    return 1;
}


int lz77_decompress() {
    FILE *fileInput;
    FILE *fileOutput;
    int shift, offset, match, c_in;
    bool done = false;
    unsigned char c;
    unsigned char window[windowSize];
    unsigned char writeBuffer[windowSize];
    unsigned char readBuffer[2];

    // Open I/O files
    fileInput = fopen("lz77_compressed.txt", "rb");
    fileOutput = fopen("lz77_decompressed.txt", "wb");

    if (!fileInput) {
        fprintf(stderr, "Unable to open fileInput %s", "output.lz77");
        return 0;
    }

    // Load array with initial bytes and write to file
    fread(window, 1, windowSize, fileInput);
    fwrite(window, 1, windowSize, fileOutput);

    // Inverse algorithm beginning
    while (true) {
        // Read file by couples/triads to reconstruct original file
        size_t bytesRead = fread(readBuffer, 1, 2, fileInput);

        if (bytesRead >= 2) {
            offset = (int) readBuffer[0];
            match = (int) readBuffer[1];

            // If first byte of readBuffer is 255 -> offset = 0, match = 0
            if (offset == 255) {
                offset = 0;
                c = (unsigned char) match;
                match = 0;
                shift = match + 1;
            }
            else {
                shift = match + 1;
                c_in = fgetc(fileInput);
                if (c_in == EOF)
                    done = true;
                else
                    c = (unsigned char) c_in;
            }

            // Load and write occurrence to file
            for (int i = 0, j = windowSize - offset; i < match; i++, j++)
                writeBuffer[i] = window[j];
            fwrite(writeBuffer, 1, (size_t) match, fileOutput);

            if (!done)
                fputc(c, fileOutput);

            // Shift window
            for (int i = 0; i < windowSize - shift; i++)
                window[i] = window[i + shift];

            for (int i = 0, j = windowSize - shift; i < match; i++, j++)
                window[j] = writeBuffer[i];
            window[windowSize - 1] = c;
        }
        else {
            break;
        }
    }

    // Close I/O files
    fclose(fileInput);
    fclose(fileOutput);

    return 1;
}

long int findSize(char file_name[]) {
    FILE* fp = fopen(file_name, "r");

    fseek(fp, 0L, SEEK_END);

    long int res = ftell(fp);

    fclose(fp);
    return res;
}


int main(int argc, char *argv[])
{

    FILE *file = NULL;
    struct bitFILE *bitF = NULL;
    int la_size = -1, sb_size = -1; /* default size */
    char inputFile[25];



    printf("LZ77 and DEFLATE compression\n");
    printf("Operations: \n");
    printf("1. LZ77\DEFLATE ENCODE (compress)\n");
    printf("2. LZ77\DEFLATE DECODE (decompress)\n");
    int userChoose;
    printf("Enter your choose: ");
    scanf("%d", &userChoose);



    switch(userChoose){
        case 1:
            printf("Enter a filename:");
            scanf("%s", inputFile);
            char file_name[] = { inputFile };
            long int res = findSize(file_name);
            printf("Size of the %s is %ld bytes \n",inputFile, res);



            //gets(inputFile);

            //LZ77 encoding operation!
            lz77_compress(inputFile);

            //DEFLATE encoding operation!
            Deflate_Params encode_params;

            //speed of encoding operation.
            //Default parameter is "false".
            encode_params.fast = false;
            encode_params.in_file_name = inputFile;
            encode_params.out_file_name = "deflate_compressed.txt";

            Deflate_encode(&encode_params);
            char deflateC_file_name[] = { "deflate_compressed.txt" };
            long int deflateC_res = findSize(deflateC_file_name);
            printf("Size of the file compressed by deflate is: %ld bytes \n", deflateC_res);

            printf("Encoding operation done!");
            break;

        case 2:
            //LZ77 decoding operation!
            lz77_decompress();
            char lz77D_file_name[] = { "lz77_decompressed.txt" };
            long int lz77D_res = findSize(lz77D_file_name);
            printf("Size of the decompressed file by lz77 is: %ld bytes \n", lz77D_res);

            //DEFLATE decoding operation!
            Deflate_Params decode_params;


            //speed of decoding operation.
            //Default parameter is "false".
            decode_params.fast = false;
            decode_params.in_file_name = "deflate_compressed.txt";
            decode_params.out_file_name = "deflate_decompressed.txt";

            Deflate_decode(&decode_params);

            char deflateD_file_name[] = { "deflate_decompressed.txt" };
            long int deflateD_res = findSize(deflateD_file_name);
            printf("Size of the decompressed file by deflate is: %ld bytes \n", deflateD_res);

            printf("Decoding operation done!");
            break;
    }

    return 0;
}






