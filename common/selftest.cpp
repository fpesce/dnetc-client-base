/* 
 * Copyright distributed.net 1997-1999 - All Rights Reserved
 * For use in distributed.net projects only.
 * Any other distribution or use of this source violates copyright.
*/
const char *selftest_cpp(void) {
return "@(#)$Id: selftest.cpp,v 1.47 1999/04/17 07:38:37 gregh Exp $"; }

// --------------------------------------------------------------------------

#include "cputypes.h"
#include "client.h"    // CONTEST_COUNT
#include "baseincs.h"  // standard #includes
#include "problem.h"   // Problem class
#include "convdes.h"   // convert_key_from_des_to_inc 
#include "triggers.h"  // CheckExitRequestTriggerNoIO()
#include "logstuff.h"  // LogScreen()
#include "cpucheck.h"  // GetNumberOfDetectedProcessors() [for RISCOS]
#include "clicdata.h"  // CliGetContestNameFromID() 
#include "clievent.h"  // ClientEventSyncPost()

// --------------------------------------------------------------------------

#define TEST_CASE_COUNT 32

// note this is in .lo, .hi, .lo... order...

// RC5-32/12/8 test cases -- generated by gentests64.cpp:
u32 rc5_test_cases[TEST_CASE_COUNT][8] = { // seed = 691713
  {0x9CC718F9,0x82E51B9F,0xF839A5D9,0xC41F78C1,0x20656854,0x6E6B6E75,0xB74BE041,0x496DEF29},
  {0xC3A303DB,0x7602EDDB,0xCD51A16E,0xFF716C97,0x8E7D42C5,0x5B227595,0x43848FB4,0x128FA11D},
  {0x8A00EAE3,0x59DA3369,0x123CE8CB,0xBBA53837,0xFDF318DE,0x7CEE3A16,0x0A6A82A6,0x6B1078F6},
  {0x0000348F,0xA2098FD6,0x8F31AAC0,0x4BC9E293,0x8BE8DD8C,0xDCB84729,0x5522BD19,0xB8F0949C},
  {0x0000E77F,0x28A00B00,0x9EA7C076,0xD217C125,0xB8FF4A1E,0x75CD7B7B,0x7231EBCA,0xEAB2CC92},
  {0x000076B5,0x47FC0000,0x25FC8FF2,0x9CEE4653,0x4F47BDC3,0xC2BF5981,0xAE46D122,0x3935B960},
  {0x0000ECBB,0xAE000000,0xA369A703,0x21EE64EB,0x2DF84521,0x8C527F60,0x76D25053,0xBA546EB3},
  {0xDD46A410,0xE5DC48A8,0x36F59F5F,0xD03C6630,0xF930EE83,0xEA4B8519,0x8778048C,0x19347BD5},
  {0xCF1275DB,0xB7641FBB,0xB292FD2C,0x7890908E,0x79176B39,0x1DCE14B1,0x28AD98AD,0xF769DDF6},
  {0x6B560E85,0x8C3630C1,0xF011C9CB,0x8E02B0CA,0xA530AA90,0x39E08B4B,0xECFAADFB,0xA3BD0596},
  {0xAB57A618,0xD1402487,0xBBE93EE4,0x382510D9,0xF6C54560,0xAF0F1E91,0x3BFE74D2,0xC759A7E4},
  {0x5D38883E,0xCD04EA43,0x100CFC11,0x01153176,0x1417ECDB,0xE59C33AD,0xA960BF31,0xA67463CA},
  {0x9A938F24,0x4DA86E02,0xD37181BB,0xDF64F671,0xD7A89A56,0xEC4326DA,0x85E09477,0x317A8DCA},
  {0xE9C9780D,0x30A0B992,0x483DA700,0x1BA00000,0x7A96BDFF,0x22ED4B59,0x87B9D046,0x59D99663},
  {0x9C7BFED1,0x281FA835,0xC2FAAD9B,0x54BAEB35,0x8D9142F2,0xB1C62C4E,0x1DDD520A,0x2C3F93AD},
  {0x50FEE97D,0x7BEF2A15,0xBBF8DF05,0xC6F4EF03,0x1EFC3F39,0x96A61CC3,0xA72DE1FB,0x3010282D},
  {0x0D691484,0xFFE95C09,0x3583BE61,0xC14A8027,0x0D424B0B,0x3F46B409,0x86841CD4,0x84DD5ECC},
  {0xA59BB6DF,0xF2D08D69,0x37D15B24,0x45F52C6D,0x2B9EA9ED,0xB5CDD924,0xEBFAD91F,0xFF880CE3},
  {0x721E3686,0xB5995544,0x183BA27F,0xF7188C9E,0xD01EFD58,0x50056139,0x4DF3675A,0x389E2865},
  {0x8610D36F,0x629D759B,0x10D10F75,0xC5125255,0x7CEDAEE3,0x0C6DEC4A,0x10CAE693,0xD32FC562},
  {0xF90892D9,0xBE186551,0x1597DC92,0xE9AB660E,0x9952BE9F,0xE4573AB2,0x9E7497DF,0xBB3049EF},
  {0xDBA2FC96,0xE88F2D6C,0xA97ED579,0x0C086E5C,0xD3081021,0x88494D74,0xFB8A2952,0x3EBF9030},
  {0x73769D18,0x32A9F8B3,0x454BD8AD,0xCC56201E,0x60100E1D,0x1C14045D,0x7717FC80,0x8D33A5F4},
  {0x0274C68F,0x2DF37A38,0x88BC211D,0x57115FE7,0x9A056963,0xE969501F,0xCA61A1BD,0xFFF6F434},
  {0x49E0B858,0xCF516BE9,0x6F272388,0x7AD96A09,0xB1B6D2AD,0x1771541B,0xA374F004,0xC1ED6FB3},
  {0x09D3CD1B,0x3BC646BB,0xB9567DC5,0xAA8B716A,0xEB548F0A,0xFE493050,0x2AB10A76,0x75314F04},
  {0x13AB3D93,0x459E9D6C,0xC7D84F59,0x70F7F017,0x40F19337,0x6EA94E5F,0xDFB56293,0x4EFF6EB7},
  {0xF432E5AC,0xFEB6F6E4,0xF6AB68E5,0xB19C5429,0x1AF4EE53,0x9207DD4D,0x1F1F8F3C,0xC420A949},
  {0xC504E8C3,0x5F293912,0xBF226E51,0xB87BB3C2,0x25B8A82B,0x1AFC97B7,0x0AFC8A26,0x3639E24A},
  {0x28644A3B,0x58E8C1BD,0xF594A3BA,0xA0A2F1AA,0x965B47DD,0xCC95CDB5,0x8E6C738D,0x744AF0E4},
  {0x18F5DDB6,0xF1F7A699,0x26A7CE09,0xC501FDA3,0x18044DC9,0xBD4A8B38,0xDD494282,0x181B2A90},
  {0x01F3D5B8,0x06CEBA70,0xFFBE0E64,0x7A32BA15,0x89C3CAFF,0x18CE275D,0x37D0DE81,0x4BF14187}
};

// DES test cases -- key/iv/plain/cypher -- generated by gentestsdes.c
// DES test cases are now true DES keys, and can be checked with any DES package
static const u32 des_test_cases[TEST_CASE_COUNT][8] = {
  {0x54159B85L,0x316B02CEL,0xB401FC0EL,0x737B34C9L,0x96CB8518L,0x4A6649F9L,0x34A74BC8L,0x47EE6EF2L},
  {0x1AD9EA4AL,0x15527592L,0x805A8EF3L,0x21F68C4CL,0xC4B9980EL,0xD5C3BD8AL,0x5AFD5D57L,0x335981ADL},
  {0x5B199D6EL,0x40C40E6DL,0xF5756816L,0x36088043L,0x4EF1DF24L,0x6BF462ECL,0xC657515FL,0xABB6EBB0L},
  {0xAD8C0DECL,0x68385DF1L,0x19FB0D4FL,0x288D2CD6L,0x03FA0F6FL,0x038E92F8L,0x2FA04E4CL,0xBFAB830AL},
  {0xC475C22CL,0xDFE3B67AL,0x5614A37EL,0xD70F8E2DL,0xCA620ACEL,0xA1CF54BBL,0xB5BF73A1L,0xB2BB55BDL},
  {0x2FABC40DL,0xE03B8CE6L,0xF825C0CFL,0x47BDC4A9L,0x639F0904L,0x354EFC8BL,0xC745E11CL,0x698BF15FL},
  {0x80940E61L,0xDCBC7F73L,0xA30685EAL,0x67CDA3FEL,0x6E538AA3L,0xC34993BBL,0xF6DBDCE9L,0x6FCE1832L},
  {0x4A701329L,0x450D5D0BL,0x93D406FAL,0x96C9CD56L,0xAF7D2E73L,0xA1A9F844L,0x9428CB49L,0x1F93460FL},
  {0x2A73B06EL,0x8C855D6BL,0x3FC6F9D5L,0x3F07BC65L,0x9A311C3BL,0x8FC62B22L,0x0E71ECD9L,0x003B4F0BL},
  {0x255DFBB0L,0xB5290115L,0xE4663D24L,0x702B8D86L,0xC082814FL,0x6DFA89ACL,0xB76E630DL,0xF54F4D24L},
  {0xBA1A3B6EL,0x9158E3C4L,0x4C3E8CBCL,0xA19D4133L,0x7F8072ECL,0x6A19424EL,0xE09F06DAL,0x6508CD88L},
  {0xFB32138AL,0xF4F73175L,0x87C55A28L,0xC5FAA7A2L,0xDAE86B44L,0x629B5CAEL,0xAEC607BCL,0x9DD8816DL},
  {0x5B0BDA4FL,0x025B2502L,0x1F6A43E5L,0x0006E17EL,0xB0E11412L,0x64EB87EBL,0x2AED8682L,0x6A8BC154L},
  {0xB05837B3L,0xFBE083AEL,0x3248AE33L,0xD92DDA07L,0xFAF4525FL,0x4E90B16DL,0x6725DBD5L,0x746A4432L},
  {0x76BC4570L,0xBFB5941FL,0x8F2A8068L,0xCE638F26L,0xA21EBDF0L,0x585A6F8AL,0x65A3766EL,0x95B6663AL},
  {0xC7610E85L,0x5DDCBC51L,0xB0747E7FL,0x8A52D246L,0x3825CE90L,0xD70EA566L,0x50BC63A5L,0xDF9DD8FAL},
  {0xB9B02615L,0x017C3745L,0x21BAECACL,0x4771B2AAL,0x32703B09L,0x0CBEF2BCL,0x69907E24L,0x0B3928A6L},
  {0x0D7C8F0DL,0xFDC2DF6EL,0x3BBCE282L,0x7C62A9D8L,0x4E18FA5AL,0x2D942C4EL,0x5BF53685L,0x23E40E20L},
  {0xBAA426B6L,0xAED92F13L,0xC0DAC03CL,0x3382923AL,0x25F6F952L,0x3C691477L,0x49B7862AL,0x6520E509L},
  {0x7C37682AL,0x164A43B3L,0x9D31C0D1L,0x884B1EE5L,0x2DCBB169L,0xB4530B74L,0x3C93D6C3L,0x9A9CE765L},
  {0x79B55B8FL,0x6B8AC2B5L,0xE9545371L,0x004E203EL,0xA3170E57L,0x9F71563DL,0xF5DE356FL,0xBD0191DFL},
  {0xC8F80132L,0xD532972FL,0xBC2145BCL,0x42E174FEL,0xBA4DCA59L,0x6F65FA58L,0xB276ADD5L,0xA0A9F7B1L},
  {0x6E497043L,0x7C402CC2L,0x0039BB42L,0xBD8438A2L,0x508592BFL,0x1A2F40D6L,0x0F1EB5BCL,0x6B0C42E7L},
  {0xB3C4FD31L,0xD619314AL,0x39B2DBF7L,0x0295F93AL,0x4D547967L,0x36149936L,0x44B02FEEL,0xEECC0B2DL},
  {0x7FA12954L,0x08737CA8L,0x8ECDCE90L,0x5DACCF36L,0x7AA693B0L,0x62C8CA9CL,0x948CB25EL,0xF4781028L},
  {0x01BFDC08L,0x7558CD0EL,0x7D6D82DAL,0x19ACD958L,0x1EDF3781L,0x195110A7L,0x021EB315L,0xE2EA34C9L},
  {0x5161A2C4L,0x4F043B43L,0x17D76130L,0xDCB7695CL,0xA70ADBC0L,0x843A8801L,0xAEE16715L,0xE1AF0F07L},
  {0x943DF4E3L,0xB6D6CEF2L,0xC763AAA3L,0xA0179248L,0xEB61626FL,0x1B130032L,0x5630226FL,0x1C9DBFB2L},
  {0xE997049EL,0x37D5E085L,0x07C372A8L,0x3669C801L,0x689B4583L,0xDA05F0A2L,0xFA70DACDL,0x3F031F6CL},
  {0x4C2F1083L,0x5D8A6B32L,0xC38544FAL,0x017883F5L,0xD06D9EAAL,0xEE0DFBF6L,0xB1A728B7L,0x12C311C4L},
  {0x5225BCB0L,0xE51C98B6L,0x2B7ABF2DL,0xD714717EL,0xC867B0B7L,0xF24322B6L,0x0A6BF211L,0xB0B7C1CAL},
  {0xCE6823E9L,0x16A8A476L,0xCDC4DBA4L,0xD93B6603L,0xC6E231B9L,0xD84C2204L,0xDB623F7CL,0x3477E4B2L},
};

// ---------------------------------------------------------------------------

int SelfTest( unsigned int contest, int cputype )
{
  int threadpos, threadcount = 1;
  int successes = 0;
  const char *contname;

  if (contest >= CONTEST_COUNT)
  {
    LogScreen("test::error. invalid contest %u\n", contest );
    return -TEST_CASE_COUNT;
  }
  if (contest != RC5 && contest != DES)
    return 0;
  
#if (CLIENT_OS == OS_RISCOS)
  if (contest == RC5 && GetNumberOfDetectedProcessors() == 2)
    threadcount = 2;
#endif    

  contname = CliGetContestNameFromID( contest );
  for ( threadpos = 0; successes >= 0 && threadpos < threadcount; threadpos++ )
  {
    unsigned int testnum;
    long threadindex = -1L;
    if (threadcount > 1)
      threadindex = (long)threadpos;

    ClientEventSyncPost( CLIEVENT_SELFTEST_STARTED, (long)contest );
    successes = 0;

    for ( testnum = 0 ; testnum < TEST_CASE_COUNT ; testnum++ )
    {
      const u32 (*test_cases)[TEST_CASE_COUNT][8] = NULL;
      int resultcode; const char *resulttext = NULL;
      u64 expectedsolution;
      ContestWork contestwork;
      Problem problem(threadindex);

      if (CheckExitRequestTriggerNoIO())
        break;

      if (contest == RC5)
      { 
        test_cases = (const u32 (*)[TEST_CASE_COUNT][8])&rc5_test_cases[0][0];
        expectedsolution.lo = (*test_cases)[testnum][0];
        expectedsolution.hi = (*test_cases)[testnum][1];

        #if (CLIENT_OS == OS_RISCOS)
        if (threadcount == 2)
          contname = ((threadpos == 0)?("RC5 ARM"):("RC5 X86"));
        #endif

        /*
        test case 1 is the RSA pseudo-contest solution
        test cases 2,3,4,5,6,7 are specially made to 
        stress the key incrementation system
        the other test cases are generic (random)
  
        if test case N fails, then key & K isn't properly 
        incremented when key & W wrap :
  
        N         K                 W
               (hi:lo)           (hi:lo)
  
        2 00000000:00FF0000 00000000:0000FFFF
        3 00000000:FF000000 00000000:00FF0000
        4 000000FF:00000000 00000000:FF000000
        5 0000FF00:00000000 000000FF:00000000
        6 00FF0000:00000000 0000FF00:00000000
        7 FF000000:00000000 00FF0000:00000000
  
        another way of explaining this algorithm :
                                             __
        2 the solution is :       7602EDDB:C3A303DB
          we're starting from :   7602EDDB:C3A20000
                                           __
        3 the solution is :       59DA3369:8A00EAE3
          we're starting from :   59DA3369:89FF0000
                                        __
        4 the solution is :       A2098FD6:0000348F
          we're starting from :   A2098FD5:FFFF0000
                                      __
        5 the solution is :       28A00B00:0000E77F
          we're starting from :   28A00AFF:FFFF0000
                                    __
        6 the solution is :       47FC0000:000076B5
          we're starting from :   47FBFFFF:FFFF0000
                                  __
        7 the solution is :       AE000000:0000ECBB
          we're starting from :   ADFFFFFF:FFFF0000
  
        remember, in cores the running key is reversed, ie:
        keybyte_inside_core[0] == keybyte_outside_core[7] == key.hi & 0xFF000000,
        keybyte_inside_core[1] == keybyte_outside_core[6] == key.hi & 0x00FF0000,
        keybyte_inside_core[2] == keybyte_outside_core[5], etc...
        */

        contestwork.crypto.key.lo = expectedsolution.lo & 0xFFFF0000L;
        contestwork.crypto.key.hi = expectedsolution.hi;
        if (testnum>1 && testnum<=6) 
        {
          contestwork.crypto.key.lo -= 0x00010000;
          if ((expectedsolution.lo & 0xFFFF0000L) == 0)
            contestwork.crypto.key.hi--;
        }
      }
      else if (contest == DES)
      {
        test_cases = (const u32 (*)[TEST_CASE_COUNT][8])&des_test_cases[0][0];
        expectedsolution.lo = (*test_cases)[testnum][0];
        expectedsolution.hi = (*test_cases)[testnum][1];

        convert_key_from_des_to_inc ( (u32 *) &expectedsolution.hi, 
                                      (u32 *) &expectedsolution.lo);

        // to test also success on complementary keys
        if (expectedsolution.hi & 0x00800000L)
        {
          expectedsolution.hi ^= 0x00FFFFFFL;
          expectedsolution.lo = ~expectedsolution.lo;
        }
        contestwork.crypto.key.lo = expectedsolution.lo & 0xFFFF0000L;
        contestwork.crypto.key.hi = expectedsolution.hi;
      }
      
      contestwork.crypto.iv.lo =  ( (*test_cases)[testnum][2] );
      contestwork.crypto.iv.hi =  ( (*test_cases)[testnum][3] );
      contestwork.crypto.plain.lo = ( (*test_cases)[testnum][4] );
      contestwork.crypto.plain.hi = ( (*test_cases)[testnum][5] );
      contestwork.crypto.cypher.lo = ( (*test_cases)[testnum][6] );
      contestwork.crypto.cypher.hi = ( (*test_cases)[testnum][7] );
      contestwork.crypto.keysdone.lo = ( 0 );
      contestwork.crypto.keysdone.hi = ( 0 );
      contestwork.crypto.iterations.lo = ( 0x00020000L ); // 17 bits instead of 16
      contestwork.crypto.iterations.hi = ( 0 );
  
      problem.LoadState( &contestwork, contest, 0x1000, cputype);
  
      ClientEventSyncPost( CLIEVENT_SELFTEST_TESTBEGIN, (long)((Problem *)(&problem)) );
  
      while ( problem.Run() == RESULT_WORKING )
      {
        #if (CLIENT_OS == OS_WIN16) || (CLIENT_OS == OS_WIN32S)
        Yield();
        #elif (CLIENT_OS == OS_NETWARE)
        nwCliThreadSwitchLowPriority();
        #endif
      }
  
      resultcode = problem.RetrieveState( &contestwork, NULL, 1 );

      if ( resultcode != RESULT_FOUND )                /* no solution */
      {
        contestwork.crypto.key.hi = contestwork.crypto.key.lo = 0;
        resulttext = "FAILED";
        resultcode = -1;
      }
      else if (contestwork.crypto.key.lo != expectedsolution.lo || 
               contestwork.crypto.key.hi != expectedsolution.hi)   
      {                                                /* wrong solution */
        resulttext = "FAILED";
        resultcode = -1;
      } 
      else                                             /* correct solution */
      {
        resulttext = "passed";
        successes++;
        #if 0
        if (contest == DES)
        {
          /* original expected solution */
          expectedsolution.hi = (*test_cases)[testnum][1]; 
          expectedsolution.lo = (*test_cases)[testnum][0];
          convert_key_from_inc_to_des(&(contestwork.crypto.key.hi), 
                                      &(contestwork.crypto.key.lo));
        } 
        #endif
      }

      LogScreen( "%s Test %02d %s: %08X:%08X - %08X:%08X\n", 
         contname, testnum + 1, resulttext,
         contestwork.crypto.key.hi, contestwork.crypto.key.lo, 
         expectedsolution.hi, expectedsolution.lo );

      ClientEventSyncPost( CLIEVENT_SELFTEST_TESTEND, (long)resultcode );
    
    } /* for ( testnum = 0 ; testnum < TEST_CASE_COUNT ; testnum++ ) */

    if (successes > 0)
    {
      LogScreen( "\n%s: %d/%d Tests Passed\n", contname,
        (int) successes, (int) TEST_CASE_COUNT );
    }
    if (successes != TEST_CASE_COUNT)
    {
      LogScreen( "\n%s: WARNING WARNING WARNING: %d Tests FAILED!!!\n", 
        contname, (int) (TEST_CASE_COUNT - successes) );
        successes=-successes;
    }

    ClientEventSyncPost( CLIEVENT_SELFTEST_FINISHED, (long)(successes) );

  } /* for ( threadpos = 0; threadpos < threadcount; threadpos++ ) */
  
  return (successes);
}
