*  Create the Crystal Ball detector and the target assembly inside
      subroutine ugeom

************************************************************************
*                                                                      *
* UGEOM                                                                *
*                                                                      *
* Description and modification history:                                *
*                                                                      *
* Defines the geometry for the Crystal Ball simulation                 *
*                                                                      *
* 970508 (MC): replaced internal write statements with calls to the    *
*    CERNLIB subroutine CSETDI, which does the same but should be more *
*    portable (since the CERN libraries are required for this program  *
*    anyway).                                                          *
*                                                                      *
* Input  variables: none                                               *
*                                                                      *
* Output variables: none                                               *
*                                                                      *
* Called by: UGINIT                                                    *
*                                                                      *
* Calls:     PRISM, GEANT routines                                     *
*                                                                      *
* written  by: John W. Price, 29/10/93, 12.00                          *
*                                                                      *
************************************************************************
 
      implicit none
c
c  Variable declarations
c
c  1. Global variables (common blocks)
c
#include "materials.inc"
#include "gctmed.inc"
#include "g77_intrinsic.inc"
c
c  2. Local variables
c
      character
     1  hmsph*4   ! volume name for minor triangles, crystals
     1, hmsph1*4  ! volume name for minor triangles, crystals
     1, cname*4   ! volume name for minor triangles, crystals
     1, cname2*4  ! volume name for minor triangles, used when cname is
                  ! already 'taken' for crystals
     1, cnum*2    ! character string representing minor triangle/crystal
                  ! number
     1, conly*4   ! used in call to gspos ('ONLY'/'MANY')
 
      logical
     1  exist(2)  ! used to flag existing crystals in beam tunnel
                  ! regions
 
      integer
     1  crnum     ! crystal number within MIN3, MN3M, MN3P
     1, i         ! loop index
     1, icopy(9,4)! relates volume copy number of crystals within minor
                  ! triangle to crystal type
     1, ivolu     ! system volume number returned by GSVOLU
     1, j         ! loop index
     1, k         ! loop index
     1, l         ! loop index
     1, m         ! loop index
     1, missing(12,2) ! list of crystals left out to make room for the
                  ! beam tunnels. Crystal numbers are in the format mc,
                  ! where m is the minor triangle number, c the crystal
                  ! number within the minor triangle. The second index
                  ! refers to the beam openings at +/-z.
     1, mtnum     ! major triangle number
     1, tmax 
     1, nmel      ! no. of melon scintillator elements
      real
     1  alf       ! see SKIRT (V.K.)
     1, cnou_radius ! inner radius of the spherical aluminum shell
                  ! ("can") surrounding each CB hemisphere
     1, cnou_zend ! z-coordinate of the extreme ends of the Al shell
                  ! surrounding each CB hemisphere (where it meets the
                  ! beam tunnels)
     1, dz        ! half length of NaI crystals. Note that most
                  ! geometric/positioning parameters for crystals are
                  ! also used for minor triangles 1, 2, and 4, since
                  ! these use the same basic shape.
     1, elloverr  ! ratio of the length of the edge of an icosahedron to
                  ! the radius of its circumscribed sphere
     1, g_phi1(0:11) ! azimuthal angle for crystal rotation
     1, g_phi2(0:11) ! azimuthal angle for crystal rotation
     1, g_phi3(0:11) ! azimuthal angle for crystal rotation
     1, g_theta1(0:11) ! polar angle for crystal rotation
     1, g_theta2(0:11) ! polar angle for crystal rotation
     1, g_theta3(0:11) ! polar angle for crystal rotation
     1, g_x(0:11) ! x-coordinate of crystal position
     1, g_y(0:11) ! y-coordinate of crystal position
     1, g_z(0:11) ! z-coordinate of crystal position
     1, h1        ! half length in y of inner crystal face
     1, h2        ! half length in y of outer crystal face
     1, in2cm     ! conversion factor inches -> cm
     1, lb1       ! half length in x of inner crystal face at low y
     1, lb2       ! half length in x of outer crystal face at low y
     1, lh1       ! half length in x of inner crystal face at high y
     1, lh2       ! half length in x of outer crystal face at high y
     1, l_veto    ! half length of veto barrel
     1, mtphi     ! azimuthal angle for major triangle rotation
     1, mttheta(4)! polar angles for major triangle rotation
     1, par3(3)   ! volume parameters for volumes that take 3 parameters
                  ! (BOX, TUBE)
     1, par4(4)   ! volume parameters for volumes that take 4 parameters
                  ! (HYPE)
     1, par5(5)   ! volume parameters for volumes that take 5 parameters
                  ! (CONE)
     1, par6(6)   ! volume parameters for volumes that take 6 parameters
                  ! (SPHE)
     1, par7(7)   ! volume parameters for volumes that take 7 parameters
                  ! (CONS)
     1, par10(10) ! volume parameters for volumes that take 10
                  ! parameters (PGON, in the simplest case)
     1, par11(11) ! volume parameters for volumes that take 11
                  ! parameters (TRAP, CTUB)
     1, par15(15) ! volume parameters for volumes that take 15
                  ! parameters (PCON, with four planes)
     1, ph1       ! see SKIRT (V.K.)
     1, ph2       ! see SKIRT (V.K.)
     1, ph3       ! see SKIRT (V.K.)
     1, phi       ! azimuthal angle of the line joining the centers of
                  ! the two crystal (minor triangle) faces
     1, phi1      ! azimuthal angle for rotation matrix (GSROTM)
     1, phi2      ! azimuthal angle for rotation matrix (GSROTM)
     1, phi3      ! azimuthal angle for rotation matrix (GSROTM)
     1, r1        ! radius of circle inscribed to inner face of major
                  ! triangle
     1, r2        ! radius of circle inscribed to outer face of major
                  ! triangle
     1, rminor    ! volume parameter for minor triangle number 3
     1, r_veto    ! radius of circle inscribed to veto barrel
     1, skir_slope! tangent of half opening angle of "skirts" around the
                  ! beam line
     1, skrl      ! see SKIRT (V.K.)
     1, skrt      ! see SKIRT (V.K.)
     1, th        ! polar angle of the line joining the centers of the
                  ! two crystal (minor triangle) faces
     1, th1       ! angle of the inner crystal face w.r.t. the y-axis
     1, th2       ! angle of the outer crystal face w.r.t. the y-axis
     1, th3       ! see SKIRT (V.K.)
     1, theta1    ! polar angle for rotation matrix (GSROTM)
     1, theta2    ! polar angle for rotation matrix (GSROTM)
     1, theta3    ! polar angle for rotation matrix (GSROTM)
     1, tunl_rin  ! inner radius of beam tunnel
     1, tunl_rout ! outer radius of beam tunnel
     1, tunl_zpos ! outer end of cylindrical part of beam tunnel
     1, twcp_bow  ! extent in z of the target wall endcap
     1, twcp_radius ! largest radius of the target wall endcap
     1, twcp_sphere_radius ! radius of sphere from which the target wall
                  ! endcap is "cut"
     1, twov(3)   ! dimensions of the target wall overlap region
     1, t_veto    ! thickness of veto barrel
     1, x         ! volume positioning parameter (GSPOS)
     1, y         ! volume positioning parameter (GSPOS)
     1, z         ! volume positioning parameter (GSPOS)
     1, zz         ! see SKIRT (V.K.)
     1, btmthnss  ! thickness of the "bottom" of the hemispheres
c     1, taps_coor(529,4)
     1, tdeg
     1, rscifi(15)!radii of scintillating fibre layers

      double precision
     1  atest     ! test variable for pos./neg. azimuthal angle
     1, dphi      ! double precision typecast of g_phi1/2/3(i)
     1, dphi1     ! double precision typecast of phi1
     1, dphi2     ! double precision typecast of phi2
     1, dphi3     ! double precision typecast of phi3
     1, dt30      ! double precision typecast of g_theta3(0)
     1, dtheta    ! double precision typecast of g_theta1/2/3(i)
     1, dtheta1   ! double precision typecast of theta1
     1, dtheta2   ! double precision typecast of theta2
     1, dtheta3   ! double precision typecast of theta3

      data
     1  in2cm  /2.54/
     1, rminor /0.1784110665/
 
      call materials

c  Define geometry.
 
c  "lab" volume.  Simply a box large enough to contain the Crystal
c  Ball.
      par3(1) = 100.
      par3(2) = 100. + gap(1)/2 + gap(2)/2
      par3(3) = 210.
      call gsvolu ('LABB',
     1             'BOX ',
     1             air,
     1             par3,
     1             3,
     1             ivolu)
 
c  The Crystal Ball. First, define a sphere that's large enough to
c  contain the major triangles (which, in turn, contain the minor
c  triangles, which, in turn, contain the crystals). The purpose of this
c  volume is to provide an interface between the "natural" polar
c  coordinate system for the Crystal Ball, with the z-axis passing
c  through the two poles, and the lab. coordinate system, with the
c  z-axis along the beam line.
      par6(1) = 0.
      par6(2) = 100.
      par6(3) = 0.
      par6(4) = 180.
      par6(5) = 0.
      par6(6) = 360.
      call gsvolu ('CBLU',
     1             'SPHE',
     1             Air,
     1             par6,
     1             6,
     1             ivolu)
      call gsvolu ('CBLD',
     1             'SPHE',
     1             Air,
     1             par6,
     1             6,
     1             ivolu)
 
