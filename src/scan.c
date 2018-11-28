#include <const.h>
/*
    scan a single file using db.
    The FILE *fp take the file pointer of a file opened by fopen() in "rb" mode.
    db is the database

    store the name of the virus to virus_name, if founf.

    return - SUCCESS if scan finish normally, FAIL if error happens.
*/
int scan(FILE* fd, FILE* db, char* virus_name){

    //char* ret = NULL;
    char* file;
    ssize_t load_size; /* load contains file size, INCLUDE one NULL terminator*/
    load_size = load_file(fd, &file); /*load the FILE to file, file needs to be freed later*/
    if (load_size == FAIL)
    {
        printf("%s\n", "Error when loading");
        return FAIL;
    }


    ssize_t read; /*size of the string read from db*/
    //char byte;

    char* line_ptr = NULL;  /*free it later, store read line from db*/
    size_t n = 0;   /*store allocated size*/

    /*
        ssize_t getline(char **lineptr, size_t *n, FILE *stream);

        getline() reads an entire line from stream, storing the address of
       the buffer containing the text into *lineptr.  The buffer is null-
       terminated and includes the newline character, if one was found.

       If *lineptr is set to NULL and *n is set 0 before the call, then
       getline() will allocate a buffer for storing the line.  This buffer
       should be freed by the user program even if getline() failed.

       Alternatively, before calling getline(), *lineptr can contain a
       pointer to a malloc(3)-allocated buffer *n bytes in size.  If the
       buffer is not large enough to hold the line, getline() resizes it
       with realloc(3), updating *lineptr and *n as necessary.

       In either case, on a successful call, *lineptr and *n will be updated
       to reflect the buffer address and allocated size respectively.
    */
    signature_t signature;

    char found = FALSE;

    while ((read = getline(&line_ptr, &n, db)) != -1){
        /*
            reading a line from virus database.
            In each interation, scan through the file that has been loaded to file,
            Looking for line in the file. KMC can be used but here I'll use simplest
            algorithm.

            int strncmp(const char *str1, const char *str2, size_t n)
            size_t strlen(const char *str)
        */
        if (found == TRUE)
        {
            break;
        }

        extrac_signature(line_ptr, &signature);

        char* sig = signature.sig;
        char* name = signature.name;
        /*printf("%s\n", name);
        printf("%s %i\n", sig, (int)strlen(sig));
*/
        /*while(i<SIG_SIZE){
            char temp = sig[i];
            printf("%hhu ", temp);

            i++;
        }*/

        //exit(0);
        /*Do scanning here. strcpy name if detected.*/
        /*FILE: 123456789N, SIG_SIZE: 5, load_size = 10. N is null terminator
            must have last interation 1234*56789*N, i=4. limit = load_size - SIG_SIZE
        */

        ssize_t limit = load_size - SIG_SIZE;
        //char* file_temp = file;


        for (int i = 0; i < limit; ++i) /*compare byte by byte*/
        {
            /*  int strncmp(const char *str1, const char *str2, size_t n) compares at most
                the first n bytes of str1 and str2.

                    if Return value < 0 then it indicates str1 is less than str2.

                    if Return value > 0 then it indicates str2 is less than str1.

                    if Return value = 0 then it indicates str1 is equal to str2.
            */

            //file = file + i;
            char file_temp[SIG_SIZE];
            strncpy(file_temp, (file+i), SIG_SIZE);
            int cmp_result = sig_compare(file_temp, sig);

            //printf("%s %i, Litmit: %i. cmp_result = %i\n","T: Reaching here", i, (int)limit, cmp_result);

            if (cmp_result == 0){ /*detected*/
                /* char *strcpy(char *dest, const char *src)
                    use this to copy the name of to virus_name */
                int name_len = strlen(name);
                //char name_temp[name_len+1];
                //strncpy(name_temp, name, name_len);

                //printf("Name Temp: %s\n", name_temp);
                //*virus_name = name_temp;
                strncpy(virus_name, name, name_len);

                //return SUCCESS;
                found = TRUE;
                break;

            }else{
                *virus_name = 0;
            }

        }





        free(signature.name);
        free(signature.sig);

    }




    free(line_ptr);
    free(file);
    //fclose(fd);
    //fclose(db);


    //return ret;
    return SUCCESS;
}

/*Return 0 if matched, -1 other wise*/
int sig_compare(char* file, char* sig){

    int counter = 0;
    for (int i = 0; i < SIG_SIZE; ++i)
    {

        //printf("     %s, %i, counter: %i, file: %hhu, sig: %hhu\n", "Inside cmp", i, counter, file[i], sig[i]);
        if ( file[i] == sig[i])
        {
            counter++;
        }
    }

    if (counter == SIG_SIZE)
    {
        return 0;
    }else{
        return -1;
    }

}

/*This function returns file size if successful, or else it returns -1(FAIL).*/

ssize_t load_file(FILE* fd, char** mem){ /*mem needs to be free later*/

    long size;
    long alloc;
    int ret_value_1;
    int ret_value_2;
    long read;


    ret_value_1 = fseek(fd, 0, SEEK_END);
    size = ftell(fd);
    alloc = size + 1;
    ret_value_2 = fseek(fd, 0, SEEK_SET); /*Reset the file discriptor*/




    *mem = (char*) calloc(alloc, sizeof(char)); /*alloc is size + 1, including NULL terminator*/
    read = fread(*mem, sizeof(char), size, fd); /*return size_t*/

    //printf("read: %i, size: %i\n", (int)read, (int)size);


    if (*mem == NULL || (read != size))
    {
        printf("%s\n", "Error happens within load_file");
        return -1;
    }

    if (ret_value_1+ret_value_2 == 0)
    {
        //printf("Alloc: %i\n", (int)alloc);
        return alloc;

    }else{
        return FAIL;
    }

}


void extrac_signature(char* read, signature_t* signature){ /*store signature to the parameter signature_t** signature*/
    //extracting name

    /*counting the length of name*/
    int index = 0;
    while(read[index] != 0){
        if(read[index] != ','){
            index++;
        }else{
            break;
        }
    }

    /*Calloc memory for signature, these pointers will be assigned to members of signature.
    Since we implemented database.txt, the length of the signature is can be set by us, default is 11(SIG_SIZE)*/
    char* name = calloc(index, 1);
    char* sig  = calloc(SIG_SIZE, 1);

    for (int i = 0; i < (index); ++i)
    {
        name[i] = read[i];
    }



    index = index + 2;
    char* hex_string = read + index+1; /*since there're two spaces between , and first char of hex string.*/

    //printf("HEX STRING: %s\n", hex_string);
    //int length = strlen(hex_string);


    int i = 0;
    int i_s = 0;
    while( i_s < SIG_SIZE){
        char temp[2];
        temp[0] = *(hex_string+i);
        i++;
        temp[1] = *(hex_string+i);
        i++;

        /*long int strtol(const char *str, char **endptr, int base)

        converts the initial part
        of the string in str to a long int value according to the given base,
        which must be between 2 and 36 inclusive, or be the special value 0.

            str − This is the string containing the representation of an integral number.

            endptr − This is the reference to an object of type char*, whose value is set by the function
                     to the next character in str after the numerical value.

            base − This is the base, which must be between 2 and 36 inclusive, or be the special value 0.
        */
        char* ptr;
        long v = strtol(temp, &ptr, 16);
        sig[i_s] = (char)v;
        //printf("char[2]: %s/ int v: %i/ sig: %hhu\n", temp, (int)v, sig[i_s]);
        i_s++;

        temp[0] = *(hex_string+i);
        i++;
        temp[1] = *(hex_string+i);
        i++;
        v = strtol(temp, &ptr, 16);
        sig[i_s] = (char)v;
        i_s++;

        i++;
    }

    //printf("Name: %s\n", name);
    (*signature).name = name;
    (*signature).sig = sig;

}

