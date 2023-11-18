#include "pico/stdlib.h"
#include "pico/stdio.h"
#include "sensor.h"

volatile bool infraFlag = true;
volatile bool startScanningBarcodeState = false;
volatile int arrayVar = 0;
volatile int charStartEndCheck[9];
volatile int timeChanges[9];
volatile int startReading = 0;
int varcharStartEndAsterisk[9]; 
volatile int countAsterisk = 0; 
volatile int compareQ = 0; 
char varCharASCII = '~';      
char finalString[22];          
char sanitizedFinalString[20]; 
const int TIMEOUT = 1200;      
bool arrayAsteriskisNotMatch = true; 
bool patternComplete = false;
#define EXPECTED_TRANSITIONS 9


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

const int reverseCharStartEnd[9] = {0, 0, 1, 0, 1, 0, 0, 1, 0};
const int reverseCharSpace[9] = {0, 0, 1, 0, 0, 0, 1, 1, 0};
const int reverseLetterA[9] = {1, 0, 0, 1, 0, 0, 0, 0, 1};
const int reverseLetterB[9] = {1, 0, 0, 1, 0, 0, 1, 0, 0};
const int reverseLetterC[9] = {0, 0, 0, 1, 0, 1, 0, 0, 1};
const int reverseLetterD[9] = {1, 0, 0, 1, 1, 0, 0, 0, 0};
const int reverseLetterE[9] = {0, 0, 0, 1, 1, 0, 0, 0, 1};
const int reverseLetterF[9] = {0, 0, 0, 1, 1, 0, 1, 0, 0};
const int reverseLetterG[9] = {1, 0, 1, 0, 0, 0, 0, 0, 0};
const int reverseLetterH[9] = {0, 0, 0, 1, 1, 0, 0, 1, 1};
const int reverseLetterI[9] = {0, 0, 0, 1, 1, 0, 1, 0, 1};
const int reverseLetterJ[9] = {0, 0, 0, 1, 1, 1, 0, 0, 0};
const int reverseLetterK[9] = {1, 1, 0, 0, 0, 0, 0, 0, 1};
const int reverseLetterL[9] = {1, 1, 0, 0, 0, 0, 0, 1, 0};
const int reverseLetterM[9] = {0, 1, 0, 0, 0, 0, 0, 0, 1};
const int reverseLetterN[9] = {1, 1, 0, 0, 1, 0, 0, 0, 0};
const int reverseLetterO[9] = {0, 1, 0, 0, 1, 0, 0, 0, 1};
const int reverseLetterP[9] = {0, 1, 0, 0, 1, 0, 0, 1, 0};
const int reverseLetterQ[9] = {1, 1, 1, 0, 0, 0, 0, 0, 0};
const int reverseLetterR[9] = {0, 1, 1, 0, 0, 0, 0, 1, 1};
const int reverseLetterS[9] = {0, 1, 1, 0, 0, 0, 0, 0, 0};
const int reverseLetterT[9] = {0, 1, 1, 0, 1, 0, 0, 0, 0};
const int reverseLetterU[9] = {1, 0, 0, 0, 0, 0, 0, 1, 1};
const int reverseLetterV[9] = {1, 0, 0, 0, 0, 0, 0, 1, 0};
const int reverseLetterW[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};
const int reverseLetterX[9] = {1, 0, 0, 0, 1, 0, 0, 1, 0};
const int reverseLetterY[9] = {0, 0, 0, 0, 1, 0, 0, 1, 1};
const int reverseLetterZ[9] = {0, 0, 0, 0, 1, 0, 0, 1, 1};

/** Function to compare two integer arrays to see if it matches any character*/
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

/** Initialize sensors for barcode scanning and line detection */
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

/* Checks if the barcode sensor is ready to start scanning if not start */
int startScanningBarcode()
{
    if (gpio_get(BARCODE_SENSOR) == 0)
    {
        startScanningBarcodeState = true;
        return 1; // For integration into main
    };
}

/* Scans the barcode, tracking time between transitions (black to white or white to black) */
void scanning()
{
    static uint32_t lastDetectionTime = 0; 
    const uint32_t debounceTime = 10; 

    uint32_t currentTime = time_us_32() / 1000; 
    bool currentSensorState = gpio_get(BARCODE_SENSOR);

    if (currentTime - lastDetectionTime > debounceTime) {
        // Check for state change
        if ((currentSensorState == 1 && infraFlag == true) || 
            (currentSensorState == 0 && infraFlag == false)) {
            infraFlag = !infraFlag; 
            charStartEndCheck[arrayVar] = currentTime; 
            arrayVar += 1; 

            //printf("Time: %d ms, Bar: %s\n", currentTime, currentSensorState ? "Black" : "White");
            if (arrayVar == EXPECTED_TRANSITIONS) {
                patternComplete = true; // Set the pattern complete flag
            }
            lastDetectionTime = currentTime; 
        }
    }
}

/* Determines if the barcode scan is complete based on asterisk detection */
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

/** Processes the finalString to return the decoded character */
const char *returnChar()
{
    int tempCount = 0;
    int tempcountAsterisk = 0;
    for (int i = 0; finalString[i] != '\0'; ++i)
    {
        if (finalString[i] != '*')
        {
            finalString[tempCount] = finalString[i];
            tempCount++;
        }
        if (finalString[i] == '*')
        {
            tempcountAsterisk++;
        }
        if (tempcountAsterisk == 2)
        {
            finalString[tempCount] = '\0';
        }
    }

    int len = strlen(finalString);
    if (len == 3) {
        static char tempStr[2];
        tempStr[0] = finalString[1]; 
        tempStr[1] = '\0'; 
        return tempStr; 
    }
    return finalString;
}

