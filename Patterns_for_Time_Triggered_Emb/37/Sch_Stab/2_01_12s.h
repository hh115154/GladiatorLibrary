/*------------------------------------------------------------------*-
  
   2_01_12s.h (v1.00) 

  ------------------------------------------------------------------

   - see 2_01_12s.C for details


   COPYRIGHT
   ---------

   This code is from the book:

   PATTERNS FOR TIME-TRIGGERED EMBEDDED SYSTEMS by Michael J. Pont 
   [Pearson Education, 2001; ISBN: 0-201-33138-1].

   This code is copyright (c) 2001 by Michael J. Pont.
 
   See book for copyright details and other information.

-*------------------------------------------------------------------*/

#include "Main.h"
#include "SCH51.H"


// ------ Public function prototypes -------------------------------

void SCH_Init_T2(void);
void SCH_Start(void);
void SCH_Calculate_Ave_Temp_DS1621(void);
void SCH_Perform_Temperature_Adjustment(void);
                              
/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/

