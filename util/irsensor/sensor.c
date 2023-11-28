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

bool *leftSensePtr = NULL, *rightSensePtr = NULL, *barcodePtr = NULL;

/*
    @brief: Function to compare two integer arrays to see if it matches any character*
    @param: a[]: integer array a
            b[]: integer array b
    @return: 0: Array is equal 
             1: Array is not equal
*/
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

/*
    @brief: Initialize all sensor pins and required pointers
    @param: *leftSensor: pointer to the GPIO pin of the left sensor
            *rightSensor: pointer to the GPIO pin of the right sensor
            *barcodeSensor: pointer to the GPIO pin of the barcode sensor
    @return: None
*/
void initSensor(bool *leftSensor, bool *rightSensor, bool * barcodeSensor)
{
    printf("[Encoder] Init start \n");

    leftSensePtr = leftSensor;
    rightSensePtr = rightSensor;
    barcodePtr = barcodeSensor;

    // Initialization
    gpio_init(BARCODE_SENSOR);
    gpio_set_dir(BARCODE_SENSOR, GPIO_IN); // S1 in

    gpio_init(LEFT_IR_SENSOR);
    gpio_set_dir(LEFT_IR_SENSOR, GPIO_IN); // S2 in

    gpio_init(RIGHT_IR_SENSOR);
    gpio_set_dir(RIGHT_IR_SENSOR, GPIO_IN); // S3 in
    gpio_init(BARCODE_SENSOR);
    gpio_set_dir(BARCODE_SENSOR, GPIO_IN); // S1 in

    gpio_init(LEFT_IR_SENSOR);
    gpio_set_dir(LEFT_IR_SENSOR, GPIO_IN); // S2 in

    gpio_init(RIGHT_IR_SENSOR);
    gpio_set_dir(RIGHT_IR_SENSOR, GPIO_IN); // S3 in

    printf("[Encoder] Init done \n");
}

/*
    @brief: retrieves flag to determine if the barcode is ready to scan
    @param: None
    @return: 0: success
*/
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

/*
    @brief: Scans the barcode, tracking time between transitions (black to white or white to black)
    @param: None
    @return: None
*/
void scanning()
{
    static uint32_t lastDetectionTime = 0;
    const uint32_t debounceTime = 10;

    uint32_t currentTime = time_us_32() / 1000;
    bool currentSensorState = gpio_get(BARCODE_SENSOR);

    if (currentTime - lastDetectionTime > debounceTime)
    {
        // Check for state change
        if ((currentSensorState == 1 && infraFlag == true) ||
            (currentSensorState == 0 && infraFlag == false))
        {
            infraFlag = !infraFlag;
            charStartEndCheck[arrayVar] = currentTime;
            arrayVar += 1;

             printf("Time: %d ms, Bar: %s\n", currentTime, currentSensorState ? "Black" : "White");
            if (arrayVar == EXPECTED_TRANSITIONS)
            {
                patternComplete = true; // Set the pattern complete flag
            }
            lastDetectionTime = currentTime;
        }
    }
}

/*
    @brief: Determines if the barcode scan is complete based on asterisk detection
    @param: None
    @return: true: True
             false: False
*/
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

/*
    @brief:  Processes the finalString to return the decoded character
    @param: None
    @return: finalString: Success
             tempString: tempString
*/
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
    if (len == 3)
    {
        static char tempStr[2];
        tempStr[0] = finalString[1];
        tempStr[1] = '\0';
        return tempStr;
    }
    return finalString;
}

/*
    @brief: Resets variables to prepare for a new barcode scanning
    @param: None
    @return: None
*/
void resetForNewString()
{
    countAsterisk = 0;                 // Reset count asterisk
    varCharASCII = '~';                // Reset variable
    arrayAsteriskisNotMatch = true;    // Set back star as not found
    strcpy(finalString, "");           // Clear contents of string
    strcpy(sanitizedFinalString, "");  // Clear contents of cleaned string
    startScanningBarcodeState = false; // Exit barcode state
}

