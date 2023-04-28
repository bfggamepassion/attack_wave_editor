#include <stdlib.h>
#if defined(_MSC_VER)
#include "SDL.h"
#else
#include "SDL/SDL.h"
#endif

#include "SDLGfxEngine.h"               
#include "common.h"                     

#define MAXPIXEL 10000

// Stockage des points de pivot
int pixelx_pivot[MAXPIXEL];
int pixely_pivot[MAXPIXEL];
// Stockage de tout les pixels de l'attack wave
int pixelx[MAXPIXEL];
int pixely[MAXPIXEL];
// Stockage temporaire pour travail
int pixelx_pivot_copie[MAXPIXEL];
int pixely_pivot_copie[MAXPIXEL];
int pixelx_copie[MAXPIXEL];
int pixely_copie[MAXPIXEL];
// Version non compressée pour sortie
int mvtc[MAXPIXEL];
int mvtx[MAXPIXEL]; 
int mvty[MAXPIXEL];
// Version compressée pour sortie
int mvtc_comp[MAXPIXEL];
int mvtx_comp[MAXPIXEL];
int mvty_comp[MAXPIXEL];

int idx;
int idx_pivot;
int idx_cursor;
int oldx;
int oldy;

/* Génère le fichier C dans le STDOUT pour être utilisé dans un jeu */
void calculerFichierC()
{
	int i,j,k,sortie;
	int oldx,oly,difx,dify;
	int newx,newy;
	int nb,mx,my,compteur,oldmx,oldmy;

	nb=0;
	mx=0;
	my=0;
	compteur=0;
	oldmx=-2000;
	oldmy=-2000;

	for (j=0;j<MAXPIXEL;j++)
	{
			mvtc[j]=-2000;
			mvtx[j]=-2000;
			mvty[j]=-2000;
			mvtc_comp[j]=-2000;
			mvtx_comp[j]=-2000;
			mvty_comp[j]=-2000;
	}

	sortie = 0;
	i = 0;
	j = 0;

	i++;
	oldx = pixelx[i];
	oldy = pixely[i];
	i++;

	/* Version non compressée */
//	printf("const char aw[]={\n"); 
	while (sortie==0)
	{
		newx = pixelx[i];
		newy = pixely[i];
		if ( (newx!=-1) && (newy!=-1) )
		{
			mvtc[j] = 1;
			mvtx[j] = newx-oldx;
			mvty[j] = newy-oldy;

//			printf("%d,%d,\n",newx-oldx,newy-oldy); 
			oldx = newx;
			oldy = newy;

			j++;
		}
		else
			sortie = 1;

		i++;
	}
//	printf("};\n");

	sortie = 0;i=0;j=0;compteur=0;
	/* Version compressée */
	oldmx = mvtx[0];
	oldmy = mvty[0];

//	printf("oldmx : %d , oldmy : %d\n",oldmx,oldmy);

	while(sortie==0)
	{
		mx = mvtx[i];
		my = mvty[i];

//		printf("---------------------------\n");
//		printf("mx : %d , my : %d\n",mx,my);
			if ((mx==oldmx) && (my==oldmy)) /* Type  identique */
			{							
				compteur++;
				i++;
				oldmx = mx;
				oldmy = my;	
//				printf("Valeurs identique cpt:%d , i:%d , oldmx : %d , oldmy : %d , mx : %d , my : %d\n",compteur,i,oldmx,oldmy,mx,my);
			}
			else
			{
//				printf("différentes -- Stockage --");
				mvtc_comp[j] = compteur;
				mvtx_comp[j] = oldmx;
				mvty_comp[j] = oldmy;
//				printf("mvtc_comp[%d]=%d;mvtx_comp[%d]=%d;mvty_comp[%d]=%d;\n",j,compteur,j,oldmx,j,oldmy);
				j++;
				oldmx = mx;
				oldmy = my;
				i++;
				compteur = 1;
				if (mx==-2000) sortie=1;
			}
	}

}

