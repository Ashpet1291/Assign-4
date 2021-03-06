// CS 344 Assignment 4: multithreaded pipeline
// By Ashley Pettibone

// sources and inspirations
// used code from examples given by the instructor (I will label those in code)
// most of the code for buffer functions came from instructors examples
// https://stackoverflow.com/ for trouble shooting and looking up how to do a very simple specific task
// tutorialspoint.com


// to build the code
//gcc -std=gnu99 -pthread -o line_processor assign4.c

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


// Size of the buffers
#define SIZE 1100

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 1000 //80

#define MAX_LINES 49

#define MAX_CHAR 80


// these next three sections come directly from the example code given by the instructor, except I changed the int buffer arrays to char pointer
// Buffer 1, shared resource between input thread and line separator thread
char* buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the line-separator thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between line separator thread and plus sign thread
char* buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the line separator thread will put the next item
int prod_idx_2 = 0;
// Index where the plus sign thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


// Buffer 3, shared resource between plus sign thread and output thread
char* buffer_3[SIZE];
// Number of items in the buffer
int count_3 = 0;
// Index where the splus sign thread will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;


size_t lineSize = 0;
size_t characters = 0;

/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){

	size_t len = 0;
	char* line = NULL;

   	characters = getline(&line, &len, stdin);
	return line;
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* tmpLine){
	// Lock the mutex before putting the item in the buffer
	pthread_mutex_lock(&mutex_1);
	// Put the item in the buffer
	buffer_1[prod_idx_1] = tmpLine;
	// Increment the index where the next item will be put.
  	prod_idx_1 = prod_idx_1 + 1;
  	count_1++;
  	// Signal to the consumer that the buffer is no longer empty
  	pthread_cond_signal(&full_1);
  	// Unlock the mutex
  	pthread_mutex_unlock(&mutex_1);
}

/*
 Function that the input thread will run.
 Get input from the user.
 Put the item in the buffer shared with the lineseparator thread.
*/
void *get_input(void *args)
{
	char* point = NULL;
	//cahnged NUM_ITEMS to MAXLINES
    for (int i = 0; i < MAX_LINES; i++)
    {
        // get user input
    	char* line = get_user_input();
        // check if line contains "STOP"
    	point = strstr(line, "STOP");
        // if line contains STOP, stop processing and dont send STOP
        if(point != NULL) {
            break;
        }
        else {
            put_buff_1(line);
        }
    }
    return NULL;
}

/*
Get the next item from buffer 1
*/
char* get_buff_1(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_1);
  while (count_1 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_1, &mutex_1);
  char* line2 = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return line2;
}

/*
Put an item in buff_2
*/
void put_buff_2(char* line){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_2);
  // Put the item in the buffer
  buffer_2[prod_idx_2] = line;
  // Increment the index where the next item will be put.
  prod_idx_2 = prod_idx_2 + 1;
  count_2++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_2);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
}



/*
Function that the line separator thread will run
Consume an item from the buffer shared with the input thread.
replaces line separators which are newlines, with spaces
Produce an item in the buffer shared with the plussign thread.
*/
void *lineSeparator(void *args)
{
    char* line = NULL;
    char* temp_line;
    char space = ' ';
    char newLine[] = "\n";

    int string_length = 0;

    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	// get item from buffer 1- input
    	line = get_buff_1();
        string_length = strlen(line);

        //if(line[string_length-1] == '\n' )

        //printf("stringlength in linsep %d\n", string_length);
        //strcat(temp_line, line);

    //    char *ptr = strstr(temp_line, newLine);

    //    if(ptr != NULL) {
    //        for(int i=0; i < NUM_ITEMS; i++) {
    //            if(temp_line[i] == '\n') {
    //                temp_line[i] = ' ';
     //           }
    //        }
      //  }

        //printf("lineseparatorthread printin:::: %s\n", line);
        put_buff_2(line);
    }

    return NULL;
}

