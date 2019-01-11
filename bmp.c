#include <stdio.h>
#include "bmp.h"

void writeHeader(FILE *file , struct BMPFileHeader *h);
void writeBitmap(FILE *file ,struct BMPBitmapHeader *h);
void writePixel(FILE *file, struct Pixel *pixel);
void readPixel(FILE *file, struct Pixel *pixel);
void printBitmap( struct BMPBitmapHeader *h);

void writeHeader(FILE *file , struct BMPFileHeader *h){
	fwrite( &h->type, 2 , 1 , file );
	fwrite( &h->size, 4 , 1 , file );
	fwrite( &h->res1, 2 , 1 , file );
	fwrite( &h->res2 ,2 , 1 , file );
	fwrite( &h->offset , 4 , 1 , file );
}


void writeBitmap(FILE *file ,struct BMPBitmapHeader *h){
	fwrite( &h->size , 4, 1, file );
	fwrite( &h->width , 4 , 1 , file );
	fwrite( &h->height , 4 , 1 , file );
	fwrite( &h->planes , 2 , 1 , file );
	fwrite( &h->bpp , 2 , 1 , file );
	fwrite( &h->comp , 4 , 1 , file );
	fwrite( &h->bitmap , 4 , 1 , file );
	fwrite( &h->hres , 4 , 1 , file );
	fwrite( &h->vres , 4 , 1 , file );
	fwrite( &h->colorUsed ,4 , 1 , file );
	fwrite( &h->icolor , 4 , 1 , file );
}

void printBitmap( struct BMPBitmapHeader *h){
	printf("*****bitmap information******\n");
	printf("height :%d width :%d\n", h->height, h->width );
	printf("bpp :%d\n", h->bpp );
}

void writePixel(FILE *file, struct Pixel *pixel){
	fwrite( &pixel->blue, sizeof(unsigned char), 1, file);
	fwrite( &pixel->green, sizeof(unsigned char), 1, file);
	fwrite( &pixel->red, sizeof(unsigned char), 1, file);
	//fwrite( &pixel->trans, sizeof(unsigned char), 1, file);
};

void readHeader(FILE *file, struct BMPFileHeader *h){
	fread( &h->type, 2 , 1 , file );
	fread( &h->size, 4 , 1 , file );
	fread( &h->res1, 2 , 1 , file );
	fread( &h->res2 ,2 , 1 , file );
	fread( &h->offset , 4 , 1 , file );
}

void readBitmap(FILE *file ,struct BMPBitmapHeader *h){
	fread( &h->size , 4, 1, file );
	fread( &h->width , 4 , 1 , file );
	fread( &h->height , 4 , 1 , file );
	fread( &h->planes , 2 , 1 , file );
	fread( &h->bpp , 2 , 1 , file );
	fread( &h->comp , 4 , 1 , file );
	fread( &h->bitmap , 4 , 1 , file );
	fread( &h->hres , 4 , 1 , file );
	fread( &h->vres , 4 , 1 , file );
	fread( &h->colorUsed ,4 , 1 , file );
	fread( &h->icolor , 4 , 1 , file );
}

void readPixel(FILE *file, struct Pixel *pixel){
	fread( &pixel->blue, sizeof(unsigned char), 1, file);
	fread( &pixel->green, sizeof(unsigned char), 1, file);
	fread( &pixel->red, sizeof(unsigned char), 1, file);
	//fread( &pixel->trans, sizeof(unsigned char), 1, file);
};