/*
    @brief: Decodes the thick and thin bars into characters by comparing the timing array to predefined patterns
    @param: None
    @return: None
*/
void decodeChar()
{
    // Define a mapping of patterns to their respective characters
    const int *patterns[] = {letterA, letterB, letterC, letterD, letterE, letterF, letterG,
                             letterH, letterI, letterJ, letterK, letterL, letterM, letterN,
                             letterO, letterP, letterQ, letterR, letterS, letterT, letterU,
                             letterV, letterW, letterX, letterY, letterZ, reverseLetterA, reverseLetterB, reverseLetterC, reverseLetterD, reverseLetterE, reverseLetterF, reverseLetterG,
                             reverseLetterH, reverseLetterI, reverseLetterJ, reverseLetterK, reverseLetterL, reverseLetterM, reverseLetterN,
                             reverseLetterO, reverseLetterP, reverseLetterQ, reverseLetterR, reverseLetterS, reverseLetterT, reverseLetterU,
                             reverseLetterV, reverseLetterW, reverseLetterX, reverseLetterY, reverseLetterZ};
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const int numPatterns = sizeof(patterns) / sizeof(patterns[0]);

    if (patternComplete)
    {
        for (int i = 0; i < numPatterns; ++i)
        {
            // Check for a forward pattern match
            if (matchArray(varcharStartEndAsterisk, patterns[i]) == 0 && i < 26)
            {
                varCharASCII = chars[i]; // Direct mapping for forward patterns
                break;
            }
            // Check for a reverse pattern match
            else if (matchArray(varcharStartEndAsterisk, patterns[i]) == 0 && i >= 26)
            {
                varCharASCII = chars[i % 26]; // Map reverse patterns to forward character
                break;
            }
        }
    }

    // Only add the character to finalString if it's not a '*'
    if (varCharASCII != '\0' && varCharASCII != '*')
    {
        strncat(finalString, &varCharASCII, 1);
    }

    // Reset the patternComplete flag
    patternComplete = false;
 
}

/*
    @brief: Determines if one character worth of bars/spaces has been read
    @param: None
    @return: 1: True, 0: False
*/
bool oneCharRead()
{
    return (arrayVar == 10);
}


/*
    @brief: Processes the timings to identify thick and thin bars and decode the character 
    @param: None
    @return: 1: True, 0: False
*/
void decodeThickThinBar()
{
    // Check if a complete pattern has been read. If not, exit the function early.
    if (!patternComplete)
    {
        return;
    }
    // Calculate the time differences between transitions and store them in the timeChanges array.
    for (int i = 0; i < 9; i++)
    {
        timeChanges[i] = charStartEndCheck[i + 1] - charStartEndCheck[i];
        // If any time difference exceeds the TIMEOUT threshold, reset for a new barcode string.
        if (timeChanges[i] > TIMEOUT)
        {
            resetForNewString();
            break;
        }
    }

    // Initialize variables to store the indices and values of the three longest times,
    // which correspond to the thick bars in a barcode pattern.
    int h1 = INT32_MIN;    // Highest time value
    int h2 = INT32_MIN;    // Second highest time value
    int h3 = INT32_MIN;    // Third highest time value
    int h1i, h2i, h3i = 0; // Indices of the highest times

    // Loop through the timeChanges array to find the three longest times.
    for (int i = 0; i < 9; i++)
    {
        // If the current time is greater than the largest recorded time.
        if (timeChanges[i] > h1)
        // Shift the second and third highest times and their indices down.
        {
            h3i = h2i;
            h2i = h1i;
            h1i = i;

            // Record the new highest time and its index.
            h3 = h2;
            h2 = h1;
            h1 = timeChanges[i];
        }

        // If the current time is between the largest and second largest recorded times.
        else if (timeChanges[i] > h2)
        {
            // Shift the third highest time and its index down.
            h3i = h2i;
            h2i = i;

            // Record the new second highest time.
            h3 = h2;
            h2 = timeChanges[i];
        }

        // If the current time is between the second and third largest recorded times.
        else if (timeChanges[i] > h3)
        {
            // Record the new third highest time and its index.
            h3i = i;
            h3 = timeChanges[i];
        }
    }
    // Set the positions of the thick bars in the varcharStartEndAsterisk array based on the indices of the longest times.
    for (int i = 0; i < 9; i++)
    {
        if (i == h1i || i == h2i || i == h3i)
        {
            varcharStartEndAsterisk[i] = 1; // Thick bar
        }
        else
        {
            varcharStartEndAsterisk[i] = 0; // Thin bar
        }
    }

    // Print the complete pattern of thick and thin bars to the console.
    for (int i = 0; i < 9; i++)
    {
        printf("%d ", varcharStartEndAsterisk[i]);
    }
    printf("\n");

    // Check if the detected pattern matches the start/end asterisk pattern.
    if (matchArray(varcharStartEndAsterisk, charStartEndAsterisk) == 0)
    {
        printf("Star deteceted Forward \n");
        if (varCharASCII != '*')
        {
            varCharASCII = '*';
            // Increase the asterisk count, ensuring it doesn't exceed two.
            if (countAsterisk < 2)
            {
                countAsterisk++;
            }
        }
    }

    // Check if the detected pattern matches the reversed start/end asterisk pattern.
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

    // If an asterisk pattern was not matched, prepare to shift the read pattern by one position.
    if (arrayAsteriskisNotMatch)
    {
        // Shift the timing checks array by one position to the left.
        arrayVar = 9; // Set to read the next bar
        for (int i = 0; i < 9; i++)
        {
            charStartEndCheck[i] = charStartEndCheck[i + 1];
        }
        charStartEndCheck[9] = 0; // Clear the last position for the new read
    }
    else
    {
        // Reset the arrayVar to start reading a new character.
        arrayVar = 0;
    }

    decodeChar(); // Decode char & add to finalString based on array of timings

    // Reset variables for the next read cycle.
    arrayVar = 0;
    memset(charStartEndCheck, 0, sizeof(charStartEndCheck)); // Clear the array
    patternComplete = false;
}


