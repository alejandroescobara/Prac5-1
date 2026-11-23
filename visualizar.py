from collections import defaultdict
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.patches import Circle, Rectangle

ARCHIVO_SIMULACION = "simulacion.txt"

VELOCIDAD_REPRODUCCION = 1.0   # 1.0 = tiempo real, 0.5 = camara lenta
PASO_FRAMES = 1                # Sube a 2 o 3 si la animacion va lenta en tu PC
MOSTRAR_IDS = True
MOSTRAR_VELOCIDADES = True
ESCALA_VELOCIDAD = 0.15        # Longitud visual de los vectores de velocidad
MOSTRAR_COLISIONES = True
DURACION_MARCA_COLISION = 0.08 # Segundos que permanece visible la 'X' del impacto

GUARDAR_VIDEO = False          # True para exportar en vez de solo mostrar
ARCHIVO_VIDEO = "simulacion.gif"  # .gif (requiere pillow) o .mp4 (requiere ffmpeg)


def cargar_simulacion(ruta):
    box = (100.0, 100.0)
    obstacles = []
    frames_dict = defaultdict(list)
    collisions = []
    particle_ids = set()

    with open(ruta, "r", encoding="utf-8") as f:
        for linea in f:
            linea = linea.strip()
            if not linea or linea.startswith("#"):
                continue

            partes = linea.split()
            tipo = partes[0]

            if tipo == "BOX":
                box = (float(partes[1]), float(partes[2]))

            elif tipo == "OBSTACLE":
                obstacles.append({
                    "id": int(partes[1]),
                    "x": float(partes[2]),
                    "y": float(partes[3]),
                    "size": float(partes[4]),
                    "e": float(partes[5]),
                })

            elif tipo == "T":
                t = round(float(partes[1]), 4)
                pid = int(partes[2])
                particle_ids.add(pid)
                frames_dict[t].append({
                    "id": pid,
                    "x": float(partes[3]),
                    "y": float(partes[4]),
                    "vx": float(partes[5]),
                    "vy": float(partes[6]),
                    "m": float(partes[7]),
                    "r": float(partes[8]),
                })

            elif tipo == "C":
                collisions.append({
                    "t": float(partes[1]),
                    "tipo": partes[2],
                    "id1": int(partes[3]),
                    "id2": int(partes[4]),
                    "x": float(partes[5]),
                    "y": float(partes[6]),
                })

    tiempos = sorted(frames_dict.keys())
    return box, obstacles, tiempos, frames_dict, collisions, sorted(particle_ids)


