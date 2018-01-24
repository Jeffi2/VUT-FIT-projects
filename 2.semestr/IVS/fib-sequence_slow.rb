# indexovani clenu posloupnosti vychazi z http://en.wikipedia.org/wiki/Fibonacci_sequence
# tj. prvni clen ma index 0

class FibonacciSequence
    # pocitame bez pole vsechny hodnoty zvlast, inicializaci pole nevyuzijeme
    def initialize( f0 = 0, f1 = 1 ) #kvuli zachovani kompatability
        self.reset
    end
    
    # nastavi radu do vychoziho stavu index = -1
    def reset
        @idx = -1
        return true
    end
    
    # vrati fibonacciho cislo, pro kazde cislo pocita fibonacciho posloupnot od 0.clenu
    def next
    @idx += 1
    
    predposledni = 0
    posledni = 1
    @vysledek = 0
    pocet = @idx -1 #dekrementace poctu iteraci, pocitame od 0
    pocet.times { #cyklus na vypocet fib. cisla
        @vysledek = predposledni + posledni
        predposledni = posledni
        posledni = @vysledek
    }
    return @vysledek #vrati fib. cislo
end

# aktualni fibonacciho cislo (vraci nil, pokud se jeste nezacalo generovat)
def current
    if @idx >= 0 then
        return @vysledek
        else
        return nil
        end
end

# aktualni index (vraci nil, pokud se jeste nezacalo generovat)
def current_idx
    if @idx >= 0 then
        return @idx
        else
        return nil
        end
end

# vrati fibonacciho cislo s danym indexem
def [](n)
    if n < 0 then
            return nil
        end
    @idx = 0
    while @idx < n
        self.next
    end
    
    return self.current
end
end
