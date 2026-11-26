#include "csv.hpp"
#include <charconv>
#include <format>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

namespace ord {
namespace {

std::vector<std::string_view> dividir(std::string_view texto, char separador) {
    std::vector<std::string_view> partes;
    std::size_t inicio = 0;
    while (true) {
        const std::size_t pos = texto.find(separador, inicio);
        if (pos == std::string_view::npos) {
            partes.push_back(texto.substr(inicio));
            break;
        }
        partes.push_back(texto.substr(inicio, pos - inicio));
        inicio = pos + 1;
    }
    return partes;
}

// convierte un texto completo a numero. devuelve nullopt si sobra algo o si no es valido
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

// acepta exactamente 4 campos: algoritmo, n, repeticion y segundos
std::optional<ClaveMedicion> parsear_linea(std::string_view linea) {
    const auto campos = dividir(linea, ',');
    if (campos.size() != 4 || campos[0].empty()) {
        return std::nullopt;
    }
    if (!a_numero<double>(campos[3])) {
        return std::nullopt;
    }
    const auto n = a_numero<std::size_t>(campos[1]);
    const auto repeticion = a_numero<std::size_t>(campos[2]);
    if (!n || !repeticion) {
        return std::nullopt;
    }
    return ClaveMedicion{std::string(campos[0]), *n, *repeticion};
}

}

std::set<ClaveMedicion> leer_existentes(const fs::path& ruta) {
    std::set<ClaveMedicion> existentes;
    std::ifstream entrada(ruta);
    if (!entrada) {
        return existentes;
    }

    std::string linea;
    std::size_t numero = 0;
    while (std::getline(entrada, linea)) {
        ++numero;
        if (!linea.empty() && linea.back() == '\r') {
            linea.pop_back();
        }
        if (linea.empty() || (numero == 1 && linea == ENCABEZADO_CSV)) {
            continue;
        }
        if (auto clave = parsear_linea(linea)) {
            existentes.insert(std::move(*clave));
        } else {
            std::cerr << std::format("aviso: línea {} de {} ignorada (formato inválido)\n",
                                     numero, ruta.string());
        }
    }
    return existentes;
}

EscritorCsv::EscritorCsv(const fs::path& ruta) {
    const bool nuevo = !fs::exists(ruta) || fs::file_size(ruta) == 0;

    // si una corrida anterior se interrumpio a mitad de linea, empezar en una linea nueva
    bool falta_salto = false;
    if (!nuevo) {
        std::ifstream lectura(ruta, std::ios::binary);
        lectura.seekg(-1, std::ios::end);
        char ultimo = '\n';
        lectura.get(ultimo);
        falta_salto = ultimo != '\n';
    }

    salida_.open(ruta, std::ios::app);
    if (!salida_) {
        throw std::runtime_error("no se pudo abrir " + ruta.string());
    }
    if (nuevo) {
        salida_ << ENCABEZADO_CSV << '\n';
    } else if (falta_salto) {
        salida_ << '\n';
    }
    salida_.flush();
}

void EscritorCsv::escribir(std::string_view algoritmo, std::size_t n, std::size_t repeticion, double segundos) {
    salida_ << std::format("{},{},{},{:.9f}\n", algoritmo, n, repeticion, segundos);
    salida_.flush();
    if (!salida_) {
        throw std::runtime_error("error al escribir en el CSV");
    }
}

}
