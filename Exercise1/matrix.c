#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include "matrix.h"


#define MAX_CMD_COUNT 50

/*protected functions*/
void load_matrix (Matrix_t* m, unsigned int* data);

/* 
 * PURPOSE: instantiates a new matrix with the passed name, rows, cols 
 * INPUTS: 
 *	name the name of the matrix limited to 50 characters 
 *  rows the number of rows the matrix
 *  cols the number of cols the matrix
 * RETURN:
 *  If no errors occurred during instantiation then true
 *  else false for an error in the process.
 *
 **/

bool create_matrix (Matrix_t** new_matrix, const char* name, const unsigned int rows,
						const unsigned int cols) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	*new_matrix = calloc(1,sizeof(Matrix_t));
	if (!(*new_matrix)) {
		return false;
	}
	(*new_matrix)->data = calloc(rows * cols,sizeof(unsigned int));
	if (!(*new_matrix)->data) {
		return false;
	}
	(*new_matrix)->rows = rows;
	(*new_matrix)->cols = cols;
	unsigned int len = strlen(name) + 1; 
	if (len > MATRIX_NAME_LEN) {
		return false;
	}
	strncpy((*new_matrix)->name,name,len);
	return true;

}

	//TODO FUNCTION COMMENT
/*
*purpose: destroys a matrix
*input: takes in a pointer to an address of type Matrix_t
*output: returns nothing
*/
void destroy_matrix (Matrix_t** m) {

	//TODO ERROR CHECK INCOMING PARAMETERS
	
	if (m && *m){
		free((*m)->data);
		free(*m);
		*m = NULL;
	}
}


	
	//TODO FUNCTION COMMENT
/*
*purpose: checks if matrices are equal to each other
*input: takes in two pointers to the addresses of two separate matrices
*output: returns true or false depending if the 2 matrices match
*/
bool equal_matrices (Matrix_t* a, Matrix_t* b) {
	
	if (!a || !b || !a->data || !b->data) {
		return false;	
	}

	int result = memcmp(a->data,b->data, sizeof(unsigned int) * a->rows * a->cols);
	if (result == 0) {
		return true;
	}
	return false;
}

	//TODO FUNCTION COMMENT
/*
*purpose: duplicates a matrix into another
*input: takes in 2 pointers to addresses of the matrix to be duplicated and the matrix it is duplicated into
*output: true/false
*/
bool duplicate_matrix (Matrix_t* src, Matrix_t* dest) {


	//TODO ERROR CHECK INCOMING PARAMETERS

	if (!src ||!dest) {
		return false;
	}
	/*
	 * copy over data
	 */
	unsigned int bytesToCopy = sizeof(unsigned int) * src->rows * src->cols;
	memcpy(dest->data,src->data, bytesToCopy);	
	return equal_matrices (src,dest);
}

	//TODO FUNCTION COMMENT
/*
*purpose: shifts the matrix a certain direction
*input: pointer to an addres of type Matrix_t, unsigned integer for shifting, and a char for direction to shift
*output: true/false
*/
bool bitwise_shift_matrix (Matrix_t* a, char direction, unsigned int shift) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!a) {
		return false;
	}

	if (direction == 'l') {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->rows; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] << shift;
			}
		}

	}
	else {
		unsigned int i = 0;
		for (; i < a->rows; ++i) {
			unsigned int j = 0;
			for (; j < a->cols; ++j) {
				a->data[i * a->cols + j] = a->data[i * a->cols + j] >> shift;
			}
		}
	}
	
	return true;
}

	//TODO FUNCTION COMMENT
/*
*purpose: add matrices together
*input: pointers to multiple addresses of matrices
*output: true/false
*/
bool add_matrices (Matrix_t* a, Matrix_t* b, Matrix_t* c) {

	//TODO ERROR CHECK INCOMING PARAMETERS

	if (a->rows != b->rows && a->cols != b->cols) {
		return false;
	}

	for (int i = 0; i < a->rows; ++i) {
		for (int j = 0; j < b->cols; ++j) {
			c->data[i * a->cols +j] = a->data[i * a->cols + j] + b->data[i * a->cols + j];
		}
	}
	return true;
}

	//TODO FUNCTION COMMENT
/*
*input: pointer to address of a matrix
*purpose: to display the matrix to the user
*output: the matrix
*/
void display_matrix (Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!m){
		perror("couldn't display matrix");
		return;
	}

	printf("\nMatrix Contents (%s):\n", m->name);
	printf("DIM = (%u,%u)\n", m->rows, m->cols);
	for (int i = 0; i < m->rows; ++i) {
		for (int j = 0; j < m->cols; ++j) {
			printf("%u ", m->data[i * m->cols + j]);
		}
		printf("\n");
	}
	printf("\n");

}

	//TODO FUNCTION COMMENT
