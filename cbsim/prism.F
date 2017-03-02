      subroutine prism (itrityp,
     1                  dz, th, phi,
     1                  h1, lb1, lh1, th1,
     1                  h2, lb2, lh2, th2,
     1                  g_x, g_y, g_z,
     1                  g_theta1, g_theta2, g_theta3,
     1                  g_phi1, g_phi2, g_phi3)
 
************************************************************************
*                                                                      *
* PRISM                                                                *
*                                                                      *
* Description and modification history:                                *
*     Determine the GEANT parameters for a particular prism            *
*     (= crystal).                                                     *
*                                                                      *
* Input  variables:                                                    *
*     ITRITYP: the prism type, numbered from 1-11 inclusive.           *
*                                                                      *
* Output variables:                                                    *
*     The GEANT TRAP parameters:                                       *
*         DZ -- half length in Z                                       *
*         TH -- Polar angles between the prism axis                    *
*        PHI -- and the normal to the two parallel faces               *
*                                                                      *
*         H1 -- half-length in y at z=-DZ                              *
*        LB1 -- half-length in x at z=-DZ, y=low edge                  *
*        LH1 -- half length in x at z=-DZ, y=high edge                 *
*        TH1 -- angle between the y-axis and the line connecting       *
*               the center of the low y-edge and the center of the     *
*               high y-edge at z=-DZ                                   *
*                                                                      *
*         H2 -- half-length in y at z=+DZ                              *
*        LB2 -- half-length in x at z=+DZ, y=low edge                  *
*        LH2 -- half length in x at z=+DZ, y=high edge                 *
*        TH2 -- angle between the y-axis and the line connecting       *
*               the center of the low y-edge and the center of the     *
*               high y-edge at z=+DZ                                   *
*                                                                      *
*     The GEANT positioning parameters:                                *
*             G_X --                                                   *
*             G_Y -- Position of the crystal in the major triangle.    *
*             G_Z --                                                   *
*        G_THETA1 --                                                   *
*        G_THETA2 -- Polar angles describing orientation in lab        *
*        G_THETA3 --                                                   *
*          G_PHI1 --                                                   *
*          G_PHI2 -- Azimuthal angles describing orientation in lab    *
*          G_PHI3 --                                                   *
*                                                                      *
* Called by: UGEOM                                                     *
*                                                                      *
* Calls:     PLANE, CERNLIB routines CROSS (F117), VDOTN, VMOD, and    *
*            VUNIT (all F121)                                          *
*                                                                      *
* written  by: John W. Price, 06/01/94, 22.04.16                       *
* modified by: Martin Clajus, Dec. 95; added the calculation of        *
*              parameters for minor triangles (prism type 0)           *
*              Martin Clajus, 960422; added comments; removed          *
*              declarations of unused variables; declared routines     *
*              VDOTN and VMOD external (even though things seem to     *
*              have worked fine without that).                         *
*              Martin Clajus, 970520; now the data files vertices.dat  *
*              and modules.dat are read only the first time the        *
*              routine is called, rather than each time. To avoid any  *
*              problems this might possibly cause with compiler        *
*              optimizations, the corresponding variables are included *
*              in a local common block and SAVEd. Also added a safety  *
*              check against corrupted data files, and modified if-    *
*              statements that distinguish between the case of a minor *
*              triangle (itrityp = 0) and that of a crystal, to make   *
*              them a little more economical.                          *
*                                                                      *
************************************************************************
 
      implicit none
