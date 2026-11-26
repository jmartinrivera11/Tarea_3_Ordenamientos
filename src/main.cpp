// el programa mide el tiempo de los seis algoritmos de ordenamiento sobre
// arreglos aleatorios y agrega los resultados a resultados.csv

#include "algoritmos.hpp"
#include "csv.hpp"
#include "experimento.hpp"
#include <charconv>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace {

constexpr std::string_view ARCHIVO_CSV = "resultados.csv";
constexpr std::size_t REPETICIONES_POR_DEFECTO = 5;
constexpr std::uint64_t SEMILLA_POR_DEFECTO = 42;

struct Opciones {
    std::size_t n = 0;
    std::size_t repeticiones = REPETICIONES_POR_DEFECTO;
    std::uint64_t semilla = SEMILLA_POR_DEFECTO;
};

void mostrar_uso(std::ostream& os) {
    os << "Uso: ordenamientos N [REPETICIONES=5] [SEMILLA=42]\n"
          "  N            tamaño de los arreglos (entero mayor que 0)\n"
          "  REPETICIONES cantidad de arreglos aleatorios por algoritmo (mayor que 0)\n"
          "  SEMILLA      semilla del generador aleatorio (entero sin signo)\n";
}

template <typename T>
std::optional<T> a_numero(std::string_view texto) {
    T valor{};
    const char* fin = texto.data() + texto.size();
    const auto [ptr, ec] = std::from_chars(texto.data(), fin, valor);
    if (texto.empty() || ec != std::errc{} || ptr != fin) {
        return std::nullopt;
    }
    return valor;
}

// lee y valida los argumentos. lanza std::invalid_argument con un mensaje para el usuario
Opciones leer_argumentos(int argc, char** argv) {
    if (argc < 2 || argc > 4) {
        throw std::invalid_argument("se esperaban entre 1 y 3 argumentos");
    }

    Opciones opciones;
    const std::string_view texto_n = argv[1];
    const auto n = a_numero<std::size_t>(texto_n);
    if (!n || *n == 0) {
        throw std::invalid_argument(
            std::format("N debe ser un entero mayor que 0, se recibió '{}'", texto_n));
    }
    if (*n > ord::N_MAXIMO) {
        throw std::invalid_argument(
            std::format("N no puede ser mayor que {}", ord::N_MAXIMO));
    }
    opciones.n = *n;

    if (argc >= 3) {
        const std::string_view texto = argv[2];
        const auto reps = a_numero<std::size_t>(texto);
        if (!reps || *reps == 0) {
            throw std::invalid_argument(
                std::format("REPETICIONES debe ser un entero mayor que 0, se recibió '{}'", texto));
        }
        opciones.repeticiones = *reps;
    }

    if (argc == 4) {
        const std::string_view texto = argv[3];
        const auto semilla = a_numero<std::uint64_t>(texto);
        if (!semilla) {
            throw std::invalid_argument(
                std::format("SEMILLA debe ser un entero sin signo, se recibió '{}'", texto));
        }
        opciones.semilla = *semilla;
    }

    return opciones;
}

}

int main(int argc, char** argv) {
    try {
        const Opciones opciones = leer_argumentos(argc, argv);
        const std::filesystem::path ruta = std::string(ARCHIVO_CSV);

        // los arreglos se generan siempre completos, para que una corrida reanudada
        // use exactamente los mismos datos
        const auto arreglos = ord::generar_arreglos(opciones.n, opciones.repeticiones, opciones.semilla);

        const auto existentes = ord::leer_existentes(ruta);
        ord::EscritorCsv escritor(ruta);

        for (const auto& algoritmo : ord::algoritmos()) {
            std::size_t omitidas = 0;
            for (std::size_t r = 1; r <= opciones.repeticiones; ++r) {
                const ord::ClaveMedicion clave{std::string(algoritmo.nombre), opciones.n, r};
                if (existentes.contains(clave)) {
                    ++omitidas;
                    continue;
                }
                const double segundos = ord::medir(algoritmo.ordenar, arreglos[r - 1]);
                escritor.escribir(algoritmo.nombre, opciones.n, r, segundos);
            }

            std::cout << std::format("{} terminado (n={})", algoritmo.nombre, opciones.n);
            if (omitidas > 0) {
                std::cout << std::format(" - {} repeticiones omitidas (ya estaban en {})",
                                         omitidas, ARCHIVO_CSV);
            }
            std::cout << '\n';
        }

        std::cout << std::format("resultados agregados a {}\n", ARCHIVO_CSV);
        return 0;
    } catch (const std::invalid_argument& error) {
        std::cerr << "error: " << error.what() << "\n";
        mostrar_uso(std::cerr);
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << "\n";
        return 1;
    }
}
