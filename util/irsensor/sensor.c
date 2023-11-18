#include "pico/stdlib.h"
#include "pico/stdio.h"

#include "sensor.h"

volatile bool infraFlag = true;
volatile bool startScanningBarcodeState = false;
volatile int arrayVar = 0;
volatile int charStartEndCheck[9];
volatile int timeChanges[9];
int varcharStartEndAsterisk[9]; 
volatile int countAsterisk = 0; 
char varCharASCII = '~';      
char finalString[22];          
const int TIMEOUT = 1200;      
bool arrayAsteriskNotMatch = true; 

// Mapping
const int charStartEndAsterisk[9] = {0, 1, 0, 0, 1, 0, 1, 0, 0};
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

int matchArray(int a[], const int b[])
{
    int i;
    for (i = 0; i < 9; i++)
    {
        if (a[i] != b[i])
            return 1; // If array not equal, return 1
    }
    return 0; // If array equal, return 0
}

void initSensor(void)
{
    printf("[Encoder] Init start \n");

    // Initialization
    gpio_init(BARCODE_SENSOR);
    gpio_set_dir(BARCODE_SENSOR, GPIO_IN); // S1 in

    gpio_init(LEFT_IR_SENSOR);
    gpio_set_dir(LEFT_IR_SENSOR, GPIO_IN); // S2 in

    gpio_init(RIGHT_IR_SENSOR);
    gpio_set_dir(RIGHT_IR_SENSOR, GPIO_IN); // S3 in

    printf("[Encoder] Init done \n");
}

int startScanningBarcode()
{
    if (gpio_get(BARCODE_SENSOR) == 0)
    {
        startScanningBarcodeState = true;
        return 1; // For integration into main
    }

    // If not ready to scan, return 0
    return 0;
}

void scanning()
{
    if (gpio_get(BARCODE_SENSOR) == 1) // BLACK BAR
    {
        if (infraFlag == true)
        {
            infraFlag = false;
            sleep_us(100);             
            charStartEndCheck[arrayVar] = time_us_32() / 1000; 
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
            arrayVar += 1;
        }
    }
}

bool isBarcodeComplete()
{
    if (countAsterisk >= 2)
    {
        // Return star to not found
        return true;
    }
    else
        return false;
}
const char *returnChar() {
    int writeIndex = 0;
    for (int readIndex = 0; finalString[readIndex] != '\0'; ++readIndex) {
        if (finalString[readIndex] != '*') {
            finalString[writeIndex++] = finalString[readIndex];
        }
    }
    finalString[writeIndex] = '\0'; // Null-terminate the string
    return finalString;
}

void resetForNewString()
{
    countAsterisk = 0;                    // Reset count star
    varCharASCII = '~';               // Reset variable
    arrayAsteriskNotMatch = true;         // Set back star as not found
    strcpy(finalString, "");          // Clear contents of string
    startScanningBarcodeState = false; // Exit barcode state
}

void decodeChar() {
    const int* patterns[] = { letterA, letterB, letterC, letterD, letterE, letterF, letterG,
                              letterH, letterI, letterJ, letterK, letterL, letterM, letterN,
                              letterO, letterP, letterQ, letterR, letterS, letterT, letterU,
                              letterV, letterW, letterX, letterY, letterZ};
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int numPatterns = sizeof(patterns) / sizeof(patterns[0]);

    varCharASCII = '\0'; // Reset the current character

    for (int i = 0; i < numPatterns; ++i) {
        if (matchArray(varcharStartEndAsterisk, patterns[i]) == 0) {
            varCharASCII = chars[i];
            break; // Break the loop once a match is found
        }
    }
    if (varCharASCII != '\0' && !(varCharASCII == '*' && arrayAsteriskNotMatch)) {
        strncat(finalString, &varCharASCII, 1);
    }

    arrayAsteriskNotMatch = (varCharASCII == '*');
}

bool oneCharRead()
{
    return (arrayVar == 10);
}
void decodeThickThinBar()
{
    for (int i = 0; i < 9; i++)
    {
        timeChanges[i] = charStartEndCheck[i + 1] - charStartEndCheck[i];
        if (timeChanges[i] > TIMEOUT)
        {
            resetForNewString();
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
                varcharStartEndAsterisk[i] = 1;
            }
            else
            {
                varcharStartEndAsterisk[i] = 0;
            }
        }
        else
        {
            if (i == h1i || i == h2i || i == h3i)
            {
                varcharStartEndAsterisk[i] = 1;
            }
            else
            {
                varcharStartEndAsterisk[i] = 0;
            }
        }
    }
    if (matchArray(varcharStartEndAsterisk, charStartEndAsterisk) == 0)
    {
        arrayAsteriskNotMatch = false; // If star is found, change bool
        if (varCharASCII != '*')
        {
            varCharASCII = '*'; // Prevent looping in case of slow reading
            if (countAsterisk < 2)
            {
                countAsterisk++;
            }
        }
    }

    if (arrayAsteriskNotMatch)
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

    decodeChar(); // Decode char & add to finalString based on array of timings
}

//pass by reference from main, to get sensor readings
void getLeftSensor(bool* leftSensor){
    *leftSensor = gpio_get(LEFT_IR_SENSOR);
}

void getRightSensor(bool* rightSensor){
    *rightSensor = gpio_get(RIGHT_IR_SENSOR);
}

// int main() {
//     stdio_init_all(); // Initialize standard I/O
//     initSensor(); // Initialize infrared sensor

//     while (true) {
//         tight_loop_contents(); // Function to keep CPU active

//         if (startScanningBarcode()) { // Check if ready to start barcode scanning
//             while (!isBarcodeComplete()) { // Keep scanning until a character is ready to return
//                 scanning(); // Perform scanning operation

//                 if (oneCharRead()) { // Check if one character is read
//                     decodeThickThinBar(); // Process the timings to decode the character
//                 }
//             }

//             const char* decodedString = returnChar(); // Get the decoded barcode string
//             printf("Decoded Barcode: %s\n", decodedString); // Print or handle the decoded string

//             resetForNewString(); 
//         }
//     }
//     return 0;
// }

