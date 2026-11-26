#include "experimento.hpp"
#include <algorithm>
#include <chrono>
#include <random>
#include <stdexcept>
#include <utility>

namespace ord {

std::vector<std::vector<int>> generar_arreglos(std::size_t n, std::size_t repeticiones, std::uint64_t semilla) {
    std::mt19937_64 generador(semilla);
    const std::uint64_t modulo = static_cast<std::uint64_t>(n) * 10 + 1;

    std::vector<std::vector<int>> arreglos;
    arreglos.reserve(repeticiones);
    for (std::size_t r = 0; r < repeticiones; ++r) {
        std::vector<int> arreglo;
        arreglo.reserve(n);
        for (std::size_t i = 0; i < n; ++i) {
            arreglo.push_back(static_cast<int>(generador() % modulo));
        }
        arreglos.push_back(std::move(arreglo));
    }
    return arreglos;
}

double medir(FuncionOrdenamiento ordenar, std::span<const int> original) {
    // la copia se hace fuera del cronometro
    std::vector<int> copia(original.begin(), original.end());

    const auto inicio = std::chrono::steady_clock::now();
    ordenar(copia);
    const auto fin = std::chrono::steady_clock::now();

    // la verificacion tambien va fuera del cronometro. ademas, leer el resultado
    // impide que el compilador elimine el ordenamiento
    if (!std::ranges::is_sorted(copia)) {
        throw std::runtime_error("el algoritmo no dejó el arreglo ordenado");
    }
    return std::chrono::duration<double>(fin - inicio).count();
}

}
