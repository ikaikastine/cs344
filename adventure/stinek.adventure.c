/*********************************************************************
** Program Filename: stinek.adventure.c
** Author: Kevin Stine
** Date: 11/5/2015
** Description: Program that plays an adventure game
** Input: Which room to go to
** Output: Current room, connections, prompt
*********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

struct Positions 
{
	char *start;
	char *connection;
	char *end;
};

/*********************************************************************
** Function: createDirectory
** Description: gets pID, appends to stinek.adventure., creates directory
** Parameters: None
** Pre-Conditions: None
** Post-Conditions: Directory has been created 
** Return: Directory has been created 
*********************************************************************/
char *createDirectory() 
{
	int pid = getpid();
	char *directory = malloc(30);
	char *dirName = "stinek.adventure.";

	sprintf(directory, "%s%d", dirName, pid); //Stores the pid at the end of directory

	//Creates the directory
	struct stat st = {0};
	if (stat(directory, &st) == -1) {
		mkdir(directory, 0755);
	}
	return directory; 
}

/*********************************************************************
** Function: createRooms
** Description: Creates 7 files with random rooms/connections/types
** Parameters: Directory where the files are placed 
** Pre-Conditions: Directory has been created
** Post-Conditions: Rooms have been placed in directory
** Return: Location
*********************************************************************/
struct Positions createRooms(char *directory) 
{
	struct Positions location;
	location.connection = directory;

	//Creates 10 room files
	char *room[10];
	room[0] = "Barracks";
	room[1] = "Prison";
	room[2] = "Library";
	room[3] = "Shrine";
	room[4] = "Crypt";
	room[5] = "Guardhouse";
	room[6] = "Laboratory";
	room[7] = "Armory";
	room[8] = "Bestiary";
	room[9] = "Garden";
	char *currentFile = malloc(100);
	int i, j, k;

	//Swaps files around so they aren't the same each game
	for (k = 0; k < 9; k++) {
		int swap = rand() % 7;
		char* temp = room[swap];
		char* testRoom = room[k];
		room[swap] = room[k];
		room[k] = temp;	
	}

	//Opens files with write access, writes room name
	for (i = 0; i < 7; i++) {
		sprintf(currentFile, "%s/%s", directory, room[i]);
		FILE *fp = fopen(currentFile, "w");
		if (fp == NULL) {
			perror ("Error opening file.");
		}
		else {
			fprintf(fp, "Room Name: %s\n", room[i]);
		}
		fclose(fp);
	}

	int initPos = rand() % 7; //Randomizes start position
	int endPos = rand() % 7; //Randomizes end position
	char *connection;
	int connections, idx;
	char currentRoom[100];

	for(i = 0; i < 7; i++) {
		sprintf(currentFile, "%s/%s", directory, room[i]);

		FILE *fp = fopen(currentFile, "a");
		if (fp == NULL) {
			perror ("Error opening file.");
		}
		else {
			connections = rand() % 4 + 3;
			idx = 0;
			for (j = 0; j < connections; j++) {
				connection = room[idx];
				if (connection == room[i]) {
					idx++;
					connection = room[idx];
				}
				fprintf(fp, "CONNECTION %d: %s\n", j+1, connection);
				idx++;
			}
			//Sets room type as start
			if (i == initPos) {
				fprintf(fp, "ROOM TYPE: START_ROOM\n");
				location.start = room[i];
			}
			//Sets room type as end
			else if (i == endPos) {
				fprintf(fp, "ROOM TYPE: END_ROOM\n");
				location.end = room[i];
			}
			//Sets all other room types as mid
			else {
				fprintf(fp, "ROOM TYPE: MID_ROOM\n");
			}
		}
		fclose(fp);
	}
	free(currentFile);
	return location;
}

/*********************************************************************
** Function: beginAdventure
** Description: Starts the adventure game
** Parameters: The created rooms
** Pre-Conditions: Directory and rooms have been created
** Post-Conditions: Game has run
** Return: None
*********************************************************************/
void beginAdventure(struct Positions game) 
{
	char *currentRoom = game.start; //Current room is set to start room
	char *finalRoom = game.end; //Final lroom is set to end room
	char *directory = game.connection;
	int stepCount = 0;
	int i, valid, lines, connections;
	char (*steps)[15] = malloc(200); //Set to 200 as a larger buffer for more turns
	char (*contents)[15] = malloc(200); //Allows for 13 turns. If more might dump core
	//char (*steps)[15] = malloc(sizeof *steps * 8);
	//char (*contents)[15] = malloc(sizeof *contents * 8);
	char destination[15];

	//Uncomment to always win
	//printf( "Final Room is: %s\n", finalRoom);

	char *currentFile = malloc(100);
	//Loop which checks if current room equals final room
	//This is the core of the game to determine if you reach the final room
	while (!(strcmp(currentRoom, finalRoom)) == 0) {
		sprintf(currentFile, "%s/%s", directory, currentRoom);
		FILE *fp = fopen(currentFile, "r");

		connections = 0;

		if (fp) {
			//Loops through room file and at each new line increments the connection count
			while ((lines = getc(fp)) != EOF) {
				if (lines == '\n') {
					connections++;
				}
			}
			//Since it increments on every line, subtract room name & room type
			connections = connections - 2;

			char roomName[20];
			char connectionName[20];

			fseek(fp, 11, SEEK_SET); //Sets file pointer to beginning of room name
			fgets(roomName, 15, fp); //Grabs name of room
			int len = strlen(roomName);
			//Remove newline character 
			if(roomName[len-1] == '\n') {
				roomName[len-1] = 0;
			}
			strcpy(contents[0], roomName);

			for (i = 1; i <= connections; i++) {
				fseek(fp, 14, SEEK_CUR); //Sets file pointer at beginning of connection
				fgets(connectionName, 20, fp); //Grabs name of connection
				len = strlen(connectionName);
				//Remove newline character
				//Without this, the connections would not be printed as required
				if(connectionName[len-1] == '\n') {
					connectionName[len-1] = 0;
				}
				strcpy(contents[i], connectionName);
			}
			valid = 0;

			//Loop which prints locations/connections until the end room is reached
			while (valid == 0) {
				printf("CURRENT LOCATION: %s\n", contents[0]);
				printf("POSSIBLE CONNECTIONS: ");
				for (i = 1; i <= connections; i++) {
					//Prints a period if last connection
					if (i == connections) {
						printf("%s.\n", contents[i]);
					}
					else {
						printf("%s, ", contents[i]);
					}
				}
				printf("WHERE TO? >");
				scanf("%s", destination); //Read input from user
				for (i = 1; i <= connections; i++) {
					if (strcmp(destination, contents[i]) == 0) {
						valid = 1;
						currentRoom = destination;
					}
				}
				if (valid != 1) {
					printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
				}
			}
		}
		printf("\n");
		strcpy(steps[stepCount], currentRoom);
		stepCount++;
		fclose(fp);
	}
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepCount);
	for (i = 0; i < stepCount; i++) {
		printf("%s\n", steps[i]);
	}
	free(steps);
	free(contents);
	free (currentFile);
}

int main ()
{
	srand(time(NULL)); //Initializes pseudo-random number generator 

	char *roomsDirectory = createDirectory(); //Stores directory 	

	struct Positions game = createRooms(roomsDirectory); //Stores rooms

	beginAdventure(game); //Let the fun begin

	return 0;
}