#define MAX_SPRITE 500
#define MAX_IMAGE 5000
#define MAX_TILE 500
#define MAX_MAP 2
#define MAX_MAP_WIDTH 100
#define MAX_MAP_HEIGHT 100

GP_PALETTEENTRY myPal[256];
GP_PALETTEENTRY p[256];
unsigned char *images[3];
GPDRAWSURFACE gpDraw[2];
int nflip=1;
int ExKey;
int mapTiles[MAX_MAP][MAX_MAP_WIDTH][MAX_MAP_HEIGHT];
int currentWorldMapX[MAX_MAP];
int currentWorldMapY[MAX_MAP];

int sstart;
int ssend;

int keyRight = 0;
int keyLeft = 0;
int keyUp = 0;
int keyDown = 0;
int keyQuit = 0;

int keyAction1 = 0;
int keyAction2 = 0;
int keyAction3 = 0;
int keyAction4 = 0;

typedef struct si {
	int imagex;
	int imagey;
	int imageh;
	int imagel;
	int transparentColor;
	int banque;
	int banqueHeight;
	int banqueWidth;
} simage;

typedef struct ss{
	int utilise;
	int posx;
	int posy;
	int image;

	int animation[10][20]; /* 10 animations de 20 frames (-1 = on boucle ) */
	int animation_speed[10];   /* vitesse de l'animation */
	int current_animation; /* animation courante -1 non animé */
	int current_animation_frame;

	int intern1;
} ssprite;

typedef struct st {
	int image;
	int attr1;
	int attr2;
	int attr3;
} stile;

ssprite sprite[MAX_SPRITE];
simage imageBank[MAX_IMAGE];
stile tiles[MAX_TILE];

void initTileEngine()
{

 int m,x,y;
 for (m=0;m<MAX_MAP;m++)
 for (x=0;x<MAX_MAP_WIDTH;x++)
 for (y=0;y<MAX_MAP_HEIGHT;y++)
     mapTiles[m][x][y]=0; 
}

void blitImageToScreen(int imageNo,int sx,int sy,int sw,int sh,int dx,int dy,int dw,int dh,int ttw,int tth)
{
     if ( (imageNo==1) || (imageNo==3) )
     {
      GpTransBlt(NULL,&gpDraw[nflip],dx,dy,sw,sh,images[imageNo],sx,sy,ttw,tth,255);
     }
     else
     {
      GpBitBlt(NULL,&gpDraw[nflip],dx,dy,sw,sh,images[imageNo],sx,sy,ttw,tth);
     }
}


void createTile(int n,int imageNo,int a1,int a2,int a3)
{
	tiles[n].image = imageNo;
	tiles[n].attr1 = a1;
	tiles[n].attr2 = a2;
    tiles[n].attr3 = a3;	
}

void setTileInMap(int mapNo,int x,int y,int tileNo)
{
     mapTiles[mapNo][x][y]=tileNo;
}

void setWorldView(int mapNo,int x,int y)
{
    currentWorldMapX[mapNo] = x;
    currentWorldMapY[mapNo] = y;     
}

void mapDraw(int mapNo)
{

   int map_drawx = currentWorldMapX[mapNo];
   int map_drawy = currentWorldMapY[mapNo];
   int i, j;
   int mapx, mapy;
   int map_xoff, map_yoff;

   mapx = map_drawx / 16;
   mapy = map_drawy / 16;

   map_xoff = map_drawx & 15;
   map_yoff = map_drawy & 15;

   for(i = 0; i < 16; i++) {
      for(j = 0; j < 21; j++) {         
         
         if ( (j+mapx<MAX_MAP_WIDTH) && (i+mapy<MAX_MAP_HEIGHT) && (j+mapx>=0) && (i+mapy>=0) )
                  blitImageToScreen(imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].banque,imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imagex, imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imagey,imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imagel, imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imageh,j * 16 - map_xoff, i * 16 - map_yoff,imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imagel, imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].imageh,imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].banqueWidth,imageBank[tiles[mapTiles[mapNo][j+mapx][i+mapy]].image].banqueHeight); 
         
