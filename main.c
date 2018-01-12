#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

//types
typedef enum
{
    //boolean definition
    false = ( 1 == 0 ),
    true = ( ! false )
} bool;


//Globals
//(using static & automatic memory allocation this time)
//the downfall is that the size of the memory allocated can not be changed at runtime
#define ReferenceStringSize 30
//#define PhysicalFramesSize 7
//#define VirtualFramesSize 10
int N = -1;
int referenceString [ReferenceStringSize];
int referenceStringLength = 0;

#define BufferSize 2048
char inputBuffer[BufferSize];

////////////////////////////////////
//Text integer functions
bool validateCharToInteger(char integer)
{
    //return true if char is an integer
    //false if not
    if(integer < 48 || integer > 57)
        return false;
    return true;
}

int stringToPosInteger(char* string)
{
    int i =0;
    while(1)
    {
        if(!validateCharToInteger(string[i]))
            return -1;
        i++;
        if(string[i] == 0x00 || string[i] == 10 || string[i] == 13 || string[i] == '\n') // \n maybe redundent
            break;
    }
    return atoi(string);
}

void readInput()
{
    memset(inputBuffer, 0x00, BufferSize * sizeof(char));
    fgets(inputBuffer, BufferSize, stdin);
    int i;
    for(i =0; i < BufferSize; i++)
        if(inputBuffer[i] == '\n')
            return;
    printf("Input buffer overflow\n");
    exit(-1);

}

void pressEnter()
{
    printf("Press Enter...\n");
    readInput();
    return;
}

////////////////////////////////////
//reference string operations
bool userReferenceString()
{
    printf("Please enter a reference string:\n");

    //get user input
    readInput();

    //tokenize the input
    char * token = strtok(inputBuffer, " ");
    int tokenCount = 0;
    int tokens[ReferenceStringSize];
    while(token)
    {
        if(tokenCount == ReferenceStringSize)
        {
            printf("Reference string too long.\n");
            return false;
        }
        tokens[tokenCount] = stringToPosInteger(token);
        if(tokens[tokenCount] ==-1)
        {
            printf("Invalid input\n");
            return false;
        }
        tokenCount++;
        token = strtok(NULL, " ");
    }

    //set reference string
    referenceStringLength = tokenCount;
    memcpy(referenceString, tokens, sizeof(int) * ReferenceStringSize);

    return true;
}

void generateReferenceString()
{
    printf("Please enter the length of the reference\nstring that you wish to generate: ");
    readInput();
    referenceStringLength = stringToPosInteger(inputBuffer);
    if(referenceStringLength > ReferenceStringSize)
    {
        printf("Length greater than max size, setting to %i\n", ReferenceStringSize);
        referenceStringLength = ReferenceStringSize;
    }
    if(referenceStringLength < 0)
        referenceStringLength = 0;
    printf("String length : %i\n", referenceStringLength);
    srand(time(NULL));
    int i;
    for(i =0; i < referenceStringLength; i++)
    {
        referenceString[i] = (int)(rand() % 10); // from the instruction of ten pages
        printf("%i ", referenceString[i]);
    }
    printf("\n");
    return;
}

void displayReferenceString()
{
    printf("Reference String (%i):\n",referenceStringLength);
    int i;
    for(i =0; i < referenceStringLength; i++)
        printf("%i ", referenceString[i]);
    printf("\n");
    return;
}



////////////////////////////////////
//other operations

int menu()
{
    //shows the menu and gets the user's input

    printf("\nDemand Paging simulation menu\n\n");
    printf("0 - Exit\n");
    printf("1 - Read reference string\n");
    printf("2 - Generate reference string\n");
    printf("3 - Display current reference string\n");
    printf("4 - Simulate FIFO\n");
    printf("5 - Simulate OPT\n");
    printf("6 - Simulate LRU\n");
    printf("7 - Simulate LFU\n");
    printf("\nSelection: ");

    //get user input
    readInput();
    return stringToPosInteger(inputBuffer);

}

void queueWrite(int queue[], int length, int * head, int value)
{
    queue[(*head)]=value;
    (*head)++;
    if((*head) >= length)
        (*head) = 0;
    return;
}

