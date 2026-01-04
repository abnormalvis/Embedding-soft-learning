#ifndef __GLO_
#define __GLO_

struct stuBase
{
        int id;
        char name[9];
        char sex;
        int age;
};

struct stuAccount
{
        char user[16];
        char password[16];
};

struct stuScore
{
        float eng;
        float chi;
        float mat;
};

struct stuInfo
{
        struct stuBase stubs;
        struct stuAccount stuac;
        struct stuScore *stuSco;
};

#endif
