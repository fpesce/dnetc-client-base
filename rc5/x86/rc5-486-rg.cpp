// Copyright distributed.net 1997 - All Rights Reserved
// For use in distributed.net projects only.
// Any other distribution or use of this source violates copyright.
//
// $Log: rc5-486-rg.cpp,v $
// Revision 1.4  1998/06/14 10:03:54  skand
// define and use a preprocessor macro to hide the .balign directive for
// ancient assemblers
//
// Revision 1.3  1998/06/14 08:27:16  friedbait
// 'Id' tags added in order to support 'ident' command to display a bill of
// material of the binary executable
//
// Revision 1.2  1998/06/14 08:13:33  friedbait
// 'Log' keywords added to maintain automatic change history
//
//
// 386/486 optimized version
// R�mi Guyomarch - rguyom@mail.dotcom.fr
//
// 980226 :
//	- Corrected bug in the key incrementation algorithm that caused the
//	  client to core-dump at the end of some blocks.
//	  As a side-effect, this fix re-enable support for blocks of up to 2^64 keys
//	- Added alignement pseudo-instructions.
//	- Converted :
//		subl $0x01000000, %%reg   to   addl $0xFF000100, %%reg
//		addl $0x00000100, %%reg
//	  and :
//		subl $0x00010000, %%reg   to   addl $0xFFFF0001, %%reg
//		addl $0x00000001, %%reg
//
// 980104 :
//	- precalculate some things for ROUND1 & ROUND2
//
// Checking two keys at once is still a (slight) win for a 486
// probably because less load/store operations

static char *id="@(#)$Id: rc5-486-rg.cpp,v 1.4 1998/06/14 10:03:54 skand Exp $";

#define CORE_INCREMENTS_KEY

// This file is included from rc5.cpp so we can use __inline__.
#include "problem.h"

#if (PIPELINE_COUNT != 2)
#error "Expecting pipeline count of 2"
#endif

#ifndef _CPU_32BIT_
#error "everything assumes a 32bit CPU..."
#endif


// Stringify macro.

#define _(s)    __(s)
#define __(s)   #s

#if defined(__NetBSD__) || defined(__bsdi__)
#define BALIGN(x)
#else
#define BALIGN(x) ".balign 4"
#endif

// The S0 values for key expansion round 1 are constants.

#define P         0xB7E15163
#define Q         0x9E3779B9
#define S_not(N)  _((P+Q*(N)))

//  Structure used in rc5_unit_func_*

struct work_struct {
    u32 add_iter;	// +  0
    u32 s1[26];		// +  4
    u32 s2[26];		// +108
    u32 P_0;		// +212
    u32 P_1;		// +216
    u32 C_0;		// +220
    u32 C_1;		// +224
    u32 save_ebp;	// +228
    u32 key2_ebp;	// +232
    u32 key2_edi;	// +236
    u32 key2_esi;	// +240
    u32 key_hi;		// +244
    u32 key_lo;		// +248
    u32 iterations;	// +252
    u32 pre1_r1;	// +256
    u32 pre2_r1;	// +260
    u32 pre3_r1;	// +264
};

//  Offsets to access work_struct fields.

#define	work_add_iter   "0+%0"
#define	work_s1         "4+%0"
#define	work_s2         "108+%0"
#define	work_P_0        "212+%0"
#define	work_P_1        "216+%0"
#define	work_C_0        "220+%0"
#define	work_C_1        "224+%0"
#define	work_save_ebp   "228+%0"
#define	work_key2_ebp   "232+%0"
#define	work_key2_edi   "236+%0"
#define	work_key2_esi   "240+%0"
#define work_key_hi     "244+%0"
#define work_key_lo     "248+%0"
#define work_iterations "252+%0"
#define work_pre1_r1    "256+%0"
#define work_pre2_r1    "260+%0"
#define work_pre3_r1    "264+%0"

//  Macros to access the S arrays.

#define S1(N)    _(((N)*4)+4+%0)
#define S2(N)    _(((N)*4)+108+%0)

//  Offsets to access struct RC5UnitWork fields.

