#include "vad.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "noise_suppression.h"
#include "overallContext.h"


void output_fun(int flag ,int cache , int16_t* buffer , int16_t* buffer_out , int16_t (buffer_cache)[][160] )
{
    int i;
    int j;

    for (i = (cache - 1); i > 0; i--) 
    {

        for (j = 0; j < 160; j++) buffer_cache[i][j] = buffer_cache[i - 1][j];
    }


     for (j = 0; j < 160; j++) buffer_cache[0][j] = buffer[j];

  
    if (flag == 1) 
    {

        for (j = 0; j < 160; j++)
        {
            buffer_out[j] = buffer_cache[cache - 1][j];
        }
        
        
    }


}

