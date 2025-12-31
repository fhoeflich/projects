/*
 * Question:
Serial Message Parser
+++++++++++++++++++++

For this exercise, you will implement a parser that breaks a serial stream into packets, while discarding malformed data sequences. The message format and requirements for the program are described below.

Code should be written in C and compile on GCC 4.9. Please include build instructions.

Several sample files are included.

Requirements
============

* The program should read from stdin and write to stdout in the formats described below
* Status or debugging messages may be reported on stderr. There are no restrictions on the format of the status/debug messages.
* Messages consist of a header and a /variable/ length payload. Any data that does not conform to the message format should be discarded.
* The payload is an array of bytes, and may contain any 8-bit byte (0-255). No escaping is performed.
* The program should terminate cleanly when EOF is reached.
* The program should handle inputs of (reasonably) large size.

Message Format
===============

Offset  Value    Type     Meaning

0       0x21     (byte)   start marker 0
1       0x22     (byte)   start marker 1
2       (length) (uint8)  length of payload
3-258   (array)  (bytes)  payload

Output format
=============

Report the length of each packet's payload as an integer enclosed in curly braces (right justified), and report the packet payload as series of hexadecimal numbers corresponding to each byte and separated by spaces. Separate each packet with a newline. An example of the expected output is show below:

{  3} 41 42 43
{  4} 64 65 66 67

Note that the only acceptable characters in the output stream are " 012345689{}ABCDEF" and newline ("\n").

The contents of packet headers and any invalid data should not be reported. Incomplete packets should not be reported.

Example files
==============

README               this file.
2_packets            2 packets
10_short_packets     10 packets of small size
10_short_packets.out parsed output of 10_short_packets
200_packets          A larger sample file
extra_data           Another large sample file with extra data inserted between (valid) packets
 *
 * To dump practice packet files: od -t x1 2_packets
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <assert.h>

#define MAXPAYLOAD  (258 - 3 + 1)
#define MARKER0     0x21
#define MARKER1     0x22

typedef struct packet {
    unsigned char marker0;
    unsigned char marker1;
    unsigned char len;
    unsigned char payload[MAXPAYLOAD];
} packet_t;

enum pkt_state {
    PKT_BEGIN,
    PKT_MARKER0,
    PKT_MARKER1,
    PKT_LEN,
    PKT_PAYLOAD,
    PKT_END
};

static int debug = 0;

static void
print_packet(packet_t *pkt)
{
    int i;

    printf("{%3d}", pkt->len);
    for (i = 0; i < pkt->len; i++) {
        printf(" %X", pkt->payload[i]);
    }
    printf("\n");
}

static void
print_syncerror(int *printed_syncerror)
{
    if (*printed_syncerror == 0) {
        fprintf(stderr,
            "Sync error - searching for marker 0 again\n");
        *printed_syncerror = 1;
    }
}

static unsigned char
read_stdin_byte()
{
    int nread;
    unsigned char ret;

    nread = read(0, &ret, 1);
    if (nread < 0) {
        fprintf(stderr, "\n");
        fprintf(stderr, "read returned %d\n", nread);
        exit(1);
    } else if (nread == 0) {
        if (debug)
            fprintf(stderr, "EOF\n");
        exit(0);
    }

    return ret;
}

#define RESET_STATE_MACHINE(state,nread,len,printed) \
    (state)   = PKT_MARKER0; \
    (nread)   = 0; \
    (len)     = 0; \
    (printed) = 0;

int
main(int argc, char *argv[])
{
    unsigned char readbyte;
    packet_t pkt;
    int printed_syncerror;
    int nread;
    enum pkt_state state;

    do {
        switch (state) {
            case PKT_BEGIN:
                if (debug)
                    fprintf(stderr, "Entered state PKT_BEGIN\n");
                RESET_STATE_MACHINE(state, nread, pkt.len, printed_syncerror);
                break;

            case PKT_MARKER0:
                if (debug)
                    fprintf(stderr, "Entered state PKT_MARKER0\n");
                readbyte = read_stdin_byte();
                if (readbyte == MARKER0)
                    state = PKT_MARKER1;
                else
                    if (debug)
                        print_syncerror(&printed_syncerror);
                break;

            case PKT_MARKER1:
                if (debug)
                    fprintf(stderr, "Entered state PKT_MARKER1\n");
                readbyte = read_stdin_byte();
                if (readbyte == MARKER1)
                    state = PKT_LEN;
                else
                    if (debug)
                        print_syncerror(&printed_syncerror);
                break;

            case PKT_LEN:
                if (debug)
                    fprintf(stderr, "Entered state PKT_LEN\n");
                readbyte = read_stdin_byte();
                pkt.len = readbyte;
                if (debug)
                    fprintf(stderr, "packet len: %u\n", pkt.len);
                state = PKT_PAYLOAD;
                break;

            case PKT_PAYLOAD:
                if (nread == pkt.len) {
                    state = PKT_END;
                    break;
                }
                assert(nread < MAXPAYLOAD);
                readbyte = read_stdin_byte();
                pkt.payload[nread++] = readbyte;
                break;

            case PKT_END:
                /*
                 * We've read an entire packet in valid format.
                 * Do the formatted packet print and reset the state
                 * machine to start hunting for another packet or EOF.
                 */
                if (debug)
                    fprintf(stderr, "Entered state PKT_END\n");
                print_packet(&pkt);
                state = PKT_BEGIN;
                break;

            default:
                fprintf(stderr, "Bad state! %d\n", state);
                assert(0);
                /*NOTREACHED*/
        }
    } while (1);

    /*NOTREACHED*/
    exit(0);
}
