#pragma once
#include <string>

enum class TipoAnalisis {
    DENSIDAD,
    PRESION
};

struct Configuracion {
    TipoAnalisis tipo;
    double angulo;
    std::string archivoCilindros;
    std::string directorio;
    
    static Configuracion parseArgumentos(int argc, char* argv[]);
};
