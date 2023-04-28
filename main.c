#include <stdlib.h> 
#if defined(_MSC_VER)
#include "SDL.h"
#else 
#include "SDL/SDL.h"
#endif     
                 
#include "common.h"                     
                                   
// Entry point            
int main(int argc, char *argv[])
{          
 common(0);         
  return 0;  
}
