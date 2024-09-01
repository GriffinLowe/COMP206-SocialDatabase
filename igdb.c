#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include<math.h>

#include "database.h"

// Purpose: Checking if the given char * represents an integer
// Returns true if the char * is a valid int; false if not
int is_int(char * num) {

        char * endptr;
        strtol(num, &endptr, 10);

        if(*endptr == '\0') {
                return 0;
        }
        else {
                return -1;
        }
}

// Purpose: Checking if there exists a record in the specified database with the given handle
// Returns true if the handle is found; false if not
int is_handle(Database * db, char * handle) {

        Record * check = db_lookup(db, handle);

        if(check == NULL) {
                return -1;
        }
        else {
                return 0;
        }
}

// Purpose: Printing a record into a readable line
void print_record(Record * r) {

        // Handle:
        printf("%s", r->handle);
        for(int i = 0; i < 32 - strlen(r->handle); i ++) {
                printf(" ");
        }
        printf("|");

        // Followers:
        if(r->followerCount == 0) {
                printf("0           |");
        }
        else {
                printf("%d", r->followerCount);
                for(int i = 0; i < 12 - floor(log10(r->followerCount)); i ++) {
                        printf(" ");
                }
                printf("|");
        }

        // Date:
        char * time_str = ctime(r->dateMod);
        time_str[strlen(time_str) - 1] = '\0';
        printf("%s|", time_str);

        // Comment:
        printf("%s", r->comment);
        for(int i = 0; i < 64 - strlen(r->comment); i ++) {
                printf(" ");
        }
        printf("\n");
}

// Purpose: Checking if a given handle is valid
int validate_handle(Database * db, char * handle) {

        // Checking length:
        if(strlen(handle) <= 0) {
                printf("Error: handle must be comprised of at least 1 character\n");
                return -1;
        }
        else if(strlen(handle) >= 32) {
                printf("Error: handle too long\n");
                return -1;
        }
        // Each handle must start with @
        else if(handle[0] != '@') {
                printf("Error: handles must begin with \'@\'\n");
                return -1;
        }
        // Validating that only ASCII characters (excluding commas) are present:
        for(int i = 0; i < strlen(handle); i ++) {
                if(!isPrint(*(handle + i))) {
                        printf("Error: handle can consist only of printable ASCII characters\n");
                        return -1;
                }
                if(*(handle + i) == ',') {
                        printf("Error: handle cannot contain commas\n");
                        return -1;
                }
        }
        return 0;
}
// Purpose: Checking if a given comment is valid:
int validate_comment(char * comment) {

        // Checking length:
        if(strlen(comment) <= 0) {
                printf("Error: comment cannot be empty\n");
                return -1;
        }
        else if(strlen(comment) >= 64) {
                printf("Error: comment too long\n");
        }
        // Validating that only ASCII character (exclusing commas) are present:
        for(int i = 0; i < strlen(comment); i ++) {
                if(!isPrint(*(comment + i))) {
                        printf("Error: comment can consist only of printable ASCII characters\n");
                        return -1;
                }
                if(*(handle + i) == ',') {
                        printf("Error: comment cannot contain commas\n");
                        return -1;
                }
        }
        return 0;
}

}

