#include "AnalizadorDensidad.h"
#include "Geometria.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <cmath>

namespace fs = std::filesystem;

AnalizadorDensidad::AnalizadorDensidad() {
    nombre = "Analizador de Densidad";
    hist2D = std::vector<std::vector<int>>(NR, std::vector<int>(NTH, 0));
    histR = std::vector<int>(NR, 0);
    histTheta = std::vector<int>(NTH, 0);
}

void AnalizadorDensidad::procesarArchivo(const std::string& filepath) {
    std::ifstream infile(filepath);
    if (!infile.is_open()) {
        std::cerr << "No se pudo abrir: " << filepath << '\n';
        return;
    }
    
    // Reiniciar histogramas
    {
      std::lock_guard<std::mutex> lock(datosMutex);
      hist2D = std::vector<std::vector<int>>(NR, std::vector<int>(NTH, 0));
      histR = std::vector<int>(NR, 0);
      histTheta = std::vector<int>(NTH, 0);
    }
    
    std::string line;
    size_t num_atoms = 0;
    
    while (std::getline(infile, line)) {
        if (line.rfind("ITEM: NUMBER OF ATOMS", 0) == 0) {
            std::getline(infile, line);
            num_atoms = std::stoul(line);
        } else if (line.rfind("ITEM: ATOMS", 0) == 0) {
            size_t index = 0;
            while (index < num_atoms && std::getline(infile, line)) {
                // Crear el stringstream aquí y pasarlo correctamente
                std::istringstream ss(line);
                int id, tipo;
                double x, y, z;
                double vx, vy, vz, fx, fy, fz;
                double r, m;
                double sxx, syy, szz, sxy, sxz, syz;
                
                if (ss >> id >> tipo >> x >> y >> z >> vx >> vy >> vz >> fx >> fy >> fz >>
                    r >> m >> sxx >> syy >> szz >> sxy >> sxz >> syz) {
                    
                    Vector3 posicion = {x, y, z};
                    procesarLineaDatos(line, posicion);
                }
                ++index;
            }
            break;
        }
    }
    
    infile.close();
    guardarResultados(filepath);
}

void AnalizadorDensidad::procesarLineaDatos(const std::string& linea, const Vector3& pos) {
    // Recrear el stringstream desde la línea
    std::istringstream ss(linea);
    int id, tipo;
    double x, y, z;
    double vx, vy, vz, fx, fy, fz;
    double r, m;
    double sxx, syy, szz, sxy, sxz, syz;
    
    if (!(ss >> id >> tipo >> x >> y >> z >> vx >> vy >> vz >> fx >> fy >> fz >>
          r >> m >> sxx >> syy >> szz >> sxy >> sxz >> syz)) {
        return;
    }
    
    double px, py, pr, ptheta;
    bool inside;
    Geometria::proyectar(pos, cilindro, px, py, pr, ptheta, inside);
    
    if (inside) {
        double dr = cilindro.radio / NR;
        double dtheta = 2 * M_PI / NTH;
        
        int ir = std::min(static_cast<int>(pr / dr), NR - 1);
        int ith = static_cast<int>((ptheta + M_PI) / dtheta) % NTH;
        std::lock_guard<std::mutex> lock(datosMutex); 
        hist2D[ir][ith]++;
        histR[ir]++;
        histTheta[ith]++;
    }
}

void AnalizadorDensidad::guardarResultados(const std::string& filepath) {
    std::string outname = "histo_" + fs::path(filepath).filename().string() + ".dat";
    std::ofstream out(outname);
    
    double dr = cilindro.radio / NR;
    double dtheta = 2 * M_PI / NTH;
    
    out << "# Histograma radial (r_bin, count)\n";
    for (int ir = 0; ir < NR; ++ir) {
        double rmid = (ir + 0.5) * dr;
        out << rmid << " " << histR[ir] << "\n";
    }
    
    out << "\n# Histograma angular (theta_bin[rad], count)\n";
    for (int ith = 0; ith < NTH; ++ith) {
        double thmid = -M_PI + (ith + 0.5) * dtheta;
        out << thmid << " " << histTheta[ith] << "\n";
    }
    
    out << "\n# Histograma 2D (r_bin,theta_bin,count)\n";
    for (int ir = 0; ir < NR; ++ir) {
        double rmid = (ir + 0.5) * dr;
        for (int ith = 0; ith < NTH; ++ith) {
            double thmid = -M_PI + (ith + 0.5) * dtheta;
            out << rmid << " " << thmid << " " << hist2D[ir][ith] << "\n";
        }
        out << "\n";
    }
    
    out.close();
    std::cout << "✓ Procesado: " << filepath << " → " << outname << "\n";
}