/* Génère le fichier C dans le STDOUT pour être utilisé dans un jeu */
void genererFichierC()
{
	int i,j,k,sortie,diffx,diffy;

	calculerFichierC();

	sortie = 0;j=0;
	printf("const char awc[]={\n");
	while (sortie==0)
	{
			if (mvtc_comp[j]!=-2000)
			{
				printf("%d,%d,%d,\n",mvtc_comp[j],mvtx_comp[j],mvty_comp[j]);
			}
			j++;
			if (j>=9999) sortie=1;
	}
	printf("};\n");

	sortie = 0;j=0;
	printf("const char aw_pivot[]={\n");
	while (sortie==0)
	{
		printf("%d,%d,\n",pixelx_pivot[j],pixely_pivot[j]);
		j++;
		if (pixelx_pivot[j]==-1) sortie=1;
	}
	printf("};\n");

	sortie = 0;j=1;
	printf("const char aw_pivot_diff[]={\n");
	while (sortie==0)
	{
        diffx=pixelx_pivot[j] -  pixelx_pivot[j-1];
        diffy=pixely_pivot[j] -  pixely_pivot[j-1];
        
		printf("%d,%d,\n",diffx,diffy);
		j++;
		if (pixelx_pivot[j]==-1) sortie=1;
	}
	printf("};\n");
}

/* Initialise les tableaux de pivot et de pixel */
void initTab()
{
     int i;
     
     for (i=0;i<MAXPIXEL;i++)
     {
         pixelx_pivot[i]=-1;
         pixelx_pivot_copie[i]=-1;
         pixelx[i]=-1;
         pixelx_copie[i]=-1;
         
         pixely_pivot[i]=-1;
         pixely_pivot_copie[i]=-1;
         pixely[i]=-1;
         pixely_copie[i]=-1;
     }
	idx = 0;
	idx_pivot=0;
	idx_cursor = 0;
	oldx=-1;
	oldy=-1;
	loadBmp("","","./data/calque.bmp","",0);	
}

/* Trace une ligne d'un point A à un point B */
void MyLine(int xdep, int ydep, int xfin, int yfin)
{
	float a=0;
	int temp=0;
	int cste=0;
	int sortie=0;
	int x,y;
	int inv;

	int inv_x[1000];
	int inv_y[1000];
	int inv_idx;

	unsigned int dy;
	unsigned int dx;

	for (inv_idx=0;inv_idx<1000;inv_idx++)
	{
		inv_x[inv_idx]=-1;
		inv_y[inv_idx]=-1;
	}
	inv_idx=0;

	inv = 0;
	pixelx[idx] = xdep;
	pixely[idx] = ydep;
	idx++;
	dx=abs(xfin-xdep);
	dy=abs(yfin-ydep);
	if(dx>=dy && dx!=0 && dy!=0)
	{
		if(xdep>xfin)
		{	/* Il va falloir inverser le stockage ! */
			inv=1;
			temp=xdep;
			xdep=xfin;
			xfin=temp;

			temp=ydep;
			ydep=yfin;
			yfin=temp;
		}
		a=((float)(yfin-ydep)/(xfin-xdep));

		for(x=0;x<dx+1;x++)
		{
			putPixel(0, x+xdep,ydep+(x*a)+cste,255,255,5); 
			if (inv==0)
			{
				pixelx[idx] = x+xdep;
				pixely[idx] = ydep+(x*a)+cste; 
				idx++;			
			}
			else
			{
				inv_x[inv_idx] = x+xdep;
				inv_y[inv_idx] = ydep+(x*a)+cste; 
				inv_idx++;
			}
		}
	}

	if(dx<dy && dx!=0 && dy!=0)
	{
		if(ydep>yfin)
		{	/* Il va falloir inverser le stockage ... */
			inv=1;
			temp=ydep;
			ydep=yfin;
			yfin=temp;

			temp=xdep;
			xdep=xfin;
			xfin=temp;
		}
		a=((float)(xfin-xdep)/(yfin-ydep));

		for(y=0;y<dy+1;y++)
		{
			putPixel(0, xdep+(y*a)+cste,y+ydep,255,255,5);
			if (inv==0)
			{
				pixelx[idx] = xdep+(y*a)+cste;
				pixely[idx] = y+ydep;
				idx++;			
			}
			else
			{
				inv_x[inv_idx] = xdep+(y*a)+cste;
				inv_y[inv_idx] = y+ydep;
				inv_idx++;
			}
		}
	}

	if(dx==0)
	{		
		if(ydep>yfin)
		{	/* Il va falloir inverser le stockage */
			inv = 1;
			temp=ydep;
			ydep=yfin;
			yfin=temp;
		}
		for(y=ydep;y<yfin+1;y++)
		{
			putPixel(0, xdep,y,255,255,5);
			if (inv==0)
			{	    		
				pixelx[idx] = xdep;
				pixely[idx] = y;
				idx++;			
			}
			else
			{
				inv_x[inv_idx] = xdep;
				inv_y[inv_idx] = y;
				inv_idx++;
			}
        }    		
	}

	if(dy==0)
	{		
		if(xdep>xfin)
		{	/* Il va falloir inverser le stockage */
			inv=1;
			temp=xdep;
			xdep=xfin;
			xfin=temp;
		}

		for(x=xdep;x<xfin+1;x++)
		{
            putPixel(0, x,ydep,255,255,5);
			if (inv==0)
			{
				pixelx[idx] = x;
				pixely[idx] = ydep;
				idx++;	
			}
			else
			{
				inv_x[inv_idx] = x;
				inv_y[inv_idx] = ydep;
				inv_idx++;
			}
        }		     

	}

	sortie=0;
	if (inv==1) // On rebascule tout à l'endroit dans le tableau_principal
	{
		inv_idx--; // On se positionne sur le dernier pixel de ce tableau
		while(sortie==0)
		{
			pixelx[idx] = inv_x[inv_idx];
			pixely[idx] = inv_y[inv_idx];
			idx++;
			inv_idx--;
			if (inv_idx<0) sortie=1;
		}
	}
}


