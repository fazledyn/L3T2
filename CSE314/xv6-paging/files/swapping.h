#ifndef SWAPPING_H
#define SWAPPING_H

//  Swapping Algorithm
#define SWAP_FIFO    1
#define SWAP_NFU     2

#define SWAP_ALGO   1

void update_nfu();
void update_each_nfu();

void swap_out (struct proc*);
void swap_in  (struct proc*, uint);

#endif