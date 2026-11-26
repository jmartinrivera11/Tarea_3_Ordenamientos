#pragma once

#include "algoritmos.hpp"
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace ord {
// mayor N permitido: los valores van de 0 a N*10 y deben caber en int
inline constexpr std::size_t N_MAXIMO = 214'748'364;

// genera "repeticiones" arreglos de "n" enteros en [0, n*10]
std::vector<std::vector<int>> generar_arreglos(std::size_t n, std::size_t repeticiones, std::uint64_t semilla);

// ordena una copia de original con ordenar y devuelve los segundos que tarda
double medir(FuncionOrdenamiento ordenar, std::span<const int> original);
}
