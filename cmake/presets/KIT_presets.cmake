# preset that turns on just a few, frequently used packages
# this will be compiled quickly and handle a lot of common inputs.

set(ALL_PACKAGES
    ASPHERE
    BODY
    BPM
    BROWNIAN
    COLLOID
    DPD-BASIC
    DPD-MESO
    DPD-REACT
    DPD-SMOOTH
    KSPACE
    LATBOLTZ
    MANYBODY
    MC
    MOLECULE
    RIGID
    SRD
)

foreach(PKG ${ALL_PACKAGES})
  set(PKG_${PKG} ON CACHE BOOL "" FORCE)
endforeach()