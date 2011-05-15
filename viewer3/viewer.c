#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
//#include <SDL/SDL_audio.h>
#include <SDL/SDL_timer.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//#include <linux/asm/poll.h>
#include <sys/poll.h>
#include "spcaframe.h"
#include "spcav4l.h"
#include "utils.h"
#include "defines.h"
/* #include "img_proc.h" */
#include "frees.h"
#include "objects.h"
#include "rgb_to_objects.h"
#include "color_name.h"
#include "jpeg_to_rgb.h"

#define JPG_TEMP_PATH	"/home/cwolfers/ram1/test"
#define MAX_SIZE_IMAGE	(640 * 480 * 4)

struct vdIn	videoIn;
pthread_mutex_t	image_lock[4];
static videoOk = 0;

int	run = 1;

void	exit_fatal(char *messages)
{
  printf("%s \n",messages);
  exit(1);
}

void	send_info(struct s_object_list *object_list)
{
}

void	send_image(unsigned char *img, unsigned long img_size)
{
}

static void init_sdlall(void)
{	/* Initialize defaults, Video and Audio */
	if ((SDL_Init (SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_EVENTTHREAD) == -1)) {
		printf ("Could not initialize SDL: %s.\n", SDL_GetError ());
		exit (-1);
	}
	videoOk =1;
	atexit (SDL_Quit);
}

static void init_sdlvideo(void)
{	/* Initialize defaults, Video and Audio */
	if ((SDL_InitSubSystem (SDL_INIT_VIDEO ) == -1)) {
		printf ("Could not initialize SDL: %s.\n", SDL_GetError ());
		exit (-1);
	}
	videoOk = 1;
}

static void close_sdlvideo(void)
{     SDL_QuitSubSystem (SDL_INIT_VIDEO ) ;
	videoOk = 0;
}

#define R (*(p2+2))
#define G (*(p2+1))
#define B (*(p2))

#define ColorToRGB(color, r, g, b)	\
	case color:			\
		R = r;			\
		G = g;			\
		B = b;			\
		break;

inline void	writeHLSImage(UC *p2, UC *c)
{
    switch (*c)
    {
	ColorToRGB(    white, 255, 255, 255);
	ColorToRGB(      red, 255,   0,   0);
	ColorToRGB(    brown, 128,  64,   0);
	ColorToRGB(   orange, 255, 128,   0);
	ColorToRGB(   yellow, 255, 255,   0);
	ColorToRGB(    green,   0, 255,   0);
	ColorToRGB(     cyan,   0, 255, 255);
	ColorToRGB(     blue,   0,   0, 255);
	ColorToRGB( sky_blue,  60,  60, 255);//trop fonce pr se voir sinon(128,128,255)
	ColorToRGB(  magenta, 255,   0, 255);
	ColorToRGB(     pink, 255, 128, 255);
	ColorToRGB(    black,   0,   0,   0);
	ColorToRGB(  unknown, 100, 100, 100);
    }
}

