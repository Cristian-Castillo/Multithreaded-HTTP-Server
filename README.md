# Readme
## Author: Cristian C. Castillo <br />
  🧵 Multithreaded HTTP Server <br>Integrated with Redundancy 🔁<br />
--------
## Requirements 👨‍💻
The following installations are required to execute code:
- Ubuntu 18.04 environment

- Virtual Machine 

- Clang

- Hit the like if you think its useful!

--------
## Disclaimer
- Do not use this program for commerical usage. I am not responsible for any liabilities that incur should you set this out in the public, and please use at one's own risk.

- Use this program only as a reference to learn about HTTP Server Socket Programming. Should you decide to use this program in the Academia, credit the author and source. Also note do not copy/plagiarise this work for personal academic advancement, you risk dire consequences such as being given a failing grade, and possibly expelled.
## Directory Contents 📂
- httpserver.cpp: main driver program.

- httpserver_definitions.cpp: function definitions/operations

- httpserver_headers.h: Declared void functions that work with httpserver definitions to enable driver program.

- DesignDocument.pdf: Contains Design plan, pseudo code (each part), Test Case,functions, Diagram (link), assignment solutions, assumption, goals, and data structures.

- Makefile: Runs and compiles the necessary flags for the program and initiates a ready state for the program to be executed.

- README.me: What you are currently reading; Contains concise specs about program information and setup.

- Test Case: Unit Testing with a TestList document for testing reference on commands.

----------
## Notes 📝
- This program was developed on a Virtual Machine environment with Ubuntu and and processes clients request via system calls to the Kernel's Operating System with Posix (threads).

- Program is tested vigorously Valgrind for memory leaks.

- Testing can be done by typing in terminal: valgrind ./httpserver 127.0.0.1(optional --leak-check=full). Resulting in total heap usage: 3,703 allocs, 3,688 frees, 127,413 bytes allocated. Server is not in run mode.

- Testing can be done by typing in terminal: valgrind ./httpserver localhost 8080 --leak-check=full. Reseulting in total heap usage:3,703 allocs, 3,688 frees, 127,413 bytes allocated. Server is not in run mode.

- Testing can be done ./httpserver localhost 8080 with no flags, while the server is running with the following command: valgrind ./httpserver localhost 8080. Since the program is concurrently in run mode, 6574 bytes will be in use in 1 block, a total heap usage of 3645 allocs, 3598 frees, and 106,763 bytes allocated. The amount reachable is 6574 bytes in 47 blocks, but this due to getaddrinfo(), a linked list, that has yet to be freed because the HTTP Server is active while conducting this test. Should the HTTP Server not be in run mode, you will have all no leaks as expected.

- Binary files (various sizes) were tested and compared with the diff -s foo1 foo2. Both are identical when generated and compared to original file. You may also compare the original file interchangeably with foo1 and foo2.

- Files persist with this program should the client disconnect from the server. To test this feature, start the server, and go into a remote directory or desktop. In the terminal type in the following command: curl -T foo localhost:8080/1234567890 -v. This command will put the file, along with its contents, in the httpserver directory from your remote location. You may than retrieve this file from your location by typing in the following command in the terminal: curl localhost:8080/1234567890 > new_file01. To compare if your contents indeed where retrieve successfully, you may apply the following command: diff foo new_file01 -sq. For further reassurance, you may locate file 1234567890 within the Multithreaded HTTP Server directory, and compare all three contents interchangeably to ensure contents were recorded accurately.

- The Multithreaded HTTP Server program is to remain connected during all request, unless ended by the client manually with control+c or any kill mechanism. The following test checks for prolong connection persistence, in the Test-Case directory you will find various unit testing curl commands that send n amount of curl request via 1 command to illustriate the power and speed of a multithreaded server. Note, these commands where built using chmod +x foo.sh for the purpose of efficiency. In the terminal type the following: <br>

   * ./daniel_get_test.sh <br>
   * ./daniel_put_test.sh <br>
   * ./getWork.sh <br>
   * ./putWork.sh <br>
   * ./putWork_curl.sh <br>
   * ./getWork_curl.sh <br>
   * ./remove.sh (removes all the files generated in the http dir and Test-Case dir).
   * etc <br>

