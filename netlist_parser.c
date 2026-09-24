#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define MAX_NODES 300   // Minimum starting ammount of nodes that can be stored in the struct. If the .bench file has more nodes, the program will dynamically reallocate memory for the nodes.
#define MAX_LINE 100     // Maximum ammount of characters in a line of the .bench file
#define MAX_FAN_IN 5    // Maximum ammount of input edges for a node.
#define MAX_WORD_LENGTH 15 // Maximum ammount of characters in a signals name


typedef enum {
    AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF, INPUT, OUTPUT, DFF
} FUNCTION;                                                                  // Many possible logic gates


typedef struct Node{

      char input_edge[MAX_FAN_IN][MAX_WORD_LENGTH];
      char output_edge[MAX_WORD_LENGTH];
      FUNCTION function;

} Node;


Node* read_file(char A[], int *num_nodes){    // reads a .bench file!

FILE * f_ptr;
int i=0;
*num_nodes = 0;

f_ptr = fopen(A, "r");  // Open the file in read mode
char line[MAX_LINE];   // MAX Line size of the file

Node* node = malloc(MAX_NODES * sizeof(Node));  // Dynamically allocate memory for the nodes

if (node == NULL) {                       // Exit in case malloc fails for some reason
    printf("Memory allocation failed\n");
    exit(1);
}

if (f_ptr == NULL){

    printf("Error with opening the netlist file...\n");
    printf("Process terminated :(\n");
    free(node);
    exit(1);
}


while((fgets(line, sizeof(line), f_ptr)) != NULL){



    int only_whitespace = 1;

    for (int k = 0; line[k] != '\0'; k++) {

        if (line[k] != ' ' &&
            line[k] != '\t' &&
            line[k] != '\n' &&
            line[k] != '\r') {

            only_whitespace = 0;
            break;
        }
    }

    if (only_whitespace) {
        continue;
    }


    if(strncmp("\n", line, 1) == 0){       // Condition to ignore an empty line in case it exists and move on to the next one

        continue;
    }

    if(strncmp("#", line, 1) == 0){       // It is for ignoring the first line with the name of the .bench file

        continue;
    }


    if(strncmp("INPUT", line, 5) == 0){    // I check wether this line describes an input node

          for(int j = 0; j < MAX_FAN_IN; j++){
              node[i].input_edge[j][0] = '\0';
          }

          strtok(line, " (),=\n\t");
          strcpy(node[i].output_edge, strtok(NULL, " (),=\n\t"));

          node[i].function = INPUT;
    }


    else if(strncmp("OUTPUT", line, 6) == 0){  // I check wether this line describes an output node

        for(int j = 0; j < MAX_FAN_IN; j++){
            node[i].input_edge[j][0] = '\0';
        }

        strtok(line, " (),=\n\t");
        strcpy(node[i].input_edge[0], strtok(NULL, " (),=\n\t"));

        node[i].output_edge[0] = '\0';
        node[i].function = OUTPUT;

    }


    else{

      strcpy(node[i].output_edge, strtok(line, " (),=\n\t"));  // Now i check all the lines that describe a logic gate
      char *buffer = strtok(NULL, " (),=\n\t");                // and store the output edge of the node in the struct

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

      while(j < MAX_FAN_IN && (buff = strtok(NULL, " (),=\n\t")) != NULL){

          strcpy(node[i].input_edge[j], buff);
          j++;
      }


      while(j < MAX_FAN_IN){

        node[i].input_edge[j][0] = '\0';
        j++;

      }

    }

    i = i+1;
    (*num_nodes)++;

    if(i % MAX_NODES == 0){

     Node *temp = realloc(node, (i + MAX_NODES) * sizeof(Node));  // Dynamically reallocate memory for the nodes if we reach the limit


    if (temp == NULL) {

        free(node);
        fclose(f_ptr);

        printf("Sometning went wrong with reallocating memory for the nodes\n");

        exit(1);
    }


    node = temp;

    }

}

fclose(f_ptr);    // We got everything we needed from the file, so we close it.

return node;  // Return the dynamically allocated memory for the nodes

}   // end of read_file()



////////////////////////////////////////////////////////////////////////////////////////////////



int main(int argc, char *argv[]) {          // Only use main() for debugging. Will get rid of it later.


    if(argc < 2){

        printf("Usage: %s <file.bench>\n", argv[0]);

        return 1;
    }


    int num_nodes;

    Node *node = read_file(argv[1], &num_nodes);


    for (int i = 0; i < num_nodes; i++) {

        printf("Node %d: output_edge = %s, function = %d\n",
               i,
               node[i].output_edge,
               node[i].function);


        for (int j = 0; j < MAX_FAN_IN; j++) {

            printf("    input_edge[%d] = %s\n",
                   j,
                   node[i].input_edge[j]);
        }

    }


    free(node);

    return 0;
}