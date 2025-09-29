#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import sys
import glob
import os
plt.rcParams.update({
    'text.usetex': True,
    'font.family': 'serif',
    'font.serif': ['Linux Libertine O'],
    'font.size': 14,
    'axes.titlesize': 14,
    'axes.labelsize': 14,
    'legend.fontsize': 14,
    'xtick.labelsize': 10,
    'ytick.labelsize': 10
})

def leer_histograma(filepath, NR=50, NTH=72):
    """
    Lee archivo .dat generado por procesar_archivo en C++.
    Devuelve tres tuplas:
    - (r_vals, r_counts)
    - (th_vals, th_counts)
    - (r2d, th2d, c2d)
    """
    r_vals, r_counts = [], []
    th_vals, th_counts = [], []
    r2d, th2d, c2d = [], [], []

    with open(filepath, "r") as f:
        lines = [line.strip() for line in f if line.strip() and not line.startswith("#")]

    for i in range(NR):
        r, c = map(float, lines[i].split())
        r_vals.append(r)
        r_counts.append(c)

    for i in range(NR, NR+NTH):
        th, c = map(float, lines[i].split())
        th_vals.append(th)
        th_counts.append(c)

    for i in range(NR+NTH, len(lines)):
        r, th, c = map(float, lines[i].split())
        r2d.append(r)
        th2d.append(th)
        c2d.append(c)

    print(f"{filepath.split('/')[-1]} -- r_counts: {sum(r_counts)}, th_counts: {sum(th_counts)}, c2d: {sum(c2d)}")
    return (np.array(r_vals), np.array(r_counts),
            np.array(th_vals), np.array(th_counts),
            np.array(r2d), np.array(th2d), np.array(c2d))


def plot_histos_acumulados(filelist, NR=50, NTH=72):
    # Inicializar acumuladores
    r_vals, th_vals = None, None
    sum_r, sum_th = None, None
    H = np.zeros((NR, NTH))

    for filepath in filelist:
        r_v, r_c, th_v, th_c, r2d, th2d, c2d = leer_histograma(filepath, NR, NTH)

        if r_vals is None:
            r_vals = r_v
            th_vals = th_v
            sum_r = np.zeros_like(r_c)
            sum_th = np.zeros_like(th_c)

        # acumular radial y angular
        sum_r += r_c
        sum_th += th_c

        # acumular 2D
        R = r_vals[-1] * (NR+0.5)/NR
        r_bins = np.linspace(0, R, NR+1)
        th_bins = np.linspace(-np.pi, np.pi, NTH+1)

        for rr, tt, cc in zip(r2d, th2d, c2d):
            ir = np.searchsorted(r_bins, rr) - 1
            ith = np.searchsorted(th_bins, tt) - 1
            if 0 <= ir < NR and 0 <= ith < NTH:
                H[ir, ith] += cc

    # Normalización global
    sum_r = sum_r / np.sum(sum_r)
    sum_th = sum_th / np.sum(sum_th)
    H = H / np.sum(H)

    # --- Graficar ---
    R = r_vals[-1] * (NR+0.5)/NR
    r_bins = np.linspace(0, R, NR+1)
    th_bins = np.linspace(-np.pi, np.pi, NTH+1)

    fig = plt.figure(figsize=(12, 6))
    gs = fig.add_gridspec(2, 2, width_ratios=[2, 1])

    # Subplot polar
    ax0 = fig.add_subplot(gs[:, 0], projection="polar")
    pcm = ax0.pcolormesh(th_bins, r_bins * 100, H, cmap="Greens")
    fig.colorbar(pcm, ax=ax0, orientation="vertical", label="Frecuencia relativa")
    ax0.set_title("Mapa polar 2D")

    # Subplot radial
    ax1 = fig.add_subplot(gs[0, 1])
    ax1.plot(r_vals * 100, sum_r, '-o')
    ax1.grid(True)
    ax1.set_xlabel(r"$r$ [cm]")
    ax1.set_ylabel("Frecuencia relativa")
    ax1.set_title("Distribución radial")

    # Subplot angular
    ax2 = fig.add_subplot(gs[1, 1])
    ax2.plot(np.degrees(th_vals), sum_th, 'o-')
    ax2.grid(True)
    ax2.set_xlabel(r"$\theta$ [°]")
    ax2.set_ylabel("Frecuencia relativa")
    ax2.set_title("Distribución angular")

    plt.tight_layout()
    plt.savefig("histos_acumulados.pdf")
    # plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso: python plot-perfiles.py <directorio>")
        sys.exit(1)

    indir = sys.argv[1]
    filelist = sorted(glob.glob(os.path.join(indir, "histo_*.dat")))
    if not filelist:
        print("No se encontraron archivos histograma-*.dat en", indir)
        sys.exit(1)

    print(f"Procesando {len(filelist)} archivos...")
    plot_histos_acumulados(filelist, NR=50, NTH=72)

