/*****************************************************************************

	 IMPORTANT: You *must* use palloc0 and pfree, not malloc and free, in your
	 implementation.  This will allow your filter to integrate into PostgreSQL.

******************************************************************************/

#include "postgres.h"
#include "utils/cmsketch.h"

/* initialize the count-min sketch for the specified width and depth */
cmsketch* init_sketch(uint32 width, uint32 depth) {
  cmsketch* cmsPtr=(struct cmsketch *)palloc(sizeof(cmsketch));
  uint32* emptytable = palloc0(width*depth*sizeof(uint32));
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
  uint32 width = sketch->width;
  for(i=0; i < sketch->depth; i++) {
    uint32 index = i*width + bits[i];
    sketch->table[index]++;
  }
}

/* decrement 'bits' in each sketch by 1.
 * 'bits' is an array of indices into each row of the sketch.
 *    Thus, each index is between 0 and 'width', and there are 'depth' of them.
 */
void decrement_bits(cmsketch* sketch, uint32 *bits) {
  int i;
  uint32 width = sketch->width;
  for(i=0; i < sketch->depth; i++) {
    uint32 index = i*width + bits[i]; 
    if(sketch->table[index] > 0) {
      sketch->table[index]--;
    }
  }
}

/* return the minimum among the indicies pointed to by 'bits'
 * 'bits' is an array of indices into each row of the sketch.
 *    Thus, each index is between 0 and 'width', and there are 'depth' of them.
 */
uint32 estimate(cmsketch* sketch, uint32 *bits) {
  uint32 minSoFar = 0-1; //max uint32 value
  int i;
  uint32 width = sketch->width;
  for(i=0; i < sketch->depth; i++) {
    uint32 index = i*width + bits[i];
    if(sketch->table[index] < minSoFar) {
      minSoFar = sketch->table[index];
    }
  }
  return minSoFar;
}

/* set all values in the sketch to zero */
void reset_sketch(cmsketch* sketch) {
  pfree(sketch->table);
  uint32_t* newtable = palloc0(sketch->width*sketch->depth*sizeof(uint32));
  sketch->table=newtable;
}

/* destroy the sketch, freeing any memory it might be using */
void destroy_sketch(cmsketch* sketch) {
  pfree(sketch);
  sketch=NULL;
}
