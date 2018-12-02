#ifndef WHITELIST_H
#define WHITELIST_H

#endif

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/*This is the whitelist file path. Change as needed*/
//static char* WHITELISTFILE = "rsrc/wb.out";
#define WHITELISTFILE "rsrc/wb.out"
/*don't worry about these two*/
#define SHA_LENGTH 41
#define COMPLENGTH 40
#define SC_FG 99
//static int SHA_LENGTH = 41;
//static int COMPLENGTH = 40;


/* This method will take a file path, i.e. "/bin/bash" and check said file.
 * @param filepath the path to the file
 * @return 0 if it 'is not' on the whitelist, 1 if it 'is' on the whitelist.
 * 2 if there was an error finding the file, 3 if there was a problem finding the whitelist.
 * 4 if there was an issue hashing the file with sha1sum.
 */
int whitelistCheck(const char* filepath);

/*don't use this one, use the one above */
int UsingFileWhitelistCheck(FILE* file,const char* filepath);

int whitelistCheck_A(const char* filepath); /*For on access*/

int UsingFileWhitelistCheck_A(FILE* file,const char* filepath); /*For on acess*/