static void             print_result(unsigned char *pict,
				     struct s_ptr_uc	*colr,
				     struct s_object_list *object_list,
				     int width, int height,
				     SDL_Surface *pscreen)
{
    unsigned int        i, img_size = width * height * 3;
    int			x, y;
    struct s_object     *object1;
    unsigned char	*pos, *pict_end = pict + img_size;
    UC			*p2, *c;
    unsigned long	ncoord, dec;

    memset(pict + (img_size << 1), 0, img_size);

    for (i = 0; i < object_list->count; ++i)
//    i = 0;
    {
      object1 = object_list->objects + i; /* current object */

	printf("%s\t=> (%lu ; %lu) (y:%lu-%lu, x:%lu-%lu) %lu pixels\n",
	       color_name[object1->color],
	       object1->y,
	       object1->x,
	       object1->miny, object1->maxy,
	       object1->minx, object1->maxx,
	       object1->cpt);

	/* Draw every pixels of the current object */
	for (ncoord = 0; ncoord < object1->cpt; ncoord++)
	{
	    dec = object1->coord[ncoord][1] * width + object1->coord[ncoord][0];
	    c = colr->img_beg + dec;
	    p2 = pict + (img_size << 1) + 3 * dec;
	    writeHLSImage(p2, c);
	}

	/* Draw cross at the object center */
	for (x = -2; x <= 2; x++)
	    for (y = -2; y <= 2; y++)
	    {
		if (abs(x) == abs(y))
		{
		    pos = pict + ((object1->y + y) * width + object1->x + x) * 3;
		    if ((pos < pict) || (pos >= pict_end))
			continue;
		    *pos = 255 - *pos;
		    *(pos + 1) = 255 - *(pos + 1);
		    *(pos + 2) = 255 - *(pos + 2);//write img1
		    pos += img_size; //skip to next image
		    *pos = 255 - *pos;
		    *(pos + 1) = 255 - *(pos + 1);
		    *(pos + 2) = 255 - *(pos + 2);//write img2
		    pos += img_size; //skip to next image
		    *pos = 255 - *pos;
		    *(pos + 1) = 255 - *(pos + 1);
		    *(pos + 2) = 255 - *(pos + 2);//write img3
		}
	    }

	/* Draw horizontal lines framing the object */
	for (x = object1->minx; x <= object1->maxx; x += 2)
	  {
	    pos = pict + (object1->miny * width + x) * 3;
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img1
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img2
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img3

	    pos = pict + (object1->maxy * width + x) * 3;
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img1
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img2
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img3
	  }

	/* Draw vertical lines framing the object */
	for (y = object1->miny; y <= object1->maxy; y += 2)
	  {
	    pos = pict + (y * width + object1->minx) * 3;
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img1
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img2
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img3

	    pos = pict + (y * width + object1->maxx) * 3;
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img1
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img2
	    pos += img_size; //skip to next image
	    *pos = 255 - *pos;
	    *(pos + 1) = 255 - *(pos + 1);
	    *(pos + 2) = 255 - *(pos + 2);//write img3
	  }
    }
    printf("\n");
    fflush(stdout);
}

static void	initaddr (struct sockaddr_in *servadrr,char *address,int port)
{  
int adrsize = 0;
	if(address){
		adrsize = strlen(address);
		if(adrsize < 7 || adrsize > 15)
			exit_fatal("setting wrong address Abort !!");
		servadrr->sin_addr.s_addr = inet_addr(address);
	} else {
		servadrr->sin_addr.s_addr = INADDR_ANY;
	}
		
	servadrr->sin_family = AF_INET;
  	servadrr->sin_port = htons (port);
   	memset (&(servadrr->sin_zero), '\0', 8);
	
}