int queueRead(int queue[], int length, int * tail)
{
    int value = queue[(*tail)];
    (*tail)++;
    if((*tail) >= length)
        (*tail) = 0;
    return value;
}


////////////////////////////////////
//Paging operations

void displayFrame(int * frame)
{
   int x,y, p;

    printf("Reference:\t");
    for(x =0; x < referenceStringLength; x++)
        printf("%i ", referenceString[x]);
    printf("\n");

    for(y=0; y< N+2; y++)
    {
        if(y < N)
            printf("Page %i:\t\t",y);
        if(y==N)
            printf("Page fault:\t");
        if(y > N)
            printf("Victim frames:\t");
        for(x=0; x < referenceStringLength; x++)
        {
            p = frame[(x * (N+2)) + y];
            if(p != -1)
                printf("%i ", p);
            else
                printf("  ");
        }

    printf("\n");
    }
}

bool pageExist(int pages[], int page)
{
    int i;
    for(i=0; i < N; i++)
    {
        if(pages[i] == page)
            return true;
    }
    return false;
}

bool pageEmpty(int pages[], int page)
{
    int i;
    for(i=0; i < N; i++)
    {
        if(pages[i] == -1)
        {
            pages[i] = page;
            return true;
        }
    }
    return false;
}

void pageReplace(int *pages, int replace, int value)
{
    int i;
    for(i=0; i < N; i++)
    {
        if(pages[i] == replace)
        {
            pages[i] = value;
            return;
        }
    }
}


void FIFO()
{
    //iterators
    int i,x, y;

    //FIFO queue
    int qHead = 0;
    int qTail = 0;
    int qLength = 10;
    int q[qLength];

    //setup Frames
    int frames[referenceStringLength][N+2];

    int pages[N];
    for(i=0; i < N; i++)
        pages[i]=-1;

    int pageFault = -1;
    int victim = -1;
    //build frames
    for(x =0; x < referenceStringLength; x++)
    {
        pageFault = -1;
        victim = -1;

        if(!pageExist(pages, referenceString[x]))
        {
            //page fault
            pageFault = referenceString[x];
            queueWrite(q, qLength, &qHead, referenceString[x]);
            if(!pageEmpty(pages, referenceString[x]))
            {
                //page not empty
                victim = queueRead(q, qLength, &qTail);
                pageReplace(pages, victim, referenceString[x]);
            }
        }

        //write pages
        for(y=0; y < N; y++)
        {
            frames[x][y] = pages[y];
        }
        frames[x][N] = pageFault;
        frames[x][N+1] = victim;
    }

    displayFrame(frames);
}

void OPT()
{
    //iterators
    int i,x, y;

    //setup Frames
    int frames[referenceStringLength][N+2];

    int pages[N];
    for(i=0; i < N; i++)
        pages[i]=-1;


    int pageFault = -1;
    int victim = -1;
    //build frames
    for(x =0; x < referenceStringLength; x++)
    {
        pageFault = -1;
        victim = -1;

        if(!pageExist(pages, referenceString[x]))
        {
            //page fault
            pageFault = referenceString[x];
            if(!pageEmpty(pages, referenceString[x]))
            {
                //page not empty
                //find victim
                int used[N];
                for(i=0; i < N; i++)
                    used[i]=999999;

                for(i=0; i < N; i++)
                {
                    for(y=referenceStringLength-1; y > x; y--)
                    {
                        if(pages[i] == referenceString[y])
                            used[i] = y;
                    }
                }

                int notused = -1;
                int index=0;
                for(i=0; i < N;i++)
                {
                    if( used[i] > notused )
                    {
                        notused = used[i];
                        index = i;
                    }
                }
                victim = pages[index];
                //replace
                pageReplace(pages, victim, pageFault);
            }
        }

        //write pages
        for(y=0; y < N; y++)
        {
            frames[x][y] = pages[y];
        }
        frames[x][N] = pageFault;
        frames[x][N+1] = victim;
    }

    displayFrame(frames);
}

