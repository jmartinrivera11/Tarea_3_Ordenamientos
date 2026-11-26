#pragma once

#include <cstddef>
#include <span>
#include <string_view>

namespace ord {

using FuncionOrdenamiento = void (*)(std::span<int>);

struct Algoritmo {
    std::string_view nombre;
    FuncionOrdenamiento ordenar;
};

std::span<const Algoritmo> algoritmos();

void bubble_sort(std::span<int> a);
void selection_sort(std::span<int> a);
void insertion_sort(std::span<int> a);
void merge_sort(std::span<int> a);
void quick_sort(std::span<int> a);
void heap_sort(std::span<int> a);

}
