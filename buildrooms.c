#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

char dirname[250]; // To hold hte name of the directory

// The 10 possible room names
char* possibleRooms[10] = { "Red", "Green", "Blue", "Yellow", "Gold", "Silver", "Crystal", "Ruby", "Sapphire", "Emerald" };
char* chosenRooms[7]; // To hold the 7 randomly selected room names

int roomConnections[7][6]; // To hold the indices of the connections for each room
int numConnections[] = { 0, 0, 0, 0, 0, 0, 0 }; // To hold the number of connections for each room

// Function to create the directory
void createDirectory() {
	sprintf(dirname, "lopianl.rooms.%d", getpid()); // Appends the pid to the directory name
	mkdir(dirname, 0700); // Creates the directory and gives it permissions
}

// Function to create the room files
void createFiles() {
	FILE *f;
	char type[12]; // To hold the room type
	int a = 0, b, start, end;

	start = (rand() % (6 - 0 + 1)) + 0; // Selects a random index for the start room

	// Selects a random index for the end room; loops until it is different from the start room index
	do {
		end = (rand() % (6 - 0 + 1)) + 0;
	} while(end == start);

	chdir(dirname); // Opens the relevant directory

	// Loops for each of the 7 rooms
	while(a < 7) {
		memset(type, 0, sizeof type);

		// For the start room
		if(a == start) {
			strcpy(type, "START_ROOM"); // Sets the room type
			f = fopen("fstart", "a"); // Opens the file
		}
		// For all other rooms
		else {
			if(a == end) {
				strcpy(type, "END_ROOM"); // Sets the room type for the end room
			}
			else {
				strcpy(type, "MID_ROOM"); // Sets the room type for the mid rooms
			}

			f = fopen(chosenRooms[a], "a"); // Opens the file
		}

		fprintf(f, "ROOM NAME: %s\n", chosenRooms[a]); // Prints the name of the room to the file

		b = 0;

		// Loops for each connection of the room and writes the connection to the file
		while(b < numConnections[a]) {
			fprintf(f, "CONNECTION %d: %s\n", b+1, chosenRooms[roomConnections[a][b]]);
			b++;
		}

		fprintf(f, "ROOM TYPE: %s\n", type); // Writes the room type to the file

		fclose(f); // Closes the file

		a++;
	}
}

// Checks whether each room has at least 3 connections
int IsGraphFull() {
	int i = 0;

	// Loops for each room
	while(i < 7) {
		if(numConnections[i] < 3) {
			return 0; // Returns 0 if the room has less than 3 connections
		}

		i++;
	}

	return 1; // Returns 1 if all rooms have at least 3 connections
}

// Checks for already existing connections between rooms
int ExistingConnection(int r1, int r2) {
	int i = 0;

	while(i < numConnections[r1]) {
		if(roomConnections[r1][i] == r2) {
			return 1; // Returns 1 if the rooms are already connected
		}

		i++;
	}

	i = 0;

	while(i < numConnections[r2]) {
		if(roomConnections[r2][i] == r1) {
			return 1;
		}

		i++;
	}

	return 0; // Returns 0 if the rooms are not connected
}

// Function to check that the rooms do not already have 6 connections
int CanConnect(int r1, int r2) {
	if((numConnections[r1] == 6) || (numConnections[r2] == 6)) {
		return 0; // Returns 0 if either room already has 6 connections
	}
	else {
		return 1; // Otherwise, returns 1
	}
}

// Function to connect 2 rooms
void AddRandomConnection() {
	int roomA, roomB;

	// Selects 2 random rooms to connect. Checks that the rooms are different,
	// that both rooms can connect, and that they are not already connected.
	// Loops until it finds 2 rooms that can be connected.
	do {
		roomA = (rand() % (6 - 0 + 1)) + 0;
		roomB = (rand() % (6 - 0 + 1)) + 0;
	} while((roomA == roomB) || (CanConnect(roomA, roomB) == 0) || (ExistingConnection(roomA, roomB) == 1));

	roomConnections[roomA][numConnections[roomA]] = roomB; // Connects Room A to Room B
	numConnections[roomA] = numConnections[roomA] + 1; // Increments the number of connections for Room A

	roomConnections[roomB][numConnections[roomB]] = roomA; // Connects Room B to Room A
	numConnections[roomB] = numConnections[roomB] + 1; // Increments the number of connections for Room B
}

// Function to connect 2 rooms
void selectConnections() {
	// Loops until all rooms have at least 3 connections
	while(IsGraphFull() == 0) {
		AddRandomConnection(); // Calls function to add random connection
	}
}

// Function to select 7 of the 10 room names
void selectRooms() {
	int numRooms = 0;

	// Loops until 7 rooms have been chosen
	while(numRooms < 7) {
		int idx = (rand() % (9 - 0 + 1)) + 0; // Generates a random index from the 10 potential room names

		int i = 0;
		int alreadyChosen = 0;

		// Loops through each of the already selected room names to check if the room has already been chosen
		while((i < numRooms) && (alreadyChosen == 0)) {
			if(chosenRooms[i] == possibleRooms[idx]) {
				alreadyChosen = 1; // To break out of this loop if the room has already been selected
			}
			i++;
		}

		// If the room has not already been selected
		if(alreadyChosen == 0) {
			chosenRooms[numRooms] = possibleRooms[idx]; // Adds the room to the chosen rooms
			numRooms++; // Increments the number of chosen rooms
		}
	}
}

// Function to choose the rooms and connectiosn, and create the room files
void generateRooms() {
	selectRooms();

	selectConnections();

	createFiles();
}

// Main function
int main() {
	srand(time(0)); 

	createDirectory();

	generateRooms();

	/*
	int a = 0, b;

		
	while(a < 7) {
		printf("\nRoom %d: %s\n", a+1, chosenRooms[a]);
		b = 0;
		
		while(b < numConnections[a]) {
			printf("%s\n", chosenRooms[roomConnections[a][b]]);

			b++;
		}
	
		a++;
	
		printf("\n");
	}
	*/

	return 0;
}
