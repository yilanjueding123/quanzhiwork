

/*************************************************
   epos related functions
*/


#include <stdarg.h>
#include "epdk.h"


int fsyslib_vfprintf( ES_FILE *fp, const char *str, va_list argp)
{
    char string[13];
    char *p;
    __s32 hex_flag ;
    int numbytes = 0;
    char fmt = '%';

    while( *str )
    {
        if( *str == '%' )
        {
            ++str;
            p = string;
            hex_flag = HEX_X;
            switch( *str )
            {
            case 'd':
                eLIBs_int2str_dec( va_arg( argp,  __s32 ), string );
                eLIBs_fwrite(p, eLIBs_strlen(p), 1 , fp );  // write string to file fp
                numbytes += eLIBs_strlen(p);
                ++str;
                break;
            case 'x':
                hex_flag = HEX_x;	         // jump to " case 'X' "
            case 'p':
            case 'X':
                eLIBs_int2str_hex( va_arg( argp,  __s32 ), string, hex_flag );
                eLIBs_fwrite(p, eLIBs_strlen(p), 1 , fp );
                numbytes += eLIBs_strlen(p);
                ++str;
                break;
            case 'u':
                eLIBs_uint2str_dec( va_arg( argp,  __s32 ), string );
                eLIBs_fwrite(p, eLIBs_strlen(p), 1 , fp );
                numbytes += eLIBs_strlen(p);
                ++str;
                break;
            case 'c':
                string[0] = va_arg( argp,  __s32 );
                eLIBs_fwrite(&(string[0]), sizeof(char), 1 , fp );
                numbytes++;
                ++str;
                break;
            case 's':
                p =  va_arg( argp,  char * );
                eLIBs_fwrite(p, eLIBs_strlen(p), 1 , fp );
                numbytes += eLIBs_strlen(p);
                ++str;
                break;
            default :
                eLIBs_fwrite(&fmt, sizeof(char), 1 , fp );
                numbytes ++;
                eLIBs_fwrite(str, sizeof(char), 1 , fp );
                numbytes ++;
                ++str;                        // let 'str' point to next character.
            }
        }

        else
        {
            eLIBs_fwrite(str++, sizeof(char), 1 , fp );
            numbytes ++;
        }
    }
    //mira del
    //fmt = '\0';
    //eLIBs_fwrite(&fmt, sizeof(char), 1 , fp );

    return numbytes;
}




int epos_fprintf( ES_FILE *fp, const char *str, ...)
{
    int res;
    va_list argp;

    va_start( argp, str );
    res = fsyslib_vfprintf(fp, str, argp);
    va_end( argp );

    return res;
}
