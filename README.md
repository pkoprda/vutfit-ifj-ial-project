# ifj-ial-2020

## Projekt IFJ/IAL 2020

### Základy

- `make` - preloží program
- `make test` - preloží a spustí všetky testy
- `make clean` - vymaže vygenerované súbory

- Spustenie napr. pomocou `bin/main <tests/example.go`
- Používajte na printovanie na stderr funkciu **debug_print()** a na stdout funkciu **stdout_print()** (funguje iba pri spustení s -DDEBUG pri make)
- Používajte návratové hodnoty prekladača definované v súbore `error.h` 
- Súbor `libmine.h` je určená pre definíciu našich pomocných funkcií
- `ic20int` - interpret na testovanie IFJcode20 (funguje iba na Linuxe alebo WSL, vo wise je tuším aj verzia pre Windows)
