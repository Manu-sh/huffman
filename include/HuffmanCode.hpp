#pragma once

#include <bitarray/BitArray.hpp>
#include <math/math.hpp>
#include <cmath>
#include <type_traits>

using HuffmanCode = BitArray;


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

Ad ogni modo questo scenario non si verifica mai perché al crescere del numero dei simboli c'è un secondo limite
quello del massimo numero che possono assumere le frequenze.

Siccome il worst-case è dato da nodi con frequenze che seguono l'andamento della sequenza di fibonacci e siccome questa sequenza è
esponenziale anche un numero molto grosso per le frequenze viene raggiunto dopo pochi step della sequenza fibonacci

Es. uint32_t fa si che la lunghezza massima dei prefix free code generato sia 48 o giù di lì e non 255.

 [1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946,
 17711, 28657, 46368, 75025, 121393, 196418, 317811, 514229, 832040, 1346269, 2178309, 3524578, 5702887, 9227465,
 14930352, 24157817, 39088169, 63245986, 102334155, 165580141, 267914296, 433494437, 701408733, 1134903170,
 1836311903, 2971215073, 4807526976]

 uint32_t take only 48 step to be overflowed by 4807526976
 uint64_t take only 94 step to be overflowed by 19740274219868223167

 so to produce the worst case scenario for an uint32_t you have to put only 48 symbols, this lead
 the maximum height of the tree (which will be 48-1) which is directly connected to the number of
 edges which is directly connected to the maximum length for an huffman code with f:uint32_t sym:uint8_t

 https://planetmath.org/listoffibonaccinumbers
 https://cstheory.stackexchange.com/questions/4935/why-does-the-fibonacci-sequence-produce-a-worst-case-huffman-encoding

 https://cs.stackexchange.com/questions/75542/maximum-size-of-huffman-codes-for-an-alphabet-containing-256-letters
 https://github.com/ahmedss33/Introduction-to-Algorithms-Solutions/blob/master/C16-Greedy-Algorithms/16.3.md

"The Huffman algorithm considers the two least frequent elements recursively as the sibling leaves of maximum depth in code tree.
 The Fibonacci sequence (as frequencies list) is defined to satisfy F(n) + F(n+1) = F(n+2).
 As a consequence, the resulting tree will be the most unbalanced one, being a full binary tree."

data una frequenza massima FREQ_MAX(Symbol) di std::numeric_limits<uint32_t>::max()
strlen(pcode) più grande generato sarà dato da questa brutta formula:
floor( log(1.618, (2**32-1) * sqrt(5))) )

*/

template <typename TFrequency>
uint8_t huffman_code_max_length() {

    static_assert(std::is_same_v<TFrequency, uint8_t> || std::is_same_v<TFrequency, uint16_t> || std::is_same_v<TFrequency, uint32_t> || std::is_same_v<TFrequency, uint64_t>,
            "TFrequency must be an uint*_t type");

    static constexpr double GOLDEN_RATIO = 1.618;
    return std::floor<uint8_t>(log_in_base(GOLDEN_RATIO, (std::numeric_limits<TFrequency>::max()-1) * std::sqrt(5.0))) + 1; // +1 because it's a length
}