/*
         tile_draw(map[mapNo][mapy + i][mapx + j].tile_number, j * 16 - map_xoff, i * 16 - map_yoff); */
      }
   }
}

void initGfxEngine()
{
	int i;
	GpGraphicModeSet(8, NULL);	

	/* Enable and clear LCD screen */	
    for(i = 0; i < 2 ; i++) 
	{
		GpLcdSurfaceGet(&gpDraw[i], i);  
	} 	      
	GpSurfaceSet(&gpDraw[0]);
}

	
int loadBmp(char *path,char *filename,char * pathfilename,char *pathdc,int noImage)
{
   ERR_CODE err_code;
   ubyte r, g, b;
   unsigned char *tmp;

   unsigned short  Header_bfType; /* 2 octects */
   long            Header_bfSize, Header_bfReserved, Header_bfOffBits, Header_biSize, Header_biWidth, Header_biHeight; /* 6*4 octets = 24 octets */
   unsigned short  Header_biPlanes, Header_biBitCount; /* 2*2 octets = 4 octets */
   long            Header_biCompression, Header_biSizeImage, Header_biXPelsPerMeter, /* 6*4 octets = 24 octets */
                    Header_biYPelsPerMeter, Header_biClrUsed, Header_biClrImportant;

  int x,y,cpt;
  unsigned char c, Palette[1024];

  F_HANDLE h_rfile;
  unsigned long m_size;

  err_code = GpFatInit();
  err_code = GpRelativePathSet(path);

  if (err_code != SM_OK)
  {
	return(1);
  }
	
  err_code=GpFileOpen(filename, OPEN_R, &h_rfile);

  if (err_code != SM_OK)
  {
	return(2);
  }

  err_code = GpFileRead(h_rfile, &Header_bfType , 2, &m_size);
  err_code = GpFileRead(h_rfile, &Header_bfSize , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_bfReserved , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_bfOffBits , 4, &m_size); 
  err_code = GpFileRead(h_rfile, &Header_biSize , 4, &m_size); 
  err_code = GpFileRead(h_rfile, &Header_biWidth , 4, &m_size); 
  err_code = GpFileRead(h_rfile, &Header_biHeight ,4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biPlanes , 2, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biBitCount , 2, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biCompression , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biSizeImage , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biXPelsPerMeter , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biYPelsPerMeter , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biClrUsed , 4, &m_size);
  err_code = GpFileRead(h_rfile, &Header_biClrImportant , 4, &m_size);

  if (Header_bfType != 19778 || Header_bfReserved != 0 || Header_biPlanes != 1)
  {
	/* Not a valid bitmap file - don't display */
	err_code = GpFileClose(h_rfile);    
	return(3);
  }

  if (Header_biCompression != 0)
  {
    /*Compressed file - don't display*/
	err_code = GpFileClose(h_rfile);    
	return(4);
  }

  if (Header_biBitCount != 8)
  {
    /*If the file is other than 8-bit dont read.*/
	err_code = GpFileClose(h_rfile);    
	return(5);
  }

  GpFileRead(h_rfile, &Palette, 1024, &m_size);

  for (c = 0; c < 255; c++) 
  {
		r = Palette[(c*4)+2];
		g = Palette[(c*4)+1];
		b = Palette[(c*4)];
		myPal[c] = ( r>>3<< 11 ) | ( g>>3<< 6 ) | ( b>>3<< 1 );
  }
    
  tmp = (unsigned char*)malloc(Header_biWidth * Header_biHeight );

  if (images[noImage]!=NULL) free(images[noImage]);
  images[noImage] = (unsigned char*)malloc(Header_biWidth * Header_biHeight );

  GpFileRead(h_rfile,tmp,Header_biWidth * Header_biHeight ,&m_size);
  cpt=0;
  for (y=(Header_biHeight-1);y>0;y--)
  {
	for (x=0;x<Header_biWidth ;x++)
	{		
		*(images[noImage] + x*Header_biHeight + (Header_biHeight-1-y)) = tmp[cpt++];
	}
  }
  free(tmp);
  err_code =GpFileClose(h_rfile);    
  
	GP_HPALETTE plt = GpPaletteCreate (256, myPal);
	GpPaletteSelect(plt);
	GpPaletteRealize(); 
  
  return(0);
    
}


