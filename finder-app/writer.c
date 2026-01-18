/*
    Author: Joseph Hargrave
    Date: 1/17/2026
    Requirements:
        Write a C application “writer” (finder-app/writer.c)  which can be used as an alternative to the “writer.sh” test script created in assignment1 and using File IO as described in LSP chapter 2.  See the Assignment 1 requirements for the writer.sh test script and these additional instructions:
    
        Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr

        Exits with value 1 error and print statements if any of the arguments above were not specified

        Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.

        One difference from the write.sh instructions in Assignment 1:  You do not need to make your "writer" utility create directories which do not exist.  You can assume the directory is created by the caller.

        Setup syslog logging for your utility using the LOG_USER facility.

        Use the syslog capability to write a message “Writing <string> to <file>” where <string> is the text string written to file (second argument) and <file> is the file created by the script.  This should be written with LOG_DEBUG level.

        Use the syslog capability to log any unexpected errors with LOG_ERR level.
*/

#include "stdio.h"
#include <syslog.h>
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// File flags: write only and create file if it does not exist
#define FILE_FLAGS O_WRONLY | O_CREAT
// File mode: User has read/write, groupd has read/write, everyone has read permissions
#define FILE_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH

int main(int argc, char *argv[])
{
    // Setup logger
    openlog(NULL, LOG_ODELAY, LOG_USER);

    // If missing writefile
    if (argc < 2)
    {
        syslog(LOG_ERR, "%s", "Error: Missing writefile argument!");
        return 1;
    }

    // If missing writestr
    if (argc < 3)
    {
        syslog(LOG_ERR, "%s", "Error: Missing writestr argument!");
        return 1;
    }

    // Create and open file
    int file_descriptor = open(argv[1], FILE_FLAGS, FILE_MODE);
    
    // Check that file opened correctly
    if (file_descriptor == -1)
    {
        syslog(LOG_ERR, "Error: Failed to open file '%s'!", argv[1]);
        return 1;
    }

    // Log write
    syslog(LOG_DEBUG, "Writing '%s' to '%s'", argv[2], argv[1]);

    // Write contents to file
    size_t write_length = strlen(argv[1]);
    ssize_t bytes_written = write(file_descriptor, argv[1], write_length);
    if (bytes_written == -1)
    {
        syslog(LOG_ERR, "Error: Failed to write to file '%s'!", argv[1]);
        return 1;
    }
    else if (bytes_written != write_length)
    {
        syslog(LOG_ERR, "Error: Failed to write all content to file '%s'!", argv[1]);
        return 1;
    }

    // Close file
    int close_return = close(file_descriptor);
    if (close_return == -1)
    {
        syslog(LOG_ERR, "Error: Failed to close file '%s'!", argv[1]);
        return 1;
    }
    
    // Close log
    closelog();
    
    return 0;
}