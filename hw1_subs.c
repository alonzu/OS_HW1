#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int replaceWords(char *bigBuffer, char *firstWord, char *secondWord, int index) {
    int lengthFirstWord, sameWordFlag, i, k;
    lengthFirstWord = strlen(firstWord); /* we assume lengthFirstWord > 0 */
    sameWordFlag = 1;
    for (i = index; i < lengthFirstWord + 1; i++) {
        for (int j = i; j < i + lengthFirstWord; j++) {
            if (bigBuffer[j] != firstWord[j - i]) {
                sameWordFlag = 0;
            }
        }
        if (sameWordFlag == 1) {
            for (k = index; k < i; k++) {
                if (bigBuffer[k] != '\0') {
                    printf("%c", bigBuffer[k]);
                }
            }
            printf("%s", secondWord);
            return (i);
        }
        sameWordFlag = 1;
    }
    for (i = index; i < lengthFirstWord; i++) {
        if (bigBuffer[i] != '\0') {
            printf("%c", bigBuffer[i]);
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    char *firstWord, *secondWord;
    int filePathLength, lengthOfEachBuffer, a, b, index, fp, bytesRead;
    char const *HW1DIR = getenv("HW1DIR");
    char const *HW1TF = getenv("HW1TF");
    index = 0;
    bytesRead = 0;
    if (HW1DIR == NULL || HW1TF == NULL) {
        printf("Error! HW1DIR or HW1TF were not defined");
        exit(EXIT_FAILURE);
    }
    filePathLength = strlen(HW1DIR) + strlen(HW1TF) + 1;
    char *filePath = (char *) calloc(filePathLength + 5, sizeof(char));
    if (filePath == NULL) {
        printf("Error! filePath malloc failed");
        exit(EXIT_FAILURE);
    }
    strcpy(filePath,"");
    strcat(filePath, HW1DIR);
    strcat(filePath, "/");
    strcat(filePath, HW1TF);
    fp = open(filePath, O_RDONLY);
    if(fp < 0)
    {
        printf("Error opening file: %s\n", strerror(errno));
        free(filePath);
        exit(EXIT_FAILURE);
    }
    if (argc != 2 && argc != 3) {
        printf("Error! missing arguments or too many arguments");
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    }
    firstWord = argv[1];
    if (argc == 3) {
        secondWord = argv[2];
    } else {
        secondWord = "";
    }
    lengthOfEachBuffer = strlen(firstWord);
    char *buffer1 = (char *) calloc(lengthOfEachBuffer+5,sizeof(char));
    if (buffer1 == NULL) {
        printf("Error! buffer1 malloc failed");
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < lengthOfEachBuffer; i++) {
            buffer1[i] = '\0';
        }
    }
    char *buffer2 = (char *) calloc(lengthOfEachBuffer+ 5, sizeof(char) );
    if (buffer2 == NULL) {
        printf("Error! buffer2 malloc failed");
        free(buffer1);
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < lengthOfEachBuffer; i++) {
            buffer2[i] = '\0';
        }
    }
    char *bigBuffer = (char *) calloc(2 * lengthOfEachBuffer + 5, sizeof(char) );
    if (bigBuffer == NULL) {
        printf("Error! bigBuffer malloc failed");
        free(buffer1);
        free(buffer2);
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    } else {
        for (int i = 0; i < 2*lengthOfEachBuffer; i++) {
            bigBuffer[i] = '\0';
        }
    }
    a = read(fp, buffer1, lengthOfEachBuffer);
    if (a < 0) {
        printf("Error reading from file: %s\n", strerror(errno));
        free(buffer1);
        free(buffer2);
        free(bigBuffer);
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    }
    while (a < lengthOfEachBuffer) {
        bytesRead = read(fp, buffer1+a, lengthOfEachBuffer-a);
        if (bytesRead < 0) {
            printf("Error reading from file: %s\n", strerror(errno));
            free(buffer1);
            free(buffer2);
            free(bigBuffer);
            free(filePath);
            close(fp);
            exit(EXIT_FAILURE);
        } else if (bytesRead > 0) {
            a = a + bytesRead;
            bytesRead = 0;
        } else {
            break;
        }
    }
    b = read(fp, buffer2, lengthOfEachBuffer);
    if (b < 0) {
        printf("Error reading from file: %s\n", strerror(errno));
        free(buffer1);
        free(buffer2);
        free(bigBuffer);
        free(filePath);
        close(fp);
        exit(EXIT_FAILURE);
    }
    while (b < lengthOfEachBuffer) {
        bytesRead = read(fp, buffer2+b, lengthOfEachBuffer-b);
        if (bytesRead < 0) {
            printf("Error reading from file: %s\n", strerror(errno));
            free(buffer1);
            free(buffer2);
            free(bigBuffer);
            free(filePath);
            close(fp);
            exit(EXIT_FAILURE);
        } else if (bytesRead > 0) {
            b = b + bytesRead;
            bytesRead = 0;
        } else {
            break;
        }
    }
    while (1) {
        if (a <= 0 && b <= 0) {
            break;
        }
        if (a > 0) {
            for (int i = 0; i < 2*lengthOfEachBuffer; i++) {
                bigBuffer[i] = '\0';
            }
            strcpy(bigBuffer,"");
            for (int i = 0; i < lengthOfEachBuffer; i++) { /* copies the data from buffer1 to bigBuffer */
                bigBuffer[i] = buffer1[i];
            }
        }
        if (b > 0) {
            strcat(bigBuffer, buffer2);
        }
        index = replaceWords(bigBuffer, firstWord, secondWord, index);
        for (int i = 0; i < lengthOfEachBuffer; i++) {
            buffer1[i] = '\0';
        }
        for (int i = 0; i < lengthOfEachBuffer; i++) { /* copies the data from buffer2 to buffer1*/
            buffer1[i] = buffer2[i];
        }
        for (int i = 0; i < lengthOfEachBuffer; i++) {
            buffer2[i] = '\0';
        }
        a = b;
        b = read(fp, buffer2, lengthOfEachBuffer);
        if (b < 0) {
            printf("Error reading from file: %s\n", strerror(errno));
            free(buffer1);
            free(buffer2);
            free(bigBuffer);
            free(filePath);
            close(fp);
            exit(EXIT_FAILURE);
        }
        while (b < lengthOfEachBuffer) {
            bytesRead = read(fp, buffer2+b, lengthOfEachBuffer-b);
            if (bytesRead < 0) {
                printf("Error reading from file: %s\n", strerror(errno));
                free(buffer1);
                free(buffer2);
                free(bigBuffer);
                free(filePath);
                close(fp);
                exit(EXIT_FAILURE);
            } else if (bytesRead > 0) {
                b = b + bytesRead;
                bytesRead = 0;
            } else {
                break;
            }
        }
    }
    free(buffer1);
    free(buffer2);
    free(bigBuffer);
    free(filePath);
    close(fp);
    return 0;
}
