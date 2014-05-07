 
c  Declare variables for the materials used in the Crystal Ball setup
cccccccccccccccccccccccccccccccccccccccc
c  dwatts - 01/08/2001 added Argon
c  dwatts - 02/08/2001 Added radii for scintillator barrel
c  edownie - 04/02/03 added carbon
c  edownie - 18/06/03 added tungsten
c edownie - 10/07/03  added mwpc mat
c edownie - 15/07/03  added brass and fibreglass and cardboard
c Viktor Kashevarov - 10.04.06 added Kapton and PVC

      integer N_MATERIALS
 
c    dwatts   parameter (N_MATERIALS = 12)
      parameter (N_MATERIALS = 27)
 
      real rstraw(5)
     
      data
c     1  rscint  /4.06, 3.84, 3.62, 3.4, 3.18/
c     1  rstraw  /9.0, 7.7, 7.0, 5.5, 4.8/
     1  rstraw  /9.0, 7.7, 5.46, 5.22, 5.0/



      character
     1  matlist(N_MATERIALS)*8   ! array of material names
 
      common /c_materials/ matlist
 
      integer
     1  air       ! GEANT material numbers - it should be obvious, for
     1, vacuum    ! which materials.
     1, LIQH2
     1, LIQD2
     1, mylar
     1, Al
     1, Iron
     1, NaI
     1, scint
     1, vespel
     1, styrofoam
     1, BaF2
     1, Ar
     1, but
     1, LIQHe
     1, carbon
     1, copper
     1, plastic
     1, tungsten
     1, WCmat 
     1, brass
     1, fglass 
     1, cboard
     1, mumetal
     1, basemat
     1, kapton
     1, PVC

 
      common /i_materials/ air, vacuum, LIQH2, LIQD2, mylar,
     1                     Al,  Iron,   NaI,   scint, vespel,
     1                     styrofoam, BaF2, Ar, but, LIQHe, carbon,
     1                     copper, plastic, tungsten, WCmat, 
     1                     brass, fglass, cboard, mumetal, basemat, 
     1                     kapton, PVC

      integer
     1  tmat         ! target material
 
      logical
     1  l_target     ! if .true., include tgt. assembly in setup
 
      integer
     1  taps_rows(27)   ! number of TAPS detectors per row. In present
                        ! configuration we think of TAPS
                        ! forward wall consist of 522 crystals in 27 rows.
      real
     1  gap(2)       ! shift of the hemisphere along x-axis

      real
     1  straw_coor(354,6) 
 
      common /target/ l_target, tmat, gap, taps_rows

      real
     $  rgcor        ! flag controlling GHCOR1 correction for NaI

      common /material_flags/ rgcor