/* Resets variables to prepare for a new barcode scanning */
void resetForNewString()
{
    countAsterisk = 0;                    // Reset count asterisk
    varCharASCII = '~';               // Reset variable
    arrayAsteriskisNotMatch = true;         // Set back star as not found
    strcpy(finalString, "");          // Clear contents of string
    strcpy(sanitizedFinalString, ""); // Clear contents of cleaned string
    startScanningBarcodeState = false; // Exit barcode state
}

/* Decodes the thick and thin bars into characters by comparing the timing array to predefined patterns */
void decodeChar() {
    // Define a mapping of patterns to their respective characters
    const int* patterns[] = { letterA, letterB, letterC, letterD, letterE, letterF, letterG,
                              letterH, letterI, letterJ, letterK, letterL, letterM, letterN,
                              letterO, letterP, letterQ, letterR, letterS, letterT, letterU,
                              letterV, letterW, letterX, letterY, letterZ, reverseLetterA, reverseLetterB, reverseLetterC, reverseLetterD, reverseLetterE, reverseLetterF, reverseLetterG,
                                    reverseLetterH, reverseLetterI, reverseLetterJ, reverseLetterK, reverseLetterL, reverseLetterM, reverseLetterN,
                                    reverseLetterO, reverseLetterP, reverseLetterQ, reverseLetterR, reverseLetterS, reverseLetterT, reverseLetterU,
                                    reverseLetterV, reverseLetterW, reverseLetterX, reverseLetterY, reverseLetterZ};
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int numPatterns = sizeof(patterns) / sizeof(patterns[0]);

    if (patternComplete) {
        for (int i = 0; i < numPatterns; ++i) {
            // Check for a forward pattern match
            if (matchArray(varcharStartEndAsterisk, patterns[i]) == 0 && i < 26) {
                varCharASCII = chars[i]; // Direct mapping for forward patterns
                break;
            }
            // Check for a reverse pattern match
            else if (matchArray(varcharStartEndAsterisk, patterns[i]) == 0 && i >= 26) {
                varCharASCII = chars[i % 26]; // Map reverse patterns to forward character
                break;
            }
        }
    }

    // Only add the character to finalString if it's not a '*'
    if (varCharASCII != '\0' && varCharASCII != '*') {
        strncat(finalString, &varCharASCII, 1);
    }
    
    // Reset the patternComplete flag
    patternComplete = false;
    varCharASCII = '~';
}

/* Determines if one character worth of bars/spaces has been read */
bool oneCharRead()
{
    return (arrayVar == 10);
}

/* Processes the timings to identify thick and thin bars and decode the character */
void decodeThickThinBar()
{
    if (!patternComplete) {
        return;
    }

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
            if (i == h1i || i == h2i || i == h3i)
            {
                varcharStartEndAsterisk[i] = 1;
            }
            else
            {
                varcharStartEndAsterisk[i] = 0;
            }  
    }

    //printf("Full Pattern: ");
    for (int i = 0; i < 9; i++)
    {
        printf("%d ", varcharStartEndAsterisk[i]);
    }
    printf("\n");

    if (matchArray(varcharStartEndAsterisk, charStartEndAsterisk) == 0)
    {
        printf("Star deteceted Forward \n");
        if (varCharASCII != '*')
        {
            varCharASCII = '*'; 
            if (countAsterisk < 2)
            {
                countAsterisk++;
            }
        }
    }
    else if (matchArray(varcharStartEndAsterisk, reverseCharStartEnd) == 0)

    {
                printf("Star deteceted Reversed \n");
        if (varCharASCII != '*')
        {
            varCharASCII = '*'; 
            if (countAsterisk < 2)
            {
                countAsterisk++;
            }
        }
    }
    if (arrayAsteriskisNotMatch)
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

    arrayVar = 0;
    memset(charStartEndCheck, 0, sizeof(charStartEndCheck)); // Clear the array
    patternComplete = false;
}

/** Retrieve the current state of the left IR sensor */
void getLeftSensor(bool* leftSensor){
    *leftSensor = gpio_get(LEFT_IR_SENSOR);
}

/** Retrieve the current state of the right IR sensor */
void getRightSensor(bool* rightSensor){
    *rightSensor = gpio_get(RIGHT_IR_SENSOR);
}

int main() {
    stdio_init_all(); // Initialize standard I/O
    initSensor(); // Initialize infrared sensor

    while (true) {
        tight_loop_contents(); // Function to keep CPU active

        if (startScanningBarcode()) { // Check if ready to start barcode scanning
            while (!isBarcodeComplete()) { // Keep scanning until a character is ready to return
                scanning(); // Perform scanning operation

                if (oneCharRead()) { // Check if one character is read
                    decodeThickThinBar(); // Process the timings to decode the character
                }
            }

            // Once the barcode is fully scanned and decoded
            const char* decodedString = returnChar(); // Get the decoded barcode string
            printf("Decoded Barcode: %s\n", decodedString); // Print or handle the decoded string

            resetForNewString(); // Reset for the next scanning operation
        }
    }
    return 0;
}


