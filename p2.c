/*
 Stefani Bekker
 John Carroll
 CS570
 Program 4
 Due: November 29th
 Driver program that will issue a prompt for the user. A variety of metacharacters are
 handled in this program, including <, >, &, >&, !!, |, >>, >>&, and !$. Four funtions are
 used, parse(), pipeExec(), myhandler(), and main(). Parse() uses getword() from program1
 in order to either read a file line by line or get a prompt from the command line from the
 user.  It then searches for metacharacters and sets flags if one is encountered so that
 main() knows how to handle each line. The program then does whatever the line
 of code asks of it or sends an error if something unexpected is read. It exits when EOF is reached
 or when the word "done" is encountered.
 */
#include "p2.h"
extern int BACKSLASHPIPE;
char realargv[MAXSTORAGE]; // used for !!
int prevWordCount = 0; // used for !!
char argv[MAXSTORAGE];
char *newargv[MAXITEM];
char *IN = NULL; //points to the in/out file in argv
char *OUT = NULL; //points to the in/out file in argv
char *OUTAMP = NULL; // detects >& and points to file
char *APPEND = NULL; // points to append file
char *APPENDAMP = NULL; // points to append ampersand file
int num_chars = 0; //will detect if there is a EOF character
int AMPFLAG = 0,INFLAG = 0,OUTFLAG = 0, OUTAMPFLAG = 0, EXCFLAG = 0, PIPEFLAG = 0, APPENDFLAG = 0, APPENDAMPFLAG = 0, COMMENTFLAG = 0, ENVFLAG = 0; //FLAGS for the <, >, &, >&, !!, |, >>, >>&, !$
int BANGFLAG;
int ARGFLAG = 0; // keeps track if the file is being seen as an argument
int o_file = 0, i_file = 0; //this is used for redirection
int k = 0; //This holds the second newargv position for pipe
int rowNum = 1; // keeps track of the line number
int BANGROW = 0; // tells which row the user wants to repeat using bang

