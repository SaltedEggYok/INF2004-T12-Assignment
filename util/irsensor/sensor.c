#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
// Pico libraries
#include "pico/types.h"
#include "pico/stdlib.h"
#include "pico/stdio.h"

#define GPIO_PIN_INFRARED_IN1 26

volatile bool infraFlag = true;
volatile bool readyToStartBarcodeState = false;
volatile int arrayVar = 0;
volatile int charStartEndCheck[9];
volatile int timeChanges[9];
volatile int startReading = 0;
int varcharStartEndStar[9]; 
volatile int countStar = 0; 
volatile int compareQ = 0; 
char varCharASCII = '~';      
char finalString[22];          
char sanitizedFinalString[20]; 
const int TIMEOUT = 1200;      
bool arrayStarNotMatch = true; 

// Mapping
const int charStartEndStar[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0};
const int charSpace[9] = {0, 1, 1, 0, 0, 0, 1, 0, 0};
const int letterA[9] = {1, 0, 0, 0, 0, 1, 0, 0, 1};
const int letterB[9] = {0, 0, 1, 0, 0, 1, 0, 0, 1};
const int letterC[9] = {1, 0, 1, 0, 0, 1, 0, 0, 0};
const int letterD[9] = {0, 0, 0, 0, 1, 1, 0, 0, 1};
const int letterE[9] = {1, 0, 0, 0, 1, 1, 0, 0, 0};
const int letterF[9] = {0, 0, 1, 0, 1, 1, 0, 0, 0};
const int letterG[9] = {0, 0, 0, 0, 0, 1, 1, 0, 1};
const int letterH[9] = {1, 0, 0, 0, 0, 1, 1, 0, 0};
const int letterI[9] = {0, 0, 1, 0, 0, 1, 1, 0, 0};
const int letterJ[9] = {0, 0, 0, 0, 1, 1, 1, 0, 0};
const int letterK[9] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
const int letterL[9] = {0, 0, 1, 0, 0, 0, 0, 1, 1};
const int letterM[9] = {1, 0, 1, 0, 0, 0, 0, 1, 0};
const int letterN[9] = {0, 0, 0, 0, 1, 0, 0, 1, 1};
const int letterO[9] = {1, 0, 0, 0, 1, 0, 0, 1, 0};
const int letterP[9] = {0, 0, 1, 0, 1, 0, 0, 1, 0};
const int letterQ[9] = {0, 0, 0, 0, 0, 0, 1, 1, 1};
const int letterR[9] = {1, 0, 0, 0, 0, 0, 1, 1, 0};
const int letterS[9] = {0, 0, 1, 0, 0, 0, 1, 1, 0};
const int letterT[9] = {0, 0, 0, 0, 1, 0, 1, 1, 0};
const int letterU[9] = {1, 1, 0, 0, 0, 0, 0, 0, 1};
const int letterV[9] = {0, 1, 1, 0, 0, 0, 0, 0, 1};
const int letterW[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
const int letterX[9] = {0, 1, 0, 0, 1, 0, 0, 0, 1};
const int letterY[9] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
const int letterZ[9] = {0, 1, 1, 0, 1, 0, 0, 0, 0};

int compareArray(int a[], const int b[])
{
    int i;
    for (i = 0; i < 9; i++)
    {
        if (a[i] != b[i])
            return 1; // If array not equal, return 1
    }
    return 0; // If array equal, return 0
}

void INFRARED_init(void)
{
    printf("[Encoder] Init start \n");

    // Initialization
    gpio_init(GPIO_PIN_INFRARED_IN1);
    gpio_set_dir(GPIO_PIN_INFRARED_IN1, GPIO_IN); // S1 in

    printf("[Encoder] Init done \n");
}

int readyToStartBarcode()
{
    if (gpio_get(GPIO_PIN_INFRARED_IN1) == 0)
    {
        readyToStartBarcodeState = true;
        return 1; // For integration into main
    };
}
void INFRARED_scanning()
{
    if (gpio_get(GPIO_PIN_INFRARED_IN1) == 1) // BLACK BAR
    {
        if (infraFlag == true)
        {
            infraFlag = false;
            sleep_us(100);             
            charStartEndCheck[arrayVar] = time_us_32() / 1000; 
            //printf("Black bar\n");
            arrayVar += 1;                                  
        }
    }
    else
    {
        if (infraFlag == false)
        {
            infraFlag = true;
            sleep_us(100); 
            charStartEndCheck[arrayVar] = time_us_32() / 1000; 
            //printf("White bar");
            arrayVar += 1;
        }
    }
}

bool INFRARED_readyToReturnChar()
{
    if (countStar >= 2)
    {
        // Return star to not found
        return true;
    }
    else
        return false;
}
const char *INFRARED_returnChar()
{
    int tempCount = 0;
    int tempCountStar = 0;
    for (int i = 0; finalString[i] != '\0'; ++i)
    {
        if (finalString[i] != '*')
        {
            finalString[tempCount] = finalString[i];
            tempCount++;
        }
        if (finalString[i] == '*')
        {
            tempCountStar++;
        }
        if (tempCountStar == 2)
        {
            finalString[tempCount] = '\0';
        }
    }
    return finalString;
}
void INFRARED_resetForNewString()
{
    countStar = 0;                    // Reset count star
    varCharASCII = '~';               // Reset variable
    arrayStarNotMatch = true;         // Set back star as not found
    strcpy(finalString, "");          // Clear contents of string
    strcpy(sanitizedFinalString, ""); // Clear contents of cleaned string
    readyToStartBarcodeState = false; // Exit barcode state
}

void INFRARED_decodeCharTree() {
    const int* patterns[] = { letterA, letterB, letterC, letterD, letterE, letterF, letterG,
                              letterH, letterI, letterJ, letterK, letterL, letterM, letterN,
                              letterO, letterP, letterQ, letterR, letterS, letterT, letterU,
                              letterV, letterW, letterX, letterY, letterZ};
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int numPatterns = sizeof(patterns) / sizeof(patterns[0]);

    varCharASCII = '\0'; // Reset the current character

    for (int i = 0; i < numPatterns; ++i) {
        if (compareArray(varcharStartEndStar, patterns[i]) == 0) {
            varCharASCII = chars[i];
            break; // Break the loop once a match is found
        }
    }
    if (varCharASCII != '\0' && !(varCharASCII == '*' && arrayStarNotMatch)) {
        strncat(finalString, &varCharASCII, 1);
    }

    arrayStarNotMatch = (varCharASCII == '*');
}

bool INFRARED_oneCharRead()
{
    return (arrayVar == 10);
}
void INFRARED_sortingTimings()
{
    for (int i = 0; i < 9; i++)
    {
        timeChanges[i] = charStartEndCheck[i + 1] - charStartEndCheck[i];
        if (timeChanges[i] > TIMEOUT)
        {
            INFRARED_resetForNewString();
            break;
        }
    }

    int h1 = INT32_MIN;
    int h2 = INT32_MIN; 
    int h3 = INT32_MIN;
    int h1i, h2i, h3i = 0;
    for (int i = 0; i < 9; i++)
    {
        if (timeChanges[i] > h1)
        {
            h3i = h2i;
            h2i = h1i;
            h1i = i;

            h3 = h2;
            h2 = h1;
            h1 = timeChanges[i];
        }
        else if (timeChanges[i] > h2)
        {
            h3i = h2i;
            h2i = i;

            h3 = h2;
            h2 = timeChanges[i];
        }
        else if (timeChanges[i] > h3)
        {
            h3i = i;
            h3 = timeChanges[i];
        }
    }
    for (int i = 0; i < 9; i++)
    {
        if (i % 2)
        {
            if (i == h1i || i == h2i || i == h3i)
            {
                varcharStartEndStar[i] = 1;
            }
            else
            {
                varcharStartEndStar[i] = 0;
            }
        }
        else
        {
            if (i == h1i || i == h2i || i == h3i)
            {
                varcharStartEndStar[i] = 1;
            }
            else
            {
                varcharStartEndStar[i] = 0;
            }
        }
    }
    if (compareArray(varcharStartEndStar, charStartEndStar) == 0)
    {
        arrayStarNotMatch = false; // If star is found, change bool
        if (varCharASCII != '*')
        {
            varCharASCII = '*'; // Prevent looping in case of slow reading
            if (countStar < 2)
            {
                countStar++;
            }
        }
    }

    if (arrayStarNotMatch)
    {
        arrayVar = 9; 
        for (int i = 0; i < 9; i++)
        {
            charStartEndCheck[i] = charStartEndCheck[i + 1];
        }
        charStartEndCheck[9] = 0;
    }
    else
    {
        arrayVar = 0;
    }

    INFRARED_decodeCharTree(); // Decode char & add to finalString based on array of timings
}


int main() {
    stdio_init_all(); // Initialize standard I/O
    INFRARED_init(); // Initialize infrared sensor

    while (true) {
        tight_loop_contents(); // Function to keep CPU active

        if (readyToStartBarcode()) { // Check if ready to start barcode scanning
            while (!INFRARED_readyToReturnChar()) { // Keep scanning until a character is ready to return
                INFRARED_scanning(); // Perform scanning operation

                if (INFRARED_oneCharRead()) { // Check if one character is read
                    INFRARED_sortingTimings(); // Process the timings to decode the character
                }
            }

            const char* decodedString = INFRARED_returnChar(); // Get the decoded barcode string
            printf("Decoded Barcode: %s\n", decodedString); // Print or handle the decoded string

            INFRARED_resetForNewString(); 
        }
    }
    return 0;
}