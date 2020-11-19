// CS 344 Assignment 3: smallsh
// By Ashley Pettibone

// sources and inspirations
// used code from examples given by the instructor (I will label those in code)
// https://stackoverflow.com/ for trouble shooting and looking up how to o a specific task
// tutorialspoint.com
// https://www.stev.org/post/cgethomedirlocationinlinux for home directory code, and to error check

// to run the code
//gcc -std=gnu99 -pthread -o main assign4.c



#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>



// Size of the buffers
#define SIZE 100

// Number of items that will be produced. This number is less than the size of the buffer. Hence, we can model the buffer as being unbounded.
#define NUM_ITEMS 80 //80

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
	
		
	char* line = NULL;
	
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
 Put the item in the buffer shared with the square_root thread.
*/
void *get_input(void *args)
{
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      // Get the user input
      char* line = get_user_input();
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
 Function that the square root thread will run. 
 Consume an item from the buffer shared with the input thread.
 Compute the square root of the item.
 Produce an item in the buffer shared with the output thread.

/*
* This function gets rid of line separators
*/
void *lineSeparator(void *args)
{
    char* line = NULL;
    char* square_root;
    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	line = get_buff_1();
        square_root = line; //sqrt(item);
        put_buff_2(square_root);
      //  printf("\nsecondthread: \n");
    }
   
    return NULL;
}


/*
 Put an item in buff_2
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
Get the next item from buffer 2
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





void *changePlusSign(void *args)
{
    char* line = NULL;
    char* square_root;
    char lineArray[82];
    
    char plus[] = {"++"};
    char onePlus[] = {"+"};
    char carat[] = {"^"};
    char* success =NULL;
    
    for (int i = 0; i < NUM_ITEMS; i++)
    {
    	line = get_buff_2();
    	
    	
    	int i = 0;
    	size_t x;
    	char *ptr = strstr(line, plus);

		if (ptr != NULL) /* Substring found */
		{
			printf("'%s' contains '%s'\n", line, plus);
		
//			success = strncpy(lineArray, line, 80);
//			
//			if(success == NULL) {
//				printf("failure");
//			}
			for(x=0; x< NUM_ITEMS; x++) {
				
				if((strcmp(line[x], onePlus) == 0) && (strcmp(line[x + 1], onePlus) == 0)){
					line[x] = carat;
					
				//	int p = 0;  
        			while (line[x] != '\0') { 
            			line[x+1] = line[x + 2]; 
            			x++; 
       				
					   }
					   } 
				//	line[x+1] = line[x+2];
//			void removeDuplicates(char* S) 
//{ 
//    // When string is empty, return 
//    if (S[0] == '\0') 
//        return; 
//  
//    // if the adjacent characters are same 
//    if (S[0] == S[1]) { 
//          
//        // Shift character by one to left 
//        int i = 0;  
//        while (S[i] != '\0') { 
//            S[i] = S[i + 1]; 
//            i++; 
//        } 
//  
//        // Check on Updated String S 
//        removeDuplicates(S); 
//    } 
//  
//    // If the adjacent characters are not same 
//    // Check from S+1 string address 
//    removeDuplicates(S + 1); 
//} 
//  	
				
				//printf("%c\n", line[x]);
 //char * string = "hello world"; 
 //* This 11 chars long, excluding the 0-terminator. */ size_t i = 0; for (; i < 11; i++) { printf("%c ", string[i]);				
			//	printf("this is line array: %s\n", lineArray);
			}
//			(strcmp(commands[0], exitProgram) == 0)
//			int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
//   int loop;
//
//   for(loop = 0; loop < 10; loop++)
//      printf("%d ", array[loop]);


		//strcpy(line);	
//			while(*line != NULL) {
//				
//				if(strcmp(line[i], onePlus) == 0 && strcmp(line[i + 1], onePlus) == 0){
//					line[i] = carat;
//					line[i+1]= line[i+2];
//				}
//			}
 		}
		 /* Substring not found */
    	
    square_root = line; //sqrt(item);
        
        
        
    put_buff_3(square_root);
 //       printf("\nthirdthread: \n");
    }
    return NULL;
}



//
//char str[] = "teacher teach tea";
//	char search[] = "ac";
//	char *ptr = strstr(str, search);
//
//	if (ptr != NULL) /* Substring found */
//	{
//		printf("'%s' contains '%s'\n", str, search);
//	}
//	else /* Substring not found */
//	{
//		printf("'%s' doesn't contain '%s'\n", str, search);
//	}
//
//	return 0;
//}



/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void *write_output(void *args)
{
    char* line;
    for (int i = 0; i < NUM_ITEMS; i++)
    {
      line = get_buff_3();
      printf("\nOutput: %s\n", line);
    }
    return NULL;
}



int main()
{
   // srand(time(0));
    pthread_t input_t, lineSeparator_t, changePlusSign_t, output_t;
    // Create the threads
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

