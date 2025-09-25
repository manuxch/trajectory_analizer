#include <iostream>
#include <memory>
#include <cmath>
#include "Configuracion.h"
#include "AnalizadorBase.h"
#include "AnalizadorDensidad.h"
#include "AnalizadorPresion.h"
#include "Geometria.h"

int main(int argc, char* argv[]) {
    try {
        auto config = Configuracion::parseArgumentos(argc, argv);
        
        double radio, altura;
        Vector3 centro;
        
        if (!Geometria::cargarDatosCilindro(config.archivoCilindros, 
                                           config.angulo, radio, altura, centro)) {
            std::cerr << "Error: No se encontraron datos para el ángulo " 
                      << config.angulo << std::endl;
            return 1;
        }
        
        double rad = Geometria::gradosARadianes(config.angulo);
        Vector3 eje = {std::cos(rad), 0.0, std::sin(rad)};
        Cilindro cilindro = {centro, eje, radio, altura};
        
        std::unique_ptr<AnalizadorBase> analizador;
        
        switch (config.tipo) {
            case TipoAnalisis::DENSIDAD:
                analizador = std::make_unique<AnalizadorDensidad>();
                break;
            case TipoAnalisis::PRESION:
                analizador = std::make_unique<AnalizadorPresion>();
                break;
        }
        
        analizador->configurar(cilindro, config.directorio);
        analizador->ejecutarAnalisis();
        
        std::cout << "Análisis completado: " << analizador->getNombre() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