void flipScreen()
{
	    GpSurfaceFlip(&gpDraw[nflip++]);  
	    nflip &= 0x01;
}

void waitInMs(int ms)
{
     unsigned int n_tick;
     n_tick = GpTickCountGet();
     while ( ( GpTickCountGet() - n_tick ) < ms);
}

int synchroStart()
{
       sstart = GpTickCountGet();
}

int synchroEnd(int ms)
{
       ssend = GpTickCountGet();
       int t = (ms-(ssend-sstart));
       if ( t>0 )
       if ( t<=ms )
       {
            waitInMs((ms-(ssend-sstart)));
       }

}

int checkController()
{
        
         keyRight=0;
         keyLeft=0;
         keyUp=0;
         keyDown=0;
         unsigned char keydata = GpKeyGet();
         
         if ( keydata & GPC_VK_RIGHT ) keyRight = 1; 
         if ( keydata & GPC_VK_LEFT ) keyLeft = 1; 
         if ( keydata & GPC_VK_UP ) keyUp = 1; 
         if ( keydata & GPC_VK_DOWN ) keyDown = 1;          
         if ( keydata & GPC_VK_FB ) keyAction1=1;
         if ( keydata & GPC_VK_FA ) keyAction2=1;
         if ( keydata & GPC_VK_FL ) keyAction3=1;
         if ( keydata & GPC_VK_FR ) keyAction4=1;
         if ( keydata & GPC_VK_START ) keyActionPause = 1;
                  
         return 0;
}

void initSpriteEngine()
{
	int i;
	for (i=0;i<MAX_SPRITE;i++)
	{
		sprite[i].utilise=0;
	}
}


/* Recupère une image n° n en x,y de taille h,l dans l'image n° imageNo
 de taille he,tc avec une couleur transparente tc */
void getImage(int n,int x,int y,int l,int h,int imageNo,int w,int he)
{
	imageBank[n].imagex = x;
	imageBank[n].imagey = y;
	imageBank[n].imageh = h;
	imageBank[n].imagel = l;
	imageBank[n].banque = imageNo;
	imageBank[n].banqueWidth = w;
	imageBank[n].banqueHeight = he;
	imageBank[n].transparentColor = 255;
}


void initSprite(int n,int x,int y,int i)
{
	sprite[n].posx = x;
	sprite[n].posy = y;
	sprite[n].image = i;
	sprite[n].utilise = 1;
	sprite[n].current_animation = -1;
	sprite[n].intern1 = 0;
	sprite[n].current_animation_frame = -1;
}

int initFreeSprite(int x,int y,int i)
{
	int s=0;
	while ((sprite[s].utilise==1) && (s<MAX_SPRITE))
	{
		s++;
	}
	sprite[s].posx = x;
	sprite[s].posy = y;
	sprite[s].image = i;
	sprite[s].utilise = 1;
	sprite[s].current_animation = -1;
	sprite[s].intern1 = 0;
	sprite[s].current_animation_frame = -1;

	return s;
}

void releaseSprite(int n)
{
	sprite[n].utilise = 0;
}

void releaseAllSprite()
{
	int i;
	for (i=0;i<MAX_SPRITE;i++)
	{
		sprite[i].utilise=0;
	}
}

