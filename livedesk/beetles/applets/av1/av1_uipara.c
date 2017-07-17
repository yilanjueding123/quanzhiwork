
#include "av1_uipara.h"

static av1_uipara_t av1_uipara_480_270 =
{

    {0, 0, 480, 272},
    {20, 20, 100, 30}
};

av1_uipara_t *app_av1_get_uipara()
{
    return &av1_uipara_480_270;
}


