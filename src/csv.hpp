#pragma once

#include <cstddef>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <string_view>
#include <tuple>

namespace ord {

inline constexpr std::string_view ENCABEZADO_CSV = "algoritmo,n,repeticion,segundos";

using ClaveMedicion = std::tuple<std::string, std::size_t, std::size_t>;

// lee las mediciones que ya estan en el CSV. si el archivo no existe, devuelve un conjunto vacio
// las lineas con formato invalido se ignoran con un aviso en std::cerr
std::set<ClaveMedicion> leer_existentes(const std::filesystem::path& ruta);

// agrega filas al CSV en modo append. escribe el encabezado solo si el archivo no existia
// o estaba vacio, y hace flush después de cada fila
class EscritorCsv {
public:
    explicit EscritorCsv(const std::filesystem::path& ruta);

    void escribir(std::string_view algoritmo, std::size_t n, std::size_t repeticion,
                  double segundos);

private:
    std::ofstream salida_;
};

}