static int	open_clientsock(char * address, int port)
{
  struct sockaddr_in servadr;
  int client_handle;
 
  /* Create a new socket */
  if ((client_handle = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    exit_fatal ("Error opening socket Abort !");
  
/* Now set the server address struct and connect client socket to the port*/
  initaddr (&servadr,address,port);

  if (connect(client_handle,(struct sockaddr *) &servadr,
       sizeof (struct sockaddr)) == -1)
	  exit_fatal ("connect failed Abort !");
  return client_handle;
}

/* static int read_sock (int sockhandle, unsigned char *buf, int length) */
/* { */
/*   int byte_read = -1; */
/*   unsigned char *ptbuf =buf; */
/*   int mlength = length; */
/*   int i = 0; */

/*   do */
/*   { */
/*       byte_read = read (sockhandle, ptbuf,mlength); */
/*       if (byte_read > 0) */
/*       { */
/* 	  ptbuf = ptbuf+byte_read; */
/* 	  mlength = mlength-byte_read; */
/* //	printf("reste to read %d \n",mlength); */
/*       } */
/*       //printf("buffer value 0x%02X 0x%02X \n",buf[0],buf[1]); */
/*       i++; */
/*       //printf("waiting %d \n",i); */
/*       if(i > 100000) return -1; */
/* /\* 	if(i > 100) {usleep(1000); return -1;} *\/ */
/*   } while (mlength > 0); */

/*   return (mlength); */
/* } */

static int	read_sock2(int sock, unsigned char *buf, unsigned long size)
{
    int		nr = 0;

    for (; size; size -= nr, buf += nr)
	if ((nr = read(sock, buf, size)) <= 0)
	{
	    fprintf(stderr, "CATA (%i)\n", nr);
	    fflush(stderr);
	    break;
	}
    return (nr > 0) ? 0 : -1;
}

/* #define POLL_DELAY	10 */
/* static int		read_max(int sock, unsigned char *buf, unsigned long size_max) */
/* { */
/*     int			nr; */
/*     struct pollfd	pfd; */
/*     unsigned long	size = 0; */

/*     pfd.fd = sock; */
/*     pfd.events = POLLIN; */
/*     do */
/*     { */
/* 	nr = read(sock, buf, size_max); */
/* //printf("%i ", nr); */
/* 	if (nr > 0) */
/* 	{ */
/* 	    buf += nr; */
/* 	    size += nr; */
/* 	    size_max -= nr; */
/* 	} else */
/* 	    if (nr == -1) */
/* 		return -1; */
/* 	poll(&pfd, 1, POLL_DELAY); */
/*     } while (pfd.revents & POLLIN); */

/* printf("\nRead %6lu bytes\n", size); */
/*     return size; */
/* } */

static int	read_image(int sock, unsigned char *buf, struct frame_t *headerframe)
{
  int		size, nr;
  unsigned long	img_no;

/*     nr = read_max(sock, buf, MAX_SIZE_IMAGE);  */
/*     nr = read_sock2(sock, buf, 4); */
    nr = recv(sock, buf, 4, 0);
    if (nr == -1)
    {
	perror("recv:");
	return -1;
    }
    size = *((int *) buf);
/*     write(sock, (char *) &size, 4); */
    send(sock, (char *) &size, 4, 0);

/*     nr = read_max(sock, buf, MAX_SIZE_IMAGE); */
    nr = read_sock2(sock, buf, size);
/*     nr = recv(sock, buf, size, 0); */
    if (nr == -1)
    {
	perror("recv:");
	return -1;
    }
/*     write(sock, (char *) &size, 4); */
    send(sock, (char *) &size, 4, 0);

    recv(sock, &img_no, 4, 0);
    printf("Reading image #%lu\n", img_no); fflush(stdout);

    headerframe->size = size;

    return size;
}

static int		spcaClient(char *Ip, short port,
				   int owidth, int oheight, int format, 
				   /*unsigned long img_div_dec,*/ int statOn)
{
    struct frame_t	*headerframe;
//    struct client_t *messcallback;
    unsigned char	*buf = NULL;
    int			width, height;
    int			jpegsize;
    int			sock_client;
    int			run = 1;
    int			quit = 1;
    int			keypressed = 0;
    int			bpp = 3;
    SDL_Surface		*pscreen;
    SDL_Event		sdlevent;
    unsigned char	*p = NULL, *p2;
//    unsigned char *picture = NULL;
    struct tm		*tdate;
    time_t		curdate;
    char		titre[21] = "SpcaView - Wolfers";
    int			img_size, img_size3;
    struct s_ptr_rgb    rgb; // = { NULL };
    struct s_ptr_uc     colr;
    UC                  *c;
    struct s_object_list	*object_list;
    int			data_jpeg = 0;
    struct s_dim	dim = {owidth, oheight};
    struct s_dim	new_dim;
    unsigned long	new_size; //size after div

    //    videoIn.signalquit = 1;
    init_sdlall();
    sock_client = open_clientsock(Ip,port);
    write(sock_client, &data_jpeg, 4);

    headerframe = (struct frame_t*) malloc(sizeof(struct frame_t));
    headerframe->w = width = owidth;
    headerframe->h = height = oheight;
//    headerframe->size = 300000;
    img_size3 = width * height;
    img_size = img_size3 * 3;

/*     if (img_div_dec) */
/*     { */
/* 	new_size = img_size3 >> (img_div_dec + 1); */
/* 	new_dim.width = width >> img_div_dec; */
/* 	new_dim.height = height >> img_div_dec; */
/*     } */
/*     else */
    {
	new_size = img_size3;
	new_dim.width = width;
	new_dim.height = height;
    }

    allocs_matrix_cont(height + 8, width, sizeof (struct s_colr_rgb), 1,
		       &(rgb.img), &(rgb.img_beg), &(rgb.img_end));

    // Pour ne jamais parcourir les 8 dernieres lignes de trop ajoutees ci-dessus
    rgb.img_end -= (width << 3);

    allocs_matrix_cont(height, width, sizeof (UC), 1,
                       &(colr.img), &(colr.img_beg), &(colr.img_end));

    /* if (img_div_dec) */
    /* { */
    /* 	rgb.img_end = rgb.img_beg + new_size; */
    /* 	colr.img_end = colr.img_beg + new_size; */
    /* } */

    object_list = malloc(sizeof (struct s_object_list));
    buf = malloc(MAX_SIZE_IMAGE);

    if(videoOk) {
	pscreen = SDL_SetVideoMode (owidth, oheight * 3, bpp * 8,
				    SDL_DOUBLEBUF | SDL_SWSURFACE);
	p = pscreen->pixels;
    }
    do{
//printf("read image...\n");
 if ((jpegsize = read_image(sock_client, buf, headerframe)) < 0)
     goto error;
 /* if (*((unsigned short *) buf) == 0xD8FF) */
     format = VIDEO_PALETTE_JPEG;
 /* else */
 /*     format = VIDEO_PALETTE_RGB24; */
/* 	if ((jpegsize = readjpeg(sock_client, &buf, headerframe,0)) < 0) */
/* 	    goto error; */
//printf("read ok, test end ...\n");
/* mode sleep off */
	if (!jpegsize && videoOk)
	    close_sdlvideo();
//printf("test end ok, re-init SDL ...\n");
	if (!videoOk && jpegsize){
	    init_sdlvideo();
	    pscreen = SDL_SetVideoMode (owidth, oheight * 3, bpp * 8,
					SDL_DOUBLEBUF | SDL_SWSURFACE);
	    p = pscreen->pixels;
	}
	if(jpegsize && videoOk)
	{
//printf("re-init SDL ok, decode ...\n");
            if (format == VIDEO_PALETTE_JPEG)
/* 	    jpeg_decode_file(&rgb, buf, headerframe->size); */
 		/* jpeg_decode(&rgb, buf, &width, &height);  */
	      {
		int f;
		f = open(JPG_TEMP_PATH "/viewer.jpg", 
			 O_CREAT | O_TRUNC | O_WRONLY, 
			 S_IRUSR | S_IWUSR);
		write(f, buf, headerframe->size);
		close(f);

		jpeg_to_rgb(JPG_TEMP_PATH "/viewer.jpg", rgb.img_beg);
		/* printf("rgb gotten\n"); fflush(stdout); */
	      }
	    else
	    {
		rgb.img = NULL;
		rgb.img_beg = (struct s_colr_rgb *) buf;
		rgb.img_end = rgb.img_beg + img_size3;
	    }
//printf("decode ok\n");
/* 	    rgb.img_beg = (struct s_colr_rgb *) picture; */
/* 	    rgb.img_end = rgb.img_beg + img_size3; */
//	    rgb2color(&rgb, &colr);

	    memcpy(p, rgb.img_beg, img_size);

	    memset((unsigned long *) colr.img_beg, unknown, new_size);
            rgb_to_objects(object_list, &colr, &rgb, new_dim /*, p + 3 * img_size, pscreen*/);

	    for (c = colr.img_beg, p2 = p + img_size; c < colr.img_end; c++, p2+=3)
	      writeHLSImage(p2, c);

	    print_result(p, &colr, object_list, width, height, pscreen);


printf("fin print_result\n");
//	    free_object_list(object_list);

//	    resize (p,picture,owidth,oheight,width,height) ;
	    SDL_WM_SetCaption (titre, NULL);
	    SDL_Flip (pscreen);
	}
	fflush (stdout);
	//usleep(80000); //80ms Attente evenement (ex: fermer la fenetre)
	switch (run){
	    case 1:
		if (SDL_PollEvent (&sdlevent) == 1) {
		    switch (sdlevent.type) {
			case SDL_KEYDOWN:
			    switch (sdlevent.key.keysym.sym) {
				case SDLK_s:
				    //getPicture(buf,jpegsize);
//				    getJpegPicture(buf,width,height,
//						   VIDEO_PALETTE_JPEG,jpegsize,PICTURE,NULL);
				    break;
				case SDLK_SPACE:
				    run = 0;
				    break;
				case SDLK_q:
				    quit =0;
				    break;
			    }
			    break;
			case SDL_QUIT:
			    quit = 0;
			    break;
		    }
		} //end Poll event
		break;
	    case 0:
		if (SDL_WaitEvent (&sdlevent) == 1) {
		    switch (sdlevent.type) {
			case SDL_KEYDOWN:
			    switch (sdlevent.key.keysym.sym) {
				case SDLK_s:
				    //getPicture(buf,jpegsize);
				    break;
				case SDLK_SPACE:
				    run = 1;
				    break;
				case SDLK_q:
				    quit =0;
				    break;
			    }
			    break;
			case SDL_QUIT:
			    quit = 0;
			    break;
		    }
		} //end wait event
		break;
	    case 2:
	        {
		    if (SDL_PollEvent (&sdlevent) == 1) {
			switch (sdlevent.type) {
			    case SDL_KEYDOWN:
				keypressed = 0;
				run = 1;
				break;
			} //end event type poll 2
		    }
		} // end case 2
		break;
	}
    } while(quit);
error:

//    printf("End of image processing\n");
    fflush(stdout);

/* printf("free(colr.img); \033[41mEnleve mais A REVOIR\033[0m\n"); */
/* fflush(stdout); */
    free(colr.img); //A REVOIR!!!

/* printf("free(object_list);\n"); */
/* fflush(stdout); */
    free(object_list);

/* printf("free(rgb.img);\n"); */
/* fflush(stdout); */
/*     if(rgb.img){ */
	free(rgb.img);
/* 	rgb.img = NULL; */
/*     } */

/* printf("close(sock_client);\n"); */
/* fflush(stdout); */
    close(sock_client);

/* printf("free(buf); \033[41mEnleve mais A REVOIR\033[0m\n"); */
/* fflush(stdout); */
    free(buf);

//free(messcallback);
/* printf("free(headerframe); \033[41mEnleve mais A REVOIR\033[0m\n"); */
/* fflush(stdout); */
    free(headerframe);

/* printf("SDL_Quit ();\n"); */
/* fflush(stdout); */
    SDL_Quit ();

/* printf("All done\n"); */
/* fflush(stdout); */
    return 0;
}

static int reportip( char *src, char *ip, unsigned short *port)
{
	int j,k,done,ipdone,nbpt=0;
	char *AdIpPort= src;
	char *AdIp = ip;
	char Ports[] = "65536";
	j=0;k=0;done=0;ipdone=0;
			while(j < 22){
			switch (AdIpPort[j]){
			case '\0':
				done =1;
				break;
			case '.':
				nbpt++;
				if(nbpt > 3){
				printf("error fatal \n");
				return -1;
				}
			break;	
			case ':':
				k = 0; ipdone = 1;
				AdIp[j++] ='\0';
			break;
			default:
				
			break;
			}
			if (ipdone)
					Ports[k++]=AdIpPort[j++];
				else
					AdIp[k++]=AdIpPort[j++];
			if(done) break;
			}
			*port = (unsigned short) atoi (Ports);
			//printf ("Ip %s Port %s \n",AdIp,Ports);
			if(*port < 1024) {
			printf("ERROR Set default port to 7070 \n");
			*port = 7070;
			}
return 0;
}

int	main (int argc, char *argv[])
{
    /* Timing value used for statistic */

    const char *videodevice = NULL;
    /* default mmap */
    int grabMethod = 1;

    int format = VIDEO_PALETTE_JPEG; //= VIDEO_PALETTE_YUV420P;
    /******** output screen pointer ***/

    int image_width = 640;//320; //IMAGE_WIDTH;
    int image_height = 480;//240; //IMAGE_HEIGHT;
    int owidth = 0;
    int oheight = 0;
    /**********************************/
    /*        avi parametres          */
    char *inputfile = NULL;
    char *outputfile = NULL;
    char fourcc[4] = "MJPG";

    char *sizestring = NULL;
    int use_libjpeg = 1;
    char *separateur;
    char *mode = NULL;
    /*********************************/
    /*          Starting Flags       */
    int i;
    int videoOn = 1;
    int decodeOn =1 ;
    int statOn = 0;
    int audioout = 0;
    int videocomp = 0;
    int channel = 0;
    int norme = 0;
    int autobright = 0;
    /*********************************/
    char *AdIpPort;
    char AdIp[]= "000.000.000.000";
    unsigned short ports = 0;
    unsigned long		img_div_dec = 0;

    /*********************************/
    SPCASTATE funct;


    /* init default bytes per pixels for VIDEO_PALETTE_RAW_JPEG 	*/
    /* FIXME bpp is used as byte per pixel for the ouput screen	*/
    /* we need also a bpp_in for the input stream as spcaview   	*/
    /* can convert stream That will be a good idea to have 2 struct */
    /* with all global data one for input the other for output      */
//    bpp = 3;
    funct = GRABBER;
//    printf(" %s \n",version);
    /* check arguments */
    for (i = 1; i < argc; i++) {
	/* skip bad arguments */
	if (argv[i] == NULL || *argv[i] == 0 || *argv[i] != '-') {
	    continue;
	}
	if (strcmp (argv[i], "-d") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -d, aborting.\n");
		exit (1);
	    }
	    videodevice = strdup (argv[i + 1]);
	}
	if (strcmp (argv[i], "-n") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -n, aborting.\n");
		exit (1);
	    }
	    norme = atoi (argv[i + 1]);
	    if (norme < 0 || norme > 4)
		printf ("Norme should be between 0..4 Read the readme !.\n");
	}
	if (strcmp (argv[i], "-c") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -c, aborting.\n");
		exit (1);
	    }
	    channel = atoi (argv[i + 1]);
	    if (channel < 0 || channel > 9)
		printf ("Channel should be between [0..3] || [6..9] Read the readme !.\n");
	}
	if (strcmp (argv[i], "-v") == 0) {
	    videoOn = 0;

	}
	if (strcmp (argv[i], "-j") == 0) {
	    decodeOn = 0;

	}
	if (strcmp (argv[i], "-t") == 0) {
	    statOn = 1;

	}
	if (strcmp (argv[i], "-z") == 0) {
	    videocomp = 1;

	}
	if (strcmp (argv[i], "-b") == 0) {
	    autobright = 1;
	}
	if (strcmp (argv[i], "-f") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -f, aborting.\n");
		exit (1);
	    }
	    mode = strdup (argv[i + 1]);

	    if (strncmp (mode, "r32", 3) == 0) {
		format = VIDEO_PALETTE_RGB32;
//		bpp = 4;
		snprintf (fourcc, 5, "RGB4");

	    } else if (strncmp (mode, "r24", 3) == 0) {
		format = VIDEO_PALETTE_RGB24;
//		bpp = 3;
		snprintf (fourcc, 5, "RGB3");
	    } else if (strncmp (mode, "r16", 3) == 0) {
		format = VIDEO_PALETTE_RGB565;
//		bpp = 2;
		snprintf (fourcc, 5, "RGB2");
	    } else if (strncmp (mode, "yuv", 3) == 0) {
		format = VIDEO_PALETTE_YUV420P;
//		bpp = 3;
		snprintf (fourcc, 5, "I420");
	    } else if (strncmp (mode, "jpg", 3) == 0) {
		format = VIDEO_PALETTE_JPEG;
//		bpp = 3;
		snprintf (fourcc, 5, "MJPG");
	    }else {
		format = VIDEO_PALETTE_YUV420P;
//		bpp = 3;
		snprintf (fourcc, 5, "I420");
	    }

	}

	if (strcmp (argv[i], "-i") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -i, aborting.\n");
		exit (1);
	    }
	    inputfile = strdup (argv[i + 1]);
	    funct = PLAYER ;
	}

	if (strcmp (argv[i], "-g") == 0) {
	    /* Ask for read instead default  mmap */
	    grabMethod = 0;
	}

	if (strcmp (argv[i], "-a") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -a, aborting.\n");
		exit (1);
	    }
	    audioout = atoi (argv[i + 1]);
	    if ((audioout < 0) || (audioout > 2)) {
		audioout = 0;
	    }
	    printf ("audio channel %d\n", audioout);
	}

	if (strcmp (argv[i], "-o") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -o, aborting.\n");
		exit (1);
	    }
	    outputfile = strdup (argv[i + 1]);
	}
	/* custom ? */
	if (strcmp (argv[i], "-s") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -s, aborting.\n");
		exit (1);
	    }

	    sizestring = strdup (argv[i + 1]);

	    image_width = strtoul (sizestring, &separateur, 10);
	    if (*separateur != 'x') {
		printf ("Error in size use -s widthxheight \n");
		exit (1);
	    } else {
		++separateur;
		image_height =
		    strtoul (separateur, &separateur, 10);
		if (*separateur != 0)
		    printf ("hmm.. dont like that!! trying this height \n");
		printf (" size width: %d height: %d \n",
			image_width, image_height);
	    }
	}
	if (strcmp (argv[i], "-m") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -m, aborting.\n");
		exit (1);
	    }

	    sizestring = strdup (argv[i + 1]);

	    owidth = strtoul (sizestring, &separateur, 10);
	    if (*separateur != 'x') {
		printf ("Error in size use -m widthxheight \n");
		exit (1);
	    } else {
		++separateur;
		oheight =
		    strtoul (separateur, &separateur, 10);
		if (*separateur != 0)
		    printf ("hmm.. dont like that!! trying this height \n");
		printf (" output size width: %d height: %d \n",
			owidth, oheight);
	    }
	}
