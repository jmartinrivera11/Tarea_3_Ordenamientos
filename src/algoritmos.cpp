#include "algoritmos.hpp"
#include <array>
#include <utility>
#include <vector>

namespace ord {

std::span<const Algoritmo> algoritmos() {
    static constexpr std::array tabla = {
        Algoritmo{"bubble_sort", bubble_sort},
        Algoritmo{"selection_sort", selection_sort},
        Algoritmo{"insertion_sort", insertion_sort},
        Algoritmo{"merge_sort", merge_sort},
        Algoritmo{"quick_sort", quick_sort},
        Algoritmo{"heap_sort", heap_sort},
    };
    return tabla;
}

void bubble_sort(std::span<int> a) {
    const std::size_t n = a.size();
    for (std::size_t i = 0; i + 1 < n; ++i) {
        for (std::size_t j = 0; j + 1 < n - i; ++j) {
            if (a[j] > a[j + 1]) {
                std::swap(a[j], a[j + 1]);
            }
        }
    }
}

void selection_sort(std::span<int> a) {
    const std::size_t n = a.size();
    for (std::size_t i = 0; i + 1 < n; ++i) {
        std::size_t minimo = i;
        for (std::size_t j = i + 1; j < n; ++j) {
            if (a[j] < a[minimo]) {
                minimo = j;
            }
        }
        std::swap(a[i], a[minimo]);
    }
}

void insertion_sort(std::span<int> a) {
    for (std::size_t i = 1; i < a.size(); ++i) {
        const int actual = a[i];
        std::size_t j = i;
        while (j > 0 && a[j - 1] > actual) {
            a[j] = a[j - 1];
            --j;
        }
        a[j] = actual;
    }
}

// mezcla las dos mitades ordenadas de "a". el buffer solo guarda la mitad izquierda,
// asi que la derecha queda en su lugar y la escritura nunca pisa un dato no leido
static void mezclar(std::span<int> a, std::span<int> buffer) {
    const std::size_t mitad = a.size() / 2;
    for (std::size_t t = 0; t < mitad; ++t) {
        buffer[t] = a[t];
    }

    std::size_t i = 0;      // indice en la mitad izquierda
    std::size_t j = mitad;  // indice en la mitad derecha
    std::size_t k = 0;      // posición de escritura en a
    while (i < mitad && j < a.size()) {
        if (buffer[i] <= a[j]) {
            a[k++] = buffer[i++];
        } else {
            a[k++] = a[j++];
        }
    }
    while (i < mitad) {
        a[k++] = buffer[i++];
    }
}

static void merge_sort_recursivo(std::span<int> a, std::span<int> buffer) {
    if (a.size() <= 1) {
        return;
    }
    const std::size_t mitad = a.size() / 2;
    merge_sort_recursivo(a.first(mitad), buffer);
    merge_sort_recursivo(a.subspan(mitad), buffer);
    mezclar(a, buffer);
}

static std::size_t particionar(std::span<int> a) {
    const std::size_t alto = a.size() - 1;
    const int pivote = a[alto];
    std::size_t i = 0;
    for (std::size_t j = 0; j < alto; ++j) {
        if (a[j] <= pivote) {
            std::swap(a[i], a[j]);
            ++i;
        }
    }
    std::swap(a[i], a[alto]);
    return i;
}

// heapify: hunde "raiz" dentro del heap de los primeros "n" elementos
static void hundir(std::span<int> a, std::size_t n, std::size_t raiz) {
    while (true) {
        std::size_t mayor = raiz;
        const std::size_t izq = 2 * raiz + 1;
        const std::size_t der = 2 * raiz + 2;
        if (izq < n && a[izq] > a[mayor]) {
            mayor = izq;
        }
        if (der < n && a[der] > a[mayor]) {
            mayor = der;
        }
        if (mayor == raiz) {
            return;
        }
        std::swap(a[raiz], a[mayor]);
        raiz = mayor;
    }
}

void merge_sort(std::span<int> a) {
    std::vector<int> buffer(a.size() / 2);
    merge_sort_recursivo(a, buffer);
}

void quick_sort(std::span<int> a) {
    while (a.size() > 1) {
        const std::size_t p = particionar(a);
        std::span<int> izq = a.first(p);
        std::span<int> der = a.subspan(p + 1);
        // se recursa sobre la partición menor y se itera sobre la mayor
        if (izq.size() < der.size()) {
            quick_sort(izq);
            a = der;
        } else {
            quick_sort(der);
            a = izq;
        }
    }
}

void heap_sort(std::span<int> a) {
    const std::size_t n = a.size();
    for (std::size_t i = n / 2; i-- > 0;) {
        hundir(a, n, i);
    }
    for (std::size_t i = n; i-- > 1;) {
        std::swap(a[0], a[i]);
        hundir(a, i, 0);
    }
}

}
