               subroutine UGEOM_solid_target

***********************************************************************
*      tmat = Li, carbon, Ca, or lead                                 *
*      trgt_length = 5cm(Li), 1.5cm(C), 1cm(Ca), 0.05cm(Pb)           * 
***********************************************************************
* Viktor Kashevarov                                                   *
***********************************************************************

       implicit none
#include "materials.inc"

      integer
     1 IVOLU       ! system volume number returned by GSVOLU
     
      real
     1   par(6)   ! volume parameters
     1,  trgt_length
     1,  z
     1,  zm
     1,  rm
************************************************************

c======================================================================
        zm = 20.         ! z/2 for mother volume 
        rm = 3.94        ! radius of mother volume

c target lenght:
      if(tmat.eq.Li7)trgt_length=5. 
      if(tmat.eq.carbon)trgt_length=1.5 
      if(tmat.eq.Ca40)trgt_length=1. 
      if(tmat.eq.lead)trgt_length=0.05 

c======================================================================

c========== mother volume  ===========>
      PAR(1) = 0.           
      PAR(2) = rm         
      PAR(3) = zm
      call GSVOLU('TRGT','TUBE',air,PAR,3,IVOLU) 
        z = -(par(3) - 14.5)      
      call GSPOS('TRGT',1,'CCUT',0.,0.,z,0,'ONLY')   

c=========== CFK tube ===============>
      PAR(1) = 0.           
      PAR(2) = 3.6         
      PAR(3) = zm - 0.25      
      call GSVOLU('CFKT','TUBE',carbon,PAR,3,IVOLU) 
        z = -0.25      
      call GSPOS('CFKT',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(1) = 0.           
      PAR(2) = 3.5         
      PAR(3) = par(3) - 0.5      
      call GSVOLU('VAC1','TUBE',vacuum,PAR,3,IVOLU) 
        z = -0.5      
      call GSPOS('VAC1',1,'CFKT',0.,0.,z,0,'ONLY')   

c============ Al flange ==============> 
      PAR(1) = 0.           
      PAR(2) = 3.5        
      PAR(3) = 0.5       
      call GSVOLU('FLA1','TUBE',Al,PAR,3,IVOLU) 
        z = zm - par(3) - 0.25       
      call GSPOS('FLA1',1,'CFKT',0.,0.,z,0,'ONLY')   

      PAR(1) = 0.        
      PAR(2) = 3.1           
      PAR(3) = 0.3            
      call GSVOLU('VAC2','TUBE',vacuum,PAR,3,IVOLU) 
        z = -0.2       
      call GSPOS('VAC2',1,'FLA1',0.,0.,z,0,'ONLY')   

      PAR(1) = 0.2        
      PAR(2) = 0.                   
      PAR(3) = 3.1            
      PAR(4) = 0.            
      PAR(5) = 3.19            
      call GSVOLU('VAC3','CONE',vacuum,PAR,5,IVOLU) 
        z = 0.3       
      call GSPOS('VAC3',1,'FLA1',0.,0.,z,0,'ONLY')   

c======>
      PAR(1) = 0.           
      PAR(2) = rm        
      PAR(3) = 0.15       
      call GSVOLU('FLA2','TUBE',Al,PAR,3,IVOLU) 
        z = zm - par(3) - 0.2       
      call GSPOS('FLA2',1,'TRGT',0.,0.,z,0,'ONLY')   

      PAR(1) = 0.15        
      PAR(2) = 0.                  
      PAR(3) = 3.19            
      PAR(4) = 0.            
      PAR(5) = 3.26            
      call GSVOLU('VAC4','CONE',vacuum,PAR,5,IVOLU) 
        z = 0.       
      call GSPOS('VAC4',1,'FLA2',0.,0.,z,0,'ONLY')   

c======>
      PAR(1) = rm - 0.68           
      PAR(2) = rm        
      PAR(3) = 0.1       
      call GSVOLU('FLA3','TUBE',Al,PAR,3,IVOLU) 
        z = zm - par(3)        
      call GSPOS('FLA3',1,'TRGT',0.,0.,z,0,'ONLY')   
c======>


c======= Kapton window ===================>
      PAR(1) = 0.
      PAR(2) = rm - 0.68
      par(3) = 0.006
      call GSVOLU('WIND','TUBE',kapton,PAR,3,IVOLU)        
        z = zm - 0.2 + par(3)      
      call GSPOS('WIND',1,'TRGT',0.,0.,z,0,'ONLY')   

c======= target cell ===================>
      PAR(1) = 0.                          
      PAR(2) = 1.5
      par(3) = trgt_length/2.
      call GSVOLU('CELL','TUBE',tmat,PAR,3,IVOLU)
        z = zm - 0.75 - 13.      
      call GSPOS('CELL',1,'VAC1',0.,0.,z,0,'ONLY')   

c======= target holder ===================>
      PAR(1) = 1.5                          
      PAR(2) = 3.5
      par(3) = 0.5
      call GSVOLU('HOLD','TUBE',rohacell,PAR,3,IVOLU)
      call GSPOS('HOLD',1,'VAC1',0.,0.,z,0,'ONLY')   

c==========================================================>
      end
c====================================================================!

    
