# Benchmark de algoritmos de ordenamiento

Programa en C++ que mide y compara el tiempo de ejecución de seis algoritmos de ordenamiento sobre arreglos aleatorios, y agrega los resultados a un archivo CSV para analizarlos después en Excel.

> **IMPORTANTE: `resultados.csv` NO forma parte del repositorio.**
> Está incluido en el `.gitignore` a propósito, porque los tiempos de ejecución dependen del equipo de cada persona.
> En el archivo de Excel que presenta la comparativa, **los datos que alimentan el dashboard principal se pueden intercambiar por otro conjunto de resultados**, así que cualquier persona puede cargar los suyos y ver la comparativa con sus propias mediciones.

## Descripción

El programa ordena los mismos arreglos con seis algoritmos y registra cuánto tarda cada uno:

| Algoritmo | Complejidad en el caso promedio |
|---|---|
| `bubble_sort` | O(n²) |
| `selection_sort` | O(n²) |
| `insertion_sort` | O(n²) |
| `merge_sort` | O(n log n) |
| `quick_sort` | O(n log n) |
| `heap_sort` | O(n log n) |

Los algoritmos son las variantes puras.

**Cómo funciona:**

1. Genera `REPETICIONES` arreglos de enteros aleatorios de tamaño `N` con una semilla fija. Con la misma semilla y el mismo `N`, los arreglos son siempre los mismos, por lo que el experimento es reproducible.
2. Cada algoritmo ordena una **copia** de cada uno de esos arreglos, de modo que los seis trabajan con exactamente los mismos datos.
3. El tiempo se mide con `std::chrono::steady_clock` y cubre únicamente la llamada al algoritmo.
4. Cada medición se agrega como una fila nueva a `resultados.csv`.

## Clonar el repositorio

```bash
git clone https://github.com/jmartinrivera11/Tarea_3_Ordenamientos.git
cd Tarea_3_Ordenamientos
```

## Requisitos

- Compilador con soporte de **C++23**: GCC 14 o superior, Clang 17 o superior, o MSVC 19.37 o superior (Visual Studio 2022 17.7+).
- **CMake 3.20** o superior.
- Opcional: Ninja u otro generador de CMake.

## Compilación

Desde la carpeta del proyecto:

```bash
cmake -S . -B build
cmake --build build
```

El build por defecto es `Release` (`-O2`) con `-Wall -Wextra` (o `/W4` en MSVC). Para elegir otro tipo, agrega `-DCMAKE_BUILD_TYPE=Debug` al comando `cmake -S`.

En Windows con MSVC, los ejecutables quedan en `build\Release\`. Con MinGW o Linux/macOS, quedan en `build/`.

## Uso

```bash
./build/ordenamientos N [REPETICIONES=5] [SEMILLA=42]
```

| Argumento | Obligatorio | Descripción |
|---|---|---|
| `N` | Sí | Tamaño de los arreglos. Entero mayor que 0 y no mayor que 214 748 364. |
| `REPETICIONES` | No (5) | Cantidad de arreglos aleatorios que se prueban por algoritmo. |
| `SEMILLA` | No (42) | Semilla del generador aleatorio. Entero sin signo. |

Ejemplos:

```bash
./build/ordenamientos 1000
./build/ordenamientos 5000 30
./build/ordenamientos 10000 30 7
```

En Windows, usa `build\ordenamientos.exe` en lugar de `./build/ordenamientos`.

Al terminar cada algoritmo se imprime una línea:

```
bubble_sort terminado (n=1000)
selection_sort terminado (n=1000)
...
resultados agregados a resultados.csv
```

Si algunas repeticiones ya estaban en el CSV, la línea lo indica:

```
bubble_sort terminado (n=1000) - 5 repeticiones omitidas (ya estaban en resultados.csv)
```

**Reanudar corridas interrumpidas:** al iniciar, el programa lee `resultados.csv` y omite las combinaciones `(algoritmo, n, repeticion)` que ya existen. Puedes volver a ejecutar el mismo comando sin duplicar filas.

**Para empezar desde cero**, borra `resultados.csv` antes de ejecutar.

**Sobre los tiempos de espera:** `bubble_sort`, `selection_sort` e `insertion_sort` son O(n²), así que con valores de `N` grandes pueden tardar bastante. Mientras corren, la terminal no muestra nada de ese algoritmo hasta que termina todas sus repeticiones.

El programa escribe `resultados.csv` en el **directorio desde el que se ejecuta**.

## Formato de `resultados.csv`

Una fila por cada medición:

```
algoritmo,n,repeticion,segundos
bubble_sort,1000,1,0.001937200
bubble_sort,1000,2,0.002039700
...
heap_sort,1000,10,0.000201300
```

| Columna | Descripción |
|---|---|
| `algoritmo` | Nombre del algoritmo medido. |
| `n` | Tamaño del arreglo en esa corrida. |
| `repeticion` | Número de repetición (1 a `REPETICIONES`). |
| `segundos` | Tiempo de ejecución en segundos, con 9 decimales y punto decimal. |

Cada ejecución agrega `6 × REPETICIONES` filas (menos las que se omiten por reanudación).

## Pruebas

```bash
cmake --build build
ctest --test-dir build --output-on-failure
```

o bien ejecutando directamente `build/ordenamientos_pruebas`. Las pruebas comparan cada algoritmo contra `std::ranges::sort` con casos borde (vacío, un elemento, duplicados, ordenado, inverso) y con arreglos aleatorios de varios tamaños. También verifican la generación reproducible, el manejo del CSV y la reanudación.

## Análisis en Excel

Los resultados se analizan en un archivo de Excel aparte, que presenta la comparativa entre algoritmos en un dashboard principal. Para usar tus propios resultados:

1. Ejecuta el programa con cada valor de `N` que quieras comparar, hasta tener tu `resultados.csv` completo.
2. Importa el CSV desde *Datos → Desde texto/CSV* y verifica que Excel reconozca bien el punto decimal.
3. Reemplaza en el archivo de Excel el conjunto de datos que alimenta el dashboard principal por el tuyo.

## Estructura del repositorio

```
.
├── CMakeLists.txt
├── src/
│   ├── algoritmos.hpp/.cpp
│   ├── experimento.hpp/.cpp
│   ├── csv.hpp/.cpp
│   └── main.cpp
├── tests/
│   └── pruebas.cpp
├── README.md
└── .gitignore
```