/* Redessine l'attack wave */
void retrace()
{
     int i,sortie;
     i=0;sortie=0;
     loadBmp("","","./data/calque.bmp","",0);
     while(sortie==0)
     {
        if (pixelx[i]!=-1)
        {
           putPixel(0, pixelx[i],pixely[i],255,255,5);
           i++;
		   blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);          
		   flipScreen();
        }
        else
            sortie = 1;
         
     }
              
 }
 
/* Dessine l'attack wave mais avec les points pivots*/
void retracePivot()
{
     int i,sortie;
     i=0;sortie=0;
     loadBmp("","","./data/calque.bmp","",0);
     while(sortie==0)
     {
        if (pixelx_pivot[i]!=-1)
        {
           putPixel(0, pixelx_pivot[i],pixely_pivot[i],255,255,5);
           i++;
		   blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);          
		   flipScreen();
        }
        else
            sortie = 1;
         
     }
              
 }
 
 /* Recalcule tout les pixels depuis le pivot */
 void recalculeDepuisPivot()
 {
      int i,sortie;
      
      sortie = 0;idx=0;
      
      for (i=0;i<MAXPIXEL;i++)
      {
       pixelx[i] = -1;
       pixely[i] = -1;
      }
      
      i=0;
      while (sortie==0)
      {
            if ( (pixelx_pivot[i]==-1) || (pixelx_pivot[i+1]==-1) )
            {
               sortie=1;
            }
            else
            {
                MyLine(pixelx_pivot[i],pixely_pivot[i],pixelx_pivot[i+1],pixely_pivot[i+1]);
                i++;
            }
      }
  }
  
/* Divise le nombre de point de pivot par 2 */
 void dividePivot()
 {
      int i;
      idx_pivot = 0;
      for (i=0;i<MAXPIXEL;i++)
      {
          pixelx_pivot_copie[i]=pixelx_pivot[i];
          pixely_pivot_copie[i]=pixely_pivot[i];
          pixelx_pivot[i]=-1;
          pixely_pivot[i]=-1;
      }
      for (i=0;i<MAXPIXEL;i+=2)
      {
          if (pixelx_pivot_copie[i]>0)
          {
              pixelx_pivot[idx_pivot]=pixelx_pivot_copie[i];
              pixely_pivot[idx_pivot]=pixely_pivot_copie[i];
          
 

             oldx = pixelx_pivot[idx_pivot];
	         oldy = pixely_pivot[idx_pivot];
	          idx_pivot++;
          }
          
         
      }


          
      recalculeDepuisPivot();
      retrace();
  }

