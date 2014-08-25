/* ------------------------------------------------------------------ */
/* Decimal Context module                                             */
/* ------------------------------------------------------------------ */
/* Copyright (c) IBM Corporation, 2000, 2005.  All rights reserved.   */
/*                                                                    */
/* This software is made available under the terms of the             */
/* ICU License -- ICU 1.8.1 and later.                                */
/*                                                                    */
/* The description and User's Guide ("The decNumber C Library") for   */
/* this software is called decNumber.pdf.  This document is           */
/* available, together with arithmetic and format specifications,     */
/* testcases, and Web links, at: http://www2.hursley.ibm.com/decimal  */
/*                                                                    */
/* Please send comments, suggestions, and corrections to the author:  */
/*   mfc@uk.ibm.com                                                   */
/*   Mike Cowlishaw, IBM Fellow                                       */
/*   IBM UK, PO Box 31, Birmingham Road, Warwick CV34 5JL, UK         */
/* ------------------------------------------------------------------ */
/* This module comprises the routines for handling arithmetic         */
/* context structures.                                                */
/* ------------------------------------------------------------------ */

#include "decContext.h"       // context and base types
#include "decNumberLocal.h"   // decNumber local types, etc.

/* ------------------------------------------------------------------ */
/* decContextDefault -- initialize a context structure                */
/*                                                                    */
/*  context is the structure to be initialized                        */
/*  kind selects the required set of default values, one of:          */
/*      DEC_INIT_BASE       -- select ANSI X3-274 defaults            */
/*      DEC_INIT_DECIMAL32  -- select IEEE 754r defaults, 32-bit      */
/*      DEC_INIT_DECIMAL64  -- select IEEE 754r defaults, 64-bit      */
/*      DEC_INIT_DECIMAL128 -- select IEEE 754r defaults, 128-bit     */
/*      For any other value a valid context is returned, but with     */
/*      Invalid_operation set in the status field.                    */
/*  returns a context structure with the appropriate initial values.  */
/* ------------------------------------------------------------------ */
decContext * decContextDefault(decContext *context, Int kind) {
  // set defaults...
  context->digits=9;                         // 9 digits
  context->emax=DEC_MAX_EMAX;                // 9-digit exponents
  context->emin=DEC_MIN_EMIN;                // .. balanced
  context->round=DEC_ROUND_HALF_UP;          // 0.5 rises
//  context->traps=DEC_Errors;                 // all but informational
  context->status=0;                         // cleared
  context->clamp=0;                          // no clamping
  #if DECSUBSET
  context->extended=0;                       // cleared
  #endif
  switch (kind) {
    case DEC_INIT_BASE:
      // [use defaults]
      break;
#if 0
    case DEC_INIT_DECIMAL32:
      context->digits=7;                     // digits
      context->emax=96;                      // Emax
      context->emin=-95;                     // Emin
      context->round=DEC_ROUND_HALF_EVEN;    // 0.5 to nearest even
//      context->traps=0;                      // no traps set
      context->clamp=1;                      // clamp exponents
      #if DECSUBSET
      context->extended=1;                   // set
      #endif
      break;
#endif
    case DEC_INIT_DECIMAL64:
      context->digits=16;                    // digits
      context->emax=384;                     // Emax
      context->emin=-383;                    // Emin
      context->round=DEC_ROUND_HALF_EVEN;    // 0.5 to nearest even
//      context->traps=0;                      // no traps set
      context->clamp=1;                      // clamp exponents
      #if DECSUBSET
      context->extended=1;                   // set
      #endif
      break;
#if 1
    case DEC_INIT_DECIMAL128:
      context->digits=34;                    // digits
      context->emax=6144;                    // Emax
      context->emin=-6143;                   // Emin
      context->round=DEC_ROUND_HALF_EVEN;    // 0.5 to nearest even
//      context->traps=0;                      // no traps set
      context->clamp=1;                      // clamp exponents
      #if DECSUBSET
      context->extended=1;                   // set
      #endif
      break;
#endif

    default:                                 // invalid Kind
      // use defaults, and ..
      decContextSetStatus(context, DEC_Invalid_operation); // trap
    }
  return context;} // decContextDefault


/* ------------------------------------------------------------------ */
/* decContextSetStatus -- set status and raise trap if appropriate    */
/*                                                                    */
/*  context is the controlling context                                */
/*  status  is the DEC_ exception code                                */
/*  returns the context structure                                     */
/*                                                                    */
/* Control may never return from this routine, if there is a signal   */
/* handler and it takes a long jump.                                  */
/* ------------------------------------------------------------------ */
decContext * decContextSetStatus(decContext *context, uInt status) {
  context->status|=status;
//  if (status & context->traps) raise(SIGFPE);
  return context;} // decContextSetStatus