// Parse Method
int parse(){
    // reset variables
    int p = 0;
    int ptr = 0;
    int word_count=0;
    IN = NULL;
    OUT = NULL;
    OUTAMP = NULL;
    APPEND = NULL;
    APPENDAMP = NULL;
    INFLAG = 0;
    OUTFLAG = 0;
    EXCFLAG = 0;
    OUTAMPFLAG = 0;
    PIPEFLAG = 0;
    APPENDFLAG = 0;
    APPENDAMPFLAG = 0;
    ENVFLAG = 0;
    BACKSLASHPIPE = 0;
    BANGROW = 0;
    
    while((num_chars = getword(argv + ptr))){
        // continues after !! and # is read and ignores other words in the same line
        if((EXCFLAG != 0 && num_chars != -1) || (COMMENTFLAG != 0 && ARGFLAG == 1)){
            continue;
        }
        if(num_chars == 0){
            break;
        }
        if(num_chars == -1){
            // keeps going if done is encountred but it isnt the first word in the line
            if((strcmp("done",(argv + ptr)) == 0) && word_count > 0){
                num_chars = 4;
            } else {
                break;
            }
        }
        if(AMPFLAG != 0){
            AMPFLAG = 0;
        }
        // pipe check
        if(argv[ptr] == '|'){
            if(BACKSLASHPIPE == 0){
                PIPEFLAG++;
                newargv[p++] = NULL;
                k= p;
                continue;
            }
        }
        //background check
        if(argv[ptr] == '&'){
            if((IN == NULL && OUT == NULL) && (INFLAG == 1 || OUTFLAG == 1)){
                break;
            }
            AMPFLAG++;
        }
        // comment symbol check
        if(argv[ptr] == '#' && num_chars == 1 && ARGFLAG == 1){
            COMMENTFLAG++;
            break;
        }
        // searches for >&
        if((argv[ptr] == '>' && num_chars == 2 && argv[ptr+1] == '&') || OUTAMPFLAG == 1){
            int next = ptr+1;
            if(argv[next] == '&' || OUTAMPFLAG == 1){
                OUTAMPFLAG++;
                if(OUTAMPFLAG == 2){
                    OUTAMP = argv+ptr;
                }
                ptr++;
            }
        }
        // searches for >>
        else if((argv[ptr] == '>' && num_chars == 2) || APPENDFLAG == 1){
            int next = ptr+1;
            if(argv[next] == '>' || APPENDFLAG == 1){
                APPENDFLAG++;
                if(APPENDFLAG == 2){
                    APPEND = argv+ptr;
                }
                ptr++;
            }
        }
        // searches for >>&
        else if((argv[ptr] == '>' && num_chars == 3) || APPENDAMPFLAG == 1 || APPENDAMPFLAG == 2){
            int next = ptr+1;
            if(argv[next] == '>' || APPENDAMPFLAG == 1){
                APPENDAMPFLAG++;
                next = ptr+1;
                if(argv[next] == '&' || APPENDAMPFLAG == 2){
                    APPENDAMPFLAG++;
                    if(APPENDAMPFLAG == 3){
                        APPENDAMP = argv+ptr;
                    }
                }
            }
            ptr++;
        }
        // searches for <
        else if((argv[ptr] == '<' && num_chars == 1) || INFLAG == 1){
            INFLAG++;
            if(INFLAG == 2)
                IN = argv +ptr;
        }
        // searches for >
        else if((argv[ptr] == '>' && num_chars == 1) || OUTFLAG == 1 ){
            OUTFLAG++;
            if(OUTFLAG == 2)
                OUT = argv+ptr;
        }
        // searches for !! or any other ! command
        else if(argv[ptr] == '!' && word_count == 0){
            if(EXCFLAG == 0){
                int next = ptr+1;
                if (argv[next] == '!'){
                    EXCFLAG = 1;
                    newargv[p++] =  realargv + ptr;
                    continue;
                } else{
                    BANGFLAG = 1;
                    if(argv[next] == '1'){
                        if(argv[next + 1] == '0'){
                            BANGROW = 10;
                        } else {
                          BANGROW = 1;
                        }
                    }
                    else if(argv[next] == '2'){
                        BANGROW = 2;
                    }
                    else if(argv[next] == '3'){
                        BANGROW = 3;
                    }
                    else if(argv[next] == '4'){
                        BANGROW = 4;
                    }
                    else if(argv[next] == '5'){
                        BANGROW = 5;
                    }
                    else if(argv[next] == '6'){
                        BANGROW = 6;
                    }
                    else if(argv[next] == '7'){
                        BANGROW = 7;
                    }
                    else if(argv[next] == '8'){
                        BANGROW = 8;
                    }
                    else if(argv[next] == '9'){
                        BANGROW = 9;
                    }
                    else if(argv[next] == '$'){
                        ENVFLAG++;
                    }
                    newargv[p++] =  realargv + ptr;
                    continue;
                }
            }
        }
        else {
            // realargv is used in order to make sure that the previous command is saved in case !! is encountred
            int i = 0;
            for(i = ptr; argv[i] != '\0'; i++){
                realargv[i] = argv[i];
            }
            newargv[p++] =  realargv + ptr;
        }
        argv[ptr+ num_chars] = '\0';
        realargv[ptr+ num_chars] = '\0';
        ptr += num_chars+1;
        word_count++;
        
    }
    if(EXCFLAG != 0 || BANGROW !=0){
        return prevWordCount;
    }
    // checks if & is the last character and deletes it from the array
    if(p > 0){
        int previous = p-1;
        if(strcmp(newargv[previous], "&") == 0){
            newargv[previous] = NULL;
            return (word_count - 1);
        }
    }
    newargv[p] = NULL;
    if(COMMENTFLAG == 0 || (COMMENTFLAG != 0 && num_chars > 0)){
        prevWordCount = word_count;
    }
    return word_count;
}
// method to handle SIGTERM
void myhandler(){};

