#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "bmp.c"

int client = -1;
unsigned int bytes = 0;

void die(char *err){
	perror(err);
	exit(EXIT_FAILURE);
}

void init_sock(char *ip ){
	client = socket(AF_INET, SOCK_STREAM, 0 );
	struct sockaddr_in addr;
	if( client == -1 )
		die("socket");

	memset( &addr, 0, sizeof(addr) );
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons( 9000 );

	if( connect( client, (struct sockaddr *)&addr, sizeof(addr) )<0){
		perror("");
		die("connect");
	}
}

int main(int argv, char * argc[]){
	init_sock("192.168.43.1");


	int device = open("/dev/graphics/fb0", O_RDWR);
	if( device == -1 )
		die("fb0");

	//define structures
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	struct dimension dim;

	//zero out structures
	memset( &vinfo, 0, sizeof(vinfo));
	memset( &finfo, 0, sizeof(finfo));
	memset( &dim  , 0, sizeof(dim  ));

	//screen properties
	if ( ioctl(device, FBIOGET_FSCREENINFO, &finfo) == -1)
		die("FBIOGET_FSCREENINFO");

	if ( ioctl(device, FBIOGET_VSCREENINFO, &vinfo) == -1)
		die("FBIOGET_FSCREENINFO");

	//send dimension
	dim.height = vinfo.yres;
	dim.width  = vinfo.xres;
	bytes = send(client, &dim, sizeof(dim), 0);

	if( bytes < sizeof(dim) || bytes == -1 )
		die("send dimension");

	//give time to establish connection
	sleep(1);



	//mman
	char *buffer = NULL;
	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	buffer = (char *)mmap( NULL, screensize, PROT_READ , MAP_SHARED , device , 0);
	if( buffer == MAP_FAILED )
		die("MAP_FAILED");


	unsigned int x,y,location;

	struct Pixel pixels;
	memset( &pixels, 0, sizeof(pixels) );	


	vinfo.activate = FB_ACTIVATE_FORCE;
ioctl( device , FBIOPUT_VSCREENINFO , &vinfo );

	printf("***sending image\n");

	for( ;  ; ){
		ioctl( device , FBIOPUT_VSCREENINFO , &vinfo );
		for( y = 0; y < vinfo.yres ; y++ ){

			for( x = 0 ; x < vinfo.xres; x++ ){
				location = (x+vinfo.xoffset) *  (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;

				pixels.red   = *(buffer + location );
				pixels.green = *(buffer + location + 1 );
				pixels.blue  = *(buffer + location + 2 );
				bytes = send( client, &pixels, sizeof(pixels), 0);
				if( bytes == -1 ) goto done;
			}
		}
		sleep(5);
	}

	done:
	printf("***image sent successfully\n");


	munmap( buffer, screensize );
	close( device );
	close(client);
	return 0;
}