c  Major triangles. The parameters are a bit off, to make sure things
c  overlap OK.
      elloverr = 2. * sin(0.5 * atan(2.))
 
c  Note that mttheta(1) is used for positioning major triangles.  We
c  brought it up here to help calculate the proper radius for the circle
c  inscribed in the major triangle.
      mttheta(1) = acosd((1. + 2. * cos(atan(2.))) /
     1                    sqrt(sin(atan(2.))**2 * (1. + cosd(72.))**2 +
     1                         sin(atan(2.))**2 * sind(72.)**2 +
     1                         (1. + 2. * cos(atan(2.)))**2))
      r1 = 18. / cosd(mttheta(1))
      r2 = 78. / cosd(mttheta(1))
 
      par10( 1) =   90.  ! phi1 - first corner of PGON is on +y-axis
      par10( 2) =  360.  ! dphi - PGON covers full 360 range in phi
      par10( 3) =    3.  ! npdv - triangular faces
      par10( 4) =    2.  ! nz - two z-planes, the inner & outer face
      par10( 5) =  -32.  ! Z1 - z-coordinate of inner face
      par10( 6) =    0.  ! RMIN1 - shortest distance from the z-axis to
                         ! the inner edge of the inner face. 0 means
                         ! that the PGON is solid (if that can be said
                         ! for a volume of air).
      par10( 7) =  r1*elloverr*cosd(30.)/3. ! RMAX1 - shortest distance
                         ! from the z-axis to the outer edge of the
                         ! inner face.
      par10( 8) =   28.  ! Z2 - repeat the previous three parameters for
                         ! the outer face.
      par10( 9) =    0.  ! RMIN2
      par10(10) =  r2*elloverr*cosd(30.)/3. ! RMAX2
      call gsvolu ('MAJO',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MAJB',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MAJS',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MAJP',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MJPS',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MAJM',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MJMS',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
c  Minor triangles. Since the faces of minor triangles 1, 2, and 4
c  aren't equilateral (they are isosceles triangles), we can't use the
c  PGON shape and have to use TRAP instead. For the meaning of the
c  individual parameters, see the comments that come with the variable
c  declarations, or the explanation of the TRAP shape in the GEANT
c  manual.
      call prism (0,
     1            dz, th, phi,
     1            h1, lb1, lh1, th1,
     1            h2, lb2, lh2, th2,
     1            g_x(0), g_y(0), g_z(0),
     1            g_theta1(0), g_theta2(0), g_theta3(0),
     1            g_phi1(0), g_phi2(0), g_phi3(0))
      par11( 1) =  dz
      par11( 2) =  th
      par11( 3) = phi
      par11( 4) =  h1
      par11( 5) = lb1
      par11( 6) = lh1
      par11( 7) = th1
      par11( 8) =  h2
      par11( 9) = lb2
      par11(10) = lh2
      par11(11) = th2
 
      do i = 0, 2
        call csetdi(2**i,cnum,1,1)  ! this gives us the numbers 1, 2, 4, as
                                ! 1-character strings
        cname = 'MIN'//cnum     ! full minor triangle
        call gsvolu (cname,
     1               'TRAP',
     1               Air,
     1               par11,
     1               11,
     1               ivolu)
 
        cname = 'MN'//cnum(1:1)//'P'  ! minor triangle next to beam
        call gsvolu (cname,           ! tunnel at positive z
     1               'TRAP',          ! (downstream)
     1               Air,
     1               par11,
     1               11,
     1               ivolu)
 
        cname = 'MN'//cnum(1:1)//'M'  ! minor triangle next to beam
        call gsvolu (cname,           ! tunnel at negative z (upstream)
     1               'TRAP',
     1               Air,
     1               par11,
     1               11,
     1               ivolu)
      end do  ! i = 0, 2
 
c  MIN3 (as well as MN3P, MN3M) has equilateral triangles as its faces,
c  so PGON will work here. The parameters have the same meaning as for
c  the major triangles. However, starting the definition of the triangles
c  at phi = 270 degrees (i.e. along the y-axis, as we originally did) and
c  then going up in steps of 120 degrees appears to cause a problem with
c  finding the minor triangle in some instances during the tracking
c  process. This may be because we end up with phi values larger than 360
c  degrees. In that case, 30 degrees as a starting angle should work and
c  not make any additional changes necessary.
c
      par10( 1) =   30.                          ! phi1
      par10( 2) =  360.                          ! dphi
      par10( 3) =    3.                          ! npdv
      par10( 4) =    2.                          ! nz
      par10( 5) =  -26.*sqrt(1. - 4.*rminor**2)  ! Z1
      par10( 6) =    0.                          ! RMIN1
      par10( 7) =   24.*rminor                   ! RMAX1
      par10( 8) =   26.*sqrt(1. - 4.*rminor**2)  ! Z2
      par10( 9) =    0.                          ! RMIN2
      par10(10) =   76.*rminor                   ! RMAX2
      call gsvolu ('MIN3',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MN3P',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
      call gsvolu ('MN3M',
     1             'PGON',
     1             Air,
     1             par10,
     1             10,
     1             ivolu)
 
c  The actual crystals. There are 11 different crystal types, with
c  slightly different geometric parameters, labeled CR01 - CR11. Types 1
c  to 9 are in the outer minor triangles (1,2,4); the crystal type
c  corresponds to the crystal number for minor triangle 1, and follows
c  for 2 and 4 from the threefold symmetry of a major triangle. Types 10
c  and 11 exist in the central minor triangle (3), with type 10 for the
c  crystals in the corners of that triangle and 11 for the other six.
c  The meaning of the geometry parameters is explained with the variable
c  declarations.
      do i = 1, 11
        call prism (i,
     1              dz, th, phi,
     1              h1, lb1, lh1, th1,
     1              h2, lb2, lh2, th2,
     1              g_x(i), g_y(i), g_z(i),
     1              g_theta1(i), g_theta2(i), g_theta3(i),
     1              g_phi1(i), g_phi2(i), g_phi3(i))
        par11( 1) =  dz
        par11( 2) =  th
        par11( 3) = phi
        par11( 4) =  h1
        par11( 5) = lb1
        par11( 6) = lh1
        par11( 7) = th1
        par11( 8) =  h2
        par11( 9) = lb2
        par11(10) = lh2
        par11(11) = th2
 
        cnum(1:1) = '0'          ! to get a leading 0 for crystal numbers
                                 ! less than 10
        call csetdi(i,cnum,1,2)  ! converts the crystal type into a
                             ! two-digit string.
        cname = 'CR'//cnum
        call gsvolu (cname,
     1               'TRAP',
     1               NaI,
     1               par11,
     1               11,
     1               ivolu)
      end do  ! i = 1, 11
 
c  The cylindrical cut in the Crystal Ball. This is 10 cm in radius -
c  actually, make that 10.3 cm plus 1/16". The 10.3 cm are from A.
c  Efendiev's measurement of the tunnel diameter on March 6, 96; the
c  1/16" is the steel thickness specified in the SLAC CB reports. Make
c  this volume long enough to contain anything that fits in diameter -
c  i.e., as long as the lab box


      par3(1) = 0.                ! inner radius
**      par3(2) = 8.2               ! outer radius
cas      par3(2) = 10.3              ! outer radius
      par3(2) = 10.3+ 3.*in2cm/16.              ! outer radius
cas      par3(3) = 100.              ! half length
      par3(3) = 44.              ! half length
      call gsvolu ('CCUT',   
     1             'TUBE',
     1             air,
     1             par3,
     1             3,
     1             ivolu)

cas      par5(1) = 8.2               ! inner radius
cas      par5(2) = 10.3 + in2cm/16.  ! outer radius
cas      par5(3) = 100.              ! half length
cas      par5(4) = 0.0
cas      par5(5) = 180.0
cas      call gsvolu ('CCTT',
cas     1             'TUBS',
cas     1             air,
cas     1             par5,
cas     1             5,
cas     1             ivolu)
 
c  The actual beam entrance and exit tunnels - a pair of cylindrical
c  steel tubes, 10.3 cm in inner diameter, 1/16" thick, and 11.5 cm
c  wide, one around each beam opening.
      tunl_rin = 10.3              ! inner radius; needed again later
cas      tunl_rout = 10.3 + in2cm/16. ! outer radius; needed again later
      tunl_rout = 10.3 + 3.*in2cm/16. ! outer radius; needed again later
      par5(1) = tunl_rin
      par5(2) = tunl_rout
      par5(3) = 5.75               ! half length
      par5(4) = 0.
      par5(5) = 180.
      call gsvolu ('TUNL',
     1             'TUBS',
     1             Iron,
     1             par5,
     1             5,
     1             ivolu)
 
c  Rims right after the beam tunnel and inside the CB tunnel. Simulated by placing a 4 mm
c  steel ring after the Ball. The ring is shadowing 19.93 degree cone. 

      par5(1) = 34.43
      par5(2) = 65
      par5(3) = 0.2               ! half length
      par5(4) = 0.
      par5(5) = 180.
      call gsvolu ('RIMS',
     1             'TUBS',
     1             Iron,
     1             par5,
     1             5,
     1             ivolu)

c  The inner 'can' around the Crystal Ball. This is a steel sphere ~20
c  cm in radius, 1/16" thick, with beam openings at both ends covering a
c  half angle of about 30 degrees each. The actual numbers used are from
c  measurements made by A. Efendiev on the lower hemisphere on March 6,
c  1996.
cas      par6(1) = sqrt(34.7**2 + 20.6**2)/2  ! inner radius
cas      par6(2) = par6(1) + in2cm/16.        ! outer radius
cas      par6(3) = atand(20.6/34.7)           ! low theta limit
cas      par6(4) = 180. - par6(3)             ! high theta limit
cas      par6(5) = 0.                         ! low phi limit
cas      par6(6) = 180.                       ! high phi limit
      par6(1) = 9.8* in2cm ! inner radius
cas      par6(2) = par6(1) + in2cm/16.        ! outer radius
      par6(2) = par6(1) + 3.*in2cm/16.        ! outer radius
      par6(3) = asind(tunl_rout/par6(1))           ! low theta limit
      par6(4) = 180. - par6(3)             ! high theta limit
      par6(5) = 0.                         ! low phi limit
      par6(6) = 180.                       ! high phi limit
      call gsvolu ('CNIN',
     1             'SPHE',
     1             Iron,
     1             par6,
     1             6,
     1             ivolu)

** CNOU, SKIR, and SKAL are unneedful or wrong volumes (V.K.) **
 
c  The outer shell of the 'can' around the CB. Neglecting the windows
c  for the PM tubes for simplicity and using dimensions specified in M.
c  Oreglia's dissertation otherwise, this is a 7/8" thick Al sphere with
c  an inner radius of 26 11/16". The theta limits are defined by the
c  requirement of hitting but not intersecting the previously defined
c  volume 'SKIR'. Thus theta is given by
c           theta = acos((z + l)/r),
c  where z is the positive solution of the equation
c           (z + l)^2 + (h + z*tan(phi))^2 = r^2,
c  l is the z-distance from the center at which the cone SKIR starts (or
c  the tube TUNL ends, 28.85 cm), h is the outer radius of SKIR at that
c  distance (10.3cm + 1/16"), phi is the half opening angle of SKIR
c  (19), and r is the inner radius of the Al sphere. Solving the
c  quadratic equation for x gives
c
c        l + h*tan(phi)       / /l+h*tan(phi)\^2   h^2+l^2-r^2  \
c  z = - -------------- + sqrt| |------------|   - ------------ |
c        1 + tan^2(phi)       \ \1+tan^2(phi)/     1+tan^2(phi) /
c
      tunl_zpos = 28.85  ! outer end of cylindrical tunnels region
      skir_slope = tand(19.) ! dwatts bug fix 20/6/01
      cnou_radius = (26. + 11./16.) * in2cm  ! inner radius of Al sphere
      cnou_zend = (tunl_zpos + tunl_rout * skir_slope)
     1            /(1. + skir_slope**2)
      cnou_zend = -cnou_zend + sqrt(cnou_zend**2 -
     1             (tunl_rout**2 + tunl_zpos**2 - cnou_radius**2)
     1            /(1 + skir_slope**2)) ! largest z-extent of sphere

** V.K.
**      par6(1) = cnou_radius             ! inner radius
**      par6(2) = par6(1) + 7./8. * in2cm ! outer radius
**      par6(3) = acosd((cnou_zend + tunl_zpos)/cnou_radius) ! low and
**      par6(4) = 180. - par6(3)          ! high limit in theta
**      par6(5) = 0.                      ! low limit in phi
**      par6(6) = 180.                    ! high limit in phi
**      call gsvolu ('CNOU',
**     1             'SPHE',
**     1             Al,
**     1             par6,
**     1             6,
**     1             ivolu)

 
c  The "skirt" around the beam tunnels. In reality, an irregular steel
c  hexagon, 1/16" thick, that reflects the shape of the group of 24
c  crystals left out of the ball to get each hole. Unfortunately, GEANT
c  does not have irregular hexagons in its arsenal of shapes. The only
c  exact representation of this that might be feasible is two TRAP
c  shapes, plus two smaller, but otherwise identical air TRAPs to
c  "remove" the excess steel. Calculating the parameters of these TRAPs
c  looks like a tough job, and we would still be left with the problem
c  of "seamlessly" joining the result to the TUNL volume - unlike in the
c  real world, we can't sand down or bend anything to get out the kinks.
c  Instead, approximate this by a cone that starts at the outer end of
c  TUNL (with the same diameters) and extend all the way to the edge of
c  LABB, with a half opening angle of 19. The resulting skirt is ~3 cm
c  longer than the real one, according to an old (pre-CB-construction)
c  SLAC drawing (in CB Note 118, which deals with the geometry). The
c  opening angle is selected to safely avoid any interference with the
c  crystals beyond what TUNL already does to them.

** V.K.
**      par7(1) = 35.575        ! half length
**      par7(2) = tunl_rin      ! inner radius at inner end
**      par7(3) = tunl_rout     ! outer radius at inner end
**      par7(4) = par7(2) + 2. * par7(1) * skir_slope ! inner radius at
**                                                    ! outer end
**      par7(5) = par7(4) + in2cm/16. ! outer radius at outer end
**      par7(6) = 0.
**      par7(7) = 180.
**      call gsvolu ('SKIR',
**     1             'CONS',
**     1             Iron,
**     1             par7,
**     1             7,
**     1             ivolu)
 
c  A 1/4" aluminum reinforcement of the skirt where it extends beyond
c  the outer can. The thickness is fron figure 7 of SLAC CB Report 118.
c  To fill in a small gap between SKIR and CNOU resulting from different
c  opening angles of the two, this volume will start at the inner end of
c  CNOU.

** V.K.
**      par7(1) = (100. - (cnou_zend + tunl_zpos))/2. ! half length
**      par7(2) = sqrt(cnou_radius**2 - (cnou_zend + tunl_zpos)**2)
**                                    ! inner radius at inner end
**      par7(3) = par7(2) + in2cm/4.  ! outer radius at inner end
**      par7(4) = tunl_rin + 71.15 * skir_slope + in2cm/16.
**                                    ! inner radius at outer end
**      par7(5) = par7(4) + in2cm/4.  ! outer radius at outer end
**      par7(6) = 0.
**      par7(7) = 180.
**      call gsvolu ('SKAL',
**     1             'CONS',
**     1             Al,
**     1             par7,
**     1             7,
**     1             ivolu)
 
c  The "bottom" of the hemispheres. According CB`s draw the thickness of the
c  plane from stainless steel is 0.030" (0.076 cm). This is a segment of a disk
c  with the inner radius equal inner radius of the inner `can` around the CB and
c  with out radius equal outer radius of the outer shell of the CB.
 
      btmthnss= 0.076
      gap(1)  = gap(1) + btmthnss
      gap(2)  = gap(2) + btmthnss
      par5(1) = sqrt(34.7**2 + 20.6**2)/2    ! inner radius
      par5(2) = cnou_radius + 7./8. * in2cm  ! outer radius
      par5(3) = btmthnss / 2.0
**      par5(4) = asind((tunl_rin + 71.15 * skir_slope
**     1                              + in2cm/16. - tunl_rout) / 71.15)
      par5(4) = 23.9  ! V.K.
      par5(5) = 180. - par5(4)
      call gsvolu ('BTMM',
     1             'TUBS',
     1             Iron,
     1             par5,
     1             5,
     1             ivolu)

**================= SKIRT (V.K.) ================================>
      skrl = (71.15 - 3.)/2.   !half-length of skirt's trapezoids
      skrt = in2cm/32         !half-thickness of skirt's steel (1/16")   
c      skrt = 0.079375

      par4(1) = 6.75        ! half-length along x at -dz
      par4(2) = 21.7        ! half-length along x at +dz
      par4(3) = skrt        ! half-length along y
      par4(4) = skrl        ! half-length along z
      call gsvolu ('SKI1','TRD1',Iron,par4,4,ivolu)

      par4(1) = 5.85        ! half-length along x at -dz
      par4(2) = 19.         ! half-length along x at +dz
      call gsvolu ('SKI2','TRD1',Iron,par4,4,ivolu)

      par4(1) = 5.85        ! half-length along x at -dz
      par4(2) = 19.         ! half-length along x at +dz
      call gsvolu ('SKI3','TRD1',Iron,par4,4,ivolu)

**================= SKI1 (V.K.) =================================>
         th3 = 20.9 !-0.1
         ph3 = 148.1
         alf = -0.12
      ph1=ph3 - 90. - alf
      th1=atand(-1./(tand(th3)*cosd(ph1-ph3)))             +180.
      ph2=atand((tand(th3)*cosd(ph3)-tand(th1)*cosd(ph1))
     &         /(tand(th1)*sind(ph1)-tand(th3)*sind(ph3))) +180.          
      th2=atand(-1./(tand(th3)*cosd(ph2-ph3)))             +180.
      call gsrotm(51,th1,ph1,th2,ph2,th3,ph3)
      call gsrotm(52,th1,ph1+180.,th2,ph2+180.,th3,ph3+180.)
      call gsrotm(53,th1,ph1+180.,180.-th2,ph2,180.-th3,ph3)
      call gsrotm(54,th1,ph1,180.-th2,ph2+180.,180.-th3,ph3+180.)
        z = tunl_zpos + skrl*cosd(th3)
         zz = (z - skrt/sind(th3))*tand(th3)
         x = zz*cosd(ph3)
         y = zz*sind(ph3)
      call gspos('SKI1',1,'LABB', x, y+gap(1), z,51,'ONLY')
      call gspos('SKI1',2,'LABB',-x,-y-gap(2), z,52,'ONLY')
      call gspos('SKI1',3,'LABB', x, y+gap(1),-z,53,'ONLY')
      call gspos('SKI1',4,'LABB',-x,-y-gap(2),-z,54,'ONLY')

**================= SKI2 (V.K.) =================================>
         th3 = 20.1 !-0.1
         ph3 = 89.2
         alf = 8.4
         alf = 7.9
      ph1=ph3 - 90. - alf + 360.
      th1=atand(-1./(tand(th3)*cosd(ph1-ph3)))
      ph2=atand((tand(th3)*cosd(ph3)-tand(th1)*cosd(ph1))
     &         /(tand(th1)*sind(ph1)-tand(th3)*sind(ph3)))
      th2=atand(-1./(tand(th3)*cosd(ph2-ph3)))             +180.
      call gsrotm(61,th1,ph1,th2,ph2,th3,ph3)
      call gsrotm(62,th1,ph1-180.,th2,ph2+180.,th3,ph3+180.)
      call gsrotm(63,th1,ph1-180.,180.-th2,ph2,180.-th3,ph3)
      call gsrotm(64,th1,ph1,180.-th2,ph2+180.,180.-th3,ph3+180.)
         z = tunl_zpos + skrl*cosd(th3)
        zz = (z - skrt/sind(th3))*tand(th3)
         x = zz*cosd(ph3)
         y = zz*sind(ph3)
      call gspos('SKI2',1,'LABB', x, y+gap(1), z,61,'ONLY')
      call gspos('SKI2',2,'LABB',-x,-y-gap(2), z,62,'ONLY')
      call gspos('SKI2',3,'LABB', x, y+gap(1),-z,63,'ONLY')
      call gspos('SKI2',4,'LABB',-x,-y-gap(2),-z,64,'ONLY')

**================= SKI3 (V.K.) =================================>
          th3 = 20.1 !-0.1
          ph3 = 27.2
          alf = -8.1
      ph1=ph3 - 90. - alf + 360.
      th1=atand(-1./(tand(th3)*cosd(ph1-ph3)))             +180.
      ph2=atand((tand(th3)*cosd(ph3)-tand(th1)*cosd(ph1))
     &         /(tand(th1)*sind(ph1)-tand(th3)*sind(ph3)))
      th2=atand(-1./(tand(th3)*cosd(ph2-ph3)))             +180.
      call gsrotm(71,th1,ph1,th2,ph2,th3,ph3)
      call gsrotm(72,th1,ph1-180.,th2,ph2+180.,th3,ph3+180.)
      call gsrotm(73,th1,ph1-180.,180.-th2,ph2,180.-th3,ph3)
      call gsrotm(74,th1,ph1,180.-th2,ph2+180.,180.-th3,ph3+180.)
         z = tunl_zpos + skrl*cosd(th3)
         zz = (z - skrt/sind(th3))*tand(th3)
         x = zz*cosd(ph3)
         y = zz*sind(ph3)
      call gspos('SKI3',1,'LABB', x, y+gap(1), z,71,'ONLY')
      call gspos('SKI3',2,'LABB',-x,-y-gap(2), z,72,'ONLY')
      call gspos('SKI3',3,'LABB', x, y+gap(1),-z,73,'ONLY')
      call gspos('SKI3',4,'LABB',-x,-y-gap(2),-z,74,'ONLY')
c===============================================================>
 
c  The target material, made of material "tmat" (default liquid
c  hydrogen), filling the target. All parameters are the same as for
c  TWAL, except all radii are 17.5 mil less.

c  JWP, 17-Aug-2003: Commented out, put into target_geometry.F
c        par3(1) = 0.0
c	par3(2) = 0.91
c	par3(3) = 2.5
c	par3(2) = 0.00001
c	par3(3) = 0.00001
c        call gsvolu ('TRGT',
c     1               'TUBE',
c     1               tmat,
c     1               par3,
c     1               3,
c     1               ivolu)
 
c  Shape of the individual TAPS detector as it is presented on their 
c  web page.
c        par10(1)  = 30.0
c	par10(2)  = 360.0
c	par10(3)  = 6.0
c        par10(4)  = 2.0
c        par10(5)  =-12.5
c	par10(6)  = 0.0
c	par10(7)  = 5.9/2.0
c        par10(8)  = 12.5
c	par10(9)  = 0.0
c	par10(10) = 5.9/2.0
c        call gsvolu ('TAPS',
c     1               'PGON',
c     1               BaF2,
c     1               par10,
c     1               10,
c     1               ivolu)
  
c  The beam pipe. The outer diameter is 6", the wall thickness 0.083",
c  and the length is 96". This is more than the size of our lab volume,
c  and it should be safe to ignore the far out parts of the pipe rather
c  than extend LABB. The material is aluminum, inside is vacuum.        

c  JWP, 17-Aug-2003: Commented out, put into target_geometry.F
c        par3(1) = 0.       ! inner radius
c        par3(2) = 3.0      ! outer radius of pipe = outer radius of
c                           ! vacuum volume
c        par3(3) = 60.0     ! half length - all the way through LABB 
c        call gsvolu ('VACU',
c     1               'TUBE',
c     1               vacuum,
c     1               par3,
c     1               3,
c     1               ivolu)

c  Positioning.  Start by placing 9 crystals inside each minor triangle.
c  First, read in the relationship between the 9 crystal types (index i)
c  and the copy number (icopy) for minor triangles (j) 1, 2, 4 resp.
c  between the order in which the crystals are placed (index i) and
c  icopy for minor triangle (j =) 3.
      open (unit=1, file='crystals.dat')
      do i = 1, 9
        read (1, 200) (icopy(i,j), j=1, 4)
      end do  !  i = 1, 9
      close (unit=1)
 
c  Next, find out which crystals are to be left out for the beam
c  tunnels. The crystals are specified in the format 10*minor triangle
c  number + crystal (copy) number. These numbers have to be different
c  for the upstream and downstream tunnels, indicated by the second
c  index.
      open (unit=1, file='missing.dat')
      do i = 1, 12
        read (1, 300) (missing(i,j), j=1, 2)
      end do  ! i = 1, 12
      close (unit=1)
 
      do i = 1, 9
c  Build the crystal's volume name.
        cnum(1:1) = '0'          ! to get a leading 0 for crystal numbers
                                 ! less than 10
        call csetdi(i,cnum,1,2)  ! converts the crystal type into a
                                 ! two-digit string.
        cname = 'CR'//cnum
 
c  The positions of the individual modules are calculated in PRISM using
c  a coordinate system whose z-axis is the axis from the center of the
c  ball through the center of the major triangle; whose y-axis is
c  parallel to the line from the center of the major triangle to its top
c  edge; and whose origin is the center of the ball. Rather than
c  recalculating the vertices from this coordinate system to one that is
c  centered on a minor triangle, it appears easier to transform the
c  results returned by prism, even more so since there are two different
c  types of minor triangles (one in the corners of the major triangle,
c  the other in its center). For the central minor triangle, no
c  transformation is necessary except a translation in z and a rotation
c  by 180 degrees about the z-axis (because it is upside down with
c  respect to the major triangle). For the corner triangles, we have
c  first a rotation of the coordinate system about the x-axis, by the
c  angle g_theta3(0), then translations of the crystals in y and z (not
c  in x for symmetry reasons) and finally a rotation of the crystals.
c  Again for symmetry reasons, the rotation reduces to one about the
c  x-axis by the angle g_theta3(0). Thanks to GEANT's way of specifying
c  axis rotations, this looks more complicated than it really is. For
c  the more involved calculations, it turns out we have to go to double
c  precision to avoid inaccuracies that lead to complaints from GEANT's
c  consistency checks and/or are visible in the GEANT++ displays.
 
c  Position of crystal centers relative to those of the minor triangles.
        x =  g_x(i)
        y = (g_y(i) - g_y(0)) * cosd(g_theta3(0)) -
     1      (g_z(i) - g_z(0)) * sind(g_theta3(0))
        z = (g_y(i) - g_y(0)) * sind(g_theta3(0)) +
     1      (g_z(i) - g_z(0)) * cosd(g_theta3(0))
 
        dt30 = g_theta3(0)   ! Angle between major and minor triangle
                             ! z-axes
        dtheta = g_theta1(i) ! polar angle of crystal x-axis in the
                             ! *major* triangle's coordinate system,
                             ! double precision
        dphi = g_phi1(i)     ! same for azimuthal angle
c  Converted to minor triangle c.s.:
        dtheta1 = dacosd(dsind(dtheta) * dsind(dphi) * dsind(dt30) +
     1                   dcosd(dtheta) * dcosd(dt30)) ! polar angle
        dphi1 = dacosd(dsind(dtheta) * dcosd(dphi) / dsind(dtheta1))
                       ! azimuthal angle, now check if it's + or -:
        atest = dsind(dtheta1) * dsind(dphi1)
     1        - dsind(dtheta) * dsind(dphi) * dcosd(dt30)
     1        + dcosd(dtheta) * dsind(dt30)
        if (dabs(atest) .gt. dabs(dsind(dtheta1) * dsind(dphi1)))
     1     dphi1 = 360.d0 - dphi1
        theta1 = dtheta1   ! convert back to single precision for input
        phi1 = dphi1       ! to GEANT routines
 
c  The same thing for the crystal's y-axis, without repetition of the
c  individual comments:
        dtheta = g_theta2(i)
        dphi = g_phi2(i)
        dtheta2 = dacosd(dsind(dtheta) * dsind(dphi) * dsind(dt30) +
     1                   dcosd(dtheta) * dcosd(dt30))
        dphi2 = dacosd(dsind(dtheta) * dcosd(dphi) / dsind(dtheta2))
        atest = dsind(dtheta2) * dsind(dphi2)
     1        - dsind(dtheta) * dsind(dphi) * dcosd(dt30)
     1        + dcosd(dtheta) * dsind(dt30)
        if (dabs(atest) .gt. dabs(dsind(dtheta2) * dsind(dphi2)))
     1     dphi2 = 360.d0 - dphi2
        theta2 = dtheta2
        phi2 = dphi2
 
c  And finally, the crystal's z-axis:
        dtheta = g_theta3(i)
        dphi = g_phi3(i)
        dtheta3 = dacosd(dsind(dtheta) * dsind(dphi) * dsind(dt30) +
     1                   dcosd(dtheta) * dcosd(dt30))
        dphi3 = dacosd(dsind(dtheta) * dcosd(dphi) / dsind(dtheta3))
        atest = dsind(dtheta3) * dsind(dphi3)
     1        - dsind(dtheta) * dsind(dphi) * dcosd(dt30)
     1        + dcosd(dtheta) * dsind(dt30)
        if (dabs(atest) .gt. dabs(dsind(dtheta3) * dsind(dphi3)))
     1     dphi3 = 360.d0 - dphi3
        theta3 = dtheta3
        phi3 = dphi3
 
        call gsrotm (30+i,
     1               theta1, phi1,
     1               theta2, phi2,
     1               theta3, phi3)
 
        do j = 0, 2
c  cycle through minor triangles 1, 2, 4 to position the crystals
          call csetdi(2**j,cnum,1,1)  ! this gives us the numbers 1, 2, 4,
                                      ! as 1-character strings
          cname2 = 'MIN'//cnum
 
          call gspos (cname,
     1                icopy(i,2**j),
     1                cname2,
     1                x,
     1                y,
     1                z,
     1                30+i,
     1                'ONLY')
 
c  Check which crystals have to be left out for the beam tunnels. Do
c  this in one pass for the tunnel on the positive (index l=1, MNxP) and
c  the negative z-axis (index l=2, MNxM). Then fill those minor
c  triangles accordingly. To make the simulation of the cylindrical cut
c  work, the crystals in the incomplete minor triangles (as well as
c  those triangles and the major ones that contain them) have to be of
c  the 'MANY' type.
          exist(1) = .true.
          exist(2) = .true.
          do k = 1, 12
            do l = 1, 2
              if ((exist(l)) .and.
     1            (missing(k,l) .eq. 10*(2**j) + icopy(i,2**j))) then
                exist(l) = .false.
              end if  ! exist(l) .and. ...
            end do  ! l = 1, 2
          end do  ! k = 1, 12
          if (exist(1)) then
            conly = 'MANY'
            if (j .eq. 2) conly = 'ONLY'
            cname2 = 'MN'//cnum(1:1)//'P'
            call gspos (cname,
     1                  icopy(i,2**j),
     1                  cname2,
     1                  x,
     1                  y,
     1                  z,
     1                  30+i,
     1                  conly)
          end if  ! exist(1)
 
          if (exist(2)) then
            conly = 'MANY'
            if (j .eq. 1) conly = 'ONLY'
            cname2 = 'MN'//cnum(1:1)//'M'
            call gspos (cname,
     1                  icopy(i,2**j),
     1                  cname2,
     1                  x,
     1                  y,
     1                  z,
     1                  30+i,
     1                  conly)
          end if  ! exist(2)
 
        end do  ! j = 0, 2
      end do  ! i = 1, 9
 
c  Now fill the central minor triangle, MIN3 (as well as MN3P and MN3M).
c  Here, we have a threefold symmetry for the three corner crystals and
c  a sixfold symmetry for the other six.
      do i = 10, 12
        k = min(i, 11)     ! to increase the threefold symmetry from the
        call csetdi(k,cnum,1,2) ! do-loop in j to a sixfold one
        cname = 'CR'//cnum
        do j = 1, 3
          x = g_x(k) * cosd(120.*(j-1)+60.*(i-k)) - ! obtain crystal
     1        g_y(k) * sind(120.*(j-1)+60.*(i-k))   ! positions through
          y = g_y(k) * cosd(120.*(j-1)+60.*(i-k)) + ! rotations in steps
     1        g_x(k) * sind(120.*(j-1)+60.*(i-k))   ! of 120 (CR10)
          z = g_z(k) - 50.*sqrt(1. - 4.*rminor**2)  ! resp. 60 (CR11)
 
c  Rotations - only phi varies in steps of 120/60
          theta1 = g_theta1(k)
          theta2 = g_theta2(k)
          theta3 = g_theta3(k)
          phi1   = g_phi1(k) + 120.*(j-1) + 60.*(i-k)
          phi2   = g_phi2(k) + 120.*(j-1) + 60.*(i-k)
          phi3   = g_phi3(k) + 120.*(j-1) + 60.*(i-k)
 
          crnum = 3 * (i-10) + j
          call gsrotm (40+crnum,
     1                 theta1, phi1,
     1                 theta2, phi2,
     1                 theta3, phi3)
 
          call gspos (cname,
     1                icopy(crnum,3),
     1                'MIN3',
     1                x,
     1                y,
     1                z,
     1                40+crnum,
     1                'ONLY')
 
c  Again, a special treatment for the tunnel regions
          exist(1) = .true.
          exist(2) = .true.
          do m = 1, 12
            do l = 1, 2
              if ((exist(l)) .and.
     1            (missing(m,l) .eq. 30 + icopy(crnum,3))) then
                exist(l) = .false.
              end if
            end do  ! l = 1, 2
          end do  ! m = 1, 12
          if (exist(1)) then
            call gspos (cname,
     1                  icopy(crnum,3),
     1                  'MN3P',
     1                  x,
     1                  y,
     1                  z,
     1                  40+crnum,
     1                  'MANY')
          end if  ! (exist(1))
 
          if (exist(2)) then
            call gspos (cname,
     1                  icopy(crnum,3),
     1                  'MN3M',
     1                  x,
     1                  y,
     1                  z,
     1                  40+crnum,
     1                  'MANY')
          end if  ! (exist(2))
 
        end do  ! j = 1, 3
      end do  ! i = 10, 12
 
c  Order the minor triangles in phi, to hopefully help with the tracking
c  According to the GEANT manual, this no longer has any effect with
c  GEANT 3.21. It is left in here just in case someone wants (or needs)
c  to go back to earlier GEANT versions, but is commented out.
c      do i = 1, 4
c        call csetdi(i,cnum,1,1)  ! this gives us the numbers 1, 2, 3,
c                                 ! 4, as 1-character strings
c        cname = 'MIN'//cnum
c        call gsord (cname, 6)
c        cname = 'MN'//cnum(1:1)//'P'
c        call gsord (cname, 6)
c        cname = 'MN'//cnum(1:1)//'M'
c        call gsord (cname, 6)
c      end do  ! i = 1, 4
 
c  Place the minor triangles in the major ones. Start with minor
c  triangles 1,2,4. For symmetry reasons, the positions and azimuthal
c  angles involve rotations in 120 steps about the z-axis. The
c  z-position of the minor triangle's center is just offset from the
c  major triangle's.
      do j = 0, 2
        call csetdi(2**j,cnum,1,1)  ! this gives us the numbers 1, 2, 4, as
                                    ! 1-character strings
        x = g_x(0) * cosd(120.*j) - g_y(0) * sind(120.*j)
        y = g_y(0) * cosd(120.*j) + g_x(0) * sind(120.*j)
        z = g_z(0) - 50.
 
        theta1 = g_theta1(0)
        theta2 = g_theta2(0)
        theta3 = g_theta3(0)
        phi1   = g_phi1(0) + 120.*j
        phi2   = g_phi2(0) + 120.*j
        phi3   = g_phi3(0) + 120.*j
 
        call gsrotm (20 + 2**j,
     1               theta1, phi1,
     1               theta2, phi2,
     1               theta3, phi3)
 
        cname = 'MIN'//cnum
        call gspos (cname,
     1              2**j,
     1              'MAJO',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              'ONLY')
 
        if (j .eq. 0)
     1   call gspos (cname,
     1              2**j,
     1              'MAJS',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              'ONLY')
 
        if (j .ne. 0)
     1   call gspos (cname,
     1              2**j,
     1              'MAJB',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              'ONLY')
 
c  Repeat for the beam tunnels
        cname = 'MN'//cnum(1:1)//'P'
        conly = 'MANY'
        if (j .eq. 2) conly = 'ONLY'
        if (j .ne. 0) then
         call gspos (cname,
     1              2**j,
     1              'MAJP',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              conly)
 
        else
         call gspos (cname,
     1              2**j,
     1              'MJPS',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              conly)
        end if ! j .ne. 0
 
        cname = 'MN'//cnum(1:1)//'M'
        conly = 'MANY'
        if (j .eq. 1) conly = 'ONLY'
        if (j .ne. 0) then
         call gspos (cname,
     1              2**j,
     1              'MAJM',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              conly)
 
        else
         call gspos (cname,
     1              2**j,
     1              'MJMS',
     1              x,
     1              y,
     1              z,
     1              20 + 2**j,
     1              conly)
        end if ! j .ne. 0
 
      end do  ! j = 0, 2
 
c  Position minor triangle 3. This involves no rotations whatsoever,
c  just a shift along the z-axis.
      call gspos ('MIN3',
     1            3,
     1            'MAJO',
     1            0.,
     1            0.,
     1            50.*(sqrt(1. - 4.*rminor**2) - 1.),
     1            0,
     1            'MANY')
 
      call gspos ('MIN3',
     1            3,
     1            'MAJB',
     1            0.,
     1            0.,
     1            50.*(sqrt(1. - 4.*rminor**2) - 1.),
     1            0,
     1            'MANY')
 
      call gspos ('MN3P',
     1            3,
     1            'MAJP',
     1            0.,
     1            0.,
     1            50.*(sqrt(1. - 4.*rminor**2) - 1.),
     1            0,
     1            'MANY')
 
      call gspos ('MN3M',
     1            3,
     1            'MAJM',
     1            0.,
     1            0.,
     1            50.*(sqrt(1. - 4.*rminor**2) - 1.),
     1            0,
     1            'MANY')
 
c  Place the major triangles within CBAL. Start with MTs 1, 5, 9, 13,
c  17.  MT 9 has an edge along the X-axis, which allows us to easily
c  determine the value for theta.  The phi values are trivially
c  obtained. Note that the actual calculation of mttheta(1) was
c  performed earlier in the routine.
      mttheta(4) = 180. - mttheta(1)
 
c  Next, get the theta value for the triangles in the middle.  Use MT
c  18, since it has an edge along the -y axis (which is identical to the
c  -x axis of the lab. coordinate system).
      mttheta(2) = acosd(cos(atan(2.)) /
     1             sqrt(sin(atan(2.))**2 *
     1                  (1. + cosd(72.) - cosd(144.))**2 +
     1                  sin(atan(2.))**2 * (sind(72.) + sind(144.))**2 +
     1                  cos(atan(2.))**2))
      mttheta(3) = 180. - mttheta(2)
 
      mtphi = -18.
      do i = 1, 5
        do j = 1, 4
          mtnum = 4*(i-1)+j
 
          theta1 = 90.
          if (j .lt. 3) then
              hmsph  = 'CBLU'
              hmsph1 = 'CBLD'
           else
              hmsph  = 'CBLD'
              hmsph1 = 'CBLU'
           endif
c  Polar angles
              if (mod(mtnum,2) .eq. 0) then
                 theta2 = 180. - abs(mttheta(j) - 90.)
              else
                 theta2 = abs(mttheta(j) - 90.)
              end if  ! mod(mtnum,2) .eq. 0
c  Azimuthal angles
              if (mod(mtnum,2) .eq. 0) then
                 phi1   = mtphi - 90.
              else
                 phi1   = mtphi + 90.
              end if  ! mod(mtnum,2) .eq. 0
              if (mod(mtnum,4) .lt. 2) then
                 phi2   = mtphi + 180.
              else
                 phi2   = mtphi
              end if  ! mod(mtnum,4) .lt. 2
              theta3 = mttheta(j)
              phi3   = mtphi
 
          call gsrotm (mtnum,
     1                 theta1, phi1,
     1                 theta2, phi2,
     1                 theta3, phi3)
 
          if ((mtnum .eq. 2) .or. (mtnum .eq. 3)) then
            call gspos ('MAJP',
     1                  mtnum,
     1                  hmsph,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
            call gspos ('MJPS',
     1                  mtnum,
     1                  hmsph1,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
          else if ((mtnum .eq. 11) .or. (mtnum .eq. 14)) then
            call gspos ('MAJM',
     1                  mtnum,
     1                  hmsph,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
            call gspos ('MJMS',
     1                  mtnum,
     1                  hmsph1,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
          else if ((j .eq. 2) .or. (j .eq. 3)) then
            call gspos ('MAJB',
     1                  mtnum,
     1                  hmsph,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
            call gspos ('MAJS',
     1                  mtnum,
     1                  hmsph1,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
          else
            call gspos ('MAJO',
     1                  mtnum,
     1                  hmsph,
     1                  50.*sind(mttheta(j))*cosd(mtphi),
     1                  50.*sind(mttheta(j))*sind(mtphi),
     1                  50.*cosd(mttheta(j)),
     1                  mtnum,
     1                  'MANY')
          end if  ! mtnum .eq. 2,3 / mtnum .eq. 11,14
c  Each odd-numbered triangle and the following even-numbered one are at
c  the same azimuthal angle; after that, we move ahead by 36.
          if (mod(mtnum,2) .eq. 0) then
            mtphi = mtphi + 36.
          end if  ! mod(mtnum,2) .eq. 0
        end do  ! j = 1, 4
      end do  ! i = 1, 5
 
c  The spherical shells around the inside and outside of the Crystal
c  Ball. It would be more convenient to place them directly in LABB,
c  because the coordinate systems are identical, but logically, they
c  belong to the Crystal Ball. Also, on the practical side, I don't want
c  to redo Fig. 4 of Crystal Ball report CB-96-1, which places them
c  inside CBAL. This means, we need a rotation matrix that inverts the
c  effect of matrix 25 below.
      call gsrotm (26,
     1             90., 90.,
     1              0.,  0.,
     1             90.,  0.)
 
      call gsrotm (95,
     1             90., 90.,
     1            180.,  0.,
     1             90.,  0.)
 
      call gspos ('CNIN',
     1            1,
     1            'CBLU',
     1            0.,
     1            0.,
     1            0.,
     1            26,
     1            'ONLY')

      call gspos ('CNIN',
     1            2,
     1            'CBLD',
     1            0.,
     1            0.,
     1            0.,
     1            95,
     1            'ONLY')

c  Place CBAL in the master reference system LABB, cyclically switching
c  the coordinate axes as follows (using the format CBAL coord. -> LABB
c  coord.): x -> z; y -> x; z -> y
      call gsrotm (25,
     1              0.,  0.,
     1             90.,  0.,
     1             90., 90.)
 
      call gspos ('CBLU',
     1            1,
     1            'LABB',
     1            0.,
     1            0. + gap(1),
     1            0.,
     1            25,
     1            'MANY')
 
      call gspos ('CBLD',
     1            2,
     1            'LABB',
     1            0.,
     1            0. - gap(2),
     1            0.,
     1            25,
     1            'MANY')
 
c  Place the iron cylinders representing the beam tunnels in LABB. Make
c  them ONLY volumes to make sure they are seen when "competing" with
c  the crystals. The upstream tunnel is rotated about the y-axis so its
c  (local) z-axis points outward, though this probably does not make any
c  difference, due to the symmetry of a cylinder.
 
      call gsrotm (96,
     1             90.,   0.,
     1             90., 270.,
     1              0.,   0.)

      call gspos ('TUNL',
     1            1,
cas     1            'CCTT',
     1            'CCUT',
     1            0.,
     1            0.,
cas     1            23.1,
     1            28.4,
     1            0,
     1            'ONLY')
 
      call gspos ('TUNL',
     1            2,
cas     1            'CCTT',
     1            'CCUT',
     1            0.,
     1            0.,
cas     1            -23.1,
     1            -28.4,
     1            0,
     1            'ONLY')

      call gspos ('TUNL',
     1            3,
cas     1            'CCTT',
     1            'CCUT',
     1            0.,
     1            0.,
cas     1            23.1,
     1            28.4,
     1            96,
     1            'ONLY')
 
      call gspos ('TUNL',
     1            4,
cas     1            'CCTT',
     1            'CCUT',
     1            0.,
     1            0.,
cas     1            -23.1,
     1            -28.4,
     1            96,
     1            'ONLY')
 
c  Steel rims right after the TUNL cylinder and inside the CB tunnel. I do not know how to put
c  them in to the tunnel, so I will place the rims outside the Ball such that they will shadow 19.93
c  degree area.
 
      call gspos ('RIMS',
     1            1,
     1            'LABB',
     1            0.,
     1            0.+ gap(1),
     1            101.0,
     1            0,
     1            'ONLY')
 
      call gspos ('RIMS',
     1            2,
     1            'LABB',
     1            0.,
     1            0.- gap(2),
     1            101.0,
     1            96,
     1            'ONLY')
 
c  The following rotation matrix is needed in the target positioning (if
c  it is done), but also after that. The only way to avoid defining it
c  twice is to define it before the conditional section.
      call gsrotm (23,
     1             90., 180.,
     1             90.,  90.,
     1            180.,   0.)
 
**      call target_geometry
      call UGEOM_target          ! V.K.

c  Place the fully equipped volume CCUT in the master reference system
c  LABB.
      call gspos ('CCUT',
     1            1,
     1            'LABB',
     1            0.,
     1            0.,
     1            0.,
     1            0,
     1            'ONLY')

c  Place the to copy of volume CCTT in the master reference system
c  LABB with shift along Y-axis.
cas      call gspos ('CCTT',
cas     1            1,
cas     1            'LABB',
cas     1            0.,
cas     1            0. + gap(1),
cas     1            0.,
cas     1            0,
cas     1            'MANY')
cas      call gspos ('CCTT',
cas     1            2,
cas     1            'LABB',
cas     1            0.,
cas     1            0. - gap(2),
cas     1            0.,
cas     1            96,
cas     1            'MANY')

c  Position the iron and aluminum "skirts", one each at each beam
c  tunnel. They don't fit in CBAL, so have them directly in LABB.
 
      call gsrotm (98,
     1              0.,  0.,
     1             90., 180.,
     1             90., 90.)
 
      call gspos ('BTMM',
     1            1,
     1            'LABB',
     1            0.,
     1            0. + gap(1) - btmthnss/2.0,
     1            0.,
     1            25,
     1            'ONLY')
 
      call gspos ('BTMM',
     1            2,
     1            'LABB',
     1            0.,
     1            0. - gap(2) + btmthnss/2.0,
     1            0.,
     1            25,
     1            'ONLY')
 
      call gspos ('BTMM',
     1            3,
     1            'LABB',
     1            0.,
     1            0. + gap(1) - btmthnss/2.0,
     1            0.,
     1            98,
     1            'ONLY')
 
      call gspos ('BTMM',
     1            4,
     1            'LABB',
     1            0.,
     1            0. - gap(2) + btmthnss/2.0,
     1            0.,
     1            98,
     1            'ONLY')
 
      k = 0
c      call taps_geometry
      call ugeom_taps

c***********************************************************************
c Particle ID Detector  - Evie Downie, Glasgow University, 10/07/03
c***********************************************************************

c PARTICLE ID DETECTOR VOLUME
 
      par3(1)=4.13
      par3(2)=5.54
cas      par3(3)=30.00
      par3(3)=26.00

      call gsrotm(80,90.,1.2,90.,91.2,0.,0.)          !V.K.

      call gsvolu ('PIDD','TUBE',air,par3,3,ivolu)
c      call gspos('PIDD',1,'CCUT',0.,0.,0.,0,'ONLY')  !V.K.
cas      call gspos('PIDD',1,'CCUT',0.,0.,0.,80,'ONLY')  !V.K.
      call gspos('PIDD',1,'CCUT',0.,0.,1.2,80,'ONLY')

c SCINTILLATORS
c Please note, rather than using the general TRAP shape to produce the 
c assymetric shape of the individual scintillators exactly, we use gsdvn 
c to produce scinillators of the same top and bottom surface length and 
c same thickness as our physical detector, but with a 7.5 degree angle 
c at both sides rather than a 15 degree angle at one side and a 90 degree 
c angle at the other.  The overall barrel shape is the same, but edge 
c effects between scintillators may not be so accurate. However, this 
c should simplify tracking considerably.

        r_veto = 4.785
        t_veto = 0.2        ! thickness of veto barrel
        par10(1)  = 0.0
	par10(2)  = 360.0
	par10(3)  = 24
        par10(4)  = 2.0
        par10(5)  =-15.745
	par10(6)  = r_veto
	par10(7)  = r_veto + t_veto
        par10(8)  = 15.745
	par10(9)  = r_veto
	par10(10) = r_veto + t_veto
        call gsvolu ('TRCY',
     1               'PGON',
     1               scint,
     1               par10,
     1               10,
     1               ivolu)
 
        call gsdvn('TRC0', 'TRCY', 24, 2)


c LIGHT GUIDES 

c Again we use the symmetrical TRD1 rather than complicate things 
c with the general TRAP.  This does not represent the sideways 
c taper as the light guide narrows towards the face of the PMT, but c
c should simplify tracking and is not vital as the light guide is 
c in the shaddow of the MWPC detector supports anyway.
 
        par4(1)=0.655 
        par4(2)=0.6675
        par4(3)=0.4
        par4(4)=0.05
        CALL GSVOLU('LGFO','TRD1',plastic,par4,4,ivolu)

        par4(1)=0.63 
        par4(2)=0.655
        par4(3)=0.15
        par4(4)=0.1
        CALL GSVOLU('LGFM','TRD1',plastic,par4,4,ivolu)

        par4(1)=0.6175 
        par4(2)=0.63
        par4(3)=0.4
        par4(4)=0.05
        CALL GSVOLU('LGFI','TRD1',plastic,par4,4,ivolu)

        par3(1)=0.0
        par3(2)=0.485
        par3(3)=0.1
        CALL GSVOLU('LGTU','TUBE',plastic,par3,3,ivolu)

c PHOTOMULTIPLIER TUBES AND MU METAL

c We filled the bases with CH2 of the measured density of the bases
c as the internal structure of the bases was far too complex to
c accurately describe.
c The PMTs are simply air filled glass tubes formed from glass of 
c the measured thickness. The complex internal electronics are ommited.
c The mumetal sleeves are tubes of the measured thickness of a single layer of 
c the mumetal foil.  In the actual detector there is a slight overlap
c at the end of the foil sheets when the tubes are wrapped, so there 
c is a thin double-thickness strip parallel to the tube axis.
c This has been ignored in the simulation as a slight effect which varies
c from tube to tube.

 
        par3(1) = 0.24      ! inner radius
cas        par3(1) = 0.44      ! inner radius
        par3(2) = 0.485     ! outer radius 
        par3(3) = 2.315     ! half length 
        call gsvolu ('TPMT',
     1               'TUBE',
     1               fglass,
     1               par3,
     1               3,
     1               ivolu)

        par3(1) = 0.0    ! inner radius
        par3(2) = 0.50     ! outer radius 
        par3(3) = 2.3    ! half length 
        call gsvolu ('BASE',
     1               'TUBE',
     1               fglass,
     1               par3,
     1               3,
     1               ivolu)
cas        call gsvolu ('BASE',
cas     1               'TUBE',
cas     1               basemat,
cas     1               par3,
cas     1               3,
cas     1               ivolu)

        par3(1) = 0.548    ! inner radius
        par3(2) = 0.55     ! outer radius 
        par3(3) = 4.615    ! half length 
        call gsvolu ('MUME',
     1               'TUBE',
     1               mumetal,
     1               par3,
     1               3,
     1               ivolu)

c SUPPORTING STRUCTURES

c Brass tube at upstream end

        par3(1)=5.455
        par3(2)=5.5
        par3(3)=3.875
        CALL GSVOLU('BRTU','TUBE',brass,par3,3,ivolu)

c Aluminium ring at upstream end, 

c Note UPS1 should have a more complex, 24 sided outer structure 
c to match the shape of the inside of the PMT barrel.  The thickest 
c part of this structure would however only be 2.04mm and therefore 
c the extra complexity would add little to the accuracy and also would 
c complicate the tracking.  

        par3(1)=4.585
        par3(2)=4.785
        par3(3)=0.15
        CALL GSVOLU('UPS1','TUBE',Al,par3,3,ivolu)

c UPS2 is the central part of the aluminium upstream support ring (with
c the largest diameter) which holds it in place in the brass tube.

        par3(1)=4.585
        par3(2)=5.35
        par3(3)=0.15
        CALL GSVOLU('UPS2','TUBE',Al,par3,3,ivolu)

c UPS3 is the sloping edge of the upstream aluminium support ring 
c which allows us to locate it easily into the brass tube.

        par5(1)=0.15
        par5(2)=4.585
        par5(3)=5.05
        par5(4)=4.585
        par5(5)=5.35
        CALL GSVOLU('UPS3','CONE',Al,par5,5,ivolu)

c Aluminium ring for holding PMTs at the downstream end of the detector.

        par3(1)=4.135
        par3(2)=5.535
        par3(3)=0.5
        CALL GSVOLU('PMTR','TUBE',Al,par3,3,ivolu)

c POSITIONING

c Positioning of scintillator barrel

        call gspos ('TRCY',
     1              1,
     1              'PIDD',
     1              0.,
     1              0.,
     1              -0.295,
     1              0,
     1              'ONLY')

c Positioning of light guides and PMTs

           do i = 0, 23
           call gsrotm((1000+i),90.,(97.5+15.*i),0.,(97.5+15.*i),90.,
      1         (7.5+15.*i))
           call gspos('LGFO',i,'PIDD',(5.035*cosd(7.5+(15.*i))),
      1         (5.035*sind(7.5+(15.*i))),15.35,(1000+i),'ONLY')
           call gspos('LGFM',i,'PIDD',(4.885*cosd(7.5+(15.*i))),
      1         (4.885*sind(7.5+(15.*i))),15.6,(1000+i),'ONLY')
           call gspos('LGFI',i,'PIDD',(4.735*cosd(7.5+(15.*i))),
      1         (4.735*sind(7.5+(15.*i))),15.35,(1000+i),'ONLY')
           call gspos('LGTU',i,'PIDD',(4.885*cosd(7.5+(15.*i))),
      1         (4.885*sind(7.5+(15.*i))),15.85,0,'ONLY')            
           call gspos('TPMT',i,'PIDD',(4.885*cosd(7.5+(15.*i))),
      1         (4.885*sind(7.5+(15.*i))),18.265,0,'ONLY')
           call gspos('MUME',i,'PIDD',(4.885*cosd(7.5+(15.*i))),
      1         (4.885*sind(7.5+(15.*i))),20.565,0,'ONLY')    
           call gspos('BASE',i,'PIDD',(4.885*cosd(7.5+(15.*i))),
      1         (4.885*sind(7.5+(15.*i))),22.88,0,'ONLY')    
              
           enddo

c positioning of the support rings

           call gspos('UPS1',1,'PIDD',0.,0.,-15.89,0,'ONLY')       
           call gspos('UPS2',1,'PIDD',0.,0.,-16.19,0,'ONLY')     
           call gspos('UPS3',1,'PIDD',0.,0.,-16.49,0,'ONLY')     
           call gspos('PMTR',1,'PIDD',0.,0.,20.565,0,'MANY')     
           call gspos('BRTU',1,'PIDD',0.,0.,-19.575,0,'ONLY')


c*********************************************************************
c Multi-Wire Proportional Chambers - Evie Downie, Glasgow University, 08/08/03
c*********************************************************************

c Positioning of the master wire chamber volume

           par3(1)=5.55
           par3(2)=10.0
cas           par3(3)=50.0
           par3(3)=42.0
       CALL GSVOLU('MWPC','TUBE',air,par3,3,ivolu)
        CALL GSPOS('MWPC',1,'CCUT',0.,0.,-0.8,0,'ONLY')  !V.K.


C --- wire chambers
c NB there are 1mm Rohacell walls wrapped in 25x10^-6m thick Kapton 
c film both inside and outside each of the wire chambers.
c These walls have 0.1x10^-6 m Al cathodes on the inside of the
c wire chamber and 0.1x10^-6 m Al foil on the outside to contain 
c the E-field. These details have been ommited in order to simpify 
c tracking, as a particle traveling at theta = 90 degrees through both 
c wire chambers travels through only 0.000769 radiation lengths
c that are not described in the simplified geometry presented below. 

           par3(1)=6.
           par3(2)=6.8
           par3(3)=18.95
        CALL GSVOLU('CHF1','TUBE',WCmat,par3,3,ivolu)
           par3(1)=9.2
           par3(2)=10.0
           par3(3)=28.5
        CALL GSVOLU('CHF2','TUBE',WCmat,par3,3,ivolu)


c Fiberglass supports for wire chambers

c.Please note the second wire chamber has been adjusted to fit 
c inside the crystal ball and is currently in Pavia.  These 
c measurements come from a combination of the scale diagram of
c the DAPHNE wire chambers commonly available and measurements
c made by D.Watts and passed on to me in diagramatic form.

           par3(1)=5.56
           par3(2)=8.16
           par3(3)=0.95
        CALL GSVOLU('WS1U','TUBE',fglass,par3,3,ivolu)

           par3(1)=5.56
           par3(2)=8.16
           par3(3)=0.95
        CALL GSVOLU('WS1D','TUBE',fglass,par3,3,ivolu)

           par5(1)=1.275
           par5(2)=5.56
           par5(3)=8.16
           par5(4)=5.56
           par5(5)=7.16
        CALL GSVOLU('SC1U','CONE',fglass,par5,5,ivolu)

           par5(1)=1.275
           par5(2)=5.56
           par5(3)=7.16
           par5(4)=5.56
           par5(5)=8.16
        CALL GSVOLU('SC1D','CONE',fglass,par5,5,ivolu)

           par3(1)=8.45
           par3(2)=10.00
           par3(3)=2.525
        CALL GSVOLU('WS2U','TUBE',fglass,par3,3,ivolu)

           par3(1)=8.45
           par3(2)=10.00
           par3(3)=2.525
        CALL GSVOLU('WS2D','TUBE',fglass,par3,3,ivolu)

c aluminium tube linking MWPCs

           par3(1)=8.2
           par3(2)=8.4
           par3(3)=6.05
        CALL GSVOLU('ALTU','TUBE',AL,par3,3,ivolu)


c Electronics approximated to 1mm thick cardboard cylinders.

c NB There are some resistors on the cardboard cylinders, but 
c these are mostly positioned in the shadow of the fiberglass 
c supports and thus to include them would increase the 
c simulation complexity without adding much to it's effectiveness. 

csa           par3(1)=6.46
           par3(1)=5.87
           par3(2)=6.47
           par3(3)=2.7
        CALL GSVOLU('WE1U','TUBE',fglass,par3,3,ivolu)
csa        CALL GSVOLU('WE1U','TUBE',cboard,par3,3,ivolu)
csa           par3(1)=6.46
           par3(1)=5.87
           par3(2)=6.47
           par3(3)=3.175
        CALL GSVOLU('WE1D','TUBE',fglass,par3,3,ivolu)
csa        CALL GSVOLU('WE1D','TUBE',cboard,par3,3,ivolu)
csa           par3(1)=9.68
           par3(1)=9.10
           par3(2)=9.70
           par3(3)=2.7
        CALL GSVOLU('WE2U','TUBE',fglass,par3,3,ivolu)
csa        CALL GSVOLU('WE2U','TUBE',cboard,par3,3,ivolu)
csa           par3(1)=9.68
           par3(1)=9.10
           par3(2)=9.70
           par3(3)=3.175
        CALL GSVOLU('WE2D','TUBE',fglass,par3,3,ivolu)
csa        CALL GSVOLU('WE2D','TUBE',cboard,par3,3,ivolu)

c POSITIONING


c  chambers

           call gspos('CHF1',1,'MWPC',0.,0.,0.,0,'ONLY')
           call gspos('CHF2',1,'MWPC',0.,0.,0.,0,'ONLY')

c  supports

           call gspos('WS1D',1,'MWPC',0.,0.,22.475,0,'ONLY')
           call gspos('WS1U',1,'MWPC',0.,0.,-22.475,0,'ONLY')       
           call gspos('SC1U',1,'MWPC',0.,0.,-20.225,0,'ONLY')
           call gspos('SC1D',1,'MWPC',0.,0.,20.225,0,'ONLY')     
           call gspos('WS2D',1,'MWPC',0.,0.,31.025,0,'ONLY')
           call gspos('WS2U',1,'MWPC',0.,0.,-31.025,0,'ONLY')
           call gspos('ALTU',1,'MWPC',0.,0.,-27.5375,0,'ONLY')
           call gspos('ALTU',2,'MWPC',0.,0.,27.5375,0,'ONLY')

c electronics

           call gspos('WE1U',1,'MWPC',0.,0.,-26.15,0,'ONLY')
           call gspos('WE1D',1,'MWPC',0.,0.,26.625,0,'ONLY')       
           call gspos('WE2U',1,'MWPC',0.,0.,-36.25,0,'ONLY')
           call gspos('WE2D',1,'MWPC',0.,0.,36.725,0,'ONLY')


c Approximation for all the cables which are comming from the PID and the 
c wire chamber. The cables are located in the cone between 12 and 21 degrees.
c Material for the cables in approximated by the cardboard material used for the 
c wire chamber electronics. Thinkness of the material is taken approximately 1cm (PID)
c and 10 cm (MWPC)

cas           par5(1)=0.5
cas           par5(2)=15.39
cas           par5(3)=9.36
cas           par5(4)=15.73
cas           par5(5)=9.56
cascas        CALL GSVOLU('PIDW','CONE',cboard,par5,5,ivolu)
cas        CALL GSVOLU('PIDW','CONE',fglass,par5,5,ivolu)
cas        call gspos('PIDW',1,'LABB',0.,0.,45.5,0,'ONLY')

cas           par5(1)=1.5
cas           par5(2)=15.73
cas           par5(3)=10.97
cas           par5(4)=16.76
cas           par5(5)=11.69
cascas        CALL GSVOLU('MWPW','CONE',cboard,par5,5,ivolu)
cas        CALL GSVOLU('MWPW','CONE',fglass,par5,5,ivolu)
cas        call gspos('MWPW',1,'LABB',0.,0.,47.5,0,'ONLY')

cas           par5(1)=1.0
cas           par5(2)=16.13
cas           par5(3)=9.36
cas           par5(4)=16.84
cas           par5(5)=9.77
           par5(1)=1.0
           par5(2)=16.13
           par5(3)=7.04
           par5(4)=16.84
           par5(5)=7.35
        CALL GSVOLU('PIDW','CONE',fglass,par5,5,ivolu)
        call gspos('PIDW',1,'LABB',0.,0.,46.,0,'ONLY')

cas           par5(1)=2.0
cas           par5(2)=16.84
cas           par5(3)=11.21
cas           par5(4)=18.28
cas           par5(5)=12.16
           par5(1)=2.0
           par5(2)=16.84
           par5(3)=8.81
           par5(4)=18.28
           par5(5)=9.56
        CALL GSVOLU('MWPW','CONE',fglass,par5,5,ivolu)
        call gspos('MWPW',1,'LABB',0.,0.,49.,0,'ONLY')


c*********************************************************************
c Close geometry banks etc.
c*********************************************************************

           call gpvolu(0)
           call gptmed(0)
           call gprotm(0)
           call GPRINT ('MATE', 0)   ! Materials

           call GGCLOS
           call grfile(21,"cbsasha.rz","ON")
c
c  Format statements
c
200     format (3(i1, 1x), i1)
300     format (i2, 1x, i2)     

      return
      end
