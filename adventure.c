#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

char dirname[250]; // To hold the name of the directory
char start[12]; // To hold the name of the start room

int steps = 0; // The number of steps taken
char** path; // The path taken ot the end room

char** contents; // Holds the contents of a room file
int contSize; // The number of items in the file
char roomType[12]; // Holds the type of the room

// For the threading functionality
pthread_t tid;
pthread_mutex_t lock;

// Function to find the relevant directory
void openDirectory() {
        struct dirent *de;
        struct stat *dir = malloc(sizeof(struct stat));
        time_t mostRecent = 0, lastMod;
        DIR *dr = opendir("."); // Opens the current directory

	// Reads the contents of the directory
        while((de = readdir(dr)) != NULL) {
		// Checks for sub-directories starting with "lopianl.rooms."
                if(strstr(de->d_name, "lopianl.rooms.") != NULL) {
                        stat(de->d_name, dir);
                        lastMod = dir->st_mtime; // The time the directory was last modified

			// Checks if it has been modified more recently than other directories; stores info if it has
                        if(lastMod > mostRecent) {
                                mostRecent = lastMod;

                                memset(dirname, 0, sizeof dirname);
                                strcpy(dirname, de->d_name);
                        }
                }
        }

        closedir(dr); // Closes the current directory
}

// Function to read from the "currentTime.txt" file
void readTime() {
        FILE *fptr = fopen("currentTime.txt", "r"); // Opens the file
        char c;
        char s[500];
        memset(s, 0, sizeof(s)); 

	// Reads the contents of the file
        c = fgetc(fptr);
        while(c != EOF) {
                s[strlen(s)] = c; // Stores the contents in the character array
                s[strlen(s)+1] = '\0';

		c = fgetc(fptr);
        }

        printf("%s\n\n", s); // Writes the contents to the console

        fclose(fptr); // Closes the file
}

// Function to write the date and time to the "currentTime.txt" file
void* writeTime(void *arg) {
        pthread_mutex_lock(&lock); // Sets the mutex lock

        FILE *fptr = fopen("currentTime.txt", "w"); // Opens the file

        char buffer[100];       
        time_t rawtime; // The current time
        struct tm * timeinfo; 

        time(&rawtime);
        timeinfo = localtime(&rawtime);

	// Puts the date and time in the correct format
        strftime(buffer, sizeof(buffer), "%I:%M%P, %A, %B %d, %Y", timeinfo); 

	// Prints the date and time to the file
        fprintf(fptr, "%s", buffer);

	// Closes the file
        fclose(fptr);

	// Unlocks the mutex lock
        pthread_mutex_unlock(&lock);

        return NULL;
}

// Function to set up the threading functionality
void threadFunc() {
        pthread_mutex_init(&lock, NULL);
        pthread_create(&(tid), NULL, &writeTime, NULL);
        pthread_join(tid, NULL);
        pthread_mutex_destroy(&lock); // Destroys the lock when finished
}

// Function to write the time to a file and retrive it from that file
void getTime() {
	threadFunc(); // Calls the threadFunc() function, which creates the second thread
	readTime(); // Calls the readTime() function in the first thread
}

// Function to free the contents and path arrays
void freeMem(char** f) {
	int n;

	if(f == contents) {
		n = contSize;
	}
	else {
		n = steps;
	}

	int i = 0;

	// Loop to free each element of the array
	while(i < n) {
		free(f[i]);

		i++;
	}

	free(f); // Frees the array
}

// Function to print the results of the game
void endGame() {
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
        printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);

        int idx = 0;

        while(idx < steps) {
                printf("%s\n", path[idx++]);
        }

	freeMem(path); // Frees the path array
}

// Function to check if the current room is the end room
int isEndRoom() {
        if(strcmp(roomType, "END_ROOM") == 0) {
                return 1; // Returns 1 if it is the end room
        }
        else {
                return 0; // Otherwise, returns 0
        }
}

