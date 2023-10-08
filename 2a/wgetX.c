/**
 *  Jiazi Yi
 *
 * LIX, Ecole Polytechnique
 * jiazi.yi@polytechnique.edu
 *
 * Updated by Pierre Pfister
 *
 * Cisco Systems
 * ppfister@cisco.com
 *
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "url.h"
#include "wgetX.h"

int main(int argc, char* argv[]) {
    url_info info;
    const char * file_name = "index.html";
    if (argc < 2) {
	fprintf(stderr, "Missing argument. Please enter URL.\n");
	return 1;
    }

    char *url = argv[1];

    // Get optional file name
    if (argc > 2) {
	file_name = argv[2];
    }

    // First parse the URL
    int ret = parse_url(url, &info);
    if (ret) {
	fprintf(stderr, "Could not parse URL '%s': %s\n", url, parse_url_errstr[ret]);
	return 2;
    }

    // Download the page
    struct http_reply reply;

    ret = download_page(&info, &reply);
    if (ret) {
	return 3;
    }
    //printf((&reply)->reply_buffer);
    // Now parse the responses
    char *response = read_http_reply(&reply);
    if (response == NULL) {
	fprintf(stderr, "Could not parse http reply\n");
	return 4;
    }

    // Write response to a file
    write_data(file_name, response, reply.reply_buffer_length - (response - reply.reply_buffer));

    // Free allocated memory
    free(reply.reply_buffer);

    // Just tell the user where is the file
    fprintf(stderr, "the file is saved in %s.", file_name);
    return 0;
}
int download_page(url_info *info, http_reply *reply) {

    
    // from the hostName it gets a struct a hostents struct containing maany new field, 
    // the most important are the ->h_addr the iP address and its length represented by ->h_length
    struct hostent *he = gethostbyname( info->host );

 

    //we create the dsstination socket (endpoint with an IP and port number)
    struct sockaddr_in dest;
    //memset() function, which sets all bytes in the dest structure to zero.
    memset(&dest,0,sizeof(dest));
    //IPv4
    dest.sin_family = AF_INET;
    // fill the dest.sin_addr.s_addr with the value of the IP address
    memcpy(&dest.sin_addr.s_addr,he->h_addr,he->h_length); // Copy adress ip


    //fill the port value now anh then we have our destionation socket
    //the htons==host byte to network byte
    dest.sin_port = htons(info->port); // Web port number = 80

    //now we define our little IPv4 socket 
    int mysocket = socket(AF_INET, SOCK_STREAM, 0);

    //we make the connection between the two sockets 
    connect(mysocket, (struct sockaddr*) &dest, sizeof(struct sockaddr_in));


    // prepare the get request we wanna send 
    char *request_to_send = http_get_request(info);

    //We write our liitle request
    write(mysocket,request_to_send,strlen(request_to_send));

    //always free the char*
    free(request_to_send);


    //====================prepare he reply_buffer================================

    //its initial memory space allocated is 1000 bytes (1ko)
    //the received len is the total length of the http response we have received
    //The bytes variable stores the number of bytes received in each iteration of the loop
    //char* cur is the current position in the buffer
    int buffer_length = 1000, received_len = 0, bytes = 0;
    reply->reply_buffer = (char *)malloc(buffer_length);
    reply->reply_buffer_length = buffer_length;
    char *cur = reply->reply_buffer;


    do {
        received_len += bytes;
        //if we exceed the length of the buffer that wa have already allocated. 
        if (received_len >= reply->reply_buffer_length) {
            reply->reply_buffer_length += buffer_length;
     		reply->reply_buffer = realloc(reply->reply_buffer, reply->reply_buffer_length);
        };

        //The recv() function is used to receive data from a connected socket.
        // cur is the buffer that receives the data
        // the third arg represents the the maximum number of bytes to be received
        // 0, i don't really know wat it is: sort of flag
        // return the  the number of bytes actually received.
        bytes = recv(mysocket, cur, reply->reply_buffer_length - received_len, 0);
        cur += bytes;


    } while (bytes > 0);

    close(mysocket);
    return 0;
}

//copy paste, forgot the source
void write_data(const char *path, const char * data, int len) {
    /*
     * To be completed:
     *   Use fopen, fwrite and fclose functions.
     */
    FILE *f = fopen(path, "wb");
    fwrite(data, 1, len, f);
    fclose(f);
}

// besed on the url_info info it returns the get request we wanna send to the server
char* http_get_request(url_info *info) {
    char * request_buffer = (char *) malloc(100 + strlen(info->path) + strlen(info->host));
    snprintf(request_buffer, 1024, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
	    info->path, info->host);
    return request_buffer;
}

char *next_line(char *buff, int len) {
    if (len == 0) {
	return NULL;
    }

    char *last = buff + len - 1;
    while (buff != last) {
	if (*buff == '\r' && *(buff+1) == '\n') {
	    return buff;
	}
	buff++;
    }
    return NULL;
}

char *read_http_reply(struct http_reply *reply) {

    // Let's first isolate the first line of the reply
    char *status_line = next_line(reply->reply_buffer, reply->reply_buffer_length);
    if (status_line == NULL) {
	fprintf(stderr, "Could not find status\n");
	return NULL;
    }
    *status_line = '\0'; // Make the first line is a null-terminated string

    // Now let's read the status (parsing the first line)
    int status;
    double http_version;
    int rv = sscanf(reply->reply_buffer, "HTTP/%lf %d", &http_version, &status);
    if (rv != 2) {
	fprintf(stderr, "Could not parse http response first line (rv=%d, %s)\n", rv, reply->reply_buffer);
	return NULL;
    }

    if (status == 301 || status == 302) {
        printf("Redirection !");
        return NULL;
    }
    else if (status != 200) {
	fprintf(stderr, "Server returned status %d (should be 200)\n", status);
	return NULL;
    };


    /*
     * To be completed:
     *   The previous code only detects and parses the first line of the reply.
     *   But servers typically send additional header lines:
     *     Date: Mon, 05 Aug 2019 12:54:36 GMT<CR><LF>
     *     Content-type: text/css<CR><LF>
     *     Content-Length: 684<CR><LF>
     *     Last-Modified: Mon, 03 Jun 2019 22:46:31 GMT<CR><LF>
     *     <CR><LF>
     *
     *   Keep calling next_line until you read an empty line, and return only what remains (without the empty line).
     *
     *   Difficul challenge:
     *     If you feel like having a real challenge, go on and implement HTTP redirect support for your client.
     *
     */
    //len will represent the remaining length of the msg 
    // it is necessary while using the next_line function
    //the previous code changes the the reply_buffer
    // it  points on the beginng of the first line and ends there after adding the '\0'
    //the length of the first line will ofc be the length o fthe iniatial buffer - the size of the first line 
    int len = reply->reply_buffer_length - sizeof(reply->reply_buffer) - 2; //the message of the 
    char *buf = status_line + 2; // +2 for the "\r\n" 

    while (*buf != '\r') { // empty line
    	status_line = next_line(buf, len); // points the begining of each line 
    	*status_line = '\0';
    	len -= sizeof(buf) + 2;
    	buf = status_line + 2; // if we add 2 and not find a '\r' it will for sure not be an empty line
    }
    buf += 2; // don't forget the next line points on '\r', we still need adding 2 to acces to the new line.

    return buf; //it is what is 

}