/*
    @brief: Retrieve the current state of the left IR sensor
    @param: *leftSensor: pointer to the gpio pin of the left IR sensor
    @return: None
*/
void getLeftSensor(bool *leftSensor)
{
    *leftSensor = gpio_get(LEFT_IR_SENSOR);
}

/*
    @brief: Retrieve the current state of the right IR sensor
    @param: *rightSensor: pointer to the gpio pin of the right IR sensor
    @return: None
*/
void getRightSensor(bool *rightSensor)
{
    *rightSensor = gpio_get(RIGHT_IR_SENSOR);
}


/*
    @brief: Creates the sensor task 
    @param: None
    @return: None
*/
void sensorTask(__unused void *params)
{
    float fps = 30;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    while (true)
    {
        *leftSensePtr = gpio_get(LEFT_IR_SENSOR);
        *rightSensePtr = gpio_get(RIGHT_IR_SENSOR);
        printf("Left Sensor Task: %d, Right Sensor Task: %d\n", *leftSensePtr, *rightSensePtr); // DEBUG
        vTaskDelay(frame_time);
    }
}
/*
    @brief: Creates the barcode Task
    @param: None
    @return: None
*/
void barcodeTask(__unused void *params)
{
    float fps = 30;
    float frame_time = 1000 / fps;
    float dt = frame_time / 1000;

    while (true)
    {
        printf("Barcode Task\n"); // DEBUG
        // Check if ready to start barcode scanning
        if (startScanningBarcode())
        { 
            printf("Start Scanning Barcode\n"); // DEBUG
            // Keep scanning until a character is ready to return
            while (!isBarcodeComplete())
            {               
                //printf("Scanning\n"); // DEBUG
                scanning(); // Perform scanning operation

                // Check if one character is read
                if (oneCharRead())
                {          
                    printf("One Char Read\n"); // DEBUG
                    decodeThickThinBar(); // Process the timings to decode the character
                }
            }

            const char *decodedString = returnChar();       // Get the decoded barcode string
            printf("Decoded Barcode: %s\n", decodedString); // Print or handle the decoded string

            resetForNewString();
        }
    }
}
