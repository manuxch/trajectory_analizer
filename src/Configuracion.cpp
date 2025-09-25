#include "Configuracion.h"
#include <iostream>
#include <stdexcept>

Configuracion Configuracion::parseArgumentos(int argc, char* argv[]) {
    Configuracion config;
    
    if (argc < 4) {
        throw std::runtime_error(
            "Uso: \n\t" + std::string(argv[0]) + " <tipo_analisis> <angulo_en_grados> <archivo_cilindros.txt> [directorio]\n"
            "\tTipos de análisis: densidad, presion"
        );
    }
    
    // Parsear tipo de análisis
    std::string tipoStr = argv[1];
    if (tipoStr == "densidad") {
        config.tipo = TipoAnalisis::DENSIDAD;
    } else if (tipoStr == "presion") {
        config.tipo = TipoAnalisis::PRESION;
    } else {
        throw std::runtime_error("Tipo de análisis desconocido: " + tipoStr);
    }
    
    // Parsear ángulo
    try {
        config.angulo = std::stod(argv[2]);
    } catch (const std::exception& e) {
        throw std::runtime_error("Ángulo inválido: " + std::string(argv[2]));
    }
    
    // Archivo de cilindros
    config.archivoCilindros = argv[3];
    
    // Directorio (opcional)
    config.directorio = (argc > 4) ? argv[4] : "./";
    
    return config;
}