# =============================================================================
# ANIMACION CON MATPLOTLIB
# =============================================================================
def main():
    box, obstacles, tiempos, frames_dict, collisions, particle_ids = cargar_simulacion(
        ARCHIVO_SIMULACION
    )

    if not tiempos:
        print(f"No se encontraron datos de particulas ('T') en {ARCHIVO_SIMULACION}.")
        return

    box_w, box_h = box
    dt = (tiempos[1] - tiempos[0]) if len(tiempos) > 1 else 0.01
    tiempos_render = tiempos[::PASO_FRAMES]
    intervalo_ms = max(1, int((dt * PASO_FRAMES * 1000) / VELOCIDAD_REPRODUCCION))

    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_xlim(0, box_w)
    ax.set_ylim(0, box_h)
    ax.set_aspect("equal", adjustable="box")
    ax.set_xlabel("X (m)")
    ax.set_ylabel("Y (m)")
    ax.set_title("Simulacion de colision de particulas")
    ax.grid(True, linestyle="--", alpha=0.3)

    # 1. Dibujar obstaculos estaticos (una sola vez)
    for obs in obstacles:
        rect = Rectangle(
            (obs["x"], obs["y"]),
            obs["size"],
            obs["size"],
            facecolor="#4a5568",
            edgecolor="#1a202c",
            linewidth=1.5,
            zorder=2,
        )
        ax.add_patch(rect)
        ax.text(
            obs["x"] + obs["size"] / 2,
            obs["y"] + obs["size"] / 2,
            f"O{obs['id']}\ne={obs['e']:.1f}",
            ha="center",
            va="center",
            color="white",
            fontsize=8,
            weight="bold",
            zorder=3,
        )

    # 2. Pre-crear artistas para cada particula (mas rapido y estable al desaparecer)
    cmap = plt.get_cmap("tab10")
    circulos = {}
    etiquetas = {}
    vectores = {}

    for pid in particle_ids:
        color = cmap(pid % 10)
        c = Circle(
            (0, 0),
            radius=1.0,
            facecolor=color,
            edgecolor="black",
            linewidth=1.0,
            alpha=0.85,
            visible=False,
            zorder=4,
        )
        ax.add_patch(c)
        circulos[pid] = c

        etiquetas[pid] = ax.text(
            0, 0, str(pid),
            ha="center", va="center",
            color="white", fontsize=8, weight="bold",
            visible=False, zorder=5,
        )

        linea_vel, = ax.plot(
            [], [], color="black", linewidth=1.2, alpha=0.7, visible=False, zorder=5
        )
        vectores[pid] = linea_vel

    # 3. Marcador de colisiones y panel de telemetria
    col_plot, = ax.plot(
        [], [],
        linestyle="None",
        marker="x",
        markersize=9,
        markeredgewidth=2.0,
        color="crimson",
        zorder=6,
    )

    info_text = ax.text(
        0.02, 0.98, "",
        transform=ax.transAxes,
        va="top",
        fontsize=9,
        family="monospace",
        bbox=dict(boxstyle="round", facecolor="white", alpha=0.85),
        zorder=7,
    )

    # 4. Funcion de actualizacion por cuadro
    def actualizar(idx):
        t = tiempos_render[idx]
        activos = frames_dict[t]
        ids_activos = set()
        energia_cinetica = 0.0

        for p in activos:
            pid = p["id"]
            ids_activos.add(pid)
            x, y, vx, vy, m, r = p["x"], p["y"], p["vx"], p["vy"], p["m"], p["r"]

            energia_cinetica += 0.5 * m * (vx * vx + vy * vy)

            circ = circulos[pid]
            circ.set_center((x, y))
            circ.set_radius(r)
            circ.set_visible(True)

            if MOSTRAR_IDS:
                lbl = etiquetas[pid]
                lbl.set_position((x, y))
                lbl.set_text(f"{pid}\n{m:.1f}kg")
                lbl.set_visible(True)

            if MOSTRAR_VELOCIDADES:
                vec = vectores[pid]
                vec.set_data([x, x + vx * ESCALA_VELOCIDAD],
                             [y, y + vy * ESCALA_VELOCIDAD])
                vec.set_visible(True)

        # Ocultar particulas que ya fueron absorbidas
        for pid in particle_ids:
            if pid not in ids_activos:
                circulos[pid].set_visible(False)
                etiquetas[pid].set_visible(False)
                vectores[pid].set_visible(False)

        # Mostrar impactos ocurridos en la ventana reciente
        if MOSTRAR_COLISIONES:
            xs = [c["x"] for c in collisions if 0.0 <= (t - c["t"]) <= DURACION_MARCA_COLISION]
            ys = [c["y"] for c in collisions if 0.0 <= (t - c["t"]) <= DURACION_MARCA_COLISION]
            col_plot.set_data(xs, ys)

        info_text.set_text(
            f"t = {t:5.2f} s\n"
            f"Activas: {len(activos)}/{len(particle_ids)}\n"
            f"Ek = {energia_cinetica:7.1f} J"
        )

    ani = FuncAnimation(
        fig,
        actualizar,
        frames=len(tiempos_render),
        interval=intervalo_ms,
        repeat=True,
        blit=False,
    )

    if GUARDAR_VIDEO:
        fps = max(1, int(round(1000 / intervalo_ms)))
        print(f"Exportando animacion a {ARCHIVO_VIDEO} ({fps} fps)...")
        ani.save(ARCHIVO_VIDEO, fps=fps)
        print("Exportacion completada.")
    else:
        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    main()
