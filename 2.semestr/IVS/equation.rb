class Equation
    def self.solve_quadratic(a, b, c)
    
    vysledek = []
    return nil if (a == 0 && b == 0)
    if a == 0 then
    vysledek << -c/b.to_f
    else
        diskriminant = (b ** 2) - (4*a*c)
        return nil if diskriminant < 0
        if diskriminant == 0 then
            pom =(-b / (2*a))
            vysledek << pom
            return vysledek
        end

        if diskriminant > 0 then
            odmocnina = Math.sqrt(diskriminant)
            x1 = (-b + odmocnina) / (2*a)
            vysledek << x1
            x2 = (-b - odmocnina) / (2*a)
            vysledek << x2
        end
    end
end
end
