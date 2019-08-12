# MossIM
##### Ian Johnson

For my final project for Computer Networks, I made an IRC-style instant-messaging client-server program. I used basic multi-threading to handle sockets from multiple clients on the server-side, and the ncurses library to give the client-side some basic UI and application-like functionality. A channel can handle up to 16 clients, who can all communicate with each other simultaneously.

This program doesn't even begin to cover all the functionalities of IRC (As in RFC 1459), but as of August 2019 I've returned to the project and am currently working on adding things such as encryption, roles such as Admins/Mods, the ability to promote, ban, and kick users, and improving the interface of the client program.

simply run the program with `./MossIM`, type 0 or 1 for client or server respectively, specify a port number, and an IP address and username if you're a client. (NOTE: If you intend on running a server, you may have to set up port forwarding with the port of your choosing if you want the server to be available to the public internet.)

##### COMMANDS
- `!leave` - Leave the server


## Final Project Write-up
#### What I learned
I learned more about how to correctly implement sockets and send/receive calls, specifically with non-blocking; something I had been struggling with throughout the semester and didn't fully understand for way too long.

I became more comfortable with Makefiles, including external libraries / header files, and splitting a project up into different files / headers. I also had to figure out how to design the format of messages so that important information like the time the message was sent, who sent it, what kind of message it was, and the actual message itself, would be received properly.

I became more familiar with multithreading, something I'd never used before, to handle multiple clients and "high-level multicast" over the transport layer with regular TCP sockets.

I became more comfortable with using the ncurses library, something I've used in the past but had never used completely from scratch.

#### Limitations
There's still some ugliness with just a console application (The biggest one being only 8 standard colors without doing some `init_color()` trickery, so clients may have to re-use username colors), and the size of the text boxes is pretty much hard-coded and can't change with the window size, so messages that are too big will go outside the text box until they eventually wrap around to the next line. The window where messages are displayed refreshes every tick and the messages (Which are stored in a left-shifting list) are printed line by line one after the other. So if a message is more than one line, part of it will get overwritten by the next message. I have a couple ideas on how to fix this; and the client interface in general could use an overhaul, seeing as ncurses is a pretty powerful terminal library and it could be greatly improved. Eventually, I might try and switch to just a lightweight GUI library.