#include "g77_intrinsic.inc"
 
      integer
     1  i         ! loop/array index
     1, imodule   ! module number - dummy argument for read statement
     1, itrinum   ! number of the triangle that represents the type
                  ! wanted
     1, itrityp   ! prism type (1-11 inclusive for crystals,
                  !             0 for minor triangles)
     1, itype(13) ! array of prism types, in the order in which they are
                  ! first needed in a crystal. In that order, type 9
                  ! appears twice before 10 and 11 do, so we need one
                  ! extra element.
     1, iv1i(13)  ! row index of 1st prism vertex, same order as itype
     1, iv1j(13)  ! col. index of 1st prism vertex, same order as itype
     1, iv2i(13)  ! row index of 2nd prism vertex, same order as itype
     1, iv2j(13)  ! col. index of 2nd prism vertex, same order as itype
     1, iv3i(13)  ! row index of 3rd prism vertex, same order as itype
     1, iv3j(13)  ! col. index of 3rd prism vertex, same order as itype
     1, j         ! loop/array index
     1, k         ! loop/array index
 
      logical
     1  done      ! indicates status of search for the right prism type
     1, firstcall ! true the first time this routine is called, then false
 
      real
     1  b1(3)     ! solution of plane-equation for inner prism face
     1, b2(3)     ! solution of plane-equation for outer prism face
     1, b2norm(3) ! b2, normalized to unit length
     1, c1(3)     ! centroid of inner prism face
     1, c2(3)     ! centroid of outer prism face
     1, dir(3)    ! direction from c1 to c2
     1, dz        ! Half-length in z of the prism
     1, g_phi1    ! azimuthal angle for prism orientation
     1, g_phi2    ! azimuthal angle for prism orientation
     1, g_phi3    ! azimuthal angle for prism orientation
     1, g_theta1  ! polar angle for prism orientation
     1, g_theta2  ! polar angle for prism orientation
     1, g_theta3  ! polar angle for prism orientation
     1, g_x       ! x-coordinate of prism position
     1, g_y       ! y-coordinate of prism position
     1, g_z       ! z-coordinate of prism position
     1, h1        ! half-length in y of inner prism face
     1, h2        ! half-length in y of outer prism face
     1, lb1       ! half length in x of inner prism face at low y
     1, lb2       ! half length in x of outer prism face at low y
     1, lh1       ! half length in x of inner prism face at high y
     1, lh2       ! half length in x of outer prism face at high y
     1, mid(3)    ! midpoint of the low-y edge of the inner or outer
                  ! prism face
     1, p1(3)     ! prism corner on inner face
     1, p2(3)     ! prism corner on inner face
     1, p3(3)     ! prism corner on inner face
     1, p4(3)     ! prism corner on outer face
     1, p5(3)     ! prism corner on outer face
     1, p6(3)     ! prism corner on outer face
     1, phi       ! azimuthal angle of the line joining the centers of
                  ! the two triangular prism faces
     1, th        ! polar angle of the line joining the centers of the
                  ! two triangular prism faces
     1, th1       ! angle of the inner prism face w.r.t. the y-axis
     1, th2       ! angle of the outer prism face w.r.t. the y-axis
     1, v1(3)     ! low-y edge of the inner prism face
     1, v2(3)     ! line from the midpoint of the low-y edge to the
                  ! corner not on that edge, on the inner prism face
     1, v3(3)     ! low-y edge of the outer prism face
     1, v4(3)     ! line from the midpoint of the low-y edge to the
                  ! corner not on that edge, on the outer prism face
     1, v5(3)     ! line from c1 to xproj
     1, v6(3)     ! vector product b1 x v1
     1, vdotn     ! normalized scalar (dot) product of two vectors
                  ! (CERNLIB routine, declared external below)
     1, vertices (7,7,3)  ! coordinates of prism vertices, for
                  ! "standard" major triangle (i.e. one whose axis is
                  ! the z-axis), normalized to unit length.
     1, vmod      ! norm of a vector (CERNLIB routine, declared external
                  ! below)
     1, xproj(3)  ! projection of the centroid of the outer plane onto
                  ! the inner plane
 
      external
     1  vdotn
     1, vmod
 
      common /loc_prism/ itype, iv1i, iv1j, iv2i, iv2j, iv3i, iv3j,
     1                   vertices, firstcall
 
      data firstcall /.true./
 
      save
 
      if (firstcall) then