/*
*purpose: read in a matrix
*input: pointer to a filename and pointer to address of type Matrix_t
*output:
*/
bool read_matrix (const char* matrix_input_filename, Matrix_t** m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!m) {
		perror("Can't Read Matrix");
		return false;
	}


	int fd = open(matrix_input_filename,O_RDONLY);
	if (fd < 0) {
		printf("FAILED TO OPEN FOR READING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}

	/*read the wrote dimensions and name length*/
	unsigned int name_len = 0;
	unsigned int rows = 0;
	unsigned int cols = 0;
	
	if (read(fd,&name_len,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READING FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	char name_buffer[50];
	if (read (fd,name_buffer,sizeof(char) * name_len) != sizeof(char) * name_len) {
		printf("FAILED TO READ MATRIX NAME\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (read (fd,&rows, sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX ROW SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	if (read(fd,&cols,sizeof(unsigned int)) != sizeof(unsigned int)) {
		printf("FAILED TO READ MATRIX COLUMN SIZE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;
	}

	unsigned int numberOfDataBytes = rows * cols * sizeof(unsigned int);
	unsigned int *data = calloc(rows * cols, sizeof(unsigned int));
	if (read(fd,data,numberOfDataBytes) != numberOfDataBytes) {
		printf("FAILED TO READ MATRIX DATA\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}

		return false;	
	}

	if (!create_matrix(m,name_buffer,rows,cols)) {
		return false;
	}

	load_matrix(*m,data);

	if (close(fd)) {
		return false;

	}
	return true;
}

	//TODO FUNCTION COMMENT
/*
*purpose: write matrix to file
*input: pointer to file that will be written to and pointer to address of type Matrix_t
*output: true/false
*/
bool write_matrix (const char* matrix_output_filename, Matrix_t* m) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (!m || !m->name){
		perror("Error, matrix cannot write");
		return false;
	}

	int fd = open (matrix_output_filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* ERROR HANDLING USING errorno*/
	if (fd < 0) {
		printf("FAILED TO CREATE/OPEN FILE FOR WRITING\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXISTS\n");
		}
		return false;
	}
	/* Calculate the needed buffer for our matrix */
	unsigned int name_len = strlen(m->name) + 1;
	unsigned int numberOfBytes = sizeof(unsigned int) + (sizeof(unsigned int)  * 2) + name_len + sizeof(unsigned int) * m->rows * m->cols + 1;
	/* Allocate the output_buffer in bytes
	 * IMPORTANT TO UNDERSTAND THIS WAY OF MOVING MEMORY
	 */
	unsigned char* output_buffer = calloc(numberOfBytes,sizeof(unsigned char));
	unsigned int offset = 0;
	memcpy(&output_buffer[offset], &name_len, sizeof(unsigned int)); // IMPORTANT C FUNCTION TO KNOW
	offset += sizeof(unsigned int);	
	memcpy(&output_buffer[offset], m->name,name_len);
	offset += name_len;
	memcpy(&output_buffer[offset],&m->rows,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy(&output_buffer[offset],&m->cols,sizeof(unsigned int));
	offset += sizeof(unsigned int);
	memcpy (&output_buffer[offset],m->data,m->rows * m->cols * sizeof(unsigned int));
	offset += (m->rows * m->cols * sizeof(unsigned int));
	output_buffer[numberOfBytes - 1] = EOF;

	if (write(fd,output_buffer,numberOfBytes) != numberOfBytes) {
		printf("FAILED TO WRITE MATRIX TO FILE\n");
		if (errno == EACCES ) {
			perror("DO NOT HAVE ACCESS TO FILE\n");
		}
		else if (errno == EADDRINUSE ){
			perror("FILE ALREADY IN USE\n");
		}
		else if (errno == EBADF) {
			perror("BAD FILE DESCRIPTOR\n");	
		}
		else if (errno == EEXIST) {
			perror("FILE EXIST\n");
		}
		return false;
	}
	
	if (close(fd)) {
		return false;
	}
	free(output_buffer);

	return true;
}

	//TODO FUNCTION COMMENT
/*
*purpose: generates a random matrix
*input: pointer to address of type matrix_t, and 2 unsigned integers to determine the start and end range of matrix numbers
*output: true/false
*/
bool random_matrix(Matrix_t* m, unsigned int start_range, unsigned int end_range) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS

	for (unsigned int i = 0; i < m->rows; ++i) {
		for (unsigned int j = 0; j < m->cols; ++j) {
			m->data[i * m->cols + j] = rand() % end_range + start_range;
		}
	}
	return true;
}

/*Protected Functions in C*/

	//TODO FUNCTION COMMENT
/*
*purpose: loads a matrix
*input: pointer to address of matrix and pointer to unsigned integer data
*output:
*/

void load_matrix (Matrix_t* m, unsigned int* data) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	if (m && data)
	{
		memcpy(m->data,data,m->rows * m->cols * sizeof(unsigned int));
	}
	else{
		perror("Error");
	}
}

	//TODO FUNCTION COMMENT
/*
*purpose: adds matrix to an array
*input: 2 pointers to a current matrix and a new matrix (addresses) and an unsigned int to number of matrixes
*output: returns matrix added to array
*/
unsigned int add_matrix_to_array (Matrix_t** mats, Matrix_t* new_matrix, unsigned int num_mats) {
	
	//TODO ERROR CHECK INCOMING PARAMETERS
	static long int current_position = 0;
	const long int pos = current_position % num_mats;

	if (!mats){
		perror("Error Allocating\n");
		return 0;
	}

	if (!new_matrix){
		perror("Error");
		return 0;
	}

	if ( mats[pos] ) {
		destroy_matrix(&mats[pos]);
	} 
	mats[pos] = new_matrix;
	current_position++;
	return pos;
}