void animateSprite(int s,int a)
{
	sprite[s].current_animation = a;
	sprite[s].current_animation_frame = -1;
	sprite[s].intern1 = 0;
}

void setSpriteAnimation(int s,int a,int speed,int f0,int f1,int f2,int f3,int f4,int f5,int f6,int f7,int f8,int f9,int f10,int f11,int f12,int f13,int f14,int f15,int f16,int f17,int f18,int f19)
{
	sprite[s].animation_speed[a] = speed;
	sprite[s].animation[a][0] = f0;
	sprite[s].animation[a][1] = f1;
	sprite[s].animation[a][2] = f2;
	sprite[s].animation[a][3] = f3;
	sprite[s].animation[a][4] = f4;
	sprite[s].animation[a][5] = f5;
	sprite[s].animation[a][6] = f6;
	sprite[s].animation[a][7] = f7;
	sprite[s].animation[a][8] = f8;
	sprite[s].animation[a][9] = f9;
	sprite[s].animation[a][10] = f10;
	sprite[s].animation[a][11] = f11;
	sprite[s].animation[a][12] = f12;
	sprite[s].animation[a][13] = f13;
	sprite[s].animation[a][14] = f14;
	sprite[s].animation[a][15] = f15;
	sprite[s].animation[a][15] = f16;
	sprite[s].animation[a][17] = f17;
	sprite[s].animation[a][18] = f18;
	sprite[s].animation[a][19] = f19;
}

void moveSprite(int n,int x,int y)
{
	sprite[n].posx=x;
	sprite[n].posy=y;
}

void changeSpriteImage(int n,int i)
{
	sprite[n].image=i;
}

void pasteImage(int n,int x,int y)
{
        blitImageToScreen(imageBank[sprite[n].image].banque,imageBank[sprite[n].image].imagex, imageBank[sprite[n].image].imagey,imageBank[sprite[n].image].imagel, imageBank[sprite[n].image].imageh,x,y,imageBank[sprite[n].image].imagel, imageBank[sprite[n].image].imageh,imageBank[sprite[n].image].banqueWidth,imageBank[sprite[n].image].banqueHeight);
}

void showSprite(int n)
{
	if (sprite[n].utilise== 1)
	{
		if (sprite[n].current_animation!=-1)
		{
			if (sprite[n].intern1<=0)
			{
				sprite[n].current_animation_frame ++;
				if (sprite[n].current_animation_frame>19) sprite[n].current_animation_frame=0;
				if (sprite[n].animation[sprite[n].current_animation][sprite[n].current_animation_frame] == -1) sprite[n].current_animation_frame=0;
				sprite[n].image = sprite[n].animation[sprite[n].current_animation][sprite[n].current_animation_frame];
				sprite[n].intern1 = sprite[n].animation_speed[sprite[n].current_animation];
			}
			sprite[n].intern1--;
		}
		else
		{
			sprite[n].intern1 = 0;
		}

        blitImageToScreen(imageBank[sprite[n].image].banque,imageBank[sprite[n].image].imagex, imageBank[sprite[n].image].imagey,imageBank[sprite[n].image].imagel, imageBank[sprite[n].image].imageh,sprite[n].posx, sprite[n].posy,imageBank[sprite[n].image].imagel, imageBank[sprite[n].image].imageh,imageBank[sprite[n].image].banqueWidth,imageBank[sprite[n].image].banqueHeight);


	}
}

void stopAnimateSprite(int s)
{
	sprite[s].current_animation = -1;
	sprite[s].current_animation_frame = -1;
	sprite[s].intern1 = sprite[s].animation_speed[sprite[s].current_animation];
}

void showLRSprite(int n)
{
}


void showAllSprite()
{
	int i;
	for (i=0;i<MAX_SPRITE;i++)
	{
		if (sprite[i].utilise==1)
		{
			showSprite(i);
		}
	}
}

/* Récupère un pixel d'une image N° imageNo en position x,y
 On précise la hauteur de l'image (GP32) */
