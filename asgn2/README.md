# Readme
## Author: Cristian C. Castillo & Baubak Saadat <br />
## CruzID: ccarri11 <br /> CruzID: bsaadat
   🗄️ multi-threaded HTTP Server with Redundancy <br />Assignment 2 <br />
   Professor Nawab  <br />
   UCSC CSE 130 <br /> 
--------
## Requirements 🖥️ 
The following installations are required to execute code:
- Ubuntu 18.04 environment

- Virtual Machine 

- Clang

--------
## Assignment 1 Directory Contents 📂
- httpserver.cpp: main driver program.

- httpserver_definitions.cpp: function definitions/operations

- httpserver_headers.h: Declared void functions that work with httpserver definitions to enable driver program.

- DesignDocument.pdf: Contains Design plan, pseudo code (each part), Test Case,fucntions, assignment solution, assumption, goals, and data structures.

- Makefile: Runs and compiles the necessary flags for the program and initiates a ready state for the program to be executed.

- README.me: What you are currently reading; Contains concise specs about program information and setup.

----------
## Notes 📝
- This program was developed on a virtual machine environment with Ubuntu and replicates the linux cat command via system calls to the Kernel's Operating System.

- Program is tested vigorously Valgrind for memory leaks.

- Testing can be done by typing in terminal: valgrind ./httpserver (optional --leak-check=full). Resulting in total heap usage: 52 allocs, 52 frees, 86,742 bytes allocated. Server is not in run mode.

- Testing can be done by typing in terminal: valgrind ./httpserver localhost 8080 --leak-check=full. Reseulting in total heap usage: 3 allocs, 3 frees, 74,280 bytes allocated. Server is not in run mode.

---------
## Limitations/Issues ❗
- HTTP Server program runs properly on Unix files and environment.


---------
## Instructions 👾
- To utilize the HTTP Server program you require two open terminals.



-------
## Sources 📄
[<img align="left" alt="C++" width="80" height="80" src="https://cdn.freebiesupply.com/logos/large/2x/c-logo-png-transparent.png" />][cPlusPlus]

[<img align="left" alt="ShellWave" width="80" height="80" src="https://upload.wikimedia.org/wikipedia/commons/9/9f/Youtube%28amin%29.png" />][youtubeShell]

[<img align="left" alt="Beej's Guide" width="80" height="80" src="https://images-na.ssl-images-amazon.com/images/I/41VzZdCKtFL._SX360_BO1,204,203,200_.jpg" />][beejs]

[<img align="left" alt="Geeks Picture" width="80" height="80" src="https://media-exp1.licdn.com/dms/image/C4E0BAQEF0x_q7m5JjQ/company-logo_200_200/0?e=2159024400&v=beta&t=tEbDQX9zSpsKFIZHJzuUCLCTEIXdSAbvaRirs5PKA1M" />][geeks]

[<img align="left" alt="MDN Web" width="80" height="80" src="https://developer.mozilla.org/static/img/opengraph-logo.72382e605ce3.png" />][MDN]

[<img align="left" alt="Linux" width="80" height="80" src="https://media.glassdoor.com/sqll/771794/netcat-biz-squarelogo-1462451320719.png" />][linuxTech]

[<img align="left" alt="Berkley Sockets" width="80" height="80" src="https://encrypted-tbn0.gstatic.com/images?q=tbn%3AANd9GcSm5a8lKt9XFTLvZwJKxFxuo52oTa2Rvlj0fQ&usqp=CAU" />][berkleysocks]

[<img align="left" alt="Linux Man" width="80" height="80" src="https://man7.org/tlpi/cover/TLPI-front-cover-small-256.png" />][man7]


[<img align="left" alt="Tutor point" width="80" height="80" src="https://i.pinimg.com/280x280_RS/da/08/dc/da08dcf59c717413b711e2bfbaf2ab45.jpg" />][tutorpoint]

[<img align="left" alt="Linux" width="80" height="80" src="https://image.flaticon.com/icons/png/512/518/518713.png" />][linux]
<br /><br /><br />

[cPlusPlus]: http://www.cplusplus.com/doc/tutorial/
[geeks]: https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/#:~:text=File%20descriptor%20is%20integer%20that,pointers%20to%20file%20table%20entries.&nbsp;
[youtubeShell]: https://www.youtube.com/watch?v=LERSkW_pLBs 
[linux]: https://man7.org/linux/man-pages/man2/syscalls.2.html
[berkleysocks]: https://en.wikipedia.org/wiki/Berkeley_sockets
[beejs]: http://beej.us/guide/bgnet/html/
[linuxTech]: https://linuxtechlab.com/nc-command-ncat-for-beginners/
[MDN]: https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages
[tutorpoint]: https://www.tutorialspoint.com/http/http_requests.htm
[man7]: https://man7.org/linux/man-pages/man2/accept.2.html

