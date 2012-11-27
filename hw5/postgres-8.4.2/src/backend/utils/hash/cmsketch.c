/*****************************************************************************

	 IMPORTANT: You *must* use palloc0 and pfree, not malloc and free, in your
	 implementation.  This will allow your filter to integrate into PostgreSQL.

******************************************************************************/

#include "postgres.h"
#include "utils/cmsketch.h"

/* initialize the count-min sketch for the specified width and depth */
cmsketch* init_sketch(uint32 width, uint32 depth) {
  cmsketch* cmsPtr=(struct cmsketch *)palloc0(sizeof(cmsketch));
  uint32 emptytable[depth][width]={0};
  cmsPtr->table=emptytable;
  cmsPtr->width=width;
  cmsPtr->depth=depth;
  return cmsPtr;
}

/* increment 'bits' in each sketch by 1. 
 * 'bits' is an array of indices into each row of the sketch.
 *    Thus, each index is between 0 and 'width', and there are 'depth' of them.
 */
void increment_bits(cmsketch* sketch, uint32 *bits) {
  int i;
  for(i=0; i<sketch->depth; i++) {
    sketch->table[i][bits[i]]++; //=sketch->table[i][bits[i]]+1;
  }
}

/* decrement 'bits' in each sketch by 1.
 * 'bits' is an array of indices into each row of the sketch.
 *    Thus, each index is between 0 and 'width', and there are 'depth' of them.
 */
void decrement_bits(cmsketch* sketch, uint32 *bits) {
  int i;
  for(i=0; i<sketch->depth; i++) {
    if(sketch->table[i][bits[i]]>0) {
      sketch->table[i][bits[i]]--;
    }
  }
}

/* return the minimum among the indicies pointed to by 'bits'
 * 'bits' is an array of indices into each row of the sketch.
 *    Thus, each index is between 0 and 'width', and there are 'depth' of them.
 */
uint32 estimate(cmsketch* sketch, uint32 *bits) {
  uint32 minSoFar=0;
  int i;
  for(i=0; i<sketch->depth; i++) {
    if(sketch->table[i][bits[i]]<minSoFar) {
      minSoFar=sketch->table[i][bits[i]];
    }
  }
  return minSoFar;
}

/* set all values in the sketch to zero */
void reset_sketch(cmsketch* sketch) {
  uint32_t newtable[][]={0};
  sketch->table=newtable;
}

/* destroy the sketch, freeing any memory it might be using */
void destroy_sketch(cmsketch* sketch) {
  pfree(sketch);
  sketch=NULL;
}
