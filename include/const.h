#ifndef CONST_H
#define CONST_H

#endif

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <linux/limits.h>

#define TRUE 1
#define FALSE 0
#define LINE_SIZE 100
#define SIG_SIZE 20
#define SUCCESS 1
#define FAIL -1
#define MAX_NAME_LEN 50
#define DATABASE "rsrc/db.txt"

typedef struct signature_t
{
    char* name; /*Null terminated*/
    char* sig;  /*sig is fixed size of SIG_SIZE bytes*/
} signature_t;

/*
    The FILE *fp take the file pointer of a file opened by fopen() in "rb" mode.
    db is the database

    store the name of the virus to virus_name, if founf.
    char* virus_name is pre-allocated, need to be freed after scan.
    return - SUCCESS if scan finish normally, FAIL if error happens.
*/
int scan(FILE* fd, FILE* db, char* virus_name);


/*This function retuns file size if successful, or else it returns a FAIL.*/
ssize_t load_file(FILE *fp, char** mem);

/*Extract signature from char* read, and store it to members of signature: name and sig.
    char* line              - A line of string read from database.txt
    signature_t* signature  - A struct to store signature, containing char* name and char* sig.
                                char* name and char* sig need to be freed later.*/
void extrac_signature(char* line, signature_t* signature);

int sig_compare(char* file, char* sig);


/*Main boyd of on-demand scanning
    char* path - The path or the filename that the use provided.*/
void on_demand(const char* path);

bool is_dir(const char* path);

int rename_and_remove_permission(const char* pathname);