C  Read in the positions of the vertices.  These positions are on the
C  unit sphere.
        open (unit = 1,  file = 'vertices.dat')
        do i = 1, 7
          do j = 1, i
            read (1, 100) (vertices(i,j,k), k = 1, 3)
          end do
        end do
        close (unit = 1)
 
C  Read in the relationship between the modules and the vertices. For
C  the benefit of human users, this relationship is given for one minor
C  triangle and all 36 crystals in file modules.dat. The program only
C  needs one such relationship for each crystal type, so reading the
C  first 13 lines of modules.dat is sufficient.
        open (unit = 1, file = 'modules.dat')
        do i = 1, 13
          read (1, 200) imodule, itype(i),
     1                  iv1i(i), iv1j(i),
     1                  iv2i(i), iv2j(i),
     1                  iv3i(i), iv3j(i)
        end do
        close (unit = 1)
        firstcall = .false.
      end if
 
C  Get the first module that has the specified type.
      done = .false.
      itrinum = 1
      do while (.not. done)
        if (itrityp .eq. itype(itrinum)) then
          done = .true.
        else
          itrinum = itrinum + 1
c  Make sure we're not getting into an endless loop because the data
c  file is corrupted or something like that.
          if (itrinum .gt. 13) then
            write (*,*) 'Could not find correct prism type. Please ',
     1                  'make sure you have the correct data files.'
            call exit(0)
          end if
        end if
      end do
 
      if (itrityp .ne. 0) then
C  This is the normal case, individual crystals.
        do i = 1, 3
C  Get equation of inner surface.
          P1(i) = vertices(iv1i(itrinum), iv1j(itrinum), i) * 25.4
          P2(i) = vertices(iv2i(itrinum), iv2j(itrinum), i) * 25.4
          P3(i) = vertices(iv3i(itrinum), iv3j(itrinum), i) * 25.4
C  Get equation of outer surface.
          P4(i) = vertices(iv1i(itrinum), iv1j(itrinum), i) * 66.04
          P5(i) = vertices(iv2i(itrinum), iv2j(itrinum), i) * 66.04
          P6(i) = vertices(iv3i(itrinum), iv3j(itrinum), i) * 66.04
        end do
      else
C  Special handling for minor triangles - we have to move a little
C  inward to make sure we can fully enclose the crystals
        do i = 1, 3
C  Get equation of inner surface.
          P1(i) = vertices(iv1i(itrinum), iv1j(itrinum), i) * 24.
          P2(i) = vertices(iv2i(itrinum), iv2j(itrinum), i) * 24.
          P3(i) = vertices(iv3i(itrinum), iv3j(itrinum), i) * 24.
C  Get equation of outer surface.
          P4(i) = vertices(iv1i(itrinum), iv1j(itrinum), i) * 76.
          P5(i) = vertices(iv2i(itrinum), iv2j(itrinum), i) * 76.
          P6(i) = vertices(iv3i(itrinum), iv3j(itrinum), i) * 76.
        end do
      end if
      call plane (p1, p2, p3, b1)
      call plane (p4, p5, p6, b2)
 
C  Get distance between planes.  See CRC Standard Math Tables, pg.
C  297.
      dz = (abs(b1(1)*P4(1) + b1(2)*P4(2) + b1(3)*P4(3) - 1.) /
     1      sqrt(b1(1)**2 + b1(2)**2 + b1(3)**2)) / 2.
 
C  Find centroids of the inner and outer faces. Since the prisms are
C  presented to GEANT as TRAP shapes with one edge on each face
C  "happening" to have zero length, the corner on that edge needs to be
C  counted twice.
      do i = 1, 3
        c1(i) = (2.*p1(i) + p2(i) + p3(i))/4.
        c2(i) = (2.*p4(i) + p5(i) + p6(i))/4.
      end do
 
C  Find the direction of the line between the centroids.
      do i = 1, 3
        dir(i) = c2(i) - c1(i)
      end do
 
C  Find the angle between that line and the normal to the inner face.
      th = acosd (vdotn (dir, b1, 3))
 
C  Normalize the normal to the outer plane to unit length.
      call vunit(b2, b2norm, 3)
 
