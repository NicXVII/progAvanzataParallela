#!/usr/bin/env python3
"""
CyberPi Case - Generatore STL
Riproduzione approssimativa del case CyberPi (Raspberry Pi - estetica cyberpunk/gaming)
Dimensioni in mm. Scala reale per Raspberry Pi Zero 2W.

Struttura:
  - Scocca nera principale (corpo base + dettagli top)
  - Cap frontale rosso con forma angolare
  - Striscia connettore dorata (tip)
  - Dettagli top: logo block, griglie ventilazione, barra accent rossa
  - Porte laterali: USB x3, mini HDMI
"""

import numpy as np
import trimesh
from trimesh.creation import extrude_polygon, box, cylinder
from shapely.geometry import Polygon
import trimesh.boolean as tb

# ─── DIMENSIONI GLOBALI ────────────────────────────────────────────────────────
BL  = 88    # lunghezza corpo nero
BW  = 44    # larghezza totale
BH  = 13    # altezza corpo nero
CAP = 38    # lunghezza cap rosso (incluso tip)
TIP = 8     # lunghezza tip connector (dorato)

# ──────────────────────────────────────────────────────────────────────────────

def t(mesh, tx=0, ty=0, tz=0):
    """Trasla un mesh"""
    mesh.apply_translation([tx, ty, tz])
    return mesh

def make_box(lx, ly, lz, tx=0, ty=0, tz=0):
    m = box(extents=[lx, ly, lz])
    return t(m, tx + lx/2, ty + ly/2, tz + lz/2)


# ═══════════════════════════════════════════════════════════════════════════════
#  1. CORPO NERO PRINCIPALE
# ═══════════════════════════════════════════════════════════════════════════════

def build_black_body():
    parts = []

    # Base rettangolare principale
    base = make_box(BL, BW, BH)
    parts.append(base)

    # Bordo superiore rialzato posteriore (angolo posteriore sinistro)
    top_rail = make_box(BL, 6, 3, 0, BW - 6, BH)
    parts.append(top_rail)

    # Spigoli/guide laterali bassi
    left_rail  = make_box(BL, 3, 2, 0, 0, BH)
    right_rail = make_box(BL, 3, 2, 0, BW - 3, BH)
    parts.append(left_rail)
    parts.append(right_rail)

    # Blocco logo (quadrato rialzato top-left)
    logo_block = make_box(18, 18, 2, 4, BW - 24, BH)
    parts.append(logo_block)

    # Griglia ventilazione (barre parallele)
    for i in range(5):
        vent = make_box(2, 14, 2.5, 24 + i * 4, BW - 22, BH)
        parts.append(vent)

    # 3 protuberanze cilindriche (dettaglio top centrale)
    for i in range(3):
        cyl = cylinder(radius=2.5, height=3)
        t(cyl, 50 + i * 7, BW / 2, BH + 1.5)
        parts.append(cyl)

    # Barra accent rossa (top posteriore) - sarà colorata nel slicer
    accent_bar = make_box(BL - 10, 5, 4, 5, BW - 5.5, BH - 1)
    parts.append(accent_bar)

    # Piccoli pilastri angolari (mounting points)
    corners = [(3, 3), (3, BW - 6), (BL - 6, 3), (BL - 6, BW - 6)]
    for cx, cy in corners:
        pillar = cylinder(radius=2.5, height=BH + 2)
        t(pillar, cx, cy, 0)
        parts.append(pillar)

    return trimesh.util.concatenate(parts)


# ═══════════════════════════════════════════════════════════════════════════════
#  2. CAP ROSSO ANTERIORE (forma freccia/angolare)
# ═══════════════════════════════════════════════════════════════════════════════

def build_red_cap():
    parts = []

    # Profilo 2D visto dall'alto: trapezoide a punta
    # Origine a x=BL, si allunga fino a BL+CAP
    # Larghezza iniziale = BW, si restringe a 0 sul lato destro
    cap_profile = Polygon([
        (BL,          0),
        (BL,          BW),
        (BL + CAP - TIP, BW * 0.85),
        (BL + CAP - TIP, BW * 0.15),
    ])
    cap_mesh = extrude_polygon(cap_profile, height=BH)
    t(cap_mesh, 0, 0, 0)
    parts.append(cap_mesh)

    # Esagono rialzato sul top del cap (dettaglio decorativo)
    hex_pts = [(0, 0)]
    for i in range(6):
        angle = np.radians(i * 60)
        hex_pts.append((7 * np.cos(angle), 7 * np.sin(angle)))
    hex_pts.append((0, 0))
    hex_poly = Polygon(hex_pts).convex_hull
    hex_mesh = extrude_polygon(hex_poly, height=3)
    t(hex_mesh, BL + CAP * 0.45, BW * 0.5, BH)
    parts.append(hex_mesh)

    # Dettaglio romboidale / scanalatura decorativa top
    rhomb = make_box(CAP * 0.4, BW * 0.2, 2.5, BL + CAP * 0.1, BW * 0.38, BH)
    parts.append(rhomb)

    # Cerchi decorativi laterali (borchie)
    for rx, ry in [(BL + 8, BW * 0.6), (BL + 15, BW * 0.75)]:
        stud = cylinder(radius=3, height=2)
        t(stud, rx, ry, BH - 1)
        parts.append(stud)

    # Bordo superiore rialzato del cap (rispecchia rail della scocca)
    cap_rail = make_box(CAP - TIP - 2, 5, 3, BL + 1, BW - 6, BH)
    parts.append(cap_rail)

    return trimesh.util.concatenate(parts)