/* Réduit la liste de pivot si on en à supprimé un */
void reduitPivot()
{
    int i;
      idx_pivot = 0;
      for (i=0;i<MAXPIXEL;i++)
      {
          pixelx_pivot_copie[i]=pixelx_pivot[i];
          pixely_pivot_copie[i]=pixely_pivot[i];
          pixelx_pivot[i]=-1;
          pixely_pivot[i]=-1;
      }
      for (i=0;i<MAXPIXEL;i++)
      {
          if (pixelx_pivot_copie[i]>0)
          {
              pixelx_pivot[idx_pivot]=pixelx_pivot_copie[i];
              pixely_pivot[idx_pivot]=pixely_pivot_copie[i];
          
 

             oldx = pixelx_pivot[idx_pivot];
	         oldy = pixely_pivot[idx_pivot];
	          idx_pivot++;
          }
          
         
      }
          
      recalculeDepuisPivot();
      retrace();
}

void playAttack(int xbase,int ybase)
{
/*	int xbase,ybase;*/
	int sp_x,sp_y,sp_m,i;
	int sortie;

	sortie = 0;
	i=0;
/*	xbase = 100;
	ybase = 100;*/
	calculerFichierC();
	while(sortie==0)
	{
		checkController();
        if (keyQuit==1)
        {
            while (keyQuit==1) {checkController();}
			sortie=1;
		}

		sp_m = mvtc_comp[i];
		sp_x = mvtx_comp[i];
		sp_y = mvty_comp[i];
		while(sp_m>0)
		{
			xbase = xbase + sp_x;
			ybase = ybase + sp_y;
			sp_m--;
			moveSprite(0,xbase,ybase);
			blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);
			showSprite(0);
			flipScreen();		
			waitInMs(1);
		}
		i++;
		if (mvtc_comp[i]==-2000) i=0;
		if (xbase>320) sortie=1;
		if (xbase<0) sortie=1;
		if (ybase>200) sortie=1;
		if (ybase<0) sortie=1;
	}
}

