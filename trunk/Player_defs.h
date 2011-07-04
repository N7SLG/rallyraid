
#ifndef PLAYER_DEFS_H
#define PLAYER_DEFS_H

#define VIEW_NUM_MAX    3
#define VIEW_MASK_MAX   4
#define VIEW_SIZE       (VIEW_NUM_MAX*VIEW_MASK_MAX)

// views
#define VIEW_0          0
#define VIEW_1          4
#define VIEW_2          8

// masks
#define VIEW_CENTER     0
#define VIEW_LEFT       1
#define VIEW_RIGHT      2
#define VIEW_BEHIND     3

#define CALC_VIEW(num, mask) (num+mask)
// (num*VIEW_MASK_MAX+mask)

#define SWITCH_TO_NEXT_VIEW(num)                    \
    do {                                            \
        num = (num + VIEW_MASK_MAX) % VIEW_SIZE;    \
    }                                               \
    while (0);


#endif // PLAYER_DEFS_H