// method to handle PIPE
void pipeExec(){
    int fildes[2];
    pid_t child, grandchild;
    fflush(stdout);
    fflush(stderr);

    //Child Fork
    child = fork();
    if (child == 0) {
        fflush(stdout);
        fflush(stderr);
        
        pipe(fildes);

        //Grandchild Fork
        grandchild = fork();
        if (grandchild == 0) {
            // for second of two child processes
            dup2(fildes[1],STDOUT_FILENO);
            close(fildes[0]);
            close(fildes[1]);
            // < code
            if(INFLAG != 0){
                dup2(i_file,STDIN_FILENO);
                close(i_file);
            }
            if((execvp(*newargv, newargv)) < 0){    //execute the command
                fprintf(stderr,"%s: Command not found.\n",newargv[0]);
                exit(9);
            }
        } // end of grandchild
        dup2(fildes[0],STDIN_FILENO);
        close(fildes[0]);
        close(fildes[1]);
        
        // > code
        if(OUTFLAG != 0){
            dup2(o_file,STDOUT_FILENO);
            close(o_file);
        }
        // >>& code
        if(APPENDAMPFLAG != 0){
            dup2(o_file,STDERR_FILENO);
            dup2(o_file,STDOUT_FILENO);
            close(o_file);
        }
        // >> code
        if(APPENDFLAG != 0){
            dup2(o_file,STDOUT_FILENO);
            close(o_file);
        }
        // >& code
        if(OUTAMPFLAG != 0){
            dup2(o_file,STDERR_FILENO);
            dup2(o_file,STDOUT_FILENO);
            close(o_file);
        }
        if((execvp(newargv[k], newargv+k)) < 0){    //execute the command
            fprintf(stderr,"%s: Command not found.\n",newargv[k]);
            exit(9);
        }
//        if(AMPFLAG !=0){    //when & you will place in background and set STDIN to /dev/null
//            printf("%s [%d]\n", *newargv , grandchild);
//            AMPFLAG = 0;
//        } else{    //if not background then wait till its done
//            for(;;){
//                pid_t pid;
//                pid = wait(NULL);
//                if(pid == grandchild)
//                    break;
//            }
//        }
    }
    // only the parent reaches this point in the code.
    
    // wait() might find the first child; continue reaping children until
    // the second child is found
    
    //dev/null code
    if(AMPFLAG != 0 && INFLAG == 0){        //This will send STDIN to /dev/null/
        int devnull;
        int flags = O_RDONLY;
        if((devnull = open("/dev/null",flags)) < 0){
            fprintf(stderr,"Failed to open /dev/null.\n");
            exit(9);
        }
        dup2(devnull,STDIN_FILENO);
        close(devnull);
    }
    if(AMPFLAG !=0){    //when & you will place in background and set STDIN to /dev/null
        printf("%s [%d]\n", *newargv , child);
        AMPFLAG = 0;
    }
    else{    //if not background then wait till its done
        for(;;){
            pid_t ppid;
            ppid = wait(NULL);
            if(ppid == child)
                break;
        }
    }
}

