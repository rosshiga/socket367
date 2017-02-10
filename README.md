
#EE 367L Lab 3

##Contributors:
Ross Higa, Jared Shimabukuro

##How to Automatically Compile and Run the Programs
- Run command ```./run.sh```
	- This will automatically compile *server.c* and *client.c* as well as run all of the executables.

##How to Manually Compile the Programs:
- Run command ```make```
	- This will generate executables ```server``` and ```client```

##How to Manually Run the Programs:
1. Run the server executable (use: ```./server```)
2. Suspend the server program (use: **ctrl-Z**)
3. Run the server program in the background (use: ```bg```)
4. Run the client executable with the domain 'wiliki.eng.hawaii.edu (use: ```./client wiliki.eng.hawaii.edu```)

##How to Use/End the Programs:
1. Enter a single-letter command (for a list of commands, use **'h'**)
	- For a list of the server's directory contents, use **'l'**
	- To check if a file exists in the server, use **'c'**
	- To display the contents of a file in the server, use **'p'**
	- To download a file from the server to the client, use **'d'**
2. To Exit the client program, use **'q'**
3. Bring the server program back to the foreground (use: ```fg```)
4. Kill the server program (use: **ctrl-C**)