C  Determine the projection of the centroid of the outer plane onto the
C  inner plane.
      do i = 1, 3
        xproj(i) = c2(i) - 2. * dz * b2norm(i)
      end do
 
C  Get TRAP parameters h1, lb1, lh1, th1
      do i = 1, 3
        mid(i) = (p2(i) + p3(i))/2.
        v1(i) = p3(i) - p2(i)
        v2(i) = p1(i) - mid(i)
        v5(i) = xproj(i) - c1(i)
      end do
 
      th1 = asind(vdotn(v1, v2, 3))
 
      h1 = cosd(th1) * vmod(v2, 3) / 2.
 
      if (p3(1) .lt. p2(1)) then
        lb1 = 0.
        lh1 = vmod(v1, 3) / 2.
      else
        lb1 = vmod(v1, 3) / 2.
        lh1 = 0.
      end if
 
C  Get TRAP parameters h2, lb2, lh2, th2
      do i = 1, 3
        mid(i) = (p5(i) + p6(i))/2.
        v3(i) = p6(i) - p5(i)
        v4(i) = p4(i) - mid(i)
      end do
 
      th2 = asind(vdotn(v3, v4, 3))
 
      h2 = cosd(th2) * vmod(v4, 3) / 2.
 
      if (p6(1) .lt. p5(1)) then
        lb2 = 0.
        lh2 = vmod(v3, 3) / 2.
      else
        lb2 = vmod(v3, 3) / 2.
        lh2 = 0.
      end if
 
C  Note that for downward-pointing triangles, the X-axis (p2 -> p3) is
C  pointing in the wrong direction.
      if (p3(1) .lt. p2(1)) then
        do i = 1, 3
          v1(i) = -v1(i)
        end do
      end if
 
C  Determine the azimuthal angle phi
      if (vmod(v5,3) .eq. 0.) then
        phi = 0.
      else
        if (p3(1) .gt. p2(1)) then
          phi = acosd(vdotn(v5, v1, 3))
        else
          phi = -acosd(vdotn(v5, v1, 3))
        end if
      endif
 
C  Determine prism position within the major triangle
      g_x = (2. * p1(1) + p2(1) + p3(1) +
     1       2. * p4(1) + p5(1) + p6(1)) / 8.
      g_y = (2. * p1(2) + p2(2) + p3(2) +
     1       2. * p4(2) + p5(2) + p6(2)) / 8.
      g_z = (2. * p1(3) + p2(3) + p3(3) +
     1       2. * p4(3) + p5(3) + p6(3)) / 8.
 
C  Determine the prism's orientation relative to the coordinate system
C  of the major triangle. The format is the one used in the GEANT
C  routine GSROTM.
      g_theta1 = acosd (v1(3) / vmod (v1, 3))
      if (v1(1) .ne. 0.) then
        g_phi1 = atand (v1(2) / v1(1))
      else
        g_phi1 = 90.
      end if
 
      call cross (b1, v1, v6)
      g_theta2 = acosd (v6(3) / vmod (v6, 3))
      if (v6(1) .ne. 0.) then
        g_phi2 = atand (v6(2)/v6(1))
      else
        g_phi2 = 90.
      end if
      if (g_phi2 .lt. 0.) then
        g_phi2 = g_phi2 + 180.
      end if
 
      g_theta3 = acosd (b1(3) / vmod(b1,3))
      if (b1(1) .ne. 0.) then
        g_phi3 = atand (b1(2)/b1(1))
      else
        g_phi3 = 90.
      end if
C  Check quadrant on G_PHI3; should be 0 < G_PHI3 < 180
      if (g_phi3 .lt. 0.) then
        g_phi3 = g_phi3 + 180.
      end if
 
c  Format statements:
100   format (f13.10, 5x, f13.10, 5x, f13.10)
200   format (i2, 3x, i2, 3x, 3(i1, 1x, i1, 2x))
 
      return
      end
                  ! the two triangular prism faces