//char destination[] = "Hello ";
// char source[] = "World!";
// strcat(destination,source);


/*
Get the next item from buffer 2
*/
char* get_buff_2(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_2);
  while (count_2 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_2, &mutex_2);
  char* line = buffer_2[con_idx_2];
  // Increment the index from which the item will be picked up
  con_idx_2 = con_idx_2 + 1;
  count_2--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_2);
  // Return the item
  return line;
}


/*
Put an item in buff_3
*/
void put_buff_3(char* line){
  // Lock the mutex before putting the item in the buffer
  pthread_mutex_lock(&mutex_3);
  // Put the item in the buffer
  buffer_3[prod_idx_3] = line;
  // Increment the index where the next item will be put.
  prod_idx_3 = prod_idx_3 + 1;
  count_3++;
  // Signal to the consumer that the buffer is no longer empty
  pthread_cond_signal(&full_3);
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
}

/*
Function that the plus separator thread will run
Consume an item from the buffer shared with the line separator thread.
replaces double plus signs "++" with a carat "^"
Produce an item in the buffer shared with the output thread.
*/
void *changePlusSign(void *args)
{
    char* line = NULL;
    char plus[] = {"++"};
    char onePlus = '+';
    char carat = '^';
    int s = 0;

    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	// get the item from buff 2
    	line = get_buff_2();


    	// check if the line contins ++
    	char *ptr = strstr(line, plus);
		// if this isn't null, ++ was found
		if (ptr != NULL)
		{
			for(int i=0; i < NUM_ITEMS; i++) {

				// if the spot in the line contains +, and the next spot also contains a plus, then change it to a ^
				if((line[i] == onePlus) && (line[i + 1] == onePlus)){
					line[i] = carat;

					s=i;
					// there were two plus signs being changed to a carat, decrease lineSize by 1
					lineSize = lineSize -1;

					// shift everything else over one spot, because there is one less item
					while(line[s+1] != '\0') {
						line[s+1] = line[s+2];
						s++;
					}
			   }
			}
 		}
    // put this item in the next buffer for output
    	put_buff_3(line);
	}
    return NULL;
}


/*
Get the next item from buffer 3
*/
char* get_buff_3(){
  // Lock the mutex before checking if the buffer has data
  pthread_mutex_lock(&mutex_3);
  while (count_3 == 0)
    // Buffer is empty. Wait for the producer to signal that the buffer has data
    pthread_cond_wait(&full_3, &mutex_3);
  char* line = buffer_3[con_idx_3];
  // Increment the index from which the item will be picked up
  con_idx_3 = con_idx_3 + 1;
  count_3--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_3);
  // Return the item
  return line;
}



/*
 Function that the output thread will run.
 Consume an item from the buffer shared with the plus sign thread.
 Print the item.
*/
void *write_output(void *args)
{
    char* line3;
    int size3 = 0;
    size_t size = 0;
    char* TempLine = NULL;

    for (int i = 0; i < MAX_LINES; i++)
    {

    	line3 = get_buff_3();
       // if(line3 != NULL) {
   	    // printf("Number of elements present in given array: %d\n", size);
        printf("Output: %s", line3);
       // }
       // else{
       //     exit(0);
       // }
    }
      	// check if size of incoming string divides equally among 80,
		  //if so print 80 chars and then a newline

		  // or just check if size of incoming string is 80 or greater, if so print 80 chars and a newline----then check remain left and repeat---if 80 char or great print, else no

    return NULL;
}


int main()
{
    pthread_t input_t, lineSeparator_t, changePlusSign_t, output_t;
    // Create the four threads
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&lineSeparator_t, NULL, lineSeparator, NULL);
    pthread_create(&changePlusSign_t, NULL, changePlusSign, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);
    // Wait for the threads to terminate
    pthread_join(input_t, NULL);
    pthread_join(lineSeparator_t, NULL);
    pthread_join(changePlusSign_t, NULL);
    pthread_join(output_t, NULL);
    return EXIT_SUCCESS;
}