/* 	if (strcmp (argv[i], "-p") == 0) { */
/* 	    if (argv[i + 1]) interval = 1000* (atoi(argv[i + 1])); // timer works on ms */
/* 	    else { */
/* 		printf ("No parameter specified with -p, aborting.\n"); */
/* 		exit (1); */
/* 	    } */
/* 	} */
/* 	if (strcmp (argv[i], "-N") == 0) { */
/* 	    if (argv[i + 1]){ */
/* 		numshoot = (atoi(argv[i + 1])); // timer works on ms */
/* 		if (numshoot <= 0) numshoot = 1; // in case :) */
/* 		if(!interval) interval = 1000; */
/* 		modshoot = 1; */
/* 	    } */
/* 	    else { */
/* 		printf ("No parameter specified with -p, aborting.\n"); */
/* 		exit (1); */
/* 	    } */
/* 	} */
	if (strcmp (argv[i], "-w") == 0) {
	    if (i + 1 >= argc) {
		printf ("No parameter specified with -w, aborting.\n");
		exit (1);
	    }
	    AdIpPort = strdup (argv[i + 1]);
	    if(reportip(AdIpPort,AdIp,&ports) < 0)
		printf("error in port convertion \n");
	    printf ("using Server %s Port %d \n",AdIp,ports);
	    funct = CLIENT ;
	}
	/* large ? */
	if (strcmp (argv[i], "-l") == 0) {
	    image_width = 640;
	    image_height = 480;
	}

	if (strcmp (argv[i], "-idd") == 0) {
	    if (i + 1 >= argc)
		exit (1);
	    img_div_dec = (unsigned long) atoi (argv[i + 1]);
	    if (img_div_dec > 31)
		img_div_dec = 31;
	}
	if (strcmp (argv[i], "-h") == 0) {
	    printf ("usage: spcaview [-h -d -n -c -v -j -z -g -l -s -m -a -f -b -t -p] [-i inputfile | -o outputfile]\n");
	    printf ("-h	print this message \n");
	    printf ("-d	/dev/videoX       use videoX device\n");
	    printf ("-n	norme 0->pal 2->secam 1->ntsc \n");
	    printf ("-c	input channels 0..9 exclude 4 and 5 \n");
	    printf ("-v	disable video output for raw recording \n");
	    printf ("-j	disable video output and userspace decoding \n");
	    printf ("-z	compress yuv420p video output with DPSH encoder\n");
	    printf ("-g	use read method for grab instead mmap \n");
	    printf ("-l	default 320x480   use input size 640x480 \n");
	    printf ("-s	widthxheight      use specified input size \n");
	    printf ("-m	widthxheight      use specified ouput size \n");
	    printf ("-a	default  0 -> nosound    1-> microphone 2-> mixer output\n");
	    printf ("-f	video format  default yuv  others options are r16 r24 r32 yuv jpg \n");
	    printf ("-b     enable automatic brightness adjustment \n");
	    printf ("-t     print statistics \n");
	    printf ("-p  x  getPicture every x seconds \n");
	    printf ("-p x && -o getPicture every x seconds and record in outfile\n");
	    printf ("-w 	Address:Port read from Address xxx.xxx.xxx.xxx:Port\n");
	    printf ("-N x	take a x pictures and exit if p is not set p = 1 second \n");
	    printf ("-idd nbr     image division (x,y) in bits shifts (0,1,2,...) \n");
	    exit (0);
	}
    }
    printf("format:%i\n", format);
    spcaClient(AdIp,ports,image_width,image_height, format, /*img_div_dec,*/ statOn);

    return 0;
}
