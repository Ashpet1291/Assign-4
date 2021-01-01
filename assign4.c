// CS 344 Assignment 4: multithreaded pipeline
// By Ashley Pettibone

// sources and inspirations
// used code from examples given by the instructor (I will label those in code)
// most of the code for buffer functions came from instructors examples
// https://stackoverflow.com/ for trouble shooting and looking up how to o a specific task
// tutorialspoint.com


// to run the code
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

#define MAX_LINES 45

// these next three sections come directly from the example code given by the instructor, excepts I changed the int buffer arrays to char*
// Buffer 1, shared resource between input thread and square-root thread
char* buffer_1[SIZE];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the square-root thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t full_1 = PTHREAD_COND_INITIALIZER;


// Buffer 2, shared resource between square root thread and output thread
char* buffer_2[SIZE];
// Number of items in the buffer
int count_2 = 0;
// Index where the square-root thread will put the next item
int prod_idx_2 = 0;
// Index where the output thread will pick up the next item
int con_idx_2 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_2 = PTHREAD_COND_INITIALIZER;


// Buffer 3, shared resource between 3rd thread and output thread
char* buffer_3[SIZE];
// Number of items in the buffer
int count_3 = 0;
// Index where the square-root thread will put the next item
int prod_idx_3 = 0;
// Index where the output thread will pick up the next item
int con_idx_3 = 0;
// Initialize the mutex for buffer 2
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 2
pthread_cond_t full_3 = PTHREAD_COND_INITIALIZER;


/*
Get input from the user.
This function doesn't perform any error checking.
*/
char* get_user_input(){
	size_t len = 0;
	ssize_t lineSize = 0;
	ssize_t templineSize = 0;	
	int max = 45;
	int loop;

// while loop less than 45? or null? 
	// holds the user input		
	char* line = NULL;
	
	// get input from stdIn
	lineSize = getline(&line, &len, stdin);
//	
//	if(strcmp(line, stopProcessing) == 0) {
//		line = END_MARKER;
//	}
	
	return line;
}

/*
 Put an item in buff_1
*/
void put_buff_1(char* line){
	// Lock the mutex before putting the item in the buffer
	pthread_mutex_lock(&mutex_1);
	// Put the item in the buffer
	buffer_1[prod_idx_1] = line;
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
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      char* line = get_user_input();
      // put it in the first buffer
      put_buff_1(line);
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
  char* line = buffer_1[con_idx_1];
  // Increment the index from which the item will be picked up
  con_idx_1 = con_idx_1 + 1;
  count_1--;
  // Unlock the mutex
  pthread_mutex_unlock(&mutex_1);
  // Return the item
  return line;
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
    char* square_root1;
    
    char space = ' ';
    char n = '\n';
    char newLine[] = "\n";
    
    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	// get item from buffer 1- input
    	line = get_buff_1();   	   	
    	int i = 0;
    	size_t y;
    	
    	// check if the line contins a newline
    	char *ptr = strstr(line, newLine);
		// if this isn't null, newline-\n was found
		if (ptr != NULL) 
		{
			int r = 0;
			for(y=0; y < NUM_ITEMS; y++) {
				
				// if the spot in the word contains \n, change it to a space
				if(line[y] == n) {
				
					line[y] = space;									
//					r = y;
//					// shift everything else over one spot, because we lost one item in size
//					while(line[r+1] != '\0') {
//						line[r +1] = line[r+2];
//						r++;
//					}
			   }   
    		}
		}
        square_root1 = line; //sqrt(item);
        put_buff_2(square_root1);      ///////////////////////put line here
    }
   
    return NULL;
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
    char* success =NULL;
    
    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	// get the item from buff 2
    	line = get_buff_2();
    	 	
    	int i = 0;
    	size_t x;
    	
    	// check if the line contins ++
    	char *ptr = strstr(line, plus);
		// if this isn't null, ++ was found
		if (ptr != NULL) 
		{
			int s = 0;
			for(x=0; x < NUM_ITEMS; x++) {
				
				// if the spot in the word contains +, and the next spot also contains a plus, then change it to a ^
				if((line[x] == onePlus) && (line[x + 1] == onePlus)){				
					line[x] = carat;
					
					s = x;
					
					// shift everything else over one spot, because we lost one item in size
					while(line[s+1] != '\0') {
						line[s +1] = line[s+2];
						s++;
					}
			   }   
			}							
 		}
    // put this item in the next buffer for output        
    put_buff_3(line); //////////////////////////put line here
    }
    return NULL;
}



/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the plus sign thread.
 Print the item.
*/
void *write_output(void *args)
{
    char* line;
    int size = 0;
    
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      // get the item form buffer 3 to print
      line = get_buff_3();
      
    //  	size_t size = (*line); //           dont work
      size = sizeof(line);
      
      printf("%d", size);
      
      // need to make if loop to find out size of line, if the size is mod 80, print 80 chars and a newline
      // if output is great then 80 then ,,,remainder = mod 80 the line, put string size of remainder in tempstring wait for next buffer(call function?) to concat
      // if less than buffer put line in temp string to get next input
    //  printf("\nOutput: %s\n", line);
    }
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