/* Programme principal */
int common(int param)
{
	int toucheRelache;
	int x,y,v,i;
	int myspr;
    SDL_Event event;

	idx = 0;
	idx_pivot=0;
	idx_cursor = 0;
	oldx=-1;
	oldy=-1;

	initGfxEngine();   
	initSpriteEngine();
	initTileEngine();
    initTab();
	
	v=0;
	loadBmp("","","./data/calque.bmp","",0);	
	loadBmp("","","./data/sprites.bmp","",1); /* Transparence */
	loadBmp("","","./data/calque.bmp","",2);

	blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);          
	getImage(0,0,0,16,16,1,320,240);
	myspr = initFreeSprite(0,0,0); /* x,y,image*/

	blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);
	SDL_ShowCursor(SDL_ENABLE);
	while(1){   
		checkController();   
          
          if (pixelx_pivot[idx_cursor]>=0)
          {
             putPixel(0, pixelx_pivot[idx_cursor],pixely_pivot[idx_cursor],255,0,5); 
          }
          
          if (keyRight==1)
          {
             while (keyRight==1) {checkController();}
             if (idx_cursor!=(MAXPIXEL-1))
             {
                if (pixelx_pivot[idx_cursor+1]!=-1)
                {
                  putPixel(0, pixelx_pivot[idx_cursor], pixely_pivot[idx_cursor],255,255,5);  
                  idx_cursor++;
                }
             }
          }
          if (keyLeft==1)
          {
             while (keyLeft==1){checkController();} 
             if (idx_cursor!=0)
             {
                if (pixelx_pivot[idx_cursor-1]!=-1)
                {
                  putPixel(0, pixelx_pivot[idx_cursor], pixely_pivot[idx_cursor],255,255,5);  
                  idx_cursor--;
                }
             }
          }          
          if (keyR==1)
          {
             recalculeDepuisPivot();
             retrace();
          }
          if (keyP==1)
             retracePivot();

          if (keyT==1)
             playAttack(x,y);

          if (keyN==1)
             initTab();

          if (keyF1==1)
		  {
				 while (keyF1==1) {checkController();}
				FILE *f = fopen("save.bin", "wb");
				fwrite(pixelx_pivot, 10000, 1, f);  
				fwrite(pixely_pivot, 10000, 1, f);  
			    fclose(f);  
          }

          if (keyF2==1)
		  {
    			 while (keyF2==1) {checkController();}
				FILE *f = fopen("save.bin", "rb");
				fread(pixelx_pivot, 10000, 1, f);  
				fread(pixely_pivot, 10000, 1, f);  
			    fclose(f);  

				recalculeDepuisPivot();
	             retrace();
				for (i=0;i<MAXPIXEL;i++)
				{
					if (pixelx[i]==-1)
					{
						 idx=i;
							break;
					}				
				}
				for (i=0;i<MAXPIXEL;i++)
				{
					if (pixelx_pivot[i]==-1)
					{
						 oldx=pixelx_pivot[i-1];
						 oldy=pixely_pivot[i-1];
						 idx_pivot=i;
							break;
					}				
				}
          }
             
          if (keyD==1)
          {
             while (keyD==1) {checkController();}
             dividePivot();
          }
                      if (keyDELETE==1)
                      {
                         while (keyDELETE==1) {checkController();}
                         pixelx_pivot[idx_cursor]=-1;
                         pixely_pivot[idx_cursor]=-1;
                        reduitPivot();
                      }
          /* Bouger un point pivot */
          if (keyC==1)
          {
              while (keyC==1) {checkController();}
              while(1)
              {
                      putPixel(0, pixelx_pivot[idx_cursor], pixely_pivot[idx_cursor],0,255,0);
            		  blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);          
		              flipScreen();
		              checkController();
                      if (keyC==1)
                      { 
                        while (keyC==1) {checkController();}
                        putPixel(0, pixelx_pivot[idx_cursor], pixely_pivot[idx_cursor],255,0,0);         
                        checkController();
                        break;
                      }
                      if (keyQuit==1)
                      {
                        while (keyQuit==1) {checkController();}
                        putPixel(0, pixelx_pivot[idx_cursor], pixely_pivot[idx_cursor],255,0,0);         
                        checkController();
                        break;              
                      }
                      if (keyRight==1)
                      {
                                      
                       while (keyRight==1) {checkController();}
                        pixelx_pivot[idx_cursor]++;
                        recalculeDepuisPivot();
                        retrace();
                      }
                      if (keyLeft==1)
                      { while (keyLeft==1) {checkController();}
                        pixelx_pivot[idx_cursor]--;
                        recalculeDepuisPivot();
                        retrace();
                      }
                      if (keyUp==1)
                      {while (keyUp==1) {checkController();}
                        pixely_pivot[idx_cursor]--;
                        recalculeDepuisPivot();
                        retrace();
                      }
                      if (keyDown==1)
                      {while (keyDown==1) {checkController();}
                        pixely_pivot[idx_cursor]++;
                        recalculeDepuisPivot();
                        retrace();
                      }

                      
              }
          }
        
		SDL_PumpEvents();
		// Si on appuie sur le bouton on trace un pixel
		if(SDL_GetMouseState(&x, &y)&SDL_BUTTON(1))
		{
			if ((x!=oldx) || (y!=oldy))
			{
				if ((oldx>0) && (oldy>0))  MyLine(oldx,oldy,x,y);
				oldx = x;
				oldy = y;
				pixelx_pivot[idx_pivot] = x;
				pixely_pivot[idx_pivot] = y;
				idx_pivot++;
				/*idx++;*/
			}
			putPixel(0, x,y,255,255,5);  
		}

		if (keyQuit==1)
		{
			pixelx[idx]=-1;
			pixely[idx]=-1;
			genererFichierC();
			return 0;  
		}
		
		blitImageToScreen(0,0,0,320,240,0,0,320,240,320,240);
		/*showSprite(0);*/
		flipScreen();
	}
}
