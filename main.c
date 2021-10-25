#include <16F877A.H>
#include <stdio.h>
#use delay(clock=4M)                                                            // Clock usado 4MHz
#fuses XT,NOWDT                                                                 // (para 16F877A)


#include <lcd.c>                                                                //LIBRERIA DE LCD
#include <TONES.C>                                                              //libreria para el sonido.
#define use_portd_lcd TRUE                                                       

#use fast_io(b)                                                                  // Optimiza manejo pines de E/S
#use fast_IO(C)                                                                  // Optimiza manejo pines de E/S
#use fast_IO(D)

#byte PORTB = 0x06                                                               //estabLece direccion del puerto b
#byte PORTC = 0x07
#byte PORTD = 0x08

#define TONE_PIN PIN_B0                                                         //define el pin B0 como salida del sonido

#bit PAUSA= PORTB.1                                                             //pines designados para los pulsadores
#bit FALTA= PORTB.2
#bit PUNTO= PORTB.3
#bit INC_P_L= PORTB.4
#bit DEC_P_L= PORTB.5
#bit INC_F_L= PORTB.6
#bit DEC_F_L= PORTB.7
#bit START= PORTC.0
#bit INC_P_V= PORTC.4
#bit DEC_P_V= PORTC.5
#bit INC_F_V= PORTC.6
#bit DEC_F_V= PORTC.7

#bit pspmode=0x89.4                                                             //configuracion de puerto paralelo en purto D

enum eEstados {                                                                 /* ESTADOS POSIBLES DE MAQUINA */
   INICIO, FALTAS_M, FALTAS_S, PERIODOS, TIEMPO_M, TIEMPO_S, FIN
   
} caso;

/*******************************************/
/*            variables globales           */
/*******************************************/

int per=1;
int a=02, b=00;                                                                 //declaracion de variables para el control del tiempo
int auxa, auxb;                                                                 //variables auxiliares..
int fal_L=0, fal_V=0, pun_L=0, pun_V=0;
int aux_fal_V=0, aux_fal_L=0;



/************************************************/
/*FUNCION DE MUESTRA DE LOS CONTENIDOS INICIALES*/
/************************************************/

void cartel_inicio(){                                                           //muestra los datos iniciales antes de empesar el partido
   lcd_gotoxy(1,1);
   printf(lcd_putc,"\f%02d:%02d",a,b);
   lcd_gotoxy(7,1);
   printf(lcd_putc,"LOC:%03d-%d",pun_L,fal_L);
   lcd_gotoxy(2,2);
   printf(lcd_putc,"P:%d",per);
   lcd_gotoxy(7,2);
   printf(lcd_putc,"VIS:%03d-%d",pun_V,fal_V);
   delay_ms(2000);
   return;
}

/************************************************/
/*      CARTEL DE SOLICITUD PARA COMIENZO       */
/************************************************/

void comienzo(){                                                                //indica que se debe presionar el boton star para 
   lcd_putc ("\f");                                                             //comensar...
   delay_ms(50);
   while(START==0){
      delay_ms(35);
      lcd_gotoxy(3,1);
      printf(lcd_putc,"Pulse START");
      lcd_gotoxy(3,2);
      printf(lcd_putc,"para iniciar");
      delay_ms(160);
   }
   //lcd_putc ("\f");
   return;
}

/************************************************/
/*CONTROL DE LOS SEGUNDOS DEL TIEMPO DEL PARTIDO*/
/************************************************/

void segundos(){
   lcd_putc("\f");
   while(a!=0 || b!=0){
      if(FALTA==1){                                                             //controla si se presiono el pulsador de faltas
         caso=FALTAS_S;
         return;
      }
      if(PAUSA==1){                                                             //controla si se presiono el pulsador de pausa
         delay_ms(50);
         caso=TIEMPO_S;
         return;
      }
      if (INC_P_L == 1){                                                        //incrementa puntos del equipo local.
         delay_ms(100);
         if (INC_P_L==0){ 
             pun_L++;
         } 
      }
      if(pun_L==0){                                                             //evita mostrar valores  negativos. 
         if(DEC_P_L==1){
            pun_L=0;
         }
      }
      else{                                                                     //decrementa puntos en caso de cometer error.
           if(DEC_P_L==1){
              delay_ms(100);
              if (DEC_P_L==0){ 
                  pun_L--;
               } 
            }
      }
      if(INC_P_V==1){                                                           //incrementa puntos del equipo visitante.
         delay_ms(100);
         if (INC_P_V==0){ 
             pun_V++;
         } 
      }
      if(pun_V==0){                                                             //evita mostrar valores  negativos.
          if(DEC_P_V==1){
             pun_V=0;
          }
      }
      else{                                                                     //decrementa puntos en caso de cometer error.
           if(DEC_P_V==1){
              delay_ms(100);
              if (DEC_P_V==0){ 
                  pun_V--;
              } 
           }
      }
      if(b==00){
         b=99;
         a--;
      }
      lcd_gotoxy(1,1);
      printf(lcd_putc,"%02d:%02d",a,b);                                         //muestras en el LCD 
      lcd_gotoxy(7,1);
      printf(lcd_putc,"LOC:%03d-%d",pun_L,fal_L);
      lcd_gotoxy(2,2);
      printf(lcd_putc,"P:%d",per);
      lcd_gotoxy(7,2);
      printf(lcd_putc,"VIS:%03d-%d",pun_V,fal_V);
      delay_ms(8);
      b--;
      if(a==0 && b==0){
         generate_tone(C_NOTE[2],3000);                                         //sonido de chicharra de 3 seg.
         lcd_gotoxy(1,1);
         printf(lcd_putc,"%02d:%02d",a,b);                                      //muestras en el LCD
         lcd_gotoxy(7,1);
         printf(lcd_putc,"LOC:%03d-%d",pun_L,fal_L);
         lcd_gotoxy(2,2);
         printf(lcd_putc,"P:%d",per);
         lcd_gotoxy(7,2);
         printf(lcd_putc,"VIS:%03d-%d",pun_V,fal_V);
         caso=PERIODOS;
         return;
      }
   }
}