int main(int argc, char *argv[]){
    pid_t pid, child_pid;
    setpgid(0,0);
    argc = 0;
    (void) signal(SIGTERM,myhandler);
    
    // use contents of argv[1]
    if(argv[1] != NULL){
        int fp = 0;
        ARGFLAG = 1;
        fp = open(argv[1],O_RDONLY);
        dup2(fp, 0);
        close(fp);
    }
    
    for(;;){
        // prints the rownumber
        if(ARGFLAG == 0){
            printf("%%%d%% ", rowNum);
            rowNum++;
        }
        argc = parse();
        //This will terminate the program when getword sees EOF
        if(num_chars == -1 && argc == 0){
            break;
        }
        //Will re-issue prompt if no words entered
        if(argc == 0){
            COMMENTFLAG = 0;
            rowNum--;
            continue;
        }
        // if the first command is not valid
        if(newargv[0] == NULL){
            fprintf(stderr,"Invalid null command.\n");
            continue;
        }
        //cd code
        if(strcmp(newargv[0],"cd") == 0){
            //Too many arguments to cd
            if(argc > 2)
                fprintf(stderr,"cd: Too many arguments.\n");
            //cd was entered with no arguments
            else if(argc == 1){
                if(chdir(getenv("HOME")) != 0)
                    fprintf(stderr,"cd: Could not find home directory.\n");
            }
            //change directory to the argument
            else{
                if(chdir(newargv[1]) != 0)
                    fprintf(stderr,"%s: No such file or directory.\n",newargv[1]);
            }
            continue;
        }
        // ! code
        if(BANGFLAG != 0){
            if(BANGROW > rowNum){
                fprintf(stderr,"Please enter a valid row number.\n");
                continue;
            }
        }
        // > code
        if(OUTFLAG != 0){
            int flags = O_CREAT | O_EXCL | O_RDWR ;
            if(OUTFLAG > 2){
                fprintf(stderr,"Inconclusive input redirect.\n");
                continue;
            }
            if(OUT == NULL){
                fprintf(stderr,"Missing name for redirection file.\n");
                continue;
            }
            if((o_file = open(OUT,flags,S_IRWXU)) < 0){
                fprintf(stderr,"%s: File already exists.\n",OUT);
                OUTFLAG = 0;
                continue;
            }
        }
        // < code
        if(INFLAG != 0){
            int flags = O_RDONLY;
            if(INFLAG > 2){
                fprintf(stderr,"Inconclusive input redirect.\n");
                continue;
            }
            if(IN == NULL){
                fprintf(stderr,"Missing name for redirection file.\n");
                continue;
            }
            if((i_file = open(IN,flags)) < 0){
                fprintf(stderr,"Failed to open: %s\n",IN);
                continue;
            }
        }
        // >& code
        if(OUTAMPFLAG == 2){
            int flags = O_CREAT | O_EXCL | O_RDWR ;
            if((o_file = open(OUTAMP,flags,S_IRWXU)) < 0){
                fprintf(stderr,"%s: File already exists.\n",OUT);
                OUTFLAG = 0;
                continue;
            }
        }
        // >> code
        if(APPENDFLAG == 2){
            int flags = O_APPEND | O_RDWR;
            if(APPEND == NULL){
                fprintf(stderr,"Missing name for append file.\n");
                continue;
            }
            if((o_file = open(APPEND,flags,S_IRWXU)) < 0){
                fprintf(stderr,"%s: File doesnt exist.\n",APPEND);
                APPEND = 0;
                continue;
            }
        }
        // >>& code
        if(APPENDAMPFLAG == 3){
            int flags = O_APPEND | O_RDWR;
            if(APPENDAMP == NULL){
                fprintf(stderr,"Missing name for append file.\n");
                continue;
            }
            if((o_file = open(APPENDAMP,flags,S_IRWXU)) < 0){
                fprintf(stderr,"%s: File doesnt exist.\n",APPENDAMP);
                APPENDAMP = 0;
                continue;
            }
        }
        // | code
        if(PIPEFLAG != 0){
            if(PIPEFLAG > 1){
                fprintf(stderr,"Error '|': Must contain only ONE pipe.\n");
                continue;
            }
            else{
                pipeExec();
                continue;
            }
        }
        // fork code
        fflush(stdout);
        fflush(stderr);
        if((child_pid = fork()) < 0){
            printf("Terminating! Cannot Fork!");
            exit(1);
        }
        else if(child_pid == 0){
            //dev/null code
            //This will send STDIN to /dev/null/
            if(AMPFLAG != 0 && INFLAG == 0){
                int devnull;
                int flags = O_RDONLY;
                if((devnull = open("/dev/null",flags)) < 0){
                    fprintf(stderr,"Failed to open /dev/null.\n");
                    exit(9);
                }
                dup2(devnull,STDIN_FILENO);
                close(devnull);
            }
            // >>& code
            if(APPENDAMPFLAG != 0){
                dup2(o_file,STDERR_FILENO);
                dup2(o_file,STDOUT_FILENO);
                close(o_file);
            }
            // >> code
            if(APPENDFLAG != 0){
                dup2(o_file,STDOUT_FILENO);
                close(o_file);
            }
            // >& code
            if(OUTAMPFLAG != 0){
                dup2(o_file,STDERR_FILENO);
                dup2(o_file,STDOUT_FILENO);
                close(o_file);
            }
            // > code
            if(OUTFLAG != 0){
                dup2(o_file,STDOUT_FILENO);
                close(o_file);
            }
            // < code
            if(INFLAG != 0){
                dup2(i_file,STDIN_FILENO);
                close(i_file);
            }
            //execute the command
            if((execvp(newargv[0], newargv)) < 0){
                fprintf(stderr,"%s: Invalid command.\n",newargv[0]);
                exit(9);
            }
        }
        else{
            // test whether to wait or not
            // when & you will place in background and set STDIN to /dev/null
            if(AMPFLAG !=0){
                printf("%s [%d]\n", newargv[0] , child_pid);
                AMPFLAG = 0;
                continue;
            }
            //if not background then wait till its done
            else{
                for(;;){
                    pid = wait(NULL);
                    if(pid == child_pid)
                        break;
                }
            }
        }
    }
    killpg(getpgrp(),SIGTERM);
    if(ARGFLAG == 0){
       printf("p2 terminated.\n");
    }
    exit(0);
}


