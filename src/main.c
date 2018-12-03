#include <const.h>
#include <whitelist.h>


//#define BUFFER_LENGTH 512
//static char receive[BUFFER_LENGTH];


static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream){
        size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
        return written;
}


int main(int argc, char const *argv[], char **envp)
{
    /* code */


        const char *load="-load";
        const char *unload="-unload";
        const char *update="-update";
        int ret = 0;
        const char *scan="-scan";
        //const char *kscan="-kscan"; /*not used*/
        CURL *curl_handle;

        FILE* pagefile;
        FILE* fp;
    
        static const char *signatureUrl = "https://raw.githubusercontent.com/Weifelin/DB/master/db.txt";
        static const char *whitelistUrl = "http://35.231.146.204/whitelist.db";

        const char *usage = "\nUsage: \n"
                            "-load  load the kernal module.\n"
                            "-unload  unload the kernal module.\n"
                            "-update  update virus database and whitelist database.\n"
                            "-scan  on-demand scan\n" ;


        if (argc == 2)
        {
            if(strcmp(argv[1],load)==0){
                printf("%s\n", "loading module....");

                fp=popen(LOAD,"r");


                if(fp == NULL){
                    printf("loading 2 failed");
                    exit(-1);
                }
                printf("%s\n","module is loaded now, check it with dmesg cmd");
                pclose(fp);
            }else if(strcmp(argv[1],unload)==0){
                fp= popen(UNLOAD,"r");
                if(fp == NULL){
                    printf("error unloading module");
                    exit(-1);
                }
                printf("%s\n", "module unloaded. check with dmesg!");
                pclose(fp);
            }else if(strcmp(argv[1],update)==0){

                printf("%s\n", "updating database....");

                //curl_handle = curl_easy_init();
                curl_global_init(CURL_GLOBAL_ALL);
                /* init the curl session */
                curl_handle = curl_easy_init();
                /* set URL to get here */
                curl_easy_setopt(curl_handle, CURLOPT_URL, signatureUrl);
                /* Switch on full protocol/debug output while testing */
                curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
                /* disable progress meter, set to 0L to enable and disable debug output */
                curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
                /* send all data to this function  */
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                /* open the file */
                pagefile = fopen(DATABASE, "wb");
                if(pagefile) {
                    /* write the page body to this file handle */
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
                    /* get it! */
                    curl_easy_perform(curl_handle);
                    /* close the header file */
                    fclose(pagefile);
                }

                printf("%s\n", "signature file updated...");
                /* set URL to get here */
                curl_easy_setopt(curl_handle, CURLOPT_URL, whitelistUrl);
                /* Switch on full protocol/debug output while testing */
                curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
                /* disable progress meter, set to 0L to enable and disable debug output */
                curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
                /* send all data to this function  */
                curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
                /* open the file */
                pagefile = fopen(WHITELISTFILE, "wb");
                if(pagefile) {
                    /* write the page body to this file handle */
                    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
                    /* get it! */
                    curl_easy_perform(curl_handle);
                    /* close the header file */
                    fclose(pagefile);
                }
                printf("%s\n", "whitelist file updated...");
                /* cleanup curl stuff */
                curl_easy_cleanup(curl_handle);
                curl_global_cleanup();
                printf("%s\n", "done updating database");
            }
            else{
                printf("Error: incorrect usage.\n%s\n", usage);
            }


        }else if (argc == 3)
        {
            if (strcmp(argv[1], scan) == 0)
            {

                //printf("%s\n", "on-demand scan ivoked....");
                fprintf(stdout, "on-demand scan ivoked...for : %s\n", argv[2]);
                ret = on_demand(argv[2]);

            }else{
                 printf("Error: incorrect usage.\n%s\n", usage);
            }
        }else{
                printf("Error: incorrect usage.\n%s\n", usage);
        }






/*  
        for (char **env = envp; *env != 0; env++)
        {
            char *thisEnv = *env;
            printf("%s\n", thisEnv);    
        }


        printf("END of envp\n" );*/
        //files and urls
        /*static const char *whitelistfilename = "whitelist.out";
        static const char *signaturefilename = "signature.out";
        static const char *signatureUrl = "http://35.231.146.204/signature.db";
        static const char *whitelistUrl = "http://35.231.146.204/whitelist.db";
        //usage printout
        const char *usage = "\nUsage: \n"
                            "-load  load the kernal module.\n"
                            "-unload  unload the kernal module.\n"
                            "-update  update virus database and whitelist database.\n"
                            "-scan  on-demand scan\n" ;
*/
        //var
        /*FILE *fp;
        CURL *curl_handle;
        FILE *pagefile;
        unsigned char **sigs;       //save all the sigs in heap
        int *sigs_length;
        int sigNumber=0;
        int status;

        char *address;
        int buffer_size = 100;
        char buffer[buffer_size];
        // should be change when module is ready
        char *moduleName = "hack_open.ko";
*/

        //testing
        //int BUFSIZ = 1024;



        // if(argc == 2 ){
        //     if(strcmp(argv[1],load)==0){
        //         printf("%s\n", "loading module....");
        //         printf("%s\n", "1.get sys_call_table address....");
        //         fp=popen("sudo cat /boot/System.map-*-generic| grep sys_call_table | awk '{print $1}'","r");
        //         if(fp == NULL){
        //             printf("step 1 failed");
        //             exit(-1);
        //         }
        //         address = malloc(sizeof( char)*buffer_size);
        //         fgets(buffer,buffer_size,fp);
        //         strcpy(address,buffer);
        //         printf("++address: %s\n", buffer );
        //         fclose(fp);
        //         printf("%s\n", "2.loading module");
        //         printf(" loading module : %s\n", moduleName);
        //         fp=popen("sudo insmod hack_open.ko","r");
        //         if(fp == NULL){
        //             printf("step 2 failed");
        //             exit(-1);
        //         }
        //         printf("%s\n","module is loaded now, check it with dmesg cmd");
        //         fclose(fp);
        //     }else if(strcmp(argv[1],unload)==0){
        //         fp= popen("sudo rmmod hack_open","r");
        //         if(fp == NULL){
        //             printf("error unloading module");
        //             exit(-1);
        //         }
        //         printf("%s\n", "module unloaded. check with dmesg!");
        //         fclose(fp);
        //     }else if(strcmp(argv[1],update)==0){
        //         printf("%s\n", "updating database....");
        //         curl_handle = curl_easy_init();
        //         curl_global_init(CURL_GLOBAL_ALL);
        //         /* init the curl session */
        //         curl_handle = curl_easy_init();
        //         /* set URL to get here */
        //         curl_easy_setopt(curl_handle, CURLOPT_URL, signatureUrl);
        //         /* Switch on full protocol/debug output while testing */
        //         curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
        //         /* disable progress meter, set to 0L to enable and disable debug output */
        //         curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
        //         /* send all data to this function  */
        //         curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        //         /* open the file */
        //         pagefile = fopen(signaturefilename, "wb");
        //         if(pagefile) {
        //             /* write the page body to this file handle */
        //             curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        //             /* get it! */
        //             curl_easy_perform(curl_handle);
        //             /* close the header file */
        //             fclose(pagefile);
        //         }
        //         printf("%s\n", "signature file updated...");
        //         /* set URL to get here */
        //         curl_easy_setopt(curl_handle, CURLOPT_URL, whitelistUrl);
        //         /* Switch on full protocol/debug output while testing */
        //         curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
        //         /* disable progress meter, set to 0L to enable and disable debug output */
        //         curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
        //         /* send all data to this function  */
        //         curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data);
        //         /* open the file */
        //         pagefile = fopen(whitelistfilename, "wb");
        //         if(pagefile) {
        //             /* write the page body to this file handle */
        //             curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, pagefile);
        //             /* get it! */
        //             curl_easy_perform(curl_handle);
        //             /* close the header file */
        //             fclose(pagefile);
        //         }
        //         printf("%s\n", "whitelist file updated...");
        //         /* cleanup curl stuff */
        //         curl_easy_cleanup(curl_handle);
        //         curl_global_cleanup();
        //         printf("%s\n", "done updating database");
        //     }else{
        //         printf("Error: incorrect usage.\n%s\n", usage);
        //     }
        // }
        // else if(argc ==3){
        //     if(strcmp(argv[1],scan)==0){

        //         printf("%s\n", "on-demand scan....");
        //         sigs = processSig(sigs,&sigNumber,&sigs_length,signaturefilename);  //at this point sigs are in memory in ascii code
        //         scan_f(argv[2],sigs,sigs_length,sigNumber);

        //     }else if(strcmp(argv[1],scan)==0){

        //         printf("%s\n", "on-access scan....");
        //         sigs = processSig(sigs,&sigNumber,&sigs_length,signaturefilename);  //at this point sigs are in memory in ascii code
        //         scan_f(argv[2],sigs,sigs_length,sigNumber);
        //     }
        //     else{
        //         printf("Error: incorrect usage.\n%s\n", usage);
        //     }
        // }else{
        //     printf("Error: incorrect usage.\n%s\n", usage);
        // }




        /*if (strcmp(argv[1], scan) == 0)
        {

            //printf("%s\n", "on-demand scan ivoked....");
            fprintf(stdout, "on-demand scan ivoked...for : %s\n", argv[2]);
            ret = on_demand(argv[2]);

        }
        else if (strcmp(argv[1], kscan) == 0) //will only be used by kernal
        {


            printf("%s\n", "on_access invoked");
            //on_access(argv[2]);
            int ret = 0;
            uid_t euid = geteuid();
            seteuid(0);
            //int fd = open("/dev/antv_char", O_RDWR);
            int fd = -1;

            do{
                fd = open("/dev/antv_char", O_RDWR);
            }while (fd == -1);
            if (fd == -1)
            {
               
        
                fprintf(stderr, "Error when open /dev/antv_char. Error: %s\n", strerror(errno));
            }


            seteuid(euid);
            while(ret>=0){
                
                ret = read(fd, receive, BUFFER_LENGTH);   //reading filename.
                if (ret < 0){
                     perror("Failed to read the message from the device.");
                     printf("FILE:%s, RET: %i\n", receive, ret);
                }

                if (ret != 0)
                {
                    int result = on_demand(receive);
                    char stringToSend[1];
                    if (result == SUCCESS)
                    {
                        stringToSend[0] = 'S';
                    }else{
                        stringToSend[0] = 'F';
                    }
                    ret = write(fd, stringToSend, strlen(stringToSend)); // Send the string to the LKM
                    if (ret < 0){
                       perror("Failed to write the message to the device.");
                       return errno;
                    }

                }
                

            }
            seteuid(euid);


        }*/






    return ret;

}




