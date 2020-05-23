# c_text_adventure_game

## Overview

This assignment is split up into two C programs (no other languages is allowed). The first program (hereafter called the "rooms program") will be contained in a file named "buildrooms.c", which when compiled with the same name (minus the extension) and run creates a series of files that hold descriptions of the in-game rooms and how the rooms are connected.

The second program (hereafter called the "game") will be called "adventure.c" and when compiled with the same name (minus the extension) and run provides an interface for playing the game using the most recently generated rooms.

In the game, the player will begin in the "starting room" and will win the game automatically upon entering the "ending room", which causes the game to exit, displaying the path taken by the player.

During the game, the player can also enter a command that returns the current time - this functionality utilizes mutexes and multithreading.

For this assignment, do not use the C99 standard: this should be done using raw C (which is C89). In the complete example and grading instructions below, note the absence of the -c99 compilation flag.

## Specifications

### Rooms Program

The first thing your rooms program must do is create a directory called "<USERNAME>.rooms.<PROCESS ID OF ROOMS PROGRAM>". Next, it must generate 7 different room files, which will contain one room per file, in the directory just created. You may use any filenames you want for these 7 room files, and these names should be hard-coded into your program.

The elements that make up an actual room defined inside a room file are listed below, along with some additional specifications:

A Room Name
A room name cannot be assigned to more than one room.
Each name can be at max 8 characters long, with only uppercase and lowercase letters allowed (thus, no numbers, special characters, or spaces). This restriction is not extended to the room file's filename.
You must hard code a list of ten different Room Names into your rooms program and have your rooms program randomly assign one of these to each room generated. Thus, for a given run of your rooms program, 7 of the 10 hard-coded room names will be used.

A Room Type
The possible room type entries are: START_ROOM, END_ROOM, and MID_ROOM.
The assignment of which room gets which type should be randomly generated each time the rooms program is run.
Naturally, only one room should be assigned the START_ROOM type, and only one room should be assigned the END_ROOM type. The rest of the rooms will receive the MID_ROOM type.

Outbound connections to other rooms
There must be at least 3 outbound connections and at most 6 outbound connections from this room to other rooms.
The oubound connections from one room to other rooms should be assigned randomly each time the rooms program is run.
Outbound connections must have matching connections coming back: if room A connects to room B, then room B must have a connection back to room A. Because of all of these specs, there will always be at least one path through.
A room cannot have an outbound connection that points to itself.
A room cannot have more than one outbound connection to the same room.

Each file that stores a room must have exactly this format, where the … is additional outbound room connections, as randomly generated:
ROOM NAME: <room name>
CONNECTION 1: <room name>
…
ROOM TYPE: <room type>

### The Game

Now let’s describe what should be presented to the player in the game. Once the rooms program has been run, which generates the room files, the game program can then be run. This program should present an interface to the player. Note that the room data must be read back into the program from the previously-generated room files, for use by the game. Since the rooms program may have been run multiple times before executing the game, your game should use the most recently created files: perform a stat() function call (Links to an external site.) on rooms directories in the same directory as the game, and open the one with most recent st_mtime component of the returned stat struct.

This player interface should list where the player current is, and list the possible connections that can be followed. It should also then have a prompt. Here is the form that must be used:

CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >

When the user types in the exact name of a connection to another room (Dungeon, for example), and then hits return, your program should write a new line, and then continue running as before.

If the user types anything but a valid room name from this location (case matters!), the game should return an error line that says “HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.”, and repeat the current location and prompt.

Trying to go to an incorrect location does not increment the path history or the step count. Once the user has reached the End Room, the game should indicate that it has been reached. It should also print out the path the user has taken to get there, the number of steps taken (not the number of rooms visited, which would be one higher because of the start room), a congratulatory message, and then exit.

When your program exits, set the exit status code to 0, and leave the rooms directory in place, so that it can be examined.

## Time Keeping

Your game program must also be able to return the current time of day by utilizing a second thread and mutex(es).

When the player types in the command "time" at the prompt, and hits enter, a second thread must write the current time in the format shown below (use strftime() (Links to an external site.) for the formatting) to a file called "currentTime.txt", which should be located in the same directory as the game. The main thread will then read this time value from the file and print it out to the user, with the next prompt on the next line. I recommend you keep the second thread running during the execution of the main program, and merely wake it up as needed via this "time" command. In any event, at least one mutex must be used to control execution between these two threads.

Here is an example of the "time" command being run in-game:

CURRENT LOCATION: XYZZY
POSSIBLE CONNECTIONS: PLOVER, Dungeon, twisty.
WHERE TO? >time

 1:03pm, Tuesday, September 13, 2016

WHERE TO? >

