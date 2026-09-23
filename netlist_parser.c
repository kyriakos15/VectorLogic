#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX 3000 
#define MAX_FAN_IN 4
 


typedef enum { 
    AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF, INPUT, OUTPUT, DFF
} FUNCTION;                                                                  // Many possible logic gates 



typedef struct Node{
    
      int input_edge[MAX_FAN_IN];
      int output_edge;
      FUNCTION function;
} Node; 



Node* read_file(char A[]){    // reads a .bench file!

FILE * f_ptr;
char c;
int i=0;

Node* node = malloc(MAX * sizeof(Node));  // Dynamically allocate memory for the nodes

if (node == NULL) {                       // Exit in case malloc fails for some reason
    printf("Memory allocation failed\n");
    exit(1);
}


f_ptr = fopen(A, "r");
char line[MAX];   // MAX Line size of the file 
char *end;  // for the end pointer of strtol() function

if (f_ptr == NULL){
	
	printf("Error with opening the netlist file...\n");
    printf("Process terminated :(\n");
    exit(1);
    }


while((fgets(line, sizeof(line), f_ptr)) != NULL){


    if(strncmp("\n", line, 1) == 0){       // Condition to ignore an empty line in case it exists and move on to the next one 
        continue;
    
    }                    

    if(strncmp("#", line, 1) == 0){       // It is for ignoring the first line with the name of the .bench file

        continue;
    }


    if(strncmp("INPUT", line, 5) == 0){    // I check wether this line describes an input node 
          node[i].input_edge[0] = -1;
          node[i].output_edge = strtol((strtok(line, " (),=")), &end, 10);
          node[i].function = INPUT;
    }

    
    else if(strncmp("OUTPUT", line, 6) == 0){  // I check wether this line describes an output node
        
        node[i].input_edge[0] = strtol(strtok(line, " (),="), &end, 10);
        node[i].output_edge = -1;
        node[i].function = OUTPUT;

    }

    else{
      node[i].output_edge = strtol((strtok(line, " (),=")), &end, 10);
      char *buffer = strtok(NULL, " (),=");
      node[i].function = 
      strcmp(buffer, "AND") == 0 ? AND   : 
      strcmp(buffer, "OR") == 0 ? OR     : 
      strcmp(buffer, "NOT") == 0 ? NOT   : 
      strcmp(buffer, "NAND") == 0 ? NAND : 
      strcmp(buffer, "NOR") == 0 ? NOR   : 
      strcmp(buffer, "XOR") == 0 ? XOR   : 
      strcmp(buffer, "XNOR") == 0 ? XNOR : BUF;
      
      char *buff; 
      int j = 0;

      while(j < MAX_FAN_IN && (buff = strtok(NULL, " (),=")) != NULL){ 

          node[i].input_edge[j] = strtol(buff, &end, 10);
          j++;
      }
    }
    
    i = i+1;

    if (i >= MAX) {
        printf("ERROR: Too many nodes!\n");
        exit(1);
    }
}

fclose(f_ptr);    // We got everything we needed from the file, so we close it.

  return node;  // Return the dynamically allocated memory for the nodes


}   // end of read_file()





////////////////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char * argv[]){          //Only use main() for debugging. Will get rid of it later.
 

Node *node = read_file(argv[1]);  // Call read_file() and store the returned pointer

for (int i = 0; i < 400; i++) {

    printf("Node %d: output_edge = %d, function = %d\n",
           i,
           node[i].output_edge,
           node[i].function);

    for (int j = 0; j < MAX_FAN_IN; j++) {
        printf("    input_edge[%d] = %d\n",
               j,
               node[i].input_edge[j]);
    }
}


free(node);  // Free the dynamically allocated memory for the nodes

return 0;

}