int on_demand(const char* start_path){

    int re = SUCCESS;
    FILE* db = fopen(DATABASE, "r");
    char virus_name[MAX_NAME_LEN];


    if (is_dir(start_path))
    {
        DIR *dir;
        char path[1000];    /*filepath containing filename, need to scan.*/
        *path = 0;
        struct dirent *entry;
        dir = opendir(start_path);



        if(dir){
            while((entry = readdir(dir)) != NULL){
                if(strcmp(entry->d_name,".")!=0 && strcmp(entry->d_name,"..")!=0){
                    strcpy(path,start_path);
                    int last = strlen(path) -1;;
                    if (path[last] != '/')
                    {
                        strcat(path,"/");
                    }
                    //strcat(path,"/");
                    strcat(path,entry->d_name);
                    //scan_f(path,sigs,sigs_length,sigNumber);
                    /*check if it's in white list*/

                    printf("Path: %s\n", path);
                    int wc = whitelistCheck(path);
                    if (wc == 0) /*Not in the white list*/
                    {
                        /*Do scanning*/
                        FILE* fd = fopen(path, "rb");
                        if (fd == NULL)
                        {
                            printf("%s\n", "ERROR WHEN OPEN FILE");
                            fclose(db);
                            //exit(EXIT_FAILURE);
                            return FAIL;
                        }

                        int result = scan(fd, db, virus_name);
                        //fclose(fd);
                        if (result == SUCCESS)
                        {
                            if(*virus_name == 0){
                                printf("%s\n", "Virus NOT DETECTED, file is safe");
                            }else{
                                printf("%s is DETECTED, removing permission.\n", virus_name);

                                /*Rename*/
                                re = FOUND;
                                int ret = rename_and_remove_permission(path);
                                if (ret == FAIL)
                                {
                                    printf("%s\n", "Error when renaming and removing permission");
                                }
                            }
                        }else{
                            printf("%s\n", "FAILED");
                        }

                    }else{  /*In the while list or ERROR*/

                        if (wc == 1)
                        {
                            printf("-------------------------\n%s\n-------------------------\n", "file is in whitelist..");
                        }else {
                            printf("Whilelist Check Return CODE: %i\n", wc);
                        }

                    }

                }
            }
            closedir(dir);
        }
    }else{

        printf("Start Path: %s\n", start_path);
        int wc = whitelistCheck(start_path);
        if (wc == 0) /*Not in the white list*/
        {
            /*Do scanning*/
            FILE* fd = fopen(start_path, "rb");
            if (fd == NULL)
            {
                printf("%s\n", "ERROR WHEN OPEN FILE");
                fclose(db);
                return FAIL;
                //exit(EXIT_FAILURE);
            }

            int result = scan(fd, db, virus_name);
            //fclose(fd);
            if (result == SUCCESS)
            {
                if(*virus_name == 0){
                    printf("%s\n", "Virus NOT DETECTED, file is safe");
                }else{
                    printf("%s is DETECTED, removing permission.\n", virus_name);

                    /*Rename*/
                    re = FOUND;
                    int ret = rename_and_remove_permission(start_path);
                    if (ret == FAIL)
                    {
                        printf("%s\n", "Error when renaming and removing permission");
                    }
                }
            }else{
                printf("%s\n", "FAILED");
            }

        }else{  /*In the while list or ERROR*/

            if (wc == 1)
            {
                printf("-------------------------\n%s\n-------------------------\n", "file is in whitelist..");
            }else {
                printf("Whilelist Check Return CODE: %i\n", wc);
            }

        }
    }



    fclose(db);
    return re;

}

