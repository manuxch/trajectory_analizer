#include "AnalizadorBase.h"
#include "ThreadPool.h"
#include <filesystem>
#include <regex>
#include <iostream>

namespace fs = std::filesystem;

void AnalizadorBase::configurar(const Cilindro& cilindro, const std::string& directorio) {
    this->cilindro = cilindro;
    this->directorioTrabajo = directorio;
}

void AnalizadorBase::ejecutarAnalisis() {
    std::regex patron(R"(particles_.*\.liggghts)");
    std::vector<std::future<void>> futures;
    
    ThreadPool pool(std::thread::hardware_concurrency());
    std::cout << "Usando " << pool.size() << " hilos para análisis: " << nombre << std::endl;
    
    for (const auto& entry : fs::directory_iterator(directorioTrabajo)) {
        if (entry.is_regular_file()) {
            const std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, patron)) {
                futures.push_back(pool.enqueue([this, entry]() {
                    this->procesarArchivo(entry.path().string());
                }));
            }
        }
    }
    
    for (auto& future : futures) {
        future.get();
    }
}