int main_loop(Database * db) {

    // Vars needed to run the input loop:
    bool save_status = true;
    char * input = NULL;
    size_t len = 0;
    ssize_t read;

    // User input loop:
    while((read = getline(&input, &len, stdin) != -1) {

        // Obtaining primary argument:
        char * arg;
        * arg = strtok(line, " ");

        // add argument:
        if(strcmp(arg, "add")) {

                char * handle = strtok(NULL, " ")
                char * follower = strtok(NULL, " ");
                char * additional_args = strtok(NULL, " ");

                // Validating command line inputs:
                if(!validate_handle(handle)) {
                        return -1;
                }
                else if(follower == NULL) {
                        printf("Error: \"add\" command requires two additional arguments");
                        return -1;
                }
                else if(!is_int(follower)) {
                        printf("Error: followerCount is not an integer\n");
                        return -1;
                }
                else if(additional_args != NULL) {
                        printf("Error: too many arguments specified for the \"add\" command\n");
                        return -1;
                }

                // Obtaining and validating comment:
                char * comment = NULL;
                size_t comment_len = 0;

                printf("Please enter a comment (64 characters maximum): ");
                getline(&comment, &comment_len, stdin);

                if(!validate_comment(comment)) {
                        return -1;
                }

                // Executing the command:
                Record * new_record = malloc(sizeof(Record));
                strcpy(new_record->handle, handle);
                strcpy(new_record->comment, comment);
                new_record->dateMod = time(NULL);

                char * endptr;
                new_record->followerCount = strtol(follower, &endptr, 10);

                db_append(db, new_record);
                save_status = false;
                free(comment);

        }
        // update argument:
        else if(strcmp(arg, "update")) {

                char * handle = strtok(NULL, " ");
                char * follower = strtok(NULL, " ");
                char * additional_args = strtok(NULL, " ");

                // Validating command line input:
                if(!is_handle(db, handle)) {
                        return -1;
                }
                else if(follower == NULL) {
                        printf("Error: \"update\" command requires at least on additional argument");
                        return -1;
                }
                else if(!is_int(follower)) {
                        printf("Error: followerCount is not an integer");
                        return -1;
                }
                else if(additional_args != NULL) {
                        printf("Error: too many arguments specified for the \"update\" command");
                        return -1;
                }

                // Obtaining and validating comment:
                char * comment = NULL;
                size_t comment_len = 0;

                printf("Please enter a comment (64 characters maximum): ");
                getline(&comment, &comment_len, stdin);

                if(!validate_comment(comment)) {
                        return -1; }

                // Executing the update command:
                Record * cur = db_lookup(db, handle);
                strcpy(cur->comment, comment);
                cur->dateMod = time(NULL);

                char * endptr;
                cur->followerCount = strtol(follower, &endptr, 10);

                save_status = false;
                free(comment);
         }
        // list argument:
        else if(strcmp(arg, "list")) {

                printf("%-20s | %-10s | %-16s | %s\n", "HANDLE", "FOLLOWERS", "LAST MODIFIED", "COMMENT");
                printf("---------------------|------------|------------------|--------------------------\n");

                for(int i = 0; i < db->size; i ++) {

                        print_record(db->records[i]);
                }
    }
        // save argument:
        else if(strcmp(arg, "save")) {

                // Verifying command line arguments:
                char * additional_args = strtok(NULL, " ");
                if(additional_args != NULL) {
                        printf("Error: \"save\" can have no additional arguments\n");
                        return -1;
                }

                // Executing the save command:
                FILE * csv = fopen("database.csv", "wt");
                db_write_csv(db, "database.csv");
                fclose("database.csv");
                save_status = true;
    }
        // exit argument:
        else if(strcmp(arg, "exit")) {

                char * flag = strtok(NULL, " ");
                char * additional_args = strtok(NULL, " ");

                // Validating command line arguments:
                if(additional_args != NULL) {
                        printf("Error: \"exit\" command takes at most one additional argument");
                        return -1;
                }
                else if(!strcmp(flag, "fr") {
                        printf("Error: second argument not recognized");
                        return -1;
                }

                // Performing the command:
                if(!save_status) {
                        if(!strcmp(flag, "fr") {
                                printf("Error: there exists unsaved changes. Use \"exit fr\" to force exiting anyway.");
                                return -1;
                                }
                        else {
                                printf("Warning: you are exiting with unsaved changes.");
                                return 0;
                                }
                        }
                return 0;
        }
        // argument not found:
        else {
            printf("Error: command does not exist\nSupported Commands: \"add\", \"exit\", \"list\", \"save\", \"update\"");
            return -1;
        }
        free(input);
}

int main()
{
    Database db = db_create();
    db_load_csv(&db, "database.csv");
    return main_loop(&db);
}
                                              