#define RC5UnitWork_plainhi   "0"
#define RC5UnitWork_plainlo   "4"
#define RC5UnitWork_cypherhi  "8"
#define RC5UnitWork_cypherlo  "12"
#define RC5UnitWork_L0hi      "16"
#define RC5UnitWork_L0lo      "20"

  // A1   = %eax  A2   = %ebp
  // Llo1 = %ebx  Llo2 = %esi
  // Lhi1 = %edx  Lhi2 = %edi

// ------------------------------------------------------------------
// S1(N) = A1 = ROTL3 (A1 + Lhi1 + S_not(N));
// S2(N) = A2 = ROTL3 (A2 + Lhi2 + S_not(N));
// Llo1 = ROTL (Llo1 + A1 + Lhi1, A1 + Lhi1);
// Llo2 = ROTL (Llo2 + A2 + Lhi2, A2 + Lhi2);
#define ROUND_1_EVEN(N)	\
"	leal	"S_not(N)"(%%eax,%%edx),%%eax	# 2
	leal	"S_not(N)"(%%ebp,%%edi),%%ebp	# 2
	roll	$3,    %%eax			# 2
	movl	%%eax, "S1(N)"			# 1
	roll	$3,    %%ebp			# 2
	movl	%%ebp, "S2(N)"			# 1
	leal	(%%eax,%%edx), %%ecx		# 2
	addl	%%ecx, %%ebx			# 1
	roll	%%cl,  %%ebx			# 3
	leal	(%%ebp,%%edi), %%ecx		# 2
	addl	%%ecx, %%esi			# 1
	roll	%%cl,  %%esi			# 3	sum = 22 \n"

// S1(N) = A1 = ROTL3 (A1 + Llo1 + S_not(N));
// S2(N) = A2 = ROTL3 (A2 + Llo2 + S_not(N));
// Lhi1 = ROTL (Lhi1 + A1 + Llo1, A1 + Llo1);
// Lhi2 = ROTL (Lhi2 + A2 + Llo2, A2 + Llo2);
#define ROUND_1_ODD(N) \
"	leal	"S_not(N)"(%%eax,%%ebx),%%eax	# 2
	leal	"S_not(N)"(%%ebp,%%esi),%%ebp	# 2
	roll	$3,    %%eax			# 2
	movl	%%eax, "S1(N)"			# 1
	roll	$3,    %%ebp			# 2
	movl	%%ebp, "S2(N)"			# 1
	leal	(%%eax,%%ebx), %%ecx		# 2
	addl	%%ecx, %%edx			# 1
	roll	%%cl,  %%edx			# 3
	leal	(%%ebp,%%esi), %%ecx		# 2
	addl	%%ecx, %%edi			# 1
	roll	%%cl,  %%edi			# 3	sum = 22 \n"

#define ROUND_1_ODD_AND_EVEN(N1,N2) \
	ROUND_1_ODD (N1) \
	ROUND_1_EVEN(N2)

// ------------------------------------------------------------------
// S1N = A1 = ROTL3 (A1 + Lhi1 + S1N);
// S2N = A2 = ROTL3 (A2 + Lhi2 + S2N);
// Llo1 = ROTL (Llo1 + A1 + Lhi1, A1 + Lhi1);
// Llo2 = ROTL (Llo2 + A2 + Lhi2, A2 + Lhi2);
#define ROUND_2_EVEN(N) \
"	addl	%%edx,  %%eax		# 1
	addl	"S1(N)",%%eax		# 2
	addl	%%edi,  %%ebp		# 1
	addl	"S2(N)",%%ebp		# 2
	roll	$3,     %%eax		# 2
	movl	%%eax,  "S1(N)"		# 1
	roll	$3,     %%ebp		# 2
	movl	%%ebp,  "S2(N)"		# 1
	leal	(%%eax, %%edx), %%ecx	# 2
	addl	%%ecx,  %%ebx		# 1
	roll	%%cl,   %%ebx		# 3
	leal	(%%ebp, %%edi), %%ecx	# 2
	addl	%%ecx,  %%esi		# 1
	roll	%%cl,   %%esi		# 3	sum = 24 \n"
	