/************************************************/
/* CONTROL DE LOS MINUTOS DEL TIEMPO DEL PARTIDO*/
/************************************************/

void minutos(){
   lcd_putc("\f");
   while(TRUE){
      if(FALTA==1){                                                             //controla si se presiono el pulsador de faltas
         caso=FALTAS_M;
         return;
      }
      delay_ms(60);
      if(PAUSA==1){                                                             //controla si se presiono el pulsador de pausa
         delay_ms(50);
         caso=TIEMPO_M;
         return;
      }
      if(INC_P_L==1){                                                           //incrementa puntos del equipo local.
         Delay_ms(80); 
            pun_L++;
      }
      if(pun_L==0){                                                             //evita mostrar valores  negativos. 
           if(DEC_P_L==1){
               pun_L=0;
            }
         }
       else{                                                                    //decrementa puntos en caso de cometer error.
           if(DEC_P_L==1){
              delay_ms(80);
              pun_L--;
            }
       }
       if(INC_P_V==1){                                                          //incrementa puntos del equipo visitante.
           delay_ms(80);
           pun_V++;
       }
       if(pun_V==0){                                                            //evita mostrar valores  negativos.
           if(DEC_P_V==1){
               pun_V=0;
            }
         }
       else{                                                                    //decrementa puntos en caso de cometer error.
           if(DEC_P_V==1){
              delay_ms(80);
              pun_V--;
           }
       }

       if(b==00){
         b=59;
         a--;
      }
      lcd_gotoxy(1,1);
      printf(lcd_putc,"%02d:%02d",a,b);
      lcd_gotoxy(7,1);
      printf(lcd_putc,"LOC:%03d-%d",pun_L,fal_L);
      lcd_gotoxy(2,2);
      printf(lcd_putc,"P:%d",per);
      lcd_gotoxy(7,2);
      printf(lcd_putc,"VIS:%03d-%d",pun_V,fal_V);
      delay_ms(600);                                                            //reatrdo de 1 seg.
      b--;
 
      if(a==00){
         a=59;
         b=99;
         segundos();
         return;
      }
   }
}


/************************************************/
/*              FUNCION PERIODOS                */
/************************************************/

void fun_periodo(){                                                             //incrementa el periodo a medidas que se termina..
   if(per<4){                                                                   //el tiempo de partidos..
      per++;                                                                    
      a=02;                                                                     //secargan con el tiempo del descanso
      b=00;
      lcd_putc("\f");
      while(a!=0 || b!=0){
         if(b==00){                                                             //decremento del tiempo del descanso
            b=59;
            a--;
         }
         delay_ms(800);                                                            
         lcd_gotoxy(6,1);
         printf(lcd_putc,"%02d:%02d",a,b);
         lcd_gotoxy(5,2);
         printf(lcd_putc,"DESCANSO");                                           
         b--;
      }
      generate_tone(C_NOTE[2],3000);
      lcd_gotoxy(6,1);
      printf(lcd_putc,"\fPERIODO: %d",per);
      delay_ms(1000);
      a=02;                                                                     //caraga el valor del tiempo del partido
      b=00;
      caso=INICIO;
      return;
   }
   caso=FIN;                                                                    //para finalizar el partido luego de haber pasado los 4 periodos
   return;
}

/************************************************/
/*          FUNCION CONTEO DE FALTAS            */
/************************************************/