# ═══════════════════════════════════════════════════════════════════════════════
#  3. CONNETTORE DORATO / TIP ANTERIORE
# ═══════════════════════════════════════════════════════════════════════════════

def build_connector_tip():
    parts = []

    tip_x = BL + CAP - TIP

    # Corpo tip (wedge)
    tip_profile = Polygon([
        (tip_x,          BW * 0.15),
        (tip_x,          BW * 0.85),
        (tip_x + TIP,    BW * 0.7),
        (tip_x + TIP,    BW * 0.3),
    ])
    tip_mesh = extrude_polygon(tip_profile, height=BH * 0.6)
    t(tip_mesh, 0, 0, BH * 0.2)
    parts.append(tip_mesh)

    # Pin del connettore (barre verticali sottili)
    pin_count = 10
    tip_width = BW * 0.7 * 0.85
    for i in range(pin_count):
        pin = make_box(
            0.8, 1.2, BH * 0.5,
            tip_x + 1,
            BW * 0.15 + 3 + i * (tip_width / pin_count),
            BH * 0.25
        )
        parts.append(pin)

    return trimesh.util.concatenate(parts)


# ═══════════════════════════════════════════════════════════════════════════════
#  4. PORTE LATERALI (rappresentazione stilizzata)
# ═══════════════════════════════════════════════════════════════════════════════

def build_ports():
    parts = []

    # 3x USB Type-A (lato anteriore / basso)
    for i in range(3):
        # Frame porta USB (bordo rialzato)
        usb_frame = make_box(14, 2, 7, 10 + i * 18, -1, 3)
        parts.append(usb_frame)

    # Mini HDMI (lato sinistro)
    hdmi = make_box(11, 2, 4, 5, -1, 5)
    parts.append(hdmi)

    # GPIO strip (lato posteriore - piccole piazzole)
    for i in range(8):
        pin = cylinder(radius=0.8, height=3)
        t(pin, 60 + i * 3.5, BW + 0.5, BH - 1)
        parts.append(pin)

    return trimesh.util.concatenate(parts)


# ═══════════════════════════════════════════════════════════════════════════════
#  ASSEMBLAGGIO FINALE
# ═══════════════════════════════════════════════════════════════════════════════

def build_full_model():
    print("🔧 Generazione corpo nero...")
    black = build_black_body()

    print("🔴 Generazione cap rosso...")
    red   = build_red_cap()

    print("🟡 Generazione connettore tip...")
    tip   = build_connector_tip()

    print("🔌 Generazione porte laterali...")
    ports = build_ports()

    print("🔗 Unione mesh...")
    all_parts = trimesh.util.concatenate([black, red, tip, ports])

    # Fix normali e mesh
    all_parts.fix_normals()
    all_parts.fill_holes()

    # Centra il modello sull'origine Z=0
    bounds = all_parts.bounds
    all_parts.apply_translation([-bounds[0][0], -bounds[0][1], -bounds[0][2]])

    return all_parts


# ─── MAIN ─────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    model = build_full_model()

    output_path = "cyberpi_case.stl"
    model.export(output_path)

    vol = model.volume
    bb  = model.bounding_box.extents

    print(f"\n✅ STL generato con successo!")
    print(f"   📁 File: {output_path}")
    print(f"   📐 Dimensioni: {bb[0]:.1f} x {bb[1]:.1f} x {bb[2]:.1f} mm")
    print(f"   📦 Volume approssimativo: {vol:.0f} mm³ ({vol/1000:.1f} cm³)")
    print(f"   🔺 Triangoli: {len(model.faces):,}")
    print(f"\n💡 Importa in PrusaSlicer / Bambu Studio / Cura per slicing.")
    print(f"   Suggerimento: stampa le due parti (nera + rossa) separatamente")
    print(f"   con cambio filamento per rispettare i colori originali.")