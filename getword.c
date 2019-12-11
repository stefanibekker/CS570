/*
 Stefani Bekker
 Program 4
 CS570 Carroll
 References: C by Discovery Foster, Ch2 inout2.c
 getword() is a simple lexical analyzer that expands on Program 0. Program 1 adds metacharacters to the getword() function. Once EOF is encountered, a -1 is returned. When a newline is encountered, a 0 is returned. Otherwise, the value returned is the length of the "word" that is encountered. This program skips leading spaces, and the backslash character is seen as a special character that changes the behavior of the character which is following it. The metacharacters "<", ">", "|", "&", and "#" result in a return value of 1. The other metacharacters that are accepted are ">&" and ">>&". If the word that is read is longer than the size of STORAGE-1, then the function only uses the first bytes.
 I had to modify this code from the original to include an external variable for the flag BACKSLASHPIPE in order to
 handle the different situations \| and |.
 */

#include <stdio.h>
#include <string.h>
#include "getword.h"

int BACKSLASHPIPE = 0;

int getword(char *w){
    int wordSize = 0;
    int iochar;
    int backslash = 0;
    while ((iochar = getchar()) != EOF){
        //makes sure that the arrays storage doesnt get full
        if(wordSize == 254){
            ungetc(iochar,stdin);
            w[wordSize] = '\0';
            return wordSize;
        }
        
        // handles a backslash
        if( iochar == '\\'){
                iochar = getchar();
                backslash = 1;
        }
        if(iochar == 124 && backslash == 1){
            BACKSLASHPIPE = 1;
        }
        //gets rid of extra space characters
        if(wordSize == 0 && iochar == 32){
            if(backslash == 1){
                w[wordSize] = iochar;
                wordSize++;
                break;
            } else {
                continue;
            }
        }
        // if there is a new line right after a valid character, the new line gets used up.
        // this puts the newline back onto the input stream so it can be seen again.
        if(iochar == 10 && wordSize != 0){
            ungetc(iochar, stdin);
            break;
        }
        // handles space
        else if(iochar == 32){
            // if there is a backslash before the space, it gets added to the array to print
            // and doesnt ignore it.
            if(backslash == 1){
                w[wordSize] = iochar;
                wordSize++;
                backslash = 0;
            } else {
                break;
            }
        }
        // handles new line
        else if(iochar == 10){
            break;
        }
        // handles metacharacters with and without a backslash
        else if(iochar == 60 || iochar == 62 || iochar == 38 || iochar == 124){
            if(wordSize == 0){
                w[wordSize] = iochar;
                wordSize++;
                if(backslash == 1){
                    backslash = 0;
                    continue;
                }
                if(iochar == 62 && backslash != 1){
                    iochar = getchar();
                    // >>&
                    if(iochar == 62){
                        w[wordSize] = iochar;
                        wordSize++;
                        iochar = getchar();
                        if(iochar == 38){
                            w[wordSize] = iochar;
                            wordSize++;
                            break;
                        } else {
                            // if the character that was read is not what is needed for the metacharacter, then it needs to be
                            // unread in order to be seen again after the return.
                            ungetc(iochar, stdin);
                            break;
                        }
                        // >&
                    } else if(iochar == 38) {
                        w[wordSize] = iochar;
                        wordSize++;
                        break;
                    } else {
                        // if the character that was read is not what is needed for the metacharacter, then it needs to be unread
                        // in order to be seen again after the return.
                        ungetc(iochar, stdin);
                        break;
                    }
                } else {
                    break;
                }
            } else {
                // handles the backslash character with metacharaters, as it is just to be seen as a normal character if it
                // immedietly follows a backslash. Backslash is then set back to zero to avoid any confusion in case it is seen
                // again.
                if(backslash == 1){
                    w[wordSize] = iochar;
                    wordSize++;
                    backslash = 0;
                    continue;
                } else {
                    ungetc(iochar, stdin);
                    break;
                }
            }
        }
        // normal characters
        else{
            w[wordSize] = iochar;
            wordSize++;
            backslash = 0;
        }
    }
    //null terminates so there is no overlap in char array
    w[wordSize] = '\0';
    
    //checks if the word seen is the word "done"
    if(strcmp("done",w) == 0){
        wordSize = -1;
    }
    
    if(iochar == EOF){
        // if it is EOF right after a valid character, it needs to be seen again after
        // the word and word size are displayed to the user. this puts it back onto the
        // input stream
        if(wordSize != 0){
            ungetc(iochar, stdin);
            return wordSize;
        }
        wordSize = -1;
    }
    return wordSize;
}
