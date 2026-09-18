/*
   Created by ervin on [2026. 09. 23.].
*/

#ifndef HADRON_PUFFER_H
#define HADRON_PUFFER_H = 4096
#define TAILS_PUSH(arr, max, head, tail, count, value) \
    do { \
        if ((count) < (max)) { \
            (arr)[(head)] = (value); \
            (head) = ((head) + 1) % (max); \
            (count)++; \
        } else { \
            /* Ha tele van, a legrégebbit (tail) kidobjuk, és oda írunk */ \
            (arr)[(head)] = (value); \
            (head) = ((head) + 1) % (max); \
            (tail) = ((tail) + 1) % (max); /* A legrégebbi mutatója is lép, így az előző kiesett! */ \
        } \
    } while(0)

#endif /* HADRON_PUFFER_H */
