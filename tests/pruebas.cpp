// pruebas de correccion de los algoritmos, la generacion de datos y el CSV
// std::ranges::sort se usa solo como referencia para comparar resultados

#include "algoritmos.hpp"
#include "csv.hpp"
#include "experimento.hpp"
#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

namespace {

int verificaciones = 0;
int fallos = 0;

void verificar(bool condicion, std::string_view descripcion) {
    ++verificaciones;
    if (!condicion) {
        ++fallos;
        std::cerr << "FALLO: " << descripcion << '\n';
    }
}

using Arreglo = std::vector<int>;

void comparar_con_referencia(const ord::Algoritmo& algoritmo, const Arreglo& original, std::string_view caso) {
    Arreglo esperado = original;
    std::ranges::sort(esperado);

    Arreglo obtenido = original;
    algoritmo.ordenar(obtenido);

    verificar(obtenido == esperado,
              std::format("{} con caso '{}' (n={})", algoritmo.nombre, caso, original.size()));
}

void casos_borde() {
    const std::vector<std::pair<std::string_view, Arreglo>> casos = {
        {"vacio", {}},
        {"un elemento", {5}},
        {"dos invertidos", {2, 1}},
        {"dos iguales", {3, 3}},
        {"todos iguales", {7, 7, 7, 7, 7}},
        {"ordenado", {1, 2, 3, 4, 5, 6}},
        {"inverso", {9, 8, 7, 6, 5, 4, 3, 2, 1}},
        {"duplicados", {4, 1, 4, 1, 4, 1, 2, 2}},
        {"valores extremos", {2147483647, 0, 2147483647, 0}},
    };

    for (const auto& algoritmo : ord::algoritmos()) {
        for (const auto& [nombre, arreglo] : casos) {
            comparar_con_referencia(algoritmo, arreglo, nombre);
        }
    }
}

void casos_aleatorios() {
    const std::vector<std::size_t> tamanos = {2, 3, 4, 5, 7, 8, 15, 16, 17, 31, 32, 33, 100, 1000};
    for (std::size_t n : tamanos) {
        const auto arreglos = ord::generar_arreglos(n, 5, 1234);
        for (const auto& algoritmo : ord::algoritmos()) {
            for (const auto& arreglo : arreglos) {
                comparar_con_referencia(algoritmo, arreglo, "aleatorio");
            }
        }
    }

    // peor caso habitual para quick_sort y bubble_sort: ya ordenado e inverso
    Arreglo ascendente(2000);
    Arreglo descendente(2000);
    for (std::size_t i = 0; i < 2000; ++i) {
        ascendente[i] = static_cast<int>(i);
        descendente[i] = static_cast<int>(2000 - i);
    }
    for (const auto& algoritmo : ord::algoritmos()) {
        comparar_con_referencia(algoritmo, ascendente, "ordenado grande");
        comparar_con_referencia(algoritmo, descendente, "inverso grande");
    }
}

void pruebas_generacion() {
    const auto a = ord::generar_arreglos(100, 4, 42);
    const auto b = ord::generar_arreglos(100, 4, 42);
    const auto c = ord::generar_arreglos(100, 4, 43);

    verificar(a.size() == 4, "generar_arreglos produce la cantidad de repeticiones pedida");
    verificar(std::ranges::all_of(a, [](const Arreglo& x) { return x.size() == 100; }),
              "cada arreglo generado tiene n elementos");
    verificar(a == b, "la misma semilla produce los mismos arreglos");
    verificar(a != c, "semillas distintas producen arreglos distintos");

    const bool en_rango = std::ranges::all_of(a, [](const Arreglo& x) {
        return std::ranges::all_of(x, [](int v) { return v >= 0 && v <= 100 * 10; });
    });
    verificar(en_rango, "los valores están en el rango [0, N*10]");
}

void pruebas_medicion() {
    const auto noop = [](std::span<int>) {};
    Arreglo desordenado = {3, 1, 2};
    bool lanzo = false;
    try {
        (void)ord::medir(+noop, desordenado);
    } catch (const std::runtime_error&) {
        lanzo = true;
    }
    verificar(lanzo, "medir lanza error si el algoritmo no ordena");

    Arreglo ordenado = {1, 2, 3};
    const double segundos = ord::medir(ord::algoritmos()[0].ordenar, ordenado);
    verificar(segundos >= 0.0, "medir devuelve un tiempo no negativo");
}

// cuenta las lineas que son exactamente el encabezado
int contar_encabezados(const fs::path& ruta) {
    std::ifstream entrada(ruta);
    std::string linea;
    int cuenta = 0;
    while (std::getline(entrada, linea)) {
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (linea == ord::ENCABEZADO_CSV) {
            ++cuenta;
        }
    }
    return cuenta;
}

std::string leer_todo(const fs::path& ruta) {
    std::ifstream entrada(ruta, std::ios::binary);
    std::ostringstream buffer;
    buffer << entrada.rdbuf();
    return buffer.str();
}

void pruebas_csv() {
    const fs::path dir = fs::temp_directory_path() / "ordenamientos_pruebas_csv";
    fs::remove_all(dir);
    fs::create_directories(dir);
    const fs::path ruta = dir / "resultados.csv";

    {
        ord::EscritorCsv escritor(ruta);
        escritor.escribir("bubble_sort", 10, 1, 0.5);
        escritor.escribir("bubble_sort", 10, 2, 0.25);
    }
    {
        // reabrir no debe repetir el encabezado
        ord::EscritorCsv escritor(ruta);
        escritor.escribir("heap_sort", 10, 1, 1.0);
    }

    verificar(contar_encabezados(ruta) == 1, "el encabezado aparece una sola vez");

    const std::string contenido = leer_todo(ruta);
    // sin el salto de linea, para que la prueba no dependa de \n o \r\n
    verificar(contenido.find("bubble_sort,10,1,0.500000000") != std::string::npos,
              "los segundos se escriben con 9 decimales y punto");

    auto existentes = ord::leer_existentes(ruta);
    verificar(existentes.size() == 3, "se leen las 3 mediciones escritas");
    verificar(existentes.contains({"bubble_sort", 10, 1}), "se reconoce bubble_sort,10,1");
    verificar(!existentes.contains({"quick_sort", 10, 1}), "no aparece quick_sort,10,1");

    // una corrida interrumpida a mitad de linea: la linea parcial se ignora y
    // la siguiente fila empieza en una línea nueva
    {
        std::ofstream parcial(ruta, std::ios::app);
        parcial << "merge_sort,10,1";
    }
    {
        ord::EscritorCsv escritor(ruta);
        escritor.escribir("quick_sort", 10, 1, 2.0);
    }
    existentes = ord::leer_existentes(ruta);
    verificar(existentes.size() == 4, "la línea parcial no cuenta y la nueva fila sí");
    verificar(existentes.contains({"quick_sort", 10, 1}),
              "la fila escrita tras una línea parcial se lee bien");
    verificar(!existentes.contains({"merge_sort", 10, 1}), "la línea parcial se ignora");

    // un archivo que no existe da un conjunto vacío
    verificar(ord::leer_existentes(dir / "no_existe.csv").empty(),
              "un archivo inexistente produce un conjunto vacío");

    fs::remove_all(dir);
}

}

int main() {
    casos_borde();
    casos_aleatorios();
    pruebas_generacion();
    pruebas_medicion();
    pruebas_csv();

    if (fallos > 0) {
        std::cerr << fallos << " de " << verificaciones << " verificaciones fallaron\n";
        return 1;
    }
    std::cout << "OK: " << verificaciones << " verificaciones pasaron\n";
    return 0;
}
