Project group number: P4 Group 49

Group member names and x500s: Cameron Nagle nagle118, Vivek Kethineni kethi012, and Aaron Meyerhofer meye3630.

The name of the CSELabs computer that you tested your code: csel-wb28-10.cselabs.umn.edu

Members’ individual contributions: Cameron made the REAME and init function. Aaron made accept_connection and commented. Vivek did send_file_to_client, send_file_to_server, receive_file_from_server, get_request_server, and setup_connection.

Any changes you made to the Makefile or existing files that would affect grading: N/A

Any assumptions that you made that weren’t outlined in section 7: Use of mallocing, buffer, recv + fwrite with uses of sizeof(), and structs.

How could you enable your program to make EACH individual request parallelized? (high-level pseudocode would be acceptable/preferred for this part)

You could make a loop in main to create a thread pool. Where each individual request would be made parallelized. This would enable scalability, concurrency, and efficiency of resources.

Main server loop:
Use accept() to handle incoming connections
Pass the client connection file descriptor (new_sock_fd) to a worker thread for handling

Worker threads:
Each thread processes the client’s request independently:
Read the request
Process the request
Close the connection