// Function to check that the user entered a valid connection room
int isValidConnection(char name[250]) {
        int idx = 1; // Index 1 is the first connection

        while(idx < contSize-1) { // 2nd to last index is the last connection
                if(strcmp(contents[idx], name) == 0) {
                        return 1; // Returns 1 if the connection name is valid
                }

                idx++;
        }

        return 0; // Returns 0 if the connection mame is invalid
}

// Function to print the information for the current room
void printInfo() {
        printf("CURRENT LOCATION: %s\n", contents[0]);

        int idx = 1;

        printf("POSSIBLE CONNECTIONS: ");
        while(idx < contSize-1) {
                printf("%s", contents[idx]);

                if(idx != contSize-2) {
                        printf(", ");
                }
                else {
                        printf(".\n");
                }

                idx++;
        }
}

// Function to get the next connection from the user
char* getNextRoom() {
        static char input[250];
        int validRoom = 0;

	// Loops until the user enters a valid connection name
        while(validRoom == 0) {
                memset(input, 0, sizeof(input));

                printf("WHERE TO? >");
                scanf("%s", input);

		printf("\n");

		// Calls the getTime() function if the user enters "time"
                if(strcmp(input, "time") == 0) {
                        getTime();
                }
		// If the user doesn't enter "time" or a valid connection name
                else if(isValidConnection(input) == 0) {
                        printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
                        printInfo(); // Reprints the room information
                }
                else {
                        validRoom = 1; // To exit the loop if the connection name is valid
                }
        }

        return input; // Returns the valid connection name
}

// Function to get the room information from its file
void roomInfo(char name[250]) {
        int store = 0;
        char s[250];
        FILE *fptr;
        char fpath[250];
        char c;
        contSize = 0;
        contents = malloc(15 * sizeof(char*));

        memset(fpath, 0, sizeof(fpath));
        sprintf(fpath, "./%s/%s", dirname, name); // Creates the file path

        fptr = fopen(fpath, "r"); // Opens the file

	// Loop to read the contents of the file
        c = fgetc(fptr);
        while(c != EOF) {
		// Only stores the necessary information
                if(c == '\n') {
                        contents[contSize] = malloc(100 * sizeof(char));
                        strcpy(contents[contSize++], s);

                        memset(s, 0, sizeof(s));
                        store = 0;
                }

		// Copies the necessary information to the character array
                if(store == 2) {
                        s[strlen(s)] = c;
                        s[strlen(s)+1] = '\0';
                }

		// Counts the number of spaces, so we know when we need to store information
                if(c == ' ') {
                        store++;
                }

                c = fgetc(fptr);
        }

        strcpy(roomType, contents[contSize-1]); // Copies the room type to the roomType variable

        fclose(fptr); // Closes the file
}

// Function to start gameplay
void game() {
        int endRoom = 0;

        char* nextRoom;

        path = malloc(1000 * sizeof(char*));

	// Loops until the end room is reached
        while(endRoom == 0) {
		// Calls the roomInfo() function with the name of the room
                if(steps > 0) {
                        roomInfo(nextRoom);
                }
		// For the first room, to ensure the start room file is opened
                else {
                        roomInfo("fstart");
                        strcpy(start, contents[0]); // Copies the name of the start room to the "start" variable
                }

		// If on the end room
                if(isEndRoom() == 1) {
                        endGame(); // Calls the endGame() function
                        endRoom = 1; // Sets the endRoom variable to exit the loop
                }
		// If not on the end room
                else {
                        printInfo(); // Prints the room information

                        nextRoom = getNextRoom(); // Gets the next room from the user

                        path[steps] = malloc(100 * sizeof(char));
                        strcpy(path[steps], nextRoom); // Adds the name of the next room to the path array if it is a valid connection
                        steps++; // Increments the number of steps taken

			// Ensures the correct file is opened if the user is returning to the start room
                        if(strcmp(nextRoom, start) == 0) {
                                strcpy(nextRoom, "fstart");
                        }
                }

		freeMem(contents); // Frees the contents of the file
        }
}

// Main function
int main() {
        openDirectory(); // Gets latest directory first
        game(); // Then starts the game

        return 0;
}