void LRU()
{
    //iterators
    int i,x, y;

    //setup Frames
    int frames[referenceStringLength][N+2];

    int pages[N];
    for(i=0; i < N; i++)
        pages[i]=-1;


    int pageFault = -1;
    int victim = -1;
    //build frames
    for(x =0; x < referenceStringLength; x++)
    {
        pageFault = -1;
        victim = -1;

        if(!pageExist(pages, referenceString[x]))
        {
            //page fault
            pageFault = referenceString[x];
            if(!pageEmpty(pages, referenceString[x]))
            {
                //page not empty
                //find victim
                int used[N];
                for(i=0; i < N; i++)
                    used[i]=0;

                for(i=0; i < N; i++)
                {
                    for(y=0; y < x; y++)
                    {
                        if(pages[i] == referenceString[y])
                            used[i] = y;
                    }
                }

                int notused =9999999;
                int index=0;
                for(i=0; i < N;i++)
                {
                    if( used[i] < notused )
                    {
                        notused = used[i];
                        index = i;
                    }
                }
                victim = pages[index];
                //replace
                pageReplace(pages, victim, pageFault);
            }
        }

        //write pages
        for(y=0; y < N; y++)
        {
            frames[x][y] = pages[y];
        }
        frames[x][N] = pageFault;
        frames[x][N+1] = victim;
    }

    displayFrame(frames);
}

void LFU()
{
    //iterators
    int i,x, y;

    //setup Frames
    int frames[referenceStringLength][N+2];

    int pages[N];
    for(i=0; i < N; i++)
        pages[i]=-1;


    int pageFault = -1;
    int victim = -1;
    //build frames
    for(x =0; x < referenceStringLength; x++)
    {
        pageFault = -1;
        victim = -1;

        if(!pageExist(pages, referenceString[x]))
        {
            //page fault
            pageFault = referenceString[x];
            if(!pageEmpty(pages, referenceString[x]))
            {
                //page not empty
                //find victim
                int used[N];
                for(i=0; i < N; i++)
                    used[i]=0;

                for(i=0; i < N; i++)
                {
                    for(y=0; y < x; y++)
                    {
                        if(pages[i] == referenceString[y])
                            used[i]++;
                    }
                }

                int notused = 999999;
                int index=0;
                for(i=0; i < N;i++)
                {
                    if( used[i] < notused )
                    {
                        notused = used[i];
                        index = i;
                    }
                }
                victim = pages[index];
                //replace
                pageReplace(pages, victim, pageFault);
            }
        }

        //write pages
        for(y=0; y < N; y++)
        {
            frames[x][y] = pages[y];
        }
        frames[x][N] = pageFault;
        frames[x][N+1] = victim;
    }

    displayFrame(frames);
}



////////////////////////////////////
//Main function
int main(int argc, char **argv)
{
    int i;

    //get input parameters
    for(i =0; i < argc; i++)
    {
        if(argv[i][0] == '-' && (argv[i][1] == 'n' || argv[i][1] == 'N') && argv[i][2] == 0x00 )
        {
            if(argc <= i+1)
            {
                printf("Parameter missing\nFormat -n 0-7\n");
                return -1;
            }
            N = atoi(argv[i+1]);
            break;
        }
    }

    //check bounds
    if(N < 0 || N > 7)
    {
        printf("Input out of bounds n = %i\n", N);
        printf("N must be between 0-7\n");
        return -1;
    }

    //show menu, get user input
    int input = -1;
    while( input != 0 )
    {
        input = menu();
        switch(input)
        {
        case 0:
            //exit
            printf("Please come again.\n");
            break;

        case 1:
            //read reference string
            userReferenceString();
            break;

        case 2:
            //generate reference string
            generateReferenceString();
            break;
        case 3:
            //Display current reference string
            displayReferenceString();
            break;
        case 4:
            //simulate FIFO
            FIFO();
            break;
        case 5:
        //similate OPT
            OPT();
            break;
        case 6:
        //simulate LRU
            LRU();
            break;
        case 7:
            //simulate LFU
            LFU();
            break;
        default:
            //bad input
            printf("Invalid input\n");
            break;
        }
        if(input != 0)
            pressEnter();
    }


    return 0;
}