The daniel command contains 82 files that are being transmitted from client to server vice versa, each file contains 16 Kib of content for this test, the max allotted content for a thread as indicated per assignment pdf documentation. The majority of chmod commands replicate the following two curls: <br>

   * curl -T foo localhost:8080/FILENAME00 > cmd0.output1 & <br>
   * curl localhost:8080/FILENAME00 -v > cmd0.output1 & <br>
 
These commands will generate a Get/PUT Request, returning the content sequentially in the order that it was called, with the content, status msg e.g 200,201,404, etc..., while threading, all while maintaining a connected status.

- The following commands may be inputted in the terminal,as presented by TA's Daniel Santos & Yiming Zhang:
  
<b> client side: </b> <br>

 * echo Hello World > t1 <br>

 * head -c 80000 /dev/urandom | od -x > t2 <br>

 * head -c 200 /dev/urandom > b1 <br>

  <b>server side:</b> <br>

 * echo Hello World > SmallFile1 <br>

 * head -c 80000 /dev/urandom | od -x >LargeFile1 <br>

 * head -c 200 /dev/urandom > SmBinFile1 <br>

<b> ./httpserver localhost 8001 -N threads & </b> <br>

 * (curl -T t1 http://localhost:8001/Small12345 -v & \ <br>

 * curl -T t2 http://localhost:8001/Large12345 -v) <br>

<b> ./httpserver localhost 8003 -N threads -r & </b> <br>

 * (curl -T t1 http://localhost:8003/Small12345 -v & \ <br>

 * curl -T b1 http://localhost:8003/binaryfile -v) <br>

<b> ./httpserver localhost 8005 -N threads & </b> <br>

 * (curl http://localhost:8005/SmallFile1 -v > out1 & \ <br>

 * curl http://localhost:8005/LargeFile1 -v > out2) <br>

<b> ./httpserver localhost 8004 -N threads -r & </b> <br>

 * (curl http://localhost:8004/SmallFile1 -v > out3 & \ <br>

 * curl http://localhost:8004/SmBinFile1 -v > out4) <br>
 
All the following commands can be implemented gracefully via the terminal, with the intended anticipated passing results.

- The Multithreaded HTTP Server program was tested with the bible's length of 4.5 million bytes on a (Put Request) along with various files included for a consecutive 8 requested threads, supporting more than 16 Kib if needed per threaded due to dynamic memory allocation. Should content length not be provided, the default buffer size of 16Kib will be adhered to as indicated. These consecutive threads are sustain succesfully keeping the connection intact to the server. This command may be implented as ./putWork_curl.sh. See Testlist document in Test-Case for further details on implementation.

- The Multithreaded HTTP Server program was tested with the bible's length of 4.5 million bytes on a (Get Request) along with various files included for a consecutive 8 requested threads, supporting more than 16 Kib if needed per threaded due to dynamic memory allocation. Should content length not be provided, the default buffer size of 16Kib will be adhered to as indicated. These consecutive threads are sustain succesfully keeping the connection intact to the server. This command may be implented as ./getWork_curl.sh. See Testlist document in Test-Case for further details on implementation.

- The Multithreaded HTTP Server program when prompted with multiple Get Request/Put Request will remain open during the entire process, patiently awaiting for the next curl upon finishing its objective, and will respond accordingly with the appropriate response should a file be present, internal error, forbidden, created, etc... all while keeping the connection alive.

- The Multithreaded HTTP Server program will acknowledged a file of content-length 0, binary, null upon a Put Request, and will retrieve them succesfully upon a Get Request.

- This program sustains a Worker/Boss model implementation (thread-pool flow style). Threads will wait until invoked by the user, synchronized accordingly (Global and Local locks), and process in parallel upon any request presented. 
---------
## Limitations/Issues ❗
- Multithreaded HTTP Server program runs properly on Unix files and environment.

- Multithreaded HTTP Server program can handle volume of extremely large files, but must be manually closed!

- Allowed terminal inputs are in the following formats: ./httpserver ip address port number, ./httpserver ip address, ./httpserver localhost, ./httpserver localhost port number. The port number can be customized, but numbers ranging from 0-1023 are reserved for privileged services and designated as well known ports.

- Note: Port number 80, is the HTTP reserved port, to run the following port input the following in the terminal: sudo ./httpserver ip address or localhost. You can either indicate 80 after the ip address/localhost, but the program has set this port to default should a port number not be provided.

- Multithreaded HTTP Server program only takes the following ASCII characters:A-Z, a-z,0-9. Should the client make a bad request, the client shall received a 400 hundred message response, and will be prompted via the terminal for the next request. However, the server will not crash, present the error on its side, and continue to run, waiting for the next request.

- The Multithreaded HTTP Server program checks for file permissions and is rescinded access should privileges not be present!

- The Multithreaded HTTP Server program checks for HTTP/1.1 Version Protocol rescinds further computation, generated with an appropriate client response.

- The Multithreaded HTTP Server program checks for specially GET/PUT client request rescinds further computation should the request not adhere to policy (e.g. HEAD would disqualify the request immediately), generated with an appropriate client response.

- Multithreaded HTTP Server program only takes the ASCII length of exactly ten characters as a valid request. Should the client make a bad request, the client shall received a 400 bad request message response, and will be prompted via the terminal for the next request. However, the server will not crash, present the error on its side, and continue to run, waiting for the next request.

- Should the client make a request to fetch a file from the Multithreaded HTTP Server program, and the file does not exist, the client will be presented with a response message of 404 file not found.  However, the server will not crash, present the error on its side, and continue to run, waiting for the next request.

- Should the client make a request to fetch a file from the Multithreaded HTTP Server program, and server malfunctions, the client will be presented with a response of message 500 internal server error. Indicating a malfunction within the server program.  However, the server will not crash, present the error on its side, and continue to run, waiting for the next request.

- Should the client make a request to put a file to the Multithreaded HTTP Server program, and the file fails along the way, the client will be presented with a response message of 404 file not found. However, the server will not crash, present the error on its side, and continue to run, waiting for the next request.

- 🚨 Syscall param socketcall.accept(addrlen_in) points to uninitialized byte(s) at 0x578E791: accept ( accept.c:26) is presented when you run (any of) the valgrind commands, varying compilers may have an offset. This offset if not accounted for... could impact generated data upon client request. This program application did not account for such offset, hence the following two commands will fail upon input via the terminal:

 * <b> Client Put Request </b> : curl -T foo http://localhost:8080/FILENAME[0-N] -v
 * <b> Client Get Request </b> : curl http://localhost:8080/FILENAME[0-N] -v
 
Note: The void in these request are trivial in a sense, as the client can submit one generated chmod request that enables over 100 request with 1 command. Hence, threading displays speed, throughput, and many various advantages over the non supported commands mentioned! 

---------
## Instructions ⚙️
- To utilize the Multithreaded HTTP Server program you require two open terminals.

- On one terminal run the server (see Limitations and Issues for formatting inputs) and hit enter. The terminal will began processing client request. The additional terminal is where the client, you, or anyone can began making multiple Get/Put request as need.

- Every successful request will result in a response with the clients terminal. However, the server will not crash should an error present itself during such a request. The server will present the error on its side, and continue to run, awaiting the clients next request.

- The client can send a Get Request (see Notes on format) to retrieve file contents, which will generate on the client side. Upon a successful fetched file, the client will be notified of a 200 OK message (along with the content files generated via the terminal), indicating that the file was retrieved succesfully from the server.

- The client can send a Put Request (see Notes on format) to store persistent files in the HTTP Server program. Upon a successful put, the client will be notified via the server of a 201 Created message, indicating that their file has been successfully uploaded to the Multithreaded HTTP Server program.

- Each PUT/GET request will terminate on its on, while the Multithreaded HTTP Server runs gracefully. Hence, there is no need for the client to terminate anything on their end. However, the client must terminate the server in the other terminal with the command: ctrl+c to terminate the server program.

- For a sample of how Multithreaded works on the fly see the Test-Case directory, type chmod +x foo.sh (foo being any bash file you wish to invoke), and witness the power of threading!.

- This program supports multiple processing threads, to invoke this feature, you may input the follwing in the terminal:
* ./httpserver localhost -N #threads (#threads = any number, e.g. 8, would mean 8 threads).

- This program supports an awesome integrated feature; Redundancy which may be invoked in the command line as followed: 
 * ./httpserver localhost -r
 
- This program supports the integration of both flags, -N and -r:
 * ./httpserver localhost -r -N #threads (in any combination ordering)
 
- Should the program not be presented with flags from the user, the thread count will default automatically to 4!
-------
## Redundancy 🔁
- When starting the terminal, the user may input the flag -r to invoke the Redundancy feature.

- Upon execution of file foo, three different copies will be generated of the file foo, and be directed to the following directories: 
 * copy1
 * copy2
 * copy3
 
 When the user checks these folders, you will find a copy of foo, in each copy directory. Hence, redunandcy!
 
 - Redundancy works with PUT/GET request only and the server does miscellanous checks internally with the file contents (matching content, which depends on the request) and respond with a client request accordingly e.g. 200, 201, 400, 404, 500, etc...
 
-------
## Sources 📄
<t>[<img align="left" alt="C++" width="80" height="80" src="https://cdn.freebiesupply.com/logos/large/2x/c-logo-png-transparent.png" />][cPlusPlus]

[<img align="left" alt="Jacob S." width="80" height="80" src="https://upload.wikimedia.org/wikipedia/commons/9/9f/Youtube%28amin%29.png" />][youtubeShell]

[<img align="left" alt="Beej's Guide" width="80" height="80" src="https://images-na.ssl-images-amazon.com/images/I/41VzZdCKtFL._SX360_BO1,204,203,200_.jpg" />][beejs]

[<img align="left" alt="Geeks Picture" width="80" height="80" src="https://media-exp1.licdn.com/dms/image/C4E0BAQEF0x_q7m5JjQ/company-logo_200_200/0?e=2159024400&v=beta&t=tEbDQX9zSpsKFIZHJzuUCLCTEIXdSAbvaRirs5PKA1M" />][geeks]

[<img align="left" alt="MDN Web" width="80" height="80" src="https://developer.mozilla.org/static/img/opengraph-logo.72382e605ce3.png" />][MDN] <br><br><br><br>

[<img align="left" alt="Linux" width="80" height="80" src="https://media.glassdoor.com/sqll/771794/netcat-biz-squarelogo-1462451320719.png" />][linuxTech]

[<img align="left" alt="Berkley Sockets" width="80" height="80" src="https://encrypted-tbn0.gstatic.com/images?q=tbn%3AANd9GcSm5a8lKt9XFTLvZwJKxFxuo52oTa2Rvlj0fQ&usqp=CAU" />][berkleysocks]

[<img align="left" alt="Linux Man" width="80" height="80" src="https://man7.org/tlpi/cover/TLPI-front-cover-small-256.png" />][man7]

[<img align="left" alt="Linux" width="80" height="80" src="https://image.flaticon.com/icons/png/512/518/518713.png" />][linux]

[<img align="left" alt="Posix" width="80" height="80" src="https://miro.medium.com/max/750/1*0BdBsbywXV2HBZuJIrw7Ng.jpeg" />][Posix]
<br /><br /><br />

[cPlusPlus]: http://www.cplusplus.com/doc/tutorial/
[geeks]: https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/#:~:text=File%20descriptor%20is%20integer%20that,pointers%20to%20file%20table%20entries.&nbsp;
[youtubeShell]: https://www.youtube.com/channel/UCwd5VFu4KoJNjkWJZMFJGHQ 
[linux]: https://man7.org/linux/man-pages/man2/syscalls.2.html
[berkleysocks]: https://en.wikipedia.org/wiki/Berkeley_sockets
[beejs]: http://beej.us/guide/bgnet/html/
[linuxTech]: https://linuxtechlab.com/nc-command-ncat-for-beginners/
[MDN]: https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
[tutorpoint]: https://www.tutorialspoint.com/http/http_requests.htm
[man7]: https://man7.org/linux/man-pages/man2/accept.2.html
[Posix]: https://www.cs.cmu.edu/afs/cs/academic/class/15492-f07/www/pthreads.html