// S1N = A1 = ROTL3 (A1 + Llo1 + S1N);
// S2N = A2 = ROTL3 (A2 + Llo2 + S2N);
// Lhi1 = ROTL (Lhi1 + A1 + Llo1, A1 + Llo1);
// Lhi2 = ROTL (Lhi2 + A2 + Llo2, A2 + Llo2);
#define ROUND_2_ODD(N) \
"	addl	%%ebx,  %%eax		# 1
	addl	"S1(N)",%%eax		# 2
	addl	%%esi,  %%ebp		# 1
	addl	"S2(N)",%%ebp		# 2
	roll	$3,     %%eax		# 2
	movl	%%eax,  "S1(N)"		# 1
	roll	$3,     %%ebp		# 2
	movl	%%ebp,  "S2(N)"		# 1
	leal	(%%eax, %%ebx), %%ecx	# 2
	addl	%%ecx,  %%edx		# 1
	roll	%%cl,   %%edx		# 3
	leal	(%%ebp, %%esi), %%ecx	# 2
	addl	%%ecx,  %%edi		# 1
	roll	%%cl,   %%edi		# 3	sum = 24 \n"

#define ROUND_2_ODD_AND_EVEN(N1,N2) \
	ROUND_2_ODD (N1) \
	ROUND_2_EVEN(N2)

// ------------------------------------------------------------------
// It's faster to do 1 key at a time with round3 and encryption mixed
// than to do 2 keys at once but round3 and encryption separated
// Too bad x86 hasn't more registers ...
	
// eA1 = ROTL (eA1 ^ eB1, eB1) + (A1 = ROTL3 (A1 + Lhi1 + S1(N)));
// Llo1 = ROTL (Llo1 + A1 + Lhi1, A1 + Lhi1);
// eB1 = ROTL (eA1 ^ eB1, eA1) + (A1 = ROTL3 (A1 + Llo1 + S1(N)));
// Lhi1 = ROTL (Lhi1 + A1 + Llo1, A1 + Llo1);

// A  = %eax  eA = %esi
// L0 = %ebx  eB = %edi
// L1 = %edx  .. = %ebp
#define ROUND_3_EVEN_AND_ODD(N,Sx) \
"_round3_486_"_(Sx)"_"_(N)":
	addl	%%edx,    %%eax		# 1
	addl	"Sx(N)",  %%eax		# 2
	roll	$3,       %%eax		# 2
	movl	%%edi,    %%ecx		# 1
	xorl	%%edi,    %%esi		# 1
	roll	%%cl,     %%esi		# 3
	addl	%%eax,    %%esi		# 1
	leal	(%%eax,   %%edx), %%ecx	# 2
	addl	%%ecx,    %%ebx		# 1
	roll	%%cl,     %%ebx		# 3
	
	addl	%%ebx,    %%eax		# 1
	addl	"Sx(N+1)",%%eax		# 2
	roll	$3,       %%eax		# 2
	movl	%%esi,    %%ecx		# 1
	xorl	%%esi,    %%edi		# 1
	roll	%%cl,     %%edi		# 3
	addl	%%eax,    %%edi		# 1
	leal	(%%eax,   %%ebx), %%ecx	# 2
	addl	%%ecx,    %%edx		# 1
	roll	%%cl,     %%edx		# 3	sum = 34 \n"


// ------------------------------------------------------------------
// rc5_unit will get passed an RC5WorkUnit to complete
// this is where all the actually work occurs, this is where you optimize.
// assembly gurus encouraged.
// Returns number of keys checked before a possible good key is found, or
// timeslice*PIPELINE_COUNT if no keys are 'good' keys.
// (ie:      if (result == timeslice*PIPELINE_COUNT) NOTHING_FOUND
//      else if (result < timeslice*PIPELINE_COUNT) SOMETHING_FOUND at result+1
//      else SOMETHING_GET_WRONG... )

// There is no way to tell gcc to save %ebp.
//	(putting %ebp in the clobbered register list has no effect)
// Even worse, if '-fomit-frame-pointer' isn't used, gcc will compile
// this function with local variables referenced with %ebp (!!).
//
// I use a structure to make this function multi-thread safe.
// (can't use static variables, and can't use push/pop in this
//  function because &work_struct is relative to %esp)

//static
u32 rc5_unit_func_486( RC5UnitWork * rc5unitwork, u32 timeslice )
{
    work_struct work;

    work.iterations = timeslice;
    work.add_iter = 0;

    __asm__ __volatile__ (

	/* save %ebp */
"	movl	%%ebp, "work_save_ebp" \n"
	
	/* pointer to rc5unitwork already loaded in %eax (see constraint 'a') */

	/* load parameters */
"	movl	"RC5UnitWork_L0lo"(%%eax), %%ebx	# ebx = l0 = Llo1
	movl	"RC5UnitWork_L0hi"(%%eax), %%edx	# edx = l1 = Lhi1
	movl	%%ebx, %%esi				# esi = l2 = Llo2
	leal	0x01000000(%%edx), %%edi		# edi = l3 = lhi2
	movl	%%ebx, "work_key_lo"
	movl	%%edx, "work_key_hi" \n"

	/* Save other parameters */
	/* (it's faster to do so, since we will only load 1 value */
	/* each time in RC5_ROUND_3xy, instead of two if we save  */
	/* only the pointer to the RC5 struct)                    */
"	movl	"RC5UnitWork_plainlo"(%%eax), %%ebp
	movl	%%ebp, "work_P_0"
	movl	"RC5UnitWork_plainhi"(%%eax), %%ebp
	movl	%%ebp, "work_P_1"
	movl	"RC5UnitWork_cypherlo"(%%eax), %%ebp
	movl	%%ebp, "work_C_0"
	movl	"RC5UnitWork_cypherhi"(%%eax), %%ebp
	movl	%%ebp, "work_C_1" \n"

	/* Pre-calculate things. Assume work.key_lo won't change it this loop */
	/* (it's pretty safe to assume that, because we're working on 28 bits */
	/* blocks) */
	/* It means also that %%ebx == %%esi (Llo1 == Llo2) */

//#define S0_ROTL3  _(((P<<3) | (P>>29)))
#define S0_ROTL3 _(0xbf0a8b1d)
//#define FIRST_ROTL _((S0_ROTL3 & 0x1f))
#define FIRST_ROTL _(0x1d)
//#define S1_S0_ROTL3 _((S_not(1) + S0_ROTL3))
#define S1_S0_ROTL3 _(0x15235639)

"_bigger_loop_486:
	addl	$"S0_ROTL3", %%ebx		# 1
	roll	$"FIRST_ROTL",  %%ebx		# 3
	movl	%%ebx, "work_pre1_r1"		# 1

	leal	"S1_S0_ROTL3"(%%ebx),%%eax	# 1
	roll	$3,    %%eax			# 2
	movl	%%eax, "work_pre2_r1"		# 1

	leal	(%%eax,%%ebx), %%ecx		# 2
	movl	%%ecx, "work_pre3_r1"		# 1

"BALIGN(4)"
_loaded_486:\n"

    /* ------------------------------ */
    /* Begin round 1 of key expansion */
    /* ------------------------------ */

"	movl	"work_pre1_r1", %%ebx		# 1
	movl	%%ebx, %%esi			# 1
	movl	"work_pre2_r1", %%eax		# 1
	movl	%%eax, %%ebp			# 1

	movl	"work_pre3_r1", %%ecx
	addl	%%ecx, %%edx			# 1
	roll	%%cl,  %%edx			# 3
	addl	%%ecx, %%edi			# 1
	roll	%%cl,  %%edi			# 3	sum = 12 \n"

	ROUND_1_EVEN            ( 2)
	ROUND_1_ODD_AND_EVEN ( 3, 4)
	ROUND_1_ODD_AND_EVEN ( 5, 6)
	ROUND_1_ODD_AND_EVEN ( 7, 8)
	ROUND_1_ODD_AND_EVEN ( 9,10)
	ROUND_1_ODD_AND_EVEN (11,12)
	ROUND_1_ODD_AND_EVEN (13,14)
	ROUND_1_ODD_AND_EVEN (15,16)
	ROUND_1_ODD_AND_EVEN (17,18)
	ROUND_1_ODD_AND_EVEN (19,20)
	ROUND_1_ODD_AND_EVEN (21,22)
	ROUND_1_ODD_AND_EVEN (23,24)
	ROUND_1_ODD          (25)


    /* ------------------------------ */
    /* Begin round 2 of key expansion */
    /* ------------------------------ */

"_end_round1_486:
	leal	"S0_ROTL3"(%%eax,%%edx),%%eax	# 2
	leal	"S0_ROTL3"(%%ebp,%%edi),%%ebp	# 2
	roll	$3,    %%eax		# 2
	movl	%%eax, "S1(0)"		# 1
	roll	$3,    %%ebp		# 2
	movl	%%ebp, "S2(0)"		# 1
	
	leal	(%%eax,%%edx), %%ecx	# 2
	addl	%%ecx, %%ebx		# 1
	roll	%%cl,  %%ebx		# 3
	leal	(%%ebp,%%edi), %%ecx	# 2
	addl	%%ecx, %%esi		# 1
	roll	%%cl,  %%esi		# 3 \n"

"	movl	"work_pre2_r1", %%ecx	# 1
	addl	%%ebx,  %%eax		# 1
	addl	%%ecx,  %%eax		# 1
	addl	%%esi,  %%ebp		# 1
	addl	%%ecx  ,%%ebp		# 1
	roll	$3,     %%eax		# 2
	movl	%%eax,  "S1(1)"		# 1
	roll	$3,     %%ebp		# 2
	movl	%%ebp,  "S2(1)"		# 1
	leal	(%%eax, %%ebx), %%ecx	# 2
	addl	%%ecx,  %%edx		# 1
	roll	%%cl,   %%edx		# 3
	leal	(%%ebp, %%esi), %%ecx	# 2
	addl	%%ecx,  %%edi		# 1
	roll	%%cl,   %%edi		# 3	sum = 23 \n"

        ROUND_2_EVEN            ( 2)
        ROUND_2_ODD_AND_EVEN ( 3, 4)
        ROUND_2_ODD_AND_EVEN ( 5, 6)
        ROUND_2_ODD_AND_EVEN ( 7, 8)
        ROUND_2_ODD_AND_EVEN ( 9,10)
        ROUND_2_ODD_AND_EVEN (11,12)
        ROUND_2_ODD_AND_EVEN (13,14)
        ROUND_2_ODD_AND_EVEN (15,16)
        ROUND_2_ODD_AND_EVEN (17,18)
        ROUND_2_ODD_AND_EVEN (19,20)
        ROUND_2_ODD_AND_EVEN (21,22)
        ROUND_2_ODD_AND_EVEN (23,24)
	ROUND_2_ODD (25)

    /* Save 2nd key parameters and initialize result variable

       I'm using the stack instead of a memory location, because
       gcc don't allow me to put more than 10 constraints in an
       asm() statement.
    */
"_end_round2_486:
	movl	%%ebp,"work_key2_ebp"
	movl	%%esi,"work_key2_esi"
	movl	%%edi,"work_key2_edi" \n"

    /* ---------------------------------------------------- */
    /* Begin round 3 of key expansion mixed with encryption */
    /* ---------------------------------------------------- */
    /* (first key)					    */

	// A  = %eax  eA = %esi
	// L0 = %ebx  eB = %edi
	// L1 = %edx  .. = %ebp

"	addl	%%edx,  %%eax		# 1	A = ROTL3(S00 + A + L1);
	addl	"S1(0)",%%eax		# 2
	roll	$3,     %%eax		# 2
	movl	"work_P_0", %%esi	# 1	eA = P_0 + A;
	addl	%%eax,  %%esi		# 1
	leal	(%%eax, %%edx), %%ecx	# 2	L0 = ROTL(L0 + A + L1, A + L1);
	addl	%%ecx,  %%ebx		# 1
	roll	%%cl,   %%ebx		# 3
	       			
	addl	%%ebx,  %%eax		# 1	A = ROTL3(S01 + A + L0);
	addl	"S1(1)",%%eax		# 2
	roll	$3,     %%eax		# 2
	movl	"work_P_1", %%edi	# 1	eB = P_1 + A;
	addl	%%eax,  %%edi		# 1
	leal	(%%eax, %%ebx), %%ecx	# 2	L1 = ROTL(L1 + A + L0, A + L0);
	addl	%%ecx,  %%edx		# 1
	roll	%%cl,   %%edx		# 3	sum = 26 \n"
	ROUND_3_EVEN_AND_ODD ( 2,S1)
	ROUND_3_EVEN_AND_ODD ( 4,S1)
	ROUND_3_EVEN_AND_ODD ( 6,S1)
	ROUND_3_EVEN_AND_ODD ( 8,S1)
	ROUND_3_EVEN_AND_ODD (10,S1)
	ROUND_3_EVEN_AND_ODD (12,S1)
	ROUND_3_EVEN_AND_ODD (14,S1)
	ROUND_3_EVEN_AND_ODD (16,S1)
	ROUND_3_EVEN_AND_ODD (18,S1)
	ROUND_3_EVEN_AND_ODD (20,S1)
	ROUND_3_EVEN_AND_ODD (22,S1)

	/* early exit */
"_end_round3_1_486:
	addl	%%edx,   %%eax		# 1	A = ROTL3(S24 + A + L1);
	addl	"S1(24)",%%eax		# 2
	roll	$3,      %%eax		# 2
	movl	%%edi,   %%ecx		# 1	eA = ROTL(eA ^ eB, eB) + A
	xorl	%%edi,   %%esi		# 1
	roll	%%cl,    %%esi		# 3
	addl	%%eax,   %%esi		# 1
					
	cmp	"work_C_0", %%esi
	jne	__exit_1_486
					
	leal	(%%eax,%%edx), %%ecx	# 2	L0 = ROTL(L0 + A + L1, A + L1);
	addl	%%ecx,   %%ebx		# 1
	roll	%%cl,    %%ebx		# 3
	addl	%%ebx,   %%eax		# 1	A = ROTL3(S25 + A + L0);
	addl	"S1(25)",%%eax		# 2
	roll	$3,      %%eax		# 2
	movl	%%esi,   %%ecx		# 1	eB = ROTL(eB ^ eA, eA) + A
	xorl	%%esi,   %%edi		# 1
	roll	%%cl,    %%edi		# 3
	addl	%%eax,   %%edi		# 1

	cmpl	"work_C_1", %%edi
	je	_full_exit_486

"BALIGN(4)"
__exit_1_486: \n"

    /* Restore 2nd key parameters */
"	movl	"work_key2_edi", %%edx
	movl	"work_key2_esi", %%ebx
	movl	"work_key2_ebp", %%eax\n"

    /* ---------------------------------------------------- */
    /* Begin round 3 of key expansion mixed with encryption */
    /* ---------------------------------------------------- */
    /* (second key)					    */

	// A  = %eax  eA = %esi
	// L0 = %ebx  eB = %edi
	// L1 = %edx  .. = %ebp

"	addl	%%edx,  %%eax		# 1	A = ROTL3(S00 + A + L1);
	addl	"S2(0)",%%eax		# 2
	roll	$3,     %%eax		# 2
	movl	"work_P_0", %%esi	# 1	eA = P_0 + A;
	addl	%%eax,  %%esi		# 1
	leal	(%%eax, %%edx), %%ecx	# 2	L0 = ROTL(L0 + A + L1, A + L1);
	addl	%%ecx,  %%ebx		# 1
	roll	%%cl,   %%ebx		# 3
	       			
	addl	%%ebx,  %%eax		# 1	A = ROTL3(S01 + A + L0);
	addl	"S2(1)",%%eax		# 2
	roll	$3,     %%eax		# 2
	movl	"work_P_1", %%edi	# 1	eB = P_1 + A;
	addl	%%eax,  %%edi		# 1
	leal	(%%eax, %%ebx), %%ecx	# 2	L1 = ROTL(L1 + A + L0, A + L0);
	addl	%%ecx,  %%edx		# 1
	roll	%%cl,   %%edx		# 3	sum = 26 \n"
	ROUND_3_EVEN_AND_ODD ( 2,S2)
	ROUND_3_EVEN_AND_ODD ( 4,S2)
	ROUND_3_EVEN_AND_ODD ( 6,S2)
	ROUND_3_EVEN_AND_ODD ( 8,S2)
	ROUND_3_EVEN_AND_ODD (10,S2)
	ROUND_3_EVEN_AND_ODD (12,S2)
	ROUND_3_EVEN_AND_ODD (14,S2)
	ROUND_3_EVEN_AND_ODD (16,S2)
	ROUND_3_EVEN_AND_ODD (18,S2)
	ROUND_3_EVEN_AND_ODD (20,S2)
	ROUND_3_EVEN_AND_ODD (22,S2)

	/* early exit */
"_end_round3_2_486:
	addl	%%edx,   %%eax		# 1	A = ROTL3(S24 + A + L1);
	addl	"S2(24)",%%eax		# 2
	roll	$3,      %%eax		# 2
	movl	%%edi,   %%ecx		# 1	eA = ROTL(eA ^ eB, eB) + A
	xorl	%%edi,   %%esi		# 1
	roll	%%cl,    %%esi		# 3
	addl	%%eax,   %%esi		# 1
					
	cmp	"work_C_0", %%esi
	jne	__exit_2_486
					
	leal	(%%eax,%%edx), %%ecx	# 2	L0 = ROTL(L0 + A + L1, A + L1);
	addl	%%ecx,   %%ebx		# 1
	roll	%%cl,    %%ebx		# 3
	addl	%%ebx,   %%eax		# 1	A = ROTL3(S25 + A + L0);
	addl	"S2(25)",%%eax		# 2
	roll	$3,      %%eax		# 2
	movl	%%esi,   %%ecx		# 1	eB = ROTL(eB ^ eA, eA) + A
	xorl	%%esi,   %%edi		# 1
	roll	%%cl,    %%edi		# 3
	addl	%%eax,   %%edi		# 1

	cmpl	"work_C_1", %%edi
	jne	__exit_2_486
	movl	$1, "work_add_iter"
	jmp	_full_exit_486

"BALIGN(4)"
__exit_2_486:

	movl	"work_key_hi", %%edx

"/* Jumps not taken are faster */"
	addl	$0x02000000,%%edx
	jc	_next_inc_486

_next_iter_486:
	movl	%%edx, "work_key_hi"
	leal	 0x01000000(%%edx), %%edi
	decl	"work_iterations"
	jg	_loaded_486
	movl	%1, %%eax				# pointer to rc5unitwork
	movl	"work_key_lo", %%ebx
	movl	%%ebx, "RC5UnitWork_L0lo"(%%eax)	# Update real data
	movl	%%edx, "RC5UnitWork_L0hi"(%%eax)	# (used by caller)
	jmp	_full_exit_486

"BALIGN(4)"
_next_iter2_486:
	movl	%%ebx, "work_key_lo"
	movl	%%edx, "work_key_hi"
	leal	 0x01000000(%%edx), %%edi
	movl	%%ebx, %%esi
	decl	"work_iterations"
	jg	_bigger_loop_486
	movl	%1, %%eax				# pointer to rc5unitwork
	movl	%%ebx, "RC5UnitWork_L0lo"(%%eax)	# Update real data
	movl	%%edx, "RC5UnitWork_L0hi"(%%eax)	# (used by caller)
	jmp	_full_exit_486

"BALIGN(4)"
_next_inc_486:
	addl	$0x00010000, %%edx
	testl	$0x00FF0000, %%edx
	jnz	_next_iter_486

	addl	$0xFF000100, %%edx
	testl	$0x0000FF00, %%edx
	jnz	_next_iter_486

	addl	$0xFFFF0001, %%edx
	testl	$0x000000FF, %%edx
	jnz	_next_iter_486


	movl	"work_key_lo", %%ebx

	subl	$0x00000100, %%edx
	addl	$0x01000000, %%ebx
	jnc	_next_iter2_486

	addl	$0x00010000, %%ebx
	testl	$0x00FF0000, %%ebx
	jnz	_next_iter2_486

	addl	$0xFF000100, %%ebx
	testl	$0x0000FF00, %%ebx
	jnz	_next_iter2_486

	addl	$0xFFFF0001, %%ebx
	testl	$0x000000FF, %%ebx
	jnz	_next_iter2_486

	# Moo !
	# We have just finished checking the last key
	# of the rc5-64 keyspace...
	# Not much to do here, since we have finished the block ...


"BALIGN(4)"
_full_exit_486:
	movl	"work_save_ebp", %%ebp \n"

: "=m"(work),
  "=m"(rc5unitwork)
: "a" (rc5unitwork)
: "%eax","%ebx","%ecx","%edx","%esi","%edi","cc");

    return (timeslice - work.iterations) * 2 + work.add_iter;
}
