require 'test/unit'
require_relative 'fib-sequence'

class FibonacciSequenceTest < Test::Unit::TestCase
  def setup
      @Fib_obj = FibonacciSequence.new
  end
  
  # do testu budou zahrnuty metody s prefixem "test_", tj. napr. nasledujici
  #inicializace třídy se netestuje
  
  # test vychoziho stavu
  def test_vychozi
    assert_equal( true, @Fib_obj.reset)
  end
  
  #test next, jiz inicializovane cislo
  def test_next_init
      assert_equal( 0, @Fib_obj.next)
  end
  
  #test next, vrati 4 fib. cislo
  def test_next_gener
      3.times { @Fib_obj.next }
      assert_equal( 2, @Fib_obj.next)
  end
  
  # test aktualniho cisla idx < 0
  def test_aktual_m0
      assert_equal( nil, @Fib_obj.current)
  end
  
  # test aktualniho cisla idx >= 0
  def test_aktual_v0
      10.times { @Fib_obj.next }
      assert_equal( 34, @Fib_obj.current)
  end
  
  # test indexu idx < 0
  def test_index_m0
      assert_equal( nil, @Fib_obj.current_idx)
  end
  
  # test indexu idx >= 0
  def test_index_v0
      10.times { @Fib_obj.next }
      assert_equal( 9, @Fib_obj.current_idx)
  end
  
  # test cisla s indexem n < 0
  def test_number_m0
      assert_equal( nil, @Fib_obj.[](-3))
  end
  
  # test cisla s indexem n <= idx
  def test_number_idx
      3.times { @Fib_obj.next }
      assert_equal( 1, @Fib_obj.[](2))
  end
  
  # test cisla s indexem n > idx
  def test_number_v0
      assert_equal( 5, @Fib_obj.[](5))
  end

end