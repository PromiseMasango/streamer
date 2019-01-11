#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "bmp.c"

int server = -1;
int client = -1;
unsigned int bytes = 0;

void die(char *err){
	printf("%s\n", err);
	exit(1);
}

int main(int argv, char *argc[] ){
	server = socket(AF_INET, SOCK_STREAM, 0 );
	struct sockaddr_in addr;
	struct sockaddr_in phone;
	struct dimension dim;

	if( server == -1 )
		die("socket create");

	memset( &addr, 0, sizeof(addr));
	memset( &phone, 0, sizeof(phone));
	memset( &dim  , 0, sizeof( dim ));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons( 9000 );

	int reuse = 1;
	setsockopt( server, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	if( bind(server, (struct sockaddr *) &addr, sizeof(addr)) < 0 )
		die("bind socket");

	if( listen(server, 1 ) < 0)
		die("listen socket");

	unsigned int sock_len = sizeof(phone);
	client = accept( server, (struct sockaddr *)&phone, &sock_len);
	printf("[ok] connected to :%s\n", inet_ntoa(phone.sin_addr));



	unsigned int dim_len = sizeof( struct dimension );

	bytes = recv( client, &dim, dim_len, 0 );
	printf("**dimension\n");
	printf("height :%d\n", dim.height );
	printf("width  :%d\n", dim.width  );

	struct Pixel pixels;
	memset( &pixels, 0, sizeof(pixels) );



//////////////////////////////////////////////////////////////////////////////////////////////////////
	int device = open("/dev/fb0", O_RDWR);
	if( device == -1 )
		die("fb0");

	//define structures
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	//zero out structures
	memset( &vinfo, 0, sizeof(vinfo));
	memset( &finfo, 0, sizeof(finfo));

	//screen properties
	if ( ioctl(device, FBIOGET_FSCREENINFO, &finfo) == -1)
		die("FBIOGET_FSCREENINFO");

	if ( ioctl(device, FBIOGET_VSCREENINFO, &vinfo) == -1)
		die("FBIOGET_FSCREENINFO");

	//mman
	char *buffer = NULL;
	long int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	buffer = (char *)mmap( NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED , device , 0);
	if( buffer == MAP_FAILED )
		die("MAP_FAILED");


	printf("writing image\n");
	unsigned int x,y,location;
	int pix_len = sizeof(pixels);


	for(;; ){
		for( y = 0 ; y < dim.height; y++ ){
			for( x = 500 ; x < dim.width+500 ; x++ ){
				bytes = recv( client, &pixels, pix_len, 0 );
				if( bytes == -1 ) goto done;
				location = (x+vinfo.xoffset) *  (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;

				*(buffer + location )     = pixels.red;
				*(buffer + location + 1 ) = pixels.green;
				*(buffer + location + 2 ) = pixels.blue;
			}
		}

	}


/*
	//flush image data to display
	vinfo.activate = FB_ACTIVATE_ALL;
	int ret = ioctl( device , FBIOPUT_VSCREENINFO , &vinfo );
	if( ret < 0 )
		printf("error, cannot force display\n");
*/
	done:
	close(server);
	close(client);
	return 0;
} 