void fun_faltas(){
   
   while(FALTA==1){   
      if(INC_F_L==1){                                                              //control de faltas de los dos equipos..
         delay_ms(35);
         fal_L++;
         aux_fal_L++;
         if(fal_L==6){
            fal_L=0;
         }
      }
      if(fal_L==0){                                                             /********************************************/
        if(DEC_F_L==1){                                                         /*  control para no tomar valores negativos */
           fal_L=0;                                                             /********************************************/
         }
      }
      else{
         if(DEC_F_L==1){
            delay_ms(35);
            aux_fal_L++;
            fal_L--;
         }
      }
      if(INC_F_V==1){
        delay_ms(35);
        fal_V++;
        aux_fal_V++;
        if(fal_V==6){
           fal_V=0;
        }
      }
      if(fal_V==0){                                                             /********************************************/
        if(DEC_F_V==1){                                                         /*  control para no tomar valores negativos */
           fal_V=0;                                                             /********************************************/
         }
      }
      else{
         if(DEC_F_V==1){
            delay_ms(35);
            fal_V--;
            aux_fal_V++;
         }
      }
      lcd_gotoxy(1,1);
      printf(lcd_putc,"FALTAS E-LOC %d",fal_L);
      lcd_gotoxy(1,2);
      printf(lcd_putc,"FALTAS E-VIS %d",fal_V);
      delay_ms(200);
   }
   return;
}

/************************************************/
/*        FUNCION SOLICITUD DE TIEMPO           */
/************************************************/

void fun_tiempo(){
   auxa=01;                                                                     //variables que cargan el tiempo de solicitud
   auxb=00;
   lcd_putc("\f");
   while(auxa!=0 || auxb!=0){                                                   /********************************************/
      if(auxb==00){                                                             /*           decremento del                 */ 
         auxb=59;                                                               /*              tiempo                      */
         auxa--;                                                                /********************************************/
      }
      delay_ms(800);                                                            //retardo de un segundo
      lcd_gotoxy(6,1);                                                          /********************************************/
      printf(lcd_putc,"%02d:%02d",auxa,auxb);                                   /*             muestras en el               */
      lcd_gotoxy(6,2);                                                          /*                  LCD                     */
      printf(lcd_putc,"TIEMPO");                                                /********************************************/
      auxb--;
   }
   generate_tone(C_NOTE[2],3000);                                               //sonido de la chicharra..
   delay_ms(1000);
   lcd_putc("\f");
   return;
}

/************************************************/
/*          FUNCION FIN DEL PARTIDO             */
/************************************************/

void fun_fin(){                                                                 //Se encaga de hacer las muestras de los puntos 
   lcd_putc ("\f");                                                             //y las faltas que tubieron los equipos en el
   lcd_gotoxy(2,2);                                                             //en el transcurso del partido.
   printf(lcd_putc,"FIN DE PARTIDO");                                           
   delay_ms(2000);
   lcd_putc ("\f");
   lcd_gotoxy(1,1);
   printf(lcd_putc,"LOCAL     FALTAS");
   lcd_gotoxy(2,2);
   printf(lcd_putc,"%03d       %03d",pun_L,aux_fal_L);
   delay_ms(2000);
   lcd_putc ("\f");
   lcd_gotoxy(1,1);
   printf(lcd_putc,"VISITANTE FALTAS");
   lcd_gotoxy(4,2);
   printf(lcd_putc,"%03d       %03d",pun_V,aux_fal_V);
   delay_ms(2000);
   lcd_putc ("\f");
   lcd_gotoxy(2,2);
   printf(lcd_putc,"PRESIONE RESET");                                           //indica que se presione reset para reconfigurar el reloj
   delay_ms(2000);
}


/************************************************/
/*                PROGRAMA PRINCIPAL            */
/************************************************/

void main(){
   set_tris_b(0xff);                                                            //configuro el puerto b como entrada
   set_tris_c(0xff);                                                            //configuro el puerto c como entrada
   set_tris_d(0x00);                                                            //configuro el puerto d como salida

   
   lcd_init();                                                                  //inicializa el display
   cartel_inicio();                                                             //invoco la funcion (cartel_inicio)
   lcd_gotoxy(6,1);                                                             //se escribe apartir del 6°caracter dela fila 1 del lcd
   printf(lcd_putc,"\fPERIODO: %d",per);                                        //escribo la palabra Periodo en el lcd
   delay_ms(1000);
   caso=INICIO;                                                                 //configuro para ingresar en el primer caso de la funcion switch
   while(true){
      switch(caso){                                                             //seleccion de las variables de la maquina de estado
            case INICIO:
               comienzo();                                                      //invoco la funcion comienzo
               minutos();                                                       //invoco la funcion minutos
            break;
            
            case PERIODOS:
               fun_periodo();                                                   //invoco la funcion fun_periodo
            break;
            
            case FALTAS_M:
               printf(lcd_putc,"\f");
               fun_faltas();                                                    //invoco la funcion fun_faltas
               minutos();                                                       //invoco la funcion minutos
            break;
            
            case FALTAS_S:
               printf(lcd_putc,"\f");
               fun_faltas();                                                    //invoco la funcion fun_faltas
               segundos();                                                       //invoco la funcion segundos
            break;

            case TIEMPO_M:
