#include "reassembler_test_harness.hh"

#include <exception>
#include <iostream>

using namespace std;

int main()
{
  try {
    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping assembled/unread section", cap };

      test.execute( Insert { "a", 0 } );
      test.execute( Insert { "ab", 0 } );

      test.execute( BytesPushed( 2 ) );
      test.execute( ReadAll( "ab" ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping assembled/read section", cap };

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "a" ) );

      test.execute( Insert { "ab", 0 } );
      test.execute( ReadAll( "b" ) );
      test.execute( BytesPushed( 2 ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping unassembled section to fill hole", cap };

      test.execute( Insert { "b", 1 } );
      test.execute( ReadAll( "" ) );

      test.execute( Insert { "ab", 0 } );
      test.execute( ReadAll( "ab" ) );
      test.execute( BytesPending { 0 } );
      test.execute( BytesPushed( 2 ) );
    }
    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping unassembled section", cap };

      test.execute( Insert { "b", 1 } );
      test.execute( ReadAll( "" ) );

      test.execute( Insert { "bc", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPending { 2 } );
      test.execute( BytesPushed( 0 ) );
    }
    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping unassembled section 2", cap };

      test.execute( Insert { "c", 2 } );
      test.execute( ReadAll( "" ) );

      test.execute( Insert { "bcd", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPending { 3 } );
      test.execute( BytesPushed( 0 ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping multiple unassembled sections", cap };

      test.execute( Insert { "b", 1 } );
      test.execute( Insert { "d", 3 } );
      test.execute( ReadAll( "" ) );

      test.execute( Insert { "bcde", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 4 ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "insert over existing section", cap };

      test.execute( Insert { "c", 2 } );
      test.execute( Insert { "bcd", 1 } );

      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 3 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "abcd" ) );
      test.execute( BytesPushed( 4 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "insert within existing section", cap };

      test.execute( Insert { "bcd", 1 } );
      test.execute( Insert { "c", 2 } );

      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 3 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "abcd" ) );
      test.execute( BytesPushed( 4 ) );
      test.execute( BytesPending( 0 ) );
    }

    {


      ReassemblerTestHarness test { "hole filled with overlap", 20 };

      test.execute( Insert { "fgh", 5 } );
      test.execute( BytesPushed( 0 ) );
      test.execute( ReadAll( "" ) );
      test.execute( IsFinished { false } );

      test.execute( Insert { "abc", 0 } );
      test.execute( BytesPushed( 3 ) );

      test.execute( Insert { "abcdef", 0 } );
      test.execute( BytesPushed( 8 ) );
      test.execute( BytesPending( 0 ) );
      test.execute( ReadAll( "abcdefgh" ) );
    }

    {

      ReassemblerTestHarness test { "multiple overlaps", 1000 };

      test.execute( Insert { "c", 2 } );
      test.execute( Insert { "e", 4 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 2 ) );

      test.execute( Insert { "bcdef", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 5 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "abcdef" ) );
      test.execute( BytesPushed( 6 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      ReassemblerTestHarness test { "overlap between two pending", 1000 };

      test.execute( Insert { "bc", 1 } );
      test.execute( Insert { "ef", 4 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 4 ) );

      test.execute( Insert { "cde", 2 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 5 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "abcdef" ) );
      test.execute( BytesPushed( 6 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      ReassemblerTestHarness test { "exact copy", 1000 };

      test.execute( Insert { "b", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 1 ) );

      test.execute( Insert { "b", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 1 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "ab" ) );
      test.execute( BytesPushed( 2 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      ReassemblerTestHarness test { "yet another overlap test", 150 };

      test.execute( Insert { "efgh", 4 } );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 4 ) );

      test.execute( Insert { "op", 14 } );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 6 ) );

      test.execute( Insert { "s", 18 } );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 7 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( BytesPushed( 1 ) );
      test.execute( BytesPending( 7 ) );

      test.execute( Insert { "abcde", 0 } );
      test.execute( BytesPushed( 8 ) );
      test.execute( BytesPending( 3 ) );

      test.execute( Insert { "opqrst", 14 } );
      test.execute( BytesPushed( 8 ) );
      test.execute( BytesPending( 6 ) );

      test.execute( Insert { "op", 14 } );
      test.execute( BytesPushed( 8 ) );
      test.execute( BytesPending( 6 ) );

      test.execute( Insert { "ijklmn", 8 } );
      test.execute( BytesPushed( 20 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      ReassemblerTestHarness test { "small capacity with overlapping insert", 2 };
      test.execute( Insert { "bc", 1 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 1 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "ab" ) );
      test.execute( BytesPushed( 2 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      const size_t cap = { 1000 };
      ReassemblerTestHarness test { "overlapping multiple unassembled sections 2", cap };

      test.execute( Insert { "bcd", 1 } );
      test.execute( Insert { "cde", 2 } );
      test.execute( ReadAll( "" ) );
      test.execute( BytesPushed( 0 ) );
      test.execute( BytesPending( 4 ) );

      test.execute( Insert { "a", 0 } );
      test.execute( ReadAll( "abcde" ) );
      test.execute( BytesPushed( 5 ) );
      test.execute( BytesPending( 0 ) );
    }

    {

      ReassemblerTestHarness test { "overlapping multiple unassembled sections 3", 30 };

      test.execute( Insert { "hello", 15 } );
      test.execute( Insert { "world!", 21 } );
      test.execute( Insert { "I am sentient", 0 } );
      test.execute( Insert { "sentient, hello world", 5 } );

      test.execute( BytesPending( 0 ) );
      test.execute( BytesPushed( 27 ) );
      test.execute( ReadAll( "I am sentient, hello world!" ) );
    }
  } catch ( const exception& e ) {
    cerr << "Exception: " << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
