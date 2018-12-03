#include <whitelist.h>
#include <errno.h>

int whitelistCheck(const char* filepath){
    FILE* file = fopen(filepath,"rb");
    if (file == NULL){
        fprintf(stderr, "%s errno: %s\n", "File to read not found", strerror(errno));
        return 2;
    }
    int out = UsingFileWhitelistCheck(file, filepath);
    fclose (file);
    return out;
}

int UsingFileWhitelistCheck(FILE* file,const char* filepath){
    FILE* wh = fopen(WHITELISTFILE, "r");
    if (wh == NULL){
        fprintf(stderr, "%s\n", "Whitelist File not found");
        return 3;
    }
    int out = 0;
    char check[SHA_LENGTH];

    /*format string*/
    char openString[500];
    sprintf(openString, "%s%s", "sha1sum ", filepath);
    /*hash the file*/
    FILE* hash = popen (openString,"r");
    if (hash == NULL){
        fprintf(stderr, "%s\n", "Error Hashing file");
        return 4;
    }
    /*get the hash*/
    fgets(check,SHA_LENGTH,hash);
    pclose(hash);

    /*for each whitelist entry we check*/
    char whCheck[SHA_LENGTH+1];

    while(fgets(whCheck,SHA_LENGTH+1,wh)){
        if (strncmp(whCheck,check,COMPLENGTH) == 0){
            out = 1;
            break;
        }
    }

    return out;

}



int whitelistCheck_A(const char* filepath){
    int fd = open(filepath, SC_FG);
    FILE* file = fdopen(fd, "rb");
    //FILE* file = fopen(filepath,"rb");
    if (file == NULL){
        fprintf(stderr, "%s errno: %s\n", "File to read not found", strerror(errno));
        return 2;
    }
    int out = UsingFileWhitelistCheck_A(file, filepath);
    fclose (file);
    return out;
}

int UsingFileWhitelistCheck_A(FILE* file,const char* filepath){
    int fd = open(WHITELISTFILE, SC_FG);
    FILE* wh = fdopen(fd, "rb");
    //FILE* wh = fopen(WHITELISTFILE, "r");
    if (wh == NULL){
        fprintf(stderr, "%s\n", "Whitelist File not found");
        return 3;
    }
    int out = 0;
    char check[SHA_LENGTH];

    /*format string*/
    char openString[500];
    sprintf(openString, "%s%s", "sha1sum ", filepath);
    /*hash the file*/
    FILE* hash = popen (openString,"r");
    if (hash == NULL){
        fprintf(stderr, "%s\n", "Error Hashing file");
        return 4;
    }
    /*get the hash*/
    fgets(check,SHA_LENGTH,hash);
    pclose(hash);

    /*for each whitelist entry we check*/
    char whCheck[SHA_LENGTH+1];

    while(fgets(whCheck,SHA_LENGTH+1,wh)){
        if (strncmp(whCheck,check,COMPLENGTH) == 0){
            out = 1;
            break;
        }
    }

    return out;

}