int on_access(const char* path){
    int dd = open(DATABASE, SC_FG);
    FILE* db = fdopen(dd, "r");
    //FILE* db = fopen(DATABASE, "r");
    char virus_name[MAX_NAME_LEN];


    printf("Start Path: %s\n", path);
    int wc = whitelistCheck_A(path);
    if (wc == 0) /*Not in the white list*/
    {
        /*Do scanning*/
        int f = open(path, SC_FG);
        FILE* fd = fdopen(f, "rb");
        //FILE* fd = fopen(start_path, "rb");
        if (fd == NULL)
        {
            printf("%s\n", "ERROR WHEN OPEN FILE");
            fclose(db);
            //exit(EXIT_SUCCESS); /*got infected file and unable to open it is normal*/
            return FAIL;
        }

        int result = scan(fd, db, virus_name);
        //fclose(fd);
        if (result == SUCCESS)
        {
            if(*virus_name == 0){
                printf("%s\n", "Virus NOT DETECTED, file is safe");
            }else{
                printf("%s is DETECTED, removing permission.\n", virus_name);

                /*Rename*/
                int ret = rename_and_remove_permission(path);
                if (ret == FAIL)
                {
                    printf("%s\n", "Error when renaming and removing permission");
                }
            }
        }else{
            printf("%s\n", "FAILED");
        }

    }else{  /*In the while list or ERROR*/

        if (wc == 1)
        {
            printf("-------------------------\n%s\n-------------------------\n", "file is in whitelist..");
        }else {
            printf("Whilelist Check Return CODE: %i\n", wc);
        }

    }


    fclose(db);
    return SUCCESS;
}
/*From Vic,
    determine if the path is directory*/
bool is_dir(const char* path){
        struct stat buf;
        stat(path,&buf);
        return S_ISDIR(buf.st_mode);
}



int rename_and_remove_permission(const char* pathname){


    fprintf(stdout, "renameing...for : %s\n", pathname);

    char newname[PATH_MAX];
    *newname = 0;
    strcpy(newname, pathname);
    strcat(newname, ".infected");
    int ret = rename(pathname, newname);
    if (ret == -1)
    {
        return FAIL;
    }

    //remove permission
    /*Rise privillege*/
    uid_t euid = geteuid();
    seteuid(0);

    struct stat st;
    mode_t mode;
    mode = st.st_mode & 00000;
    stat(newname, &st);
    ret = chmod(newname, mode);
    seteuid(euid);

    if (ret == -1)
    {
        return FAIL;
    }



    return SUCCESS;
}