unsigned int getPixel(int imageNo,int x,int y,int h)
{
    unsigned char *tmp = images[imageNo];
	return tmp[x*h + (h-1-y)];
}

int isSpriteCollide(int sprite1,int sprite2)
{
	int rect1_x, rect1_y;
	int rect2_x, rect2_y;
	int i, j, k, l;
	int coorx_1 = sprite[sprite1].posx;
	int coory_1 = sprite[sprite1].posy;
	int coorx_2 = sprite[sprite2].posx;
	int coory_2 = sprite[sprite2].posy;
	int sprite1w = imageBank[sprite[sprite1].image].imagel;
	int sprite1h = imageBank[sprite[sprite1].image].imageh;
	int sprite2w = imageBank[sprite[sprite2].image].imagel;
	int sprite2h = imageBank[sprite[sprite2].image].imageh;

	if (sprite[sprite1].utilise==0) return 0;
	if (sprite[sprite2].utilise==0) return 0;

	/*Détection par bounding box
	Retourne 0 et sort de la fonction
	si les sprites ne possédent pas de zones superposées*/
	if(coorx_1 > coorx_2+imageBank[sprite[sprite2].image].imagel) return 0;
	if(coorx_1+imageBank[sprite[sprite1].image].imagel < coorx_2) return 0;
	if(coory_1 > coory_2+imageBank[sprite[sprite2].image].imageh) return 0;
	if(coory_1+imageBank[sprite[sprite1].image].imageh < coory_2) return 0;

	/*Le but des lignes suivantes est de définir un
	rectangle qui englobe la zone d'affichage
	commune aux deux sprites
	On traite les coordonnées x du rectangle*/

	if (coorx_1 < coorx_2)
	{
		rect1_x = coorx_2;
		if (coorx_1 + sprite1w >= coorx_2 + sprite2w)
			rect2_x = coorx_2 + sprite2w;
		else
			rect2_x = coorx_1 + sprite1w;
	}
	else
	{
		rect1_x = coorx_1;
		if (coorx_2 + sprite2w >= coorx_1 + sprite1w)
			rect2_x = coorx_1 + sprite1w;
		else
			rect2_x = coorx_2 + sprite2w;
	}

	/*On traite les coordonnées y du rectangle*/
	if (coory_1 < coory_2)
	{
		rect1_y = coory_2;
		if (coory_1 + sprite1h >= coory_2 + sprite2h)
			rect2_y = coory_2 + sprite2h;
		else
			rect2_y = coory_1 + sprite1h;
	}
	else
	{
		rect1_y = coory_1;
		if (coory_2 + sprite2h > coory_1 + sprite1h)
			rect2_y = coory_1 + sprite1h;
		else
			rect2_y = coory_2 + sprite2h;
	}

	/*Il ne reste plus qu'à tester pour chaque
	pixel du rectangle précèdemment défini si ses pixels
	sont transparents
	Un pixel non transparent signifie qu'un bout de sprite
	est present dans le rectangle
	et donc que les sprites sont en collision*/



	for (i = rect1_x - coorx_1, j = rect1_x - coorx_2;i < rect2_x - coorx_1; i++, j++)
	{
		for (k = rect1_y - coory_1, l = rect1_y - coory_2;k < rect2_y - coory_1; k++, l++)
		{
			if((getPixel(imageBank[sprite[sprite1].image].banque, i+imageBank[sprite[sprite1].image].imagex, k+imageBank[sprite[sprite1].image].imagey,imageBank[sprite[sprite1].image].banqueHeight) != 255) && (getPixel(imageBank[sprite[sprite2].image].banque, j+imageBank[sprite[sprite2].image].imagex, l+imageBank[sprite[sprite2].image].imagey,imageBank[sprite[sprite2].image].banqueHeight) != 255))
			{       
                                                                                                                   
				return 1; 
			}
		}
	}

	return 0;
}

