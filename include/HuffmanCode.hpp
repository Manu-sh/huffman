#pragma once
#include <cstring>

#include <type_traits>
#include <bitarray/BitArray.hpp>

struct HuffmanCode: BitArray {

    static constexpr uint8_t MAX_LENGTH = 255;

    using BitArray::BitArray;

    HuffmanCode(BitArray &&b) {
        static_assert(sizeof(HuffmanCode) == sizeof(BitArray), "update if the memory layout differs");
        *((BitArray *)this) = std::move(b); // update if the memory layout differs
    }

#if 0
    friend FORCED(inline) int operator<=>(const HuffmanCode &a, const HuffmanCode &b) {
        return a <=> ((const BitArray &)b);
    }

    friend FORCED(inline) int operator<=>(const BitArray &a, const HuffmanCode &b) {
        return b <=> a;
    }

    friend FORCED(inline) int operator<=>(const HuffmanCode &a, const BitArray &b) { // untested, unused but should be fine

        if (a.bit_length() != b.bit_length())
            return a.bit_length() > b.bit_length() ? 1 : -1;

        // potrebbero essere 2 elementi di dimensione 0 con bit non inizializzati, è gestito sotto nell'if
        int cmp_second = a.back_byte_without_padding() - b.back_byte_without_padding();

        if (a.effective_byte_size() <= 1) // *bitstream() è grande esattamente 1 byte, evita di passare a memcmp() perchè con effective_byte_size == 0 giustamente scazza e questo numero può essere 0
            return a.bit_length() == 0 ? 0 : (cmp_second < 0 ? -1 : cmp_second > 0); // -1, 0, 1

        int cmp_first = memcmp(a.bitstream(), b.bitstream(), a.effective_byte_size() - 1); // "For a nonzero return value, the sign is determined by the sign of the difference between the first pair of bytes (interpreted as unsigned char) that differ in s1 and s2"
        return cmp_first < cmp_second ? -1 : cmp_first > cmp_second;
    }
#endif

};



/*

L'albero può degenerare a lista ma la profondità massima dell'albero di huffman è legata al numero dei simboli, quindi 256.
Questo perché per ogni nodo Z creato in fase di costruzione dell'albero ci sono sempre almeno 2 figli,
anche se uno di questi figli può essere un altro nodo fittizio aggiunto per creare il sottoalbero.
Vedi diagramma.

a:1 b:1 c:2 d:3 e:5 f:8 g:13 h:21

        [44]                                           a : 1111111
       /    \                                          b : 1111110
   h:21     [23]                                       c : 111110
           /    \                                      d : 11110
       e:13     [20]                                   e : 1110
               /    \                                  f : 110
           f:8      [12]                               g : 10
                   /    \                              h : 0
               e:5     [7]
                       /  \
                   d:3   [4]
                        /   \
                    c:2    [2]
                          /   \
                      b:1    a:1


La radice non conta come livello dell'albero, l'albero ha altezza N-1 quindi 7.
Siccome la radice non conta nel cammino preso per raggiungere un nodo non può quindi contribuire alla lunghezza di un huffman code

la lunghezza massima di un huffman code è data proprio dall'altezza dell'albero, dal numero di archi attraversati.

I prefix code nell'immagine hanno appunto al massimo lunghezza 7, mentre il numero dei prefix code è 8..
ne consegue su un alfabeto di 256 simboli la lunghezza massima dei simboli è N-1 ovvero 255.

---

Ad ogni modo questo scenario non si verifica facilmente (overflow a parte) perché al crescere del numero dei simboli c'è un secondo limite
quello del massimo numero che possono assumere le frequenze. in caso di overflow delle frequenze però può effettivamente degenerare a lista.

Siccome il worst-case è dato da nodi con frequenze che seguono l'andamento della sequenza di fibonacci e siccome questa sequenza è
esponenziale anche un numero molto grosso per le frequenze viene raggiunto dopo pochi step della sequenza fibonacci

 [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946,
 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465,
 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296, 433494437, 701408733, 1134903170,
 1836311903, 2971215073, 4807526976]

 uint32_t take only 48 step to be overflowed by 4807526976
 uint64_t take only 94 step to be overflowed by 19740274219868223167

but it's not all there, because huffman take min(Q) + min(Q) and sum together into Z.
Produce the worst case scenario for sym:uint8_t and freq:uint32_t is complex, so let's
just assume the maximum length it's sizeof(uint8_t)-1.

 https://planetmath.org/listoffibonaccinumbers
 https://cstheory.stackexchange.com/questions/4935/why-does-the-fibonacci-sequence-produce-a-worst-case-huffman-encoding

 https://cs.stackexchange.com/questions/75542/maximum-size-of-huffman-codes-for-an-alphabet-containing-256-letters
 https://github.com/ahmedss33/Introduction-to-Algorithms-Solutions/blob/master/C16-Greedy-Algorithms/16.3.md

"The Huffman algorithm considers the two least frequent elements recursively as the sibling leaves of maximum depth in code tree.
 The Fibonacci sequence (as frequencies list) is defined to satisfy F(n) + F(n+1) = F(n+2).
 As a consequence, the resulting tree will be the most unbalanced one, being a full binary tree."

*/

