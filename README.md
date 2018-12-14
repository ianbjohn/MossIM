# P438 Fall 2018 Final Project
##### Ian Johnson

For my final project for Computer Networks, I made an IRC-style instant-messaging client-server program. I used basic multi-threading to handle sockets from multiple clients on the server-side, and the ncurses library to give the client-side some basic UI and application-like functionality. A channel can handle up to 16 clients, who can all communicate with each other simultaneously.

This program doesn't even begin to cover all the functionalities of IRC (As in RFC 1459), but I believe if I were to spend more time on this I could replicate it with a fair amount of justice. Adding things like roles for clients, and the ability to kick/ban users I think would be pretty easy to implement with how I've designed the client / server programs.

simply run the program with `./final`, type 0 or 1 for client or server respectively, and specify an IP address and username if you're a client.

##### COMMANDS
- `!leave` - Leave the server


## Final Project Write-up
#### What I learned
I learned more about how to correctly implement sockets and send/receive calls, specifically with non-blocking; something I had been struggling with throughout the semester and didn't fully understand for way too long.

I became more comfortable with Makefiles, including external libraries / header files, and splitting a project up into different files / headers. I also had to figure out how to design the format of messages so that important information like the time the message was sent, who sent it, what kind of message it was, and the actual message itself, would be received properly.

I became more familiar with multithreading, something I'd never used before, to handle multiple clients and "high-level multicast" over the transport layer with regular TCP sockets.

I became more comfortable with using the ncurses library, something I've used in the past but had never used completely from scratch.

#### Limitations
There's still some ugliness with just a console application (The biggest one being only 8 standard colors without doing some `init_color()` trickery, so clients may have to re-use username colors), and the size of the text boxes is pretty much hard-coded and can't change with the window size, so messages that are too big will go outside the text box until they eventually wrap around to the next line. The window where messages are displayed refreshes every tick and the messages (Which are stored in a left-shifting list) are printed line by line one after the other. So if a message is more than one line, part of it will get overwritten by the next message. I had an idea of how to cleanly handle splitting messages up to multiple lines, and how to figure out where to draw the next message based on the Y position and length of the previous message, but there was still the issue of having the list of recently-received messages being a statically allocated length, and unable to grow and shrink with how much space the current messages take up on the screen. I would have tried to investigate and fully analyze a solution to this further, but there just wasn't enough